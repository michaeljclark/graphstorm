cmake_minimum_required (VERSION 2.8)

PROJECT (tess)

SET (tess_SOURCES
     dict.c geom.c memalloc.c mesh.c normal.c priorityq.c render.c sweep.c
     tess.c tessmono.c)

SET (tess_PUBLIC_HEADERS
    glu.h)

SET (tess_PRIVATE_HEADERS
    dict-list.h dict.h geom.h memalloc.h mesh.h normal.h priorityq-heap.h priorityq-sort.h priorityq.h render.h sweep.h tess.h tessmono.h)

ADD_LIBRARY (tess STATIC ${tess_SOURCES})
