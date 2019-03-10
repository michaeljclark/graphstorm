// See LICENSE for license details.

#include "EG.h"
#include "EGResource.h"
#include "EGZipFile.h"


/*
 * portions derived from com.android.vending.expansion.zipfile.ZipResourceFile
 *
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


static const char class_name[] = "EGZipFile";

#if defined (__APPLE__)
#include <machine/endian.h>
#else
#if WIN32 || __native_client__
#define BYTE_ORDER 4321
#define LITTLE_ENDIAN 4321
#define BIG_ENDIAN 1234
#else
#include <endian.h>
#endif
#endif

#if BYTE_ORDER == LITTLE_ENDIAN

EGushort getUshortLE(const EGubyte *addr) { return *((const EGushort*)addr); }

EGuint getUintLE(const EGubyte *addr) { return *((const EGuint*)addr); }

#endif

#if BYTE_ORDER == BIG_ENDIAN

EGushort getUshortLE(const EGubyte *addr)
{
    EGushort i = *((const EGushort*)addr);
    return ((i & 0xff) << 8 | (i & 0xff00) >> 8);
}

EGuint getUintLE(const EGubyte *addr)
{
    EGuint i = *((const EGuint*)addr);
    return ((i & 0xff) << 24) + ((i & 0xff00) << 8) + ((i & 0xff0000) >> 8) + ((i >> 24) & 0xff);
}

#endif

/* EGZipFileEntry */

EGZipFileEntry::EGZipFileEntry(EGZipFile *zipFile, EGResourcePtr zipRsrc, std::string filename)
    : zipFile(zipFile), zipRsrc(zipRsrc), filename(filename) {}

/* EGZipFile */

static const EGbool debug = false;

static const EGuint kEOCDSignature = 0x06054b50;
static const EGuint kEOCDLen = 22;
static const EGuint kEOCDNumEntries = 8; // offset to #of entries in file
static const EGuint kEOCDSize = 12; // size of the central directory
static const EGuint kEOCDFileOffset = 16; // offset to central directory

static const EGuint kMaxCommentLen = 65535; // longest possible in ushort
static const EGuint kMaxEOCDSearch = (kMaxCommentLen + kEOCDLen);

static const EGuint kLFHSignature = 0x04034b50;
static const EGuint kLFHLen = 30; // excluding variable-len fields
static const EGuint kLFHNameLen = 26; // offset to filename length
static const EGuint kLFHExtraLen = 28; // offset to extra length

static const EGuint kCDESignature = 0x02014b50;
static const EGuint kCDELen = 46; // excluding variable-len fields
static const EGuint kCDEMethod = 10; // offset to compression method
static const EGuint kCDEModWhen = 12; // offset to modification timestamp
static const EGuint kCDECRC = 16; // offset to entry CRC
static const EGuint kCDECompLen = 20; // offset to compressed length
static const EGuint kCDEUncompLen = 24; // offset to uncompressed length
static const EGuint kCDENameLen = 28; // offset to filename length
static const EGuint kCDEExtraLen = 30; // offset to extra length
static const EGuint kCDECommentLen = 32; // offset to comment length
static const EGuint kCDELocalOffset = 42; // offset to local hdr

static const EGuint kCompressStored = 0; // no compression
static const EGuint kCompressDeflated = 8; // standard deflate

EGZipFile::EGZipFile(EGResourcePtr zipRsrc) : zipRsrc(zipRsrc)
{
    if (readIndex()) {
        EGDebug("%s:%s read %d entries from zip %s\n", class_name, __func__, (EGint)zipMap.size(), zipRsrc->getBasename().c_str());
    } else {
        EGError("%s:%s failed to read entries from zip %s\n", class_name, __func__, zipRsrc->getBasename().c_str());
    }
}

EGResourcePtr EGZipFile::getZipResource()
{
    return zipRsrc;
}

EGZipFileEntryPtr EGZipFile::getZipEntry(EGstring filename)
{
    EGZipFileEntryMap::iterator zei = zipMap.find(filename);
    if (zei != zipMap.end()) {
        return (*zei).second;
    } else {
        return EGZipFileEntryPtr();
    }
}

