// See LICENSE for license details.

#pragma once

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
