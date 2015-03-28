/*
 *  EGClipboardX11.c
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#if defined (__linux__)

#include "EG.h"
#include "EGClipboard.h"

EGbool EGClipboard::setData(EGClipboardDataType dataType, const void *data, size_t dataLen)
{
    return false;
}

EGbool EGClipboard::getData(EGClipboardDataType dataType, void *data, size_t bufLen, size_t *outLen)
{
    return false;
}

#endif
