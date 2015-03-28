cmake_minimum_required (VERSION 2.8)

PROJECT (cairo)

SET (cairo_SOURCES
     src/cairo.c
     src/cairo-analysis-surface.c
     src/cairo-arc.c
     src/cairo-array.c
     src/cairo-atomic.c
     src/cairo-base64-stream.c
     src/cairo-base85-stream.c
     src/cairo-bentley-ottmann-rectangular.c
     src/cairo-bentley-ottmann-rectilinear.c
     src/cairo-bentley-ottmann.c
     src/cairo-botor-scan-converter.c
     src/cairo-boxes-intersect.c
     src/cairo-boxes.c
     src/cairo-cache.c
     src/cairo-clip-boxes.c
     src/cairo-clip-polygon.c
     src/cairo-clip-region.c
     src/cairo-clip-surface.c
     src/cairo-clip-tor-scan-converter.c
     src/cairo-clip.c
     src/cairo-color.c
     src/cairo-composite-rectangles.c
     src/cairo-compositor.c
     src/cairo-contour.c
     src/cairo-damage.c
     src/cairo-debug.c
     src/cairo-default-context.c
     src/cairo-deflate-stream.c
     src/cairo-device.c
     src/cairo-error.c
     src/cairo-fallback-compositor.c
     src/cairo-fixed.c
     src/cairo-font-face-twin-data.c
     src/cairo-font-face-twin.c
     src/cairo-font-face.c
     src/cairo-font-options.c
     src/cairo-freed-pool.c
     src/cairo-freelist.c
     src/cairo-ft-font.c
     src/cairo-gstate.c
     src/cairo-hash.c
     src/cairo-hull.c
     src/cairo-image-compositor.c
     src/cairo-image-info.c
     src/cairo-image-source.c
     src/cairo-image-surface.c
     src/cairo-lzw.c
     src/cairo-mask-compositor.c
     src/cairo-matrix.c
     src/cairo-mempool.c
     src/cairo-mesh-pattern-rasterizer.c
     src/cairo-misc.c
     src/cairo-mono-scan-converter.c
     src/cairo-mutex.c
     src/cairo-no-compositor.c
     src/cairo-observer.c
     src/cairo-paginated-surface.c
     src/cairo-path-bounds.c
     src/cairo-path-fill.c
     src/cairo-path-fixed.c
     src/cairo-path-in-fill.c
     src/cairo-path-stroke-boxes.c
     src/cairo-path-stroke-polygon.c
     src/cairo-path-stroke-traps.c
     src/cairo-path-stroke-tristrip.c
     src/cairo-path-stroke.c
     src/cairo-path.c
     src/cairo-pattern.c
     src/cairo-pdf-operators.c
     src/cairo-pdf-shading.c
     src/cairo-pdf-surface.c
     src/cairo-pen.c
     src/cairo-png.c
     src/cairo-polygon-intersect.c
     src/cairo-polygon-reduce.c
     src/cairo-polygon.c
     src/cairo-ps-surface.c
     src/cairo-raster-source-pattern.c
     src/cairo-recording-surface.c
     src/cairo-rectangle.c
     src/cairo-rectangular-scan-converter.c
     src/cairo-region.c
     src/cairo-rtree.c
     src/cairo-scaled-font-subsets.c
     src/cairo-scaled-font.c
     src/cairo-script-surface.c
     src/cairo-shape-mask-compositor.c
     src/cairo-slope.c
     src/cairo-spans-compositor.c
     src/cairo-spans.c
     src/cairo-spline.c
     src/cairo-stroke-dash.c
     src/cairo-stroke-style.c
     src/cairo-surface-clipper.c
     src/cairo-surface-fallback.c
     src/cairo-surface-observer.c
     src/cairo-surface-offset.c
     src/cairo-surface-snapshot.c
     src/cairo-surface-subsurface.c
     src/cairo-surface-wrapper.c
     src/cairo-surface.c
     src/cairo-svg-surface.c
     src/cairo-time.c
     src/cairo-tor-scan-converter.c
     src/cairo-tor22-scan-converter.c
     src/cairo-toy-font-face.c
     src/cairo-traps-compositor.c
     src/cairo-traps.c
     src/cairo-tristrip.c
     src/cairo-unicode.c
     src/cairo-user-font.c
     src/cairo-version.c
     src/cairo-wideint.c)

SET (cairo_PUBLIC_HEADERS
    )

SET (cairo_PRIVATE_HEADERS
    )

INCLUDE_DIRECTORIES (../zlib ../png ../pixman/pixman ../freetype/include ../freetype/include/freetype/config)

IF (NOT DEFINED WIN32)
ADD_DEFINITIONS( -DHAVE_INTTYPES_H=1 )
ADD_DEFINITIONS( -DHAVE_UINT64_T=1 )
ENDIF ()
ADD_DEFINITIONS( -DCAIRO_NO_MUTEX=1 )
ADD_DEFINITIONS( -DFT_CONFIG_CONFIG_H=\"ftconfig.h\" )
ADD_DEFINITIONS( -DFT_CONFIG_MODULES_H=\"ftmodule.h\" )

ADD_LIBRARY (cairo STATIC ${cairo_SOURCES})
