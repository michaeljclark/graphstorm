/*
 *  EGImage.cc
 *
 *  Copyright (c) 2008 - 2012, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGGL.h"
#include "EGImage.h"
#include "EGResource.h"


/* EGImage */

#define ANISITROPY 0

static char class_name[] = "EGImage";

EGImageIOJPEG EGImage::JPEG;
EGImageIOPNG  EGImage::PNG;

const char* EGImage::formatname[] = {
    "None",
    "Alpha",
    "RGB",
    "RGBA",
    "ARGB",
    "RGB555",
    "RGB565",
    "Luminance",
    "LuminanceAlpha",
    "Compressed",
};

EGMutex EGImage::imageCacheLock;
std::map<EGstring,EGImagePtr> EGImage::imageCache;
std::set<EGImage*> EGImage::imageSet;

EGImageIO* EGImage::getImageIOFromMagic(unsigned char magic[8])
{
    static const unsigned char JPG_MAGIC[] = { 0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46 };
    static const unsigned char PNG_MAGIC[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
#if 0
    //static const unsigned char GIF_MAGIC[] = { 0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x32, 0x00 };
#endif

    if (memcmp(magic, JPG_MAGIC, sizeof(JPG_MAGIC)) == 0) {
        return &JPEG;
    } else if (memcmp(magic, PNG_MAGIC, sizeof(PNG_MAGIC)) == 0) {
        return &PNG;
#if 0
    } else if (memcmp(magic, GIF_MAGIC, sizeof(GIF_MAGIC)) == 0) {
        return &GIF;
#endif
    } else {
        return NULL;
    }
}

EGImageIO* EGImage::getImageIOFromMagic(EGResourcePtr rsrc)
{
    EGsize bytesRead;
    unsigned char magic[8];    

    if ((bytesRead = rsrc->read(magic, sizeof(magic))) != sizeof(magic)) {
        EGError("%s:%s error reading magic: %s: %s\n", class_name, __func__, rsrc->getPath().c_str(), strerror(errno));
        rsrc->close();
        return NULL;
    }
    rsrc->close();
    
    if (bytesRead != sizeof(magic)) {
        EGError("%s:%s error reading magic: %s: short read\n", class_name, __func__, rsrc->getPath().c_str());
        return NULL;
    }

    return getImageIOFromMagic(magic);
}

EGImageIO* EGImage::getImageIOFromMagic(std::string filename)
{
    FILE *file;
    EGsize bytesRead;
    unsigned char magic[8];
    
    if ((file = fopen(filename.c_str(), "r")) == NULL) {
        EGError("%s:%s error opening file: %s: %s\n", class_name, __func__, filename.c_str(), strerror(errno));
        return NULL;
    }
    
    if ((bytesRead = fread(magic, 1, sizeof(magic), file)) != sizeof(magic)) {
        EGError("%s:%s error reading magic: %s: %s\n", class_name, __func__, filename.c_str(), strerror(errno));
        fclose(file);
        return NULL;
    }
    fclose(file);
    
    if (bytesRead != sizeof(magic)) {
        EGError("%s:%s error reading magic: %s: short read\n", class_name, __func__, filename.c_str());
        return NULL;
    }

    return getImageIOFromMagic(magic);
}

EGImageIO* EGImage::getImageIOFromExt(std::string pathname)
{
    size_t dotoffset = pathname.find_last_of('.');
    if (dotoffset == std::string::npos) {
        return NULL;
    }
    
    std::string type = pathname.substr(dotoffset + 1);
    if (type == "jpg" || type == "jpeg") {
        return &JPEG;
    } else if (type == "png") {
        return &PNG;
#if 0
    } else if (type == "tif" || type == "tiff") {
        return &TIFF;
    } else if (type == "tiled") {
        return &TILED;
    } else if (type == "pvr" || type == "pvrz") {
        return &PVR;
    } else if (type == "pkm" || type == "pkmz") {
        return &PKM;
    } else if (type == "gif") {
        return &GIF;
#endif
    } else {
        return NULL;
    }
}

void EGImage::saveToFile(std::string filename, const EGImagePtr &image, EGImageIO *imageio)
{
    if (!imageio) {
        imageio = getImageIOFromExt(filename);
    }
    if (imageio) {
        imageio->save(image.get(), filename);
    } else {
        EGError("%s:%s can't save image: image io not found: %s\n", class_name, __func__, filename.c_str());
    }
}

EGImagePtr EGImage::createBitmap(EGuint width, EGuint height, EGPixelFormat format, EGubyte *texbuf)
{
    EGImagePtr image;
    if (texbuf) {
        image = EGImagePtr(new EGImage(EGResourcePtr(), width, height, format, texbuf));
    } else {
        image = EGImagePtr(new EGImage(EGResourcePtr(), width, height, format));
    }
    image->retainBuffer = true;
    return image;
}

EGImagePtr EGImage::createFromFile(std::string filename, EGImageIO *imageio, EGbool cache, EGPixelFormat optformat)
{    
    EGResourcePtr rsrc = EGResource::getFileAsResource(filename);
    EGImagePtr image = createFromResource(rsrc, imageio, cache, optformat);
    return image;
}

EGImagePtr EGImage::createFromResourceCache(std::string rsrcName)
{
    return createFromResourceCache(EGResource::getResource(rsrcName));
}

EGImagePtr EGImage::createFromResourceCache(EGResourcePtr rsrc)
{
    EGImagePtr image;
    imageCacheLock.lock();
    std::map<EGstring,EGImagePtr>::iterator ici = imageCache.find(rsrc->getPath());
    if (ici != imageCache.end()) {
        image = (*ici).second;
    }
    imageCacheLock.unlock();
    return image;
}

EGImagePtr EGImage::createFromResource(std::string rsrcName, EGImageIO *imageio, EGbool cache, EGPixelFormat optformat)
{
    EGResourcePtr rsrc = EGResource::getResource(rsrcName);
    EGImagePtr image = createFromResource(rsrc, imageio, cache, optformat);
    return image;
}

EGImagePtr EGImage::createFromResource(EGResourcePtr rsrc, EGImageIO *imageio, EGbool cache, EGPixelFormat optformat)
{
    std::string rsrcName = rsrc->getPath();
    std::string name = rsrcName.substr(0, rsrcName.find_last_of('.'));
    std::string ext = rsrcName.substr(rsrcName.find_last_of('.')+1);

    EGImagePtr image;
    
    if (rsrcName.size() == 0) {
        EGError("%s:%s resource does not exist: %s\n", class_name, __func__, rsrcName.c_str());
        return image;
    }
    
    if (cache) {
        imageCacheLock.lock();
        std::map<EGstring,EGImagePtr>::iterator ici = imageCache.find(rsrcName);
        if (ici != imageCache.end()) {
            image = (*ici).second;
        }
        imageCacheLock.unlock();
        if (image != NULL) {
            EGDebug("%s:%s returned cache %s\n", class_name, __func__, rsrcName.c_str());
            return image;
        }
    }

    EGDebug("%s:%s loading %s\n", class_name, __func__, rsrcName.c_str());
    if (!imageio) {
        imageio = getImageIOFromExt(rsrcName);
    }
    if (!imageio) {
        imageio = getImageIOFromMagic(rsrc);
    }
    if (imageio) {
        image = EGImagePtr(imageio->load(rsrc, optformat));
        if (image && cache) {
            imageCacheLock.lock();
            if (imageCache.find(rsrc->getPath()) == imageCache.end()) {
                imageCache.insert(std::pair<std::string,EGImagePtr>(rsrcName, image));
            }
            imageCacheLock.unlock();
        }
    }
    if (image) {
        EGDebug("%s:%s width=%d height=%d format=%s\n", class_name, __func__,
                (int)image->width, (int)image->height, formatname[image->format]);
    } else {
        EGError("%s:%s could not load %s\n", class_name, __func__, rsrcName.c_str());
    }

    return image;
}

void EGImage::restoreGraphicsState()
{
#if defined (ANDROID)
    imageCacheLock.lock();
    imageCache.clear();
    imageCacheLock.unlock();
    std::set<EGImage*>::iterator ii;
    std::set<EGImage*> imageSetCopy = imageSet;
    for (ii = imageSetCopy.begin(); ii != imageSetCopy.end(); ii++) {
        EGImage *image = *ii;
        image->texId = 0;
        std::string imageName = image->rsrc ? image->rsrc->getPath() : std::string("<no-resource>");
        if (image->texbuf) {
            EGDebug("%s:%s reloading from retained buffer image=%p name=%s", class_name, __func__, image, imageName.c_str());
            image->createGLTexture();
        } else if (image->rsrc) {
            EGDebug("%s:%s reloading from resource image=%p name=%s", class_name, __func__, image, imageName.c_str());
            EGImagePtr newimage = createFromResource(image->rsrc, NULL, false);
            image->texbuf = newimage->texbuf;
            newimage->texbuf = NULL;
            image->createGLTexture();
        } else {
            EGDebug("%s:%s unable to reload image=%p: no retained buffer or resource", class_name, __func__, image);
        }
    }
#endif
}

EGImage::EGImage() :
    rsrc(), width(0), height(0), format(EGPixelFormatNone), texbuf(0), texId(0), compressedFormat(0), dataSize(0), retainBuffer(false)
{
#if defined (ANDROID)
    imageSet.insert(this);
#endif
}

EGImage::EGImage(const EGImage &image) :
    rsrc(image.rsrc), width(image.width), height(image.height), format(image.format), texId(0), compressedFormat(0), dataSize(0), retainBuffer(false)
{
    create(format, width, height);
    memcpy(texbuf, image.texbuf, (EGsize)width * height * getBytesPerPixel());
    inituv(width, height);
#if defined (ANDROID)
    imageSet.insert(this);
#endif
}

EGImage::EGImage(EGResourcePtr rsrc, EGuint width, EGuint height, EGPixelFormat format) :
    rsrc(rsrc), width(width), height(height), format(format), texId(0), compressedFormat(0), dataSize(0), retainBuffer(false)
{
    create(format, width, height);
    inituv(width, height);
#if defined (ANDROID)
    imageSet.insert(this);
#endif
}

EGImage::EGImage(EGResourcePtr rsrc, EGuint width, EGuint height, EGPixelFormat format, EGubyte *texbuf) :
    rsrc(rsrc), width(width), height(height), format(format), texbuf(texbuf), texId(0), compressedFormat(0), dataSize(0), retainBuffer(false)
{
    inituv(width, height);
#if defined (ANDROID)
    imageSet.insert(this);
#endif
}

EGImage::EGImage(EGResourcePtr rsrc, EGuint width, EGuint height, EGPixelFormat format, EGubyte *texbuf, EGenum compressedFormat, EGuint dataSize) :
    rsrc(rsrc), width(width), height(height), format(format), texbuf(texbuf), texId(0), compressedFormat(compressedFormat), dataSize(dataSize), retainBuffer(false)
{
    inituv(width, height);
#if defined (ANDROID)
    imageSet.insert(this);
#endif
}

EGImage::EGImage(EGResourcePtr rsrc, EGuint width, EGuint height, EGPixelFormat format, EGuint texId) :
    rsrc(rsrc), width(width), height(height), format(format), texbuf(NULL), texId(texId), compressedFormat(0), dataSize(0), retainBuffer(false)
{
    inituv(width, height);
#if defined (ANDROID)
    imageSet.insert(this);
#endif
}

EGImage::~EGImage()
{
#if defined (ANDROID)
    imageSet.erase(this);
#endif
    if (texbuf) {
        delete [] texbuf;
        texbuf = NULL;
    }
    if (texId) {
        EGDebug("%s:%s deleted tex id %d\n", class_name, __func__, texId);
        glDeleteTextures(1, &texId);
    }
}

void EGImage::inituv(EGuint oldwidth, EGuint oldheight)
{
    origwidth = oldwidth;
    origheight = oldheight;
    EGfloat texuv[8] = {
        0,                                      0,
        0,                                      (EGfloat)origheight / (EGfloat)height,
        (EGfloat)origwidth / (EGfloat)width,    0,
        (EGfloat)origwidth / (EGfloat)width,    (EGfloat)origheight / (EGfloat)height,
    };
    memcpy(this->texuv, texuv, sizeof(texuv));
}

void EGImage::create(EGPixelFormat format, EGuint width, EGuint height)
{
    this->width = width;
    this->height = height;
    this->format = format;

    EGsize size = (EGsize)width * height * getBytesPerPixel();
    texbuf = new EGubyte[size];
    memset(texbuf, 0, size);
}

EGuint EGImage::getGLTexId()
{
    if (texId == 0 && texbuf) {
        createGLTexture();
    }
    return texId;
}

/* Decoding macros by bd_ aka bdonlan. I release these into the public domain.
   Source: http://creatures.wikia.com/wiki/555/565 */

#define ENCODE_RGB(r, g, b) ((r) << 16 | (g) << 8 | (b))

#define DECODE_555(v)           \
    (ENCODE_RGB(                      \
    ((uint32_t)(v) & 0x7c00) >> 7,  \
    ((uint32_t)(v) & 0x03e0) >> 2,  \
    ((uint32_t)(v) & 0x001f) << 3  \
    ))

#define DECODE_565(v)           \
    (ENCODE_RGB(                      \
    ((uint32_t)(v) & 0xf800) >> 8 , \
    ((uint32_t)(v) & 0x07e0) >> 3 , \
    ((uint32_t)(v) & 0x001f) << 3   \
    ))

void EGImage::convertFormat(EGPixelFormat newformat)
{
    if (format == newformat) return;
    EGDebug("%s:%s converting from %s to %s\n", class_name, __func__, formatname[format], formatname[newformat]);
    EGsize size = (EGsize)width * height * getBytesPerPixel(newformat);
    EGubyte *newtexbuf = new EGubyte[size];
    EGubyte *src = texbuf, *dest = newtexbuf;
    EGubyte c[4] = {};
    for (EGuint i=0; i < width * height; i++) {
        // load source pixel
        switch (format) {
            case EGPixelFormatRGBA:                 
                c[0] = *(src++);
                c[1] = *(src++);
                c[2] = *(src++);
                c[3] = *(src++);
                break;
            case EGPixelFormatARGB:
                c[1] = *(src++);
                c[2] = *(src++);
                c[3] = *(src++);
                c[0] = *(src++);
                break;
            case EGPixelFormatRGB:                  
                c[0] = *(src++);
                c[1] = *(src++);
                c[2] = *(src++);
                c[3] = 0xff;
                break;
            case EGPixelFormatRGB555:
                c[0] = (*((EGushort*)src) & 0x7c00)>>7;
                c[1] = (*((EGushort*)src) & 0x3e0)>>2;
                c[2] = (*((EGushort*)src) & 0x1f)<<3;
                c[3] = 0xff;
                src += 2;
                break;
            case EGPixelFormatRGB565:
                c[0] = (*((EGushort*)src) & 0xf800)>>8;
                c[1] = (*((EGushort*)src) & 0x7e0)>>3;
                c[2] = (*((EGushort*)src) & 0x1f)<<3;
                c[3] = 0xff;
                src += 2;
                break;
            case EGPixelFormatLuminance:
                c[0] = c[1] = c[2] = *(src++);
                c[3] = 0xff;
                break;
            case EGPixelFormatAlpha:
                c[0] = c[1] = c[2] = 0x00;
                c[3] = *(src++);
                break;
            default:
                break;
        }
        // store dest pixel
        switch (newformat) {
            case EGPixelFormatRGBA:
                *(dest++) = c[0];
                *(dest++) = c[1];
                *(dest++) = c[2];
                *(dest++) = c[3];
		break;
            case EGPixelFormatARGB:
                *(dest++) = c[1];
                *(dest++) = c[2];
                *(dest++) = c[3];
                *(dest++) = c[0];
                break;
            case EGPixelFormatRGB:
                *(dest++) = c[0];
                *(dest++) = c[1];
                *(dest++) = c[2];
                break;
            case EGPixelFormatRGB555:
                *((EGushort*)dest) = (((EGushort)c[0] << 7) & 0x7c00) | (((EGushort)c[1] << 2) & 0x3e0) | (((EGushort)c[2] >> 3) & 0x1f);
                dest += 2;
                break;
            case EGPixelFormatRGB565:
                *((EGushort*)dest) = (((EGushort)c[0] << 8) & 0xf800) | (((EGushort)c[1] << 3) & 0x7e0) | (((EGushort)c[2] >> 3) & 0x1f);
                dest += 2;
                break;
            case EGPixelFormatLuminance:
                *(dest++) = (EGubyte)((c[0] + c[1] + c[2]) / 3);
                break;
            case EGPixelFormatAlpha:
                *(dest++) = c[3];
                break;
            default:
                break;
        }
    }
    delete [] texbuf;
    format = newformat;
    texbuf = newtexbuf;
}

void EGImage::resizePow2()
{
    EGuint oldwidth = width;
    EGuint oldheight = height;
    EGuint newwidth = 1;
    EGuint newheight = 1;
    while (width > newwidth) newwidth <<= 1;
    while (height > newheight) newheight <<= 1;
    if (newwidth == oldwidth && newheight == oldheight) return;
    EGDebug("%s:%s resizing non power of two texture from %ux%u to %ux%u\n", class_name, __func__, oldwidth, oldheight, newwidth, newheight);
    EGsize oldrowsize = oldwidth * getBytesPerPixel(format);
    EGsize newrowsize = newwidth * getBytesPerPixel(format);
    EGsize newsize = (EGsize)newheight * newrowsize;
    EGubyte *newtexbuf = new EGubyte[newsize];
    memset(newtexbuf, 0, newsize);
    for (EGuint row = 0; row < height; row++) {
        EGubyte *src = texbuf + row * oldrowsize;
        EGubyte *dest = newtexbuf + row * newrowsize;
        for (EGuint i = 0; i < oldrowsize; i++) {
            *(dest++) = *(src++);
        }
    }
    delete [] texbuf;
    texbuf = newtexbuf;
    width = newwidth;
    height = newheight;
    inituv(oldwidth, oldheight);
}

void EGImage::createGLTexture()
{
    if (compressedFormat) {
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, compressedFormat, width, height, 0, dataSize, texbuf);
    } else {
        resizePow2();
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    #if ANISITROPY
        EGfloat maximumAnistropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnistropy);    
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnistropy);
    #endif
        switch (format) {
            case EGPixelFormatRGB:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texbuf);
                break;
            case EGPixelFormatRGB565:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, texbuf);
                break;
            case EGPixelFormatRGB555:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, texbuf);
                break;
            case EGPixelFormatRGBA:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texbuf);
                break;
            case EGPixelFormatAlpha:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, texbuf);
                break;
            case EGPixelFormatLuminance:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texbuf);
                break;
            case EGPixelFormatLuminanceAlpha:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, texbuf);
                break;
            default:
                break;
        }
    }
    EGDebug("%s:%s bound tex id %d with format %s\n", class_name, __func__, texId, formatname[format]);
    if (!retainBuffer) {
        delete [] texbuf;
        texbuf = NULL;
    }
}

void EGImage::updateGLTexture()
{
    if (!texId) {
        createGLTexture();
        return;
    }
    if (!texbuf) {
        EGError("%s:%s buffer not retained, can't update tex id %d\n", class_name, __func__, texId);
        return;
    }
    glBindTexture(GL_TEXTURE_2D, texId);
    switch (format) {
        case EGPixelFormatRGB:
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, texbuf);
            break;
        case EGPixelFormatRGB565:
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, texbuf);
            break;
        case EGPixelFormatRGB555:
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, texbuf);
            break;
        case EGPixelFormatRGBA:
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texbuf);
            break;
        case EGPixelFormatAlpha:
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_ALPHA, GL_UNSIGNED_BYTE, texbuf);
            break;
        case EGPixelFormatLuminance:
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, texbuf);
            break;
        case EGPixelFormatLuminanceAlpha:
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, texbuf);
            break;
        default:
            break;
    }
}

