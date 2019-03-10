// See LICENSE for license details.

#if defined (__native_client__)

#include "EG.h"
#include "EGClipboard.h"

static char* clipboardData = NULL;
static size_t clipboardLen = 0;

EGbool EGClipboard::setData(EGClipboardDataType dataType, const void *data, size_t dataLen)
{
    if (clipboardData) {
        delete [] clipboardData;
    }
    clipboardData = new char[dataLen];
    clipboardLen = dataLen;
    memcpy(clipboardData, data, dataLen);
    return true;
}

EGbool EGClipboard::getData(EGClipboardDataType dataType, void *data, size_t bufLen, size_t *outLen)
{
    if (outLen) {
        *outLen = clipboardLen;
    }
    if (clipboardData && data && bufLen >= clipboardLen) {
        memcpy(data, clipboardData, clipboardLen);
    }
    return true;
}

#endif
