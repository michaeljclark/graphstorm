/*
 *  EGImageJPEG.cc
 *
 *  Copyright (c) 2008 - 2012, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGGL.h"
#include "EGImage.h"
#include "EGResource.h"

const EGfloat EGImageIOJPEG::QUALITY_DEFAULT = 0.75f;

EGImageIOJPEG::EGImageIOJPEG() : quality(QUALITY_DEFAULT) {}

#if TARGET_OS_MAC /* && ! TARGET_OS_IPHONE */

EGImage* EGImageIOJPEG::load(EGResourcePtr rsrc, EGPixelFormat optformat)
{
    CGImageRef image;
    CGContextRef bitmap;
    CGDataProviderRef provider;
    CGColorSpaceRef colorspace;
    CFStringRef path;
    CFURLRef url;
    
    std::string filename = rsrc->getPath();
    path = CFStringCreateWithCString(NULL, filename.c_str(), kCFStringEncodingUTF8);
    url = CFURLCreateWithFileSystemPath(NULL, path, kCFURLPOSIXPathStyle, 0);
    CFRelease(path);
    provider = CGDataProviderCreateWithURL(url);
    CFRelease(url);
    image = CGImageCreateWithJPEGDataProvider(provider, NULL, false, kCGRenderingIntentDefault);
    CGDataProviderRelease(provider);
    
    if(image) {
        EGsize width, height;
        EGPixelFormat format;
        EGubyte *texbuf;

        width = CGImageGetWidth(image);
        height = CGImageGetHeight(image);
        texbuf = new GLubyte[width * height * 4];
        colorspace = CGImageGetColorSpace(image);
        if (CGColorSpaceGetNumberOfComponents(colorspace) == 1) {
            format = EGPixelFormatLuminance;
            bitmap = CGBitmapContextCreate(texbuf, width, height, 8, width, colorspace, kCGImageAlphaNone);
        } else {
            format = EGPixelFormatRGBA;
            bitmap = CGBitmapContextCreate(texbuf, width, height, 8, width * 4, colorspace, kCGImageAlphaNoneSkipLast);
        }
        CGContextDrawImage(bitmap, CGRectMake(0.0, 0.0, (CGFloat)width, (CGFloat)height), image);
        CGImageRelease(image);
        CGContextRelease(bitmap);
        
        return new EGImage(rsrc, (EGint)width, (EGint)height, format, texbuf);
    }
    
    return NULL;
}

void EGImageIOJPEG::save(EGImage* image, std::string filename)
{
#if ! TARGET_OS_IPHONE
    CGImageRef cgimage;
    CGContextRef bitmap;
    CGColorSpaceRef colorspace;
    CFStringRef path, type;
    CFURLRef url;
    CFMutableDictionaryRef saveopts;
    CGImageDestinationRef dest;
    CFNumberRef qual;
    
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
                                           colorspace, kCGImageAlphaNoneSkipLast);
            break;
        default:
            fprintf(stderr, "EGImageIOPNG::save: invalid pixel format\n");
            exit(1);
            break;
    }

    cgimage = CGBitmapContextCreateImage(bitmap);
    saveopts = CFDictionaryCreateMutable(nil, 0, &kCFTypeDictionaryKeyCallBacks,  &kCFTypeDictionaryValueCallBacks);
    qual = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &quality);
    CFDictionarySetValue(saveopts, kCGImageDestinationLossyCompressionQuality, qual);
    type = CFStringCreateWithCString(NULL, "public.jpeg", kCFStringEncodingUTF8);
    path = CFStringCreateWithCString(NULL, filename.c_str(), kCFStringEncodingUTF8);
    url = CFURLCreateWithFileSystemPath(NULL, path, kCFURLPOSIXPathStyle, 0);
    dest = CGImageDestinationCreateWithURL(url, type, 1, NULL);
    CFRelease(path);
    CFRelease(type);
    CFRelease(url);
    CGImageDestinationAddImage(dest, cgimage, saveopts);
    CGImageDestinationFinalize(dest);
    CFRelease(qual);
    CFRelease(saveopts);
    CGImageRelease(cgimage);
    CGContextRelease(bitmap);
    CGColorSpaceRelease(colorspace);
#endif    
}

#else

#if defined (ANDROID)
#define JPEG_MEM_SRC 1
#else
#define JPEG_MEM_SRC 0
#endif

#include "jpeglib.h"
#if JPEG_MEM_SRC
#include "jinclude.h"
#endif
#include "jerror.h"

#include <string.h>
#include <setjmp.h>

struct my_error_mgr
{
    struct jpeg_error_mgr pub; /* "public" fields */

