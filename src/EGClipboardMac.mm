/*
 *  EGClipboardMac.mm
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#if defined (__APPLE__) && !defined (TARGET_OS_IPHONE)

#include "EG.h"
#include "EGClipboard.h"

#import <AppKit/NSPasteboard.h>
#import <AppKit/NSPasteboardItem.h>

EGbool EGClipboard::setData(EGClipboardDataType dataType, const void *data, size_t dataLen)
{
    NSPasteboard *pasteBoard = [NSPasteboard generalPasteboard];
    switch (dataType) {
        case EGClipboardDataTypeTextUTF8: {
            [pasteBoard declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:nil];
            return [pasteBoard setString:[NSString stringWithUTF8String:(const char*)data] forType:NSStringPboardType];
        }
        default:
            return false;
    }
}

EGbool EGClipboard::getData(EGClipboardDataType dataType, void *data, size_t bufLen, size_t *outLen)
{
    NSPasteboard *pasteBoard = [NSPasteboard generalPasteboard];
    switch (dataType) {
        case EGClipboardDataTypeTextUTF8: {
            NSString *nsstr = [pasteBoard stringForType:NSStringPboardType];
            if (outLen) *outLen = [nsstr length] + 1;
            if (data) [nsstr getCString:(char*)data maxLength:bufLen encoding:NSUTF8StringEncoding];
            return (nsstr != NULL);
        }
        default:
            return false;
    }
}

#endif
