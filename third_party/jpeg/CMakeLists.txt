cmake_minimum_required (VERSION 2.8)

PROJECT (jpeg)

SET (jpeg_SOURCES
    jaricom.c jcapimin.c jcapistd.c jcarith.c jccoefct.c jccolor.c
    jcdctmgr.c jchuff.c jcinit.c jcmainct.c jcmarker.c jcmaster.c
    jcomapi.c jcparam.c jcprepct.c jcsample.c jctrans.c jdapimin.c
    jdapistd.c jdarith.c jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c
    jddctmgr.c jdhuff.c jdinput.c jdmainct.c jdmarker.c jdmaster.c
    jdmerge.c jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c
    jfdctfst.c jfdctint.c jidctflt.c jidctfst.c jidctint.c jmemnobs.c
    jmemmgr.c jquant1.c jquant2.c jutils.c)

SET (jpeg_PUBLIC_HEADERS
    jpeglib.h jinclude.h jerror.h jconfig.h)

SET (jpeg_PRIVATE_HEADERS
    jdct.h jmorecfg.h jmemsys.h jpegint.h jversion.h)

ADD_LIBRARY (jpeg STATIC ${jpeg_SOURCES})
