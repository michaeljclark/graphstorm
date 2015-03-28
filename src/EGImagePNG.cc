/*
 *  EGImagePNG.cc
 *
 *  Copyright (c) 2008 - 2012, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGGL.h"
#include "EGImage.h"
#include "EGResource.h"

#if TARGET_OS_MAC

EGImage* EGImageIOPNG::load(EGResourcePtr rsrc, EGPixelFormat optformat)
{
    CGImageRef image;
    CGContextRef bitmap;
    CGDataProviderRef provider;
    CGColorSpaceRef colorspace;
    CFStringRef path;
    CFURLRef url;
    
    std::string filename = rsrc->getPath();
    path = CFStringCreateWithCString (NULL, filename.c_str(), kCFStringEncodingUTF8);
    url = CFURLCreateWithFileSystemPath(NULL, path, kCFURLPOSIXPathStyle, 0);
    CFRelease(path);
    provider = CGDataProviderCreateWithURL (url);
    CFRelease(url);
    image = CGImageCreateWithPNGDataProvider(provider, NULL, false, kCGRenderingIntentDefault);
    CGDataProviderRelease(provider);
    
    if(image) {
        EGsize width, height;
        EGPixelFormat format;
        EGubyte *texbuf;

        width = CGImageGetWidth(image);
        height = CGImageGetHeight(image);
        colorspace = CGImageGetColorSpace(image);
        if (CGColorSpaceGetNumberOfComponents(colorspace) == 1) {
            texbuf = new GLubyte[width * height];
            memset(texbuf, 0, width * height);
            format = (optformat == EGPixelFormatAlpha) ? EGPixelFormatAlpha : EGPixelFormatLuminance;
            bitmap = CGBitmapContextCreate(texbuf, width, height, 8, width, colorspace, kCGImageAlphaNone);
        } else {
            texbuf = new GLubyte[width * height * 4];
            memset(texbuf, 0, width * height * 4);
            format = EGPixelFormatRGBA;
            bitmap = CGBitmapContextCreate(texbuf, width, height, 8, width * 4, colorspace, kCGImageAlphaPremultipliedLast);
        }
        CGContextDrawImage(bitmap, CGRectMake(0.0, 0.0, (CGFloat)width, (CGFloat)height), image);
        CGImageRelease(image);
        CGContextRelease(bitmap);
        
        return new EGImage(rsrc, (EGint)width, (EGint)height, format, texbuf);
    }

    return NULL;
}

void EGImageIOPNG::save(EGImage* image, std::string filename)
{
#if ! TARGET_OS_IPHONE
    CGImageRef cgimage;
    CGContextRef bitmap;
    CGColorSpaceRef colorspace;
    CFStringRef path, type;
    CFURLRef url;
    CFMutableDictionaryRef saveopts;
    CGImageDestinationRef dest;
    
    switch (image->getPixelFormat()) {
        case EGPixelFormatLuminance:
        case EGPixelFormatAlpha:
            colorspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericGray);
            bitmap = CGBitmapContextCreate(image->getData(), image->getWidth(), image->getHeight(),
                                           8, image->getWidth() * image->getBytesPerPixel(),
                                           colorspace, kCGImageAlphaNone);
            break;
        case EGPixelFormatRGBA:
            colorspace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
            bitmap = CGBitmapContextCreate(image->getData(), image->getWidth(), image->getHeight(),
                                           8, image->getWidth() * image->getBytesPerPixel(),
                                           colorspace, kCGImageAlphaPremultipliedLast);
            break;
        default:
            fprintf(stderr, "EGImageIOPNG::save: invalid pixel format\n");
            exit(1);
            break;
    }
    
    cgimage = CGBitmapContextCreateImage(bitmap);
    saveopts = CFDictionaryCreateMutable(nil, 0, &kCFTypeDictionaryKeyCallBacks,  &kCFTypeDictionaryValueCallBacks);
    type = CFStringCreateWithCString(NULL, "public.png", kCFStringEncodingUTF8);
    path = CFStringCreateWithCString(NULL, filename.c_str(), kCFStringEncodingUTF8);
    url = CFURLCreateWithFileSystemPath(NULL, path, kCFURLPOSIXPathStyle, 0);
    dest = CGImageDestinationCreateWithURL(url, type, 1, NULL);
    CFRelease(path);
    CFRelease(type);
    CFRelease(url);    
    CGImageDestinationAddImage(dest, cgimage, saveopts);
    CGImageDestinationFinalize(dest);
    CFRelease(saveopts);
    CGImageRelease(cgimage);
    CGContextRelease(bitmap);
    CGColorSpaceRelease(colorspace);
#endif    
}

#else

#include <png.h>

#include <string.h>
#include <setjmp.h>

static const char class_name[] = "EGImageIOPNG";

static void EGImageIOPNG_png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
    EGResource *rsrc = (EGResource*)png_get_io_ptr(png_ptr);
    if (!rsrc) {
        png_error(png_ptr, "read from NULL resource");
    } else if (rsrc->read(data, length) < 0) {
        EGDebug("%s:%s error %s: %s\n", class_name, __func__, rsrc->getErrorMessage().c_str(), rsrc->getPath().c_str());
        png_error(png_ptr, rsrc->getErrorMessage().c_str());
    }
}

EGImage* EGImageIOPNG::load(EGResourcePtr rsrc, EGPixelFormat optformat)
{
    png_structp png_ptr;
    png_infop info_ptr;
    int bit_depth, color_type;
    int i;
    size_t row_stride;
    png_bytepp row_pointers = NULL;
    
    EGuint width, height;
    EGPixelFormat format;
    EGubyte *texbuf = NULL;
    
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        EGDebug("%s:%s error creating png read struct\n", class_name, __func__);
        return NULL;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        EGDebug("%s:%s error creating png infostruct\n", class_name, __func__);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        EGDebug("%s:%s error decompressing png\n", class_name, __func__);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(row_pointers);
        free(texbuf);
        return NULL;
    }

    png_set_read_fn(png_ptr, rsrc.get(), EGImageIOPNG_png_read_data);
    png_set_sig_bytes(png_ptr, 0);

    /* read all the info up to the image data  */
    png_read_info(png_ptr, info_ptr);

    png_uint_32 pwidth, pheight;
    png_get_IHDR(png_ptr, info_ptr, &pwidth, &pheight, &bit_depth, &color_type, NULL, NULL, NULL);
    width = pwidth;
    height = pheight;

    /* Set up some transforms. */
    if (color_type & PNG_COLOR_MASK_ALPHA) {
        format = EGPixelFormatRGBA;
    } else {
        format = EGPixelFormatRGB;
    }
    if (bit_depth > 8) {
        png_set_strip_16(png_ptr);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png_ptr);
    }
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
    }

    /* Update the png info struct.*/
    png_read_update_info(png_ptr, info_ptr);

    /* Rowsize in bytes. */
    row_stride = png_get_rowbytes(png_ptr, info_ptr);

    /* Allocate the image_data buffer. */
    if ((texbuf = new GLubyte[row_stride * height]) == NULL) {
        EGDebug("%s:%s error allocating memory\n", class_name, __func__);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }

    /* allocate row pointers */
    if ((row_pointers = (png_bytepp) malloc(height * sizeof(png_bytep))) == NULL) {
        EGDebug("%s:%s error allocating memory\n", class_name, __func__);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(texbuf);
        return NULL;
    }

    /* set the individual row_pointers to point at the correct offsets */
    for (i = 0; i < (int) height; ++i) {
        row_pointers[i] = texbuf + i * row_stride;
    }

    /* now we can go ahead and just read the whole image */
    png_read_image(png_ptr, row_pointers);

    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */

    /* Clean up. */
    free(row_pointers);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    rsrc->close();
    
    return new EGImage(rsrc, width, height, format, texbuf);
}

