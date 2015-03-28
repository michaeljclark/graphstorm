/*
 *  EGImage.h
 *
 *  Copyright (c) 2008 - 2012, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGIMAGE_H_
#define EGIMAGE_H_

#include "EGThread.h"

class EGImage;
typedef std::shared_ptr<EGImage> EGImagePtr;
typedef std::vector<EGImagePtr> EGImageList;
class EGImageIO;
class EGImageIOJPEG;
class EGImageIOPNG;
class EGImageDecompressor;
typedef std::shared_ptr<EGImageDecompressor> EGImageDecompressorPtr;
class EGImageDecompressItem;
typedef std::shared_ptr<EGImageDecompressItem> EGImageDecompressItemPtr;
typedef std::list<EGImageDecompressItemPtr> EGImageDecompressItemList;
typedef std::map<std::string,EGImageDecompressItemPtr> EGImageDecompressItemMap;

class EGResource;
typedef std::shared_ptr<EGResource> EGResourcePtr;


/* EGPixelFormat */

enum EGPixelFormat
{
    EGPixelFormatNone,
    EGPixelFormatAlpha,
    EGPixelFormatRGB,
    EGPixelFormatRGBA,
    EGPixelFormatARGB,
    EGPixelFormatRGB555,
    EGPixelFormatRGB565,
    EGPixelFormatLuminance,
    EGPixelFormatLuminanceAlpha,
    EGPixelFormatCompressed,
};


/* EGImage */

class EGImage
{
public:
    static const char* formatname[];
    
    static EGImageIOJPEG JPEG;
    static EGImageIOPNG PNG;
    
    EGImage();
    EGImage(const EGImage &image);
    EGImage(EGResourcePtr rsrc, EGuint width, EGuint height, EGPixelFormat format);
    EGImage(EGResourcePtr rsrc, EGuint width, EGuint height, EGPixelFormat format, EGubyte *texbuf);
    EGImage(EGResourcePtr rsrc, EGuint width, EGuint height, EGPixelFormat format, EGubyte *texbuf, EGenum compressedFormat, EGuint compressedDataSize);
    EGImage(EGResourcePtr rsrc, EGuint width, EGuint height, EGPixelFormat format, EGuint texId);
    virtual ~EGImage();
    
    static EGuint getBytesPerPixel(EGPixelFormat format)
    {
        switch (format) {
            case EGPixelFormatNone:           return 0;
            case EGPixelFormatAlpha:          return 1;
            case EGPixelFormatRGB:            return 3;
            case EGPixelFormatRGBA:           return 4;
            case EGPixelFormatARGB:           return 4;
            case EGPixelFormatRGB555:         return 2;
            case EGPixelFormatRGB565:         return 2;
            case EGPixelFormatLuminance:      return 1;
            case EGPixelFormatLuminanceAlpha: return 2;
            case EGPixelFormatCompressed:     return 0;
            default:                          return 0;
        }
    }
    
    EGuint getBytesPerPixel() { return getBytesPerPixel(format); }
    EGuint getWidth() { return width; }
    EGuint getHeight() { return height; }
    EGuint getOrigWidth() { return origwidth; }
    EGuint getOrigHeight() { return origheight; }
    EGubyte* getData() { return texbuf; }
    EGPixelFormat getPixelFormat() { return format; }
    EGfloat* getUV() { return texuv; }
        
    void create(EGPixelFormat format, EGuint width, EGuint height);
    EGuint getGLTexId();
    void convertFormat(EGPixelFormat newformat);
    void resizePow2();
    void createGLTexture();
    void updateGLTexture();
    void setRetainBuffer(EGbool flag) { retainBuffer = flag; }
    
