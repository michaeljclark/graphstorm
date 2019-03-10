// See LICENSE for license details.

#if defined (_WIN32)

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