EGbool EGZipFile::readIndex()
{
    // get zip file length
    EGoffset fileLength = zipRsrc->getLength();
    if (fileLength < kEOCDLen) {
        EGError("%s:%s invalid get zip length\n", class_name, __func__);
        return false;
    }
    
    // map buffer for zip file
    const EGubyte *buf = (const EGubyte*)zipRsrc->getBuffer();
    if (!buf) {
        EGError("%s:%s couldn't map zip buffer\n", class_name, __func__);
        return false;
    }
    
    // read zip header
    EGuint header = getUintLE(buf);
    if (header == kEOCDSignature) {
        EGError("%s:%s empty zip archive\n", class_name, __func__);
        return false;
    } else if (header != kLFHSignature) {
        EGError("%s:%s not a zip archive\n", class_name, __func__);
        return false;
    }
    
    /*
     * Perform the traditional EOCD snipe hunt. We're searching for the End
     * of Central Directory magic number, which appears at the start of the
     * EOCD block. It's followed by 18 bytes of EOCD stuff and up to 64KB of
     * archive comment. We need to read the last part of the file into a
     * buffer, dig through it to find the magic number, parse some values
     * out, and use those to determine the extent of the CD. We start by
     * pulling in the last part of the file.
     */
    const EGuint searchLen = (kMaxEOCDSearch > fileLength) ? (EGuint)fileLength : kMaxEOCDSearch;
    const EGubyte *searchBuf = buf + fileLength - searchLen;

    // EOCD == 0x50, 0x4b, 0x05, 0x06
    EGint eocdIdx;
    for (eocdIdx = searchLen - kEOCDLen; eocdIdx >= 0; eocdIdx--) {
        if (*(searchBuf + eocdIdx) == 0x50 && getUintLE(searchBuf + eocdIdx) == kEOCDSignature) {
            break;
        }
    }
    if (eocdIdx < 0) {
        EGError("%s:%s EOCD not found: %s is not a zip\n", class_name, __func__, zipRsrc->getBasename().c_str());
        return false;
    }
    
    /*
     * Grab the CD offset and size, and the number of entries in the
     * archive. After that, we can release our EOCD hunt buffer.
     */
    EGushort numEntries = getUshortLE(searchBuf + eocdIdx + kEOCDNumEntries);
    EGuint dirSize = getUintLE(searchBuf + eocdIdx + kEOCDSize);
    EGuint dirOffset = getUintLE(searchBuf + eocdIdx + kEOCDFileOffset);

    // Verify that they look reasonable.
    if (dirOffset + dirSize > (EGuint)fileLength) {
        EGError("%s:%s bad offsets (dirOffset=%u, dirSize=%u, eocdIdx=%d)\n", class_name, __func__, dirOffset, dirSize, eocdIdx);
        return false;
    }
    if (numEntries == 0) {
        EGError("%s:%s zero entries in central directory\n", class_name, __func__);
        return false;
    }
    if (debug) {
        EGDebug("%s:%s numEntries=%u dirSize=%u dirOffset=%u\n", class_name, __func__, numEntries, dirSize, dirOffset);
    }
    
    /*
     * Walk through the central directory, adding entries to the hash table.
     */
    
    EGuint currentOffset = 0;
    const EGubyte *directoryMap = buf + dirOffset;
    for (int i = 0; i < numEntries; i++) {
        if (getUintLE(directoryMap + currentOffset) != kCDESignature) {
            EGError("%s:%s missed a central dir sig at offset=%u\n", class_name, __func__, currentOffset);
            return false;
        }
        
        // useful stuff from the directory entry
        EGushort nameLen = getUshortLE(directoryMap + currentOffset + kCDENameLen);
        EGushort extraLen = getUshortLE(directoryMap + currentOffset + kCDEExtraLen);
        EGushort commentLen = getUshortLE(directoryMap + currentOffset + kCDECommentLen);

        // get the CDE filename
        std::string filename((const char*)(directoryMap + currentOffset + kCDELen), nameLen);
        
        // create the zip entry
        EGZipFileEntryPtr ze(new EGZipFileEntry(this, zipRsrc, filename));
        ze->mMethod = getUshortLE(directoryMap + currentOffset + kCDEMethod);
        ze->mWhenModified = getUintLE(directoryMap + currentOffset + kCDEModWhen);
        ze->mCRC32 = getUintLE(directoryMap + currentOffset + kCDECRC);
        ze->mCompressedLength = getUintLE(directoryMap + currentOffset + kCDECompLen);
        ze->mUncompressedLength = getUintLE(directoryMap + currentOffset + kCDEUncompLen);
        ze->mLocalHdrOffset = getUintLE(directoryMap + currentOffset + kCDELocalOffset);
        
        // get the data offset
        if (getUintLE(buf + ze->mLocalHdrOffset) != kLFHSignature) {
            EGError("%s:%s didn't find signature at start of lfh offset=%u\n", class_name, __func__, ze->mLocalHdrOffset);
            return false;
        }
        EGushort lfhNameLen = getUshortLE(buf + ze->mLocalHdrOffset + kLFHNameLen);
        EGushort lfhExtraLen = getUshortLE(buf + ze->mLocalHdrOffset + kLFHExtraLen);
        ze->mOffset = ze->mLocalHdrOffset + kLFHLen + lfhNameLen + lfhExtraLen;
        
        // put file into hash (exclude directories)
        if (filename.find_last_of('/') != filename.length() - 1) {
            zipMap.insert(std::pair<std::string,EGZipFileEntryPtr>(ze->filename, ze));
            if (debug) {
                EGDebug("%s:%s zip entry: filename=%-80s method=%hu modified=%u crc=%p compLen=%u uncompLen=%u headerOffset=%u offset=%u\n",
                        class_name, __func__,
                        ze->filename.c_str(),
                        ze->mMethod,
                        ze->mWhenModified,
                        ze->mCRC32,
                        ze->mCompressedLength,
                        ze->mUncompressedLength,
                        ze->mLocalHdrOffset,
                        ze->mOffset);
            }
        }
    
        // go to next directory entry
        currentOffset += kCDELen + nameLen + extraLen + commentLen;
    }

    return true;
}
