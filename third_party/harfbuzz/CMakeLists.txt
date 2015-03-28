cmake_minimum_required (VERSION 2.8)

PROJECT (harfbuzz)

SET (harfbuzz_SOURCES
     src/hb-blob.cc
     src/hb-buffer-serialize.cc
     src/hb-buffer.cc
     src/hb-common.cc
     src/hb-fallback-shape.cc
     src/hb-face.cc
     src/hb-font.cc
     src/hb-ot-tag.cc
     src/hb-set.cc
     src/hb-shape.cc
     src/hb-shape-plan.cc
     src/hb-shaper.cc
     src/hb-tt-font.cc
     src/hb-unicode.cc
     src/hb-warning.cc
     src/hb-ot-layout.cc
     src/hb-ot-map.cc
     src/hb-ot-shape.cc
     src/hb-ot-shape-complex-arabic.cc
     src/hb-ot-shape-complex-default.cc
     src/hb-ot-shape-complex-indic.cc
     src/hb-ot-shape-complex-indic-table.cc
     src/hb-ot-shape-complex-myanmar.cc
     src/hb-ot-shape-complex-sea.cc
     src/hb-ot-shape-complex-thai.cc
     src/hb-ot-shape-normalize.cc
     src/hb-ot-shape-fallback.cc
     src/hb-ft.cc
     src/hb-ucdn.cc
     src/hb-ucdn/ucdn.c)

SET (harfbuzz_PUBLIC_HEADERS
    )

SET (harfbuzz_PRIVATE_HEADERS
    )

INCLUDE_DIRECTORIES (. src/hb-ucdn ../cairo/src ../freetype/include ../freetype/include/freetype/config)

ADD_DEFINITIONS( -DHAVE_CONFIG_H )
ADD_DEFINITIONS( -DFT_CONFIG_CONFIG_H=\"ftconfig.h\" )
ADD_DEFINITIONS( -DFT_CONFIG_MODULES_H=\"ftmodule.h\" )

ADD_LIBRARY (harfbuzz STATIC ${harfbuzz_SOURCES})