void EGImageIOPNG::save(EGImage* image, std::string filename)
{
    FILE *outfile;
    png_structp png_ptr;
    png_infop info_ptr;
    EGubyte** row_pointers = NULL;
    
    if ((outfile = fopen(filename.c_str(), "wb")) == NULL) {
        EGDebug("%s:%s error opening file %s\n", filename.c_str(), class_name, __func__);
        return;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        EGDebug("%s:%s error creating png write struct\n", class_name, __func__);
        fclose(outfile);
        return;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        EGDebug("%s:%s error creating png info struct\n", class_name, __func__);
        fclose(outfile);
        png_destroy_write_struct(&png_ptr, NULL);
        return;
     }

    if (setjmp(png_jmpbuf(png_ptr))) {
        EGDebug("%s:%s error compressing png\n", class_name, __func__);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(outfile);
        free(row_pointers);
        return;
    }

    EGubyte color_type;
    switch (image->getPixelFormat()) {
        case EGPixelFormatAlpha: color_type = PNG_COLOR_TYPE_GRAY; break;
        case EGPixelFormatLuminance: color_type = PNG_COLOR_TYPE_GRAY; break;
        case EGPixelFormatRGB: color_type = PNG_COLOR_TYPE_RGB; break;
        case EGPixelFormatRGBA: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
        default:
            EGDebug("%s:%s pixel format not supported\n", class_name, __func__);
            fclose(outfile);
            png_destroy_write_struct(&png_ptr, NULL);
            return;
    }
    png_init_io(png_ptr, outfile);
    png_set_IHDR(png_ptr, info_ptr, image->getWidth(), image->getHeight(), 8,
            color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    EGubyte *data = image->getData();
    row_pointers = (EGubyte**) malloc(sizeof(EGubyte*) * image->getHeight());
    for (EGuint y = 0; y < image->getHeight(); y++) {
        row_pointers[y] = data;
        data += image->getWidth() * image->getBytesPerPixel();
    }
    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, NULL);

    /* Clean up */
    free(row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(outfile);
}

#endif
