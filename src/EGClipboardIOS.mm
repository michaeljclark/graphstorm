// See LICENSE for license details.

#if defined (__APPLE__) && defined (TARGET_OS_IPHONE)

#include "EG.h"
#include "EGClipboard.h"

EGbool EGClipboard::setData(EGClipboardDataType dataType, const void *data, size_t dataLen)
{
    UIPasteboard *pasteBoard = [UIPasteboard generalPasteboard];
    switch (dataType) {
        case EGClipboardDataTypeTextUTF8: {
            [pasteBoard setValue:[NSString stringWithUTF8String:(const char*)data] forType:kUTTypeUTF8PlainText];
        }
        default:
            return false;
    }
}

EGbool EGClipboard::getData(EGClipboardDataType dataType, void *data, size_t bufLen, size_t *outLen)
{
    UIPasteboard *pasteBoard = [UIPasteboard generalPasteboard];
    switch (dataType) {
        case EGClipboardDataTypeTextUTF8: {
            NSString *nsstr = (NSString*)[pasteBoard valueForType:kUTTypeUTF8PlainText];
            if (outLen) *outLen = [nsstr length] + 1;
            if (data) [nsstr getCString:(char*)data maxLength:bufLen encoding:NSUTF8StringEncoding];
            return (nsstr != NULL);
        }
        default:
            return false;
    }
}

#endif