    jmp_buf setjmp_buffer; /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

void my_error_exit(j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    /* Always display  the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message)(cinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}

#if JPEG_MEM_SRC

/* pinched from jpeg-8/jdatasrc.c */

typedef struct {
    struct jpeg_source_mgr pub;   /* public fields */
    
    FILE * infile;                /* source stream */
    JOCTET * buffer;              /* start of buffer */
    boolean start_of_file;        /* have we gotten any data yet? */
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

#define INPUT_BUF_SIZE  4096    /* choose an efficiently fread'able size */

void init_mem_source (j_decompress_ptr cinfo)
{
    /* no work necessary here */
}

boolean fill_input_buffer (j_decompress_ptr cinfo)
{
    my_src_ptr src = (my_src_ptr) cinfo->src;
    size_t nbytes;
    
    nbytes = JFREAD(src->infile, src->buffer, INPUT_BUF_SIZE);
    
    if (nbytes <= 0) {
        if (src->start_of_file)     /* Treat empty input file as fatal error */
            ERREXIT(cinfo, JERR_INPUT_EMPTY);
        WARNMS(cinfo, JWRN_JPEG_EOF);
        /* Insert a fake EOI marker */
        src->buffer[0] = (JOCTET) 0xFF;
        src->buffer[1] = (JOCTET) JPEG_EOI;
        nbytes = 2;
    }
    
    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = nbytes;
    src->start_of_file = FALSE;
    
    return TRUE;
}

boolean fill_mem_input_buffer (j_decompress_ptr cinfo)
{
    static JOCTET mybuffer[4];
    
    /* The whole JPEG data is expected to reside in the supplied memory
     * buffer, so any request for more data beyond the given buffer size
     * is treated as an error.
     */
    WARNMS(cinfo, JWRN_JPEG_EOF);
    /* Insert a fake EOI marker */
    mybuffer[0] = (JOCTET) 0xFF;
    mybuffer[1] = (JOCTET) JPEG_EOI;
    
    cinfo->src->next_input_byte = mybuffer;
    cinfo->src->bytes_in_buffer = 2;
    
    return TRUE;
}


void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    struct jpeg_source_mgr * src = cinfo->src;
    
    /* Just a dumb implementation for now.  Could use fseek() except
     * it doesn't work on pipes.  Not clear that being smart is worth
     * any trouble anyway --- large skips are infrequent.
     */
    if (num_bytes > 0) {
        while (num_bytes > (long) src->bytes_in_buffer) {
            num_bytes -= (long) src->bytes_in_buffer;
            (void) fill_input_buffer(cinfo);
            /* note we assume that fill_input_buffer will never return FALSE,
             * so suspension need not be handled.
             */
        }
        src->next_input_byte += (size_t) num_bytes;
        src->bytes_in_buffer -= (size_t) num_bytes;
    }
}

void term_source (j_decompress_ptr cinfo)
{
    /* no work necessary here */
}

void jpeg_mem_src (j_decompress_ptr cinfo, unsigned char * inbuffer, unsigned long insize)
{
    struct jpeg_source_mgr * src;

    if (inbuffer == NULL || insize == 0)	/* Treat empty input as fatal error */
        ERREXIT(cinfo, JERR_INPUT_EMPTY);

    /* The source object is made permanent so that a series of JPEG images
    * can be read from the same buffer by calling jpeg_mem_src only before
    * the first one.
    */
    if (cinfo->src == NULL) {	/* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
        (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                        SIZEOF(struct jpeg_source_mgr));
    }

    src = cinfo->src;
    src->init_source = init_mem_source;
    src->fill_input_buffer = fill_mem_input_buffer;
    src->skip_input_data = skip_input_data;
    src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->term_source = term_source;
    src->bytes_in_buffer = (size_t) insize;
    src->next_input_byte = (JOCTET *) inbuffer;
}

#endif

EGImage* EGImageIOJPEG::load(EGResourcePtr rsrc, EGPixelFormat optformat)
{
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    EGuint src_row_stride;
    EGuint dst_row_stride;
    JSAMPARRAY buffer;

    EGuint width, height;
    EGubyte *texbuf;
    EGPixelFormat srcformat;
    EGPixelFormat dstformat;

#if !JPEG_MEM_SRC
    FILE * infile;
    std::string filename = rsrc->getPath();
    if ((infile = fopen(filename.c_str(), "rb")) == NULL) {
        fprintf(stderr, "EGImageIOJPEG::load: can't open %s\n", filename.c_str());
        return NULL;
    }
#endif

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_decompress(&cinfo);
        return NULL;
    }
    jpeg_create_decompress(&cinfo);
#if JPEG_MEM_SRC
    jpeg_mem_src(&cinfo, (unsigned char*)rsrc->getBuffer(), rsrc->getLength());
#else
    jpeg_stdio_src(&cinfo, infile);
#endif
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
    height = cinfo.output_height;
    if (cinfo.output_components == 1) {
        srcformat = EGPixelFormatLuminance;
    } else if (cinfo.output_components == 2) {
        srcformat = EGPixelFormatLuminanceAlpha;
    } else if (cinfo.output_components == 3) {
        srcformat = EGPixelFormatRGB;
    } else if (cinfo.output_components == 4) {
        srcformat = EGPixelFormatRGBA;
    } else {
        fprintf(stderr, "EGImageIOJPEG::load: no appropriate pixel format");
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
#if !JPEG_MEM_SRC
        fclose(infile);
#endif
        return NULL;
    }
    
    if (optformat == EGPixelFormatNone) {
        dstformat = srcformat;
    } else {
        dstformat = optformat;
    }
        
    src_row_stride = width * cinfo.output_components;
    dst_row_stride = width * EGImage::getBytesPerPixel(dstformat);
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, src_row_stride, 16);
    texbuf = new GLubyte[(EGsize)dst_row_stride * (EGsize)height];
    while (cinfo.output_scanline < cinfo.output_height) {
        int numscanlines = jpeg_read_scanlines(&cinfo, buffer, 16);
        for (int i = 0; i < numscanlines; i++) {
            EGubyte *src = buffer[i];
            EGubyte *dst = texbuf + (cinfo.output_scanline - numscanlines + i) * dst_row_stride;
            if (srcformat == dstformat) {
                memcpy(dst, src, dst_row_stride);
            } else {
                EGubyte c[4] = {};
                for (EGuint x = 0; x < width; x++) {
                    switch (srcformat) {
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
                    switch (dstformat) {
                        case EGPixelFormatRGBA:
                            *(dst++) = c[0];
                            *(dst++) = c[1];
                            *(dst++) = c[2];
                            *(dst++) = c[3];
                        case EGPixelFormatARGB:
                            *(dst++) = c[1];
                            *(dst++) = c[2];
                            *(dst++) = c[3];
                            *(dst++) = c[0];
                            break;
                        case EGPixelFormatRGB:
                            *(dst++) = c[0];
                            *(dst++) = c[1];
                            *(dst++) = c[2];
                            break;
                        case EGPixelFormatRGB555:
                            *((EGushort*)dst) = (((EGushort)c[0] << 7) & 0x7c00) | (((EGushort)c[1] << 2) & 0x3e0) | (((EGushort)c[2] >> 3) & 0x1f);
                            dst += 2;
                            break;
                        case EGPixelFormatRGB565:
                            *((EGushort*)dst) = (((EGushort)c[0] << 8) & 0xf800) | (((EGushort)c[1] << 3) & 0x7e0) | (((EGushort)c[2] >> 3) & 0x1f);
                            dst += 2;
                            break;
                        case EGPixelFormatLuminance:
                            *(dst++) = (EGubyte)((c[0] + c[1] + c[2]) / 3);
                            break;
                        case EGPixelFormatAlpha:
                            *(dst++) = c[3];
                            break;
                        default:
                            break;
                    }                    
                }
            }
#ifdef TARGET_OS_IPHONE
            pthread_yield_np();
#endif
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    
#if JPEG_MEM_SRC
    rsrc->close();
#else
    fclose(infile);
#endif
    
    return new EGImage(rsrc, width, height, dstformat, texbuf);
}

void EGImageIOJPEG::save(EGImage* image, std::string filename)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * outfile;
    JSAMPROW row_pointer[1];
    int row_stride;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    if ((outfile = fopen(filename.c_str(), "wb")) == NULL) {
      fprintf(stderr, "EGImageIOJPEG::save: can't open %s\n", filename.c_str());
      exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = image->getWidth();
    cinfo.image_height = image->getHeight();
    switch (image->getPixelFormat()) {
    case EGPixelFormatLuminance:
    case EGPixelFormatAlpha:
        cinfo.input_components = 1;
        cinfo.in_color_space = JCS_GRAYSCALE;
	break;
    case EGPixelFormatRGB:
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;
	break;
    default:
        fprintf(stderr, "EGImageIOJPEG::save: invalid pixel format");
        exit(1);
    }
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, (int)(quality * 100), TRUE);

    jpeg_start_compress(&cinfo, TRUE);
    row_stride = image->getWidth() * image->getBytesPerPixel();
    EGubyte *image_buffer = image->getData();
    while (cinfo.next_scanline < cinfo.image_height) {
       row_pointer[0] = image_buffer + cinfo.next_scanline * row_stride;
       (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
}

#endif
