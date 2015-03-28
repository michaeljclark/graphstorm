cmake_minimum_required (VERSION 2.8)

PROJECT (png)

SET (png_SOURCES
    png.c pngerror.c pngget.c pngmem.c pngpread.c pngread.c pngrio.c
    pngrtran.c pngrutil.c pngset.c pngtrans.c pngwio.c pngwrite.c
    pngwtran.c pngwutil.c)

SET (png_PUBLIC_HEADERS
    png.h pngconf.h pnglibconf.h)

SET (png_PRIVATE_HEADERS
    config.h pngdebug.h pnginfo.h pngpriv.h pngstruct.h)

INCLUDE_DIRECTORIES (../zlib)

ADD_DEFINITIONS( -DDHAVE_CONFIG_H )

ADD_LIBRARY (png STATIC ${png_SOURCES})
