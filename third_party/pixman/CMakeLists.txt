cmake_minimum_required (VERSION 2.8)

PROJECT (pixman)

SET (pixman_SOURCES
    pixman/pixman.c
    pixman/pixman-access.c
    pixman/pixman-access-accessors.c
    pixman/pixman-bits-image.c
    pixman/pixman-combine32.c
    pixman/pixman-combine-float.c
    pixman/pixman-conical-gradient.c
    pixman/pixman-filter.c
    pixman/pixman-x86.c
    pixman/pixman-mips.c
    pixman/pixman-arm.c
    pixman/pixman-ppc.c
    pixman/pixman-edge.c
    pixman/pixman-edge-accessors.c
    pixman/pixman-fast-path.c
    pixman/pixman-glyph.c
    pixman/pixman-general.c
    pixman/pixman-gradient-walker.c
    pixman/pixman-image.c
    pixman/pixman-implementation.c
    pixman/pixman-linear-gradient.c
    pixman/pixman-matrix.c
    pixman/pixman-noop.c
    pixman/pixman-radial-gradient.c
    pixman/pixman-region16.c
    pixman/pixman-region32.c
    pixman/pixman-solid-fill.c
    pixman/pixman-timer.c
    pixman/pixman-trap.c
    pixman/pixman-utils.c)

SET (pixman_PUBLIC_HEADERS
    pixman/pixman-version.h
    pixman/pixman.h)

SET (pixman_PRIVATE_HEADERS
    pixman/loongson-mmintrin.h
    pixman/pixman-accessor.h
    pixman/pixman-arm-common.h
    pixman/pixman-arm-neon-asm.h
    pixman/pixman-arm-simd-asm.h
    pixman/pixman-combine32.h
    pixman/pixman-compiler.h
    pixman/pixman-edge-imp.h
    pixman/pixman-inlines.h
    pixman/pixman-mips-dspr2-asm.h
    pixman/pixman-mips-dspr2.h
    pixman/pixman-private.h)

INCLUDE_DIRECTORIES (../png pixman)

ADD_DEFINITIONS( -DHAVE_CONFIG_H )
ADD_DEFINITIONS( -DPIXMAN_NO_TLS=1 )

ADD_LIBRARY (pixman STATIC ${pixman_SOURCES})
