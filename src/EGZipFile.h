/*
 *  EGZipFile.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGZIPFILE_H_
#define EGZIPFILE_H_

class EGZipFile;
typedef std::shared_ptr<EGZipFile> EGZipFilePtr;
typedef std::vector<EGZipFilePtr> EGZipFileList;
struct EGZipFileEntry;
typedef std::shared_ptr<EGZipFileEntry> EGZipFileEntryPtr;
typedef std::map<EGstring,EGZipFileEntryPtr> EGZipFileEntryMap;


/* EGZipFileEntry */

struct EGZipFileEntry
{
    EGZipFile *zipFile;
    EGResourcePtr zipRsrc;
    std::string filename;
    EGushort mMethod;
    EGuint mWhenModified;
    EGuint mCRC32;
    EGuint mCompressedLength;
    EGuint mUncompressedLength;
    EGuint mLocalHdrOffset;
    EGuint mOffset;
    
    EGZipFileEntry(EGZipFile *zipFile, EGResourcePtr zipRsrc, std::string filename);
};


/* EGZipFile */

class EGZipFile
{
protected:
    EGResourcePtr zipRsrc;
    EGZipFileEntryMap zipMap;

    EGbool readIndex();

public:
    EGZipFile(EGResourcePtr zipRsrc);
    
    EGResourcePtr getZipResource();
    EGZipFileEntryPtr getZipEntry(EGstring filename);
};

#endif
