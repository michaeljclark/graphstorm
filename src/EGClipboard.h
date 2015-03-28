/*
 *  EGClipboard.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGClipboard_H
#define EGClipboard_H

enum EGClipboardDataType
{
    EGClipboardDataTypeTextUTF8,
};

class EGClipboard
{
public:
    static EGbool setData(EGClipboardDataType dataType, const void *data, size_t dataLen);
    static EGbool getData(EGClipboardDataType dataType, void *data, size_t bufLen, size_t *outLen);
};

#endif
