cmake_minimum_required (VERSION 2.8)

PROJECT (zlib)

SET (zlib_SOURCES
    adler32.c compress.c crc32.c deflate.c gzclose.c gzlib.c gzread.c
    gzwrite.c inflate.c infback.c inftrees.c inffast.c trees.c uncompr.c
    zutil.c)

SET (zlib_PUBLIC_HEADERS
    zconf.h zlib.h)

SET (zlib_PRIVATE_HEADERS
    crc32.h deflate.h gzguts.h inffast.h inffixed.h inflate.h
    inftrees.h trees.h zutil.h)

ADD_LIBRARY (zlib STATIC ${zlib_SOURCES})