    static EGImageIO* getImageIOFromMagic(unsigned char magic[8]);
    static EGImageIO* getImageIOFromMagic(std::string filename);
    static EGImageIO* getImageIOFromMagic(EGResourcePtr rsrc);
    static EGImageIO* getImageIOFromExt(std::string pathname);
    static void saveToFile(std::string filename, const EGImagePtr &image, EGImageIO *imageio = NULL);
    static EGImagePtr createBitmap(EGuint width, EGuint height, EGPixelFormat format, EGubyte *texbuf = NULL);
    static EGImagePtr createFromFile(std::string filename, EGImageIO *imageio = NULL,
                                     EGbool cache = true, EGPixelFormat optformat = EGPixelFormatNone);
    static EGImagePtr createFromResourceCache(std::string rsrcName);
    static EGImagePtr createFromResourceCache(EGResourcePtr rsrc);
    static EGImagePtr createFromResource(std::string rsrcName, EGImageIO *imageio = NULL,
                                         EGbool cache = true, EGPixelFormat optformat = EGPixelFormatNone);
    static EGImagePtr createFromResource(EGResourcePtr rsrc, EGImageIO *imageio = NULL,
                                         EGbool cache = true, EGPixelFormat optformat = EGPixelFormatNone);
    
    static void restoreGraphicsState();

protected:
    EGResourcePtr rsrc;
    EGuint width, height;
    EGPixelFormat format;
    EGubyte *texbuf;
    EGuint texId;
    EGenum compressedFormat;
    EGuint dataSize;
    EGbool retainBuffer;
    EGuint origwidth, origheight;
    EGfloat texuv[8];
    
private:
    static EGMutex imageCacheLock;
    static std::map<EGstring,EGImagePtr> imageCache;
    static std::set<EGImage*> imageSet;

    void inituv(EGuint oldwidth, EGuint oldheight);
};


/* EGImageIO */

class EGImageIO
{
public:
    virtual EGImage* load(EGResourcePtr rsrc, EGPixelFormat optformat) = 0;
    virtual void save(EGImage* image, std::string filename) = 0;
    
    virtual ~EGImageIO() {}
};

class EGImageIOJPEG : public EGImageIO
{
private:
    float quality;
    
public:
    static const EGfloat QUALITY_DEFAULT;
    
    EGImageIOJPEG();
    EGImage* load(EGResourcePtr rsrc, EGPixelFormat optformat);
    void save(EGImage* image, std::string filename);
    void setQuality(float q) { quality = q; }
};

class EGImageIOPNG : public EGImageIO
{
public:
    EGImage* load(EGResourcePtr rsrc, EGPixelFormat optformat);
    void save(EGImage* image, std::string filename);
};

class EGImageIOTIFF : public EGImageIO
{
public:
    EGImage* load(EGResourcePtr rsrc, EGPixelFormat optformat);
    void save(EGImage* image, std::string filename);
};

class EGImageIOTiled : public EGImageIO
{
public:
    EGImage* load(EGResourcePtr rsrc, EGPixelFormat optformat);
    void save(EGImage* image, std::string filename);
};

class EGImageIOPVR : public EGImageIO
{
public:
    EGImage* load(EGResourcePtr rsrc, EGPixelFormat optformat);
    void save(EGImage* image, std::string filename);
};

class EGImageIOPKM : public EGImageIO
{
public:
    EGImage* load(EGResourcePtr rsrc, EGPixelFormat optformat);
    void save(EGImage* image, std::string filename);
};

class EGImageIOGIF : public EGImageIO
{
public:
    EGImage* load(EGResourcePtr rsrc, EGPixelFormat optformat);
    void save(EGImage* image, std::string filename);
};


/* EGImageDecompressor */

class EGImageDecompressor : public EGThread
{
public:
    EGMutex queueMutex;
    EGCondVar queueCondVar;
    
    EGImageDecompressItemList itemtodecompress;
    EGImageDecompressItemList itemdecompressing;
    EGImageDecompressItemList itemtoprocess;
    EGImageDecompressItemMap imagedecompressmap;
    
    EGImageDecompressor();
        
    EGThreadExitCode run();
    void queueItem(EGImageDecompressItemPtr decompressItem);
    EGint idleProcessItems();
    EGsize queueSize() { return itemtodecompress.size() + itemdecompressing.size() + itemtoprocess.size(); }
};


/* EGImageDecompressItem */

class EGImageDecompressItem
{
public:
    EGResourcePtr rsrc;
    EGImagePtr image;
    EGbool cacheImage;
    EGImageDecompressItemList waiters;
    
    EGImageDecompressItem(EGResourcePtr rsrc, EGbool cacheImage = false);
    
    virtual ~EGImageDecompressItem();
    
    virtual void decompressCompleted() = 0;
    virtual void decompressFailed() = 0;
};

#endif
