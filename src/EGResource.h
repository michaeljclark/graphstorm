// See LICENSE for license details.

#pragma once

class EGResource;
typedef std::shared_ptr<EGResource> EGResourcePtr;
typedef std::vector<EGResourcePtr> EGResourceList;
class EGZipFile;
typedef std::shared_ptr<EGZipFile> EGZipFilePtr;
typedef std::vector<EGZipFilePtr> EGZipFileList;

/* EGResource */

class EGResource
{
private:
    static EGZipFileList zipFiles;
    
    std::string sbuf;
    size_t sbufOffset;

protected:
    std::string path;
    std::string errmsg;
    EGint errcode;

    EGResource(std::string path);

public:
    virtual ~EGResource();

    std::string getPath();
    std::string getBasename();
    std::string getErrorMessage();
    EGint getErrorCode();
    EGbool copyToPath(std::string destpath);
    char* readLine(char *buf, EGsize buflen);
    
    virtual EGbool open(EGint mode) = 0;
    virtual EGoffset read(void *buf, EGsize buflen) = 0;
    virtual const void* getBuffer() = 0;
    virtual EGoffset getLength() = 0;
    virtual EGint asFileDescriptor(EGoffset *outStart, EGoffset *outLength) = 0;
    virtual EGoffset seek(EGoffset offset, int whence) = 0;
    virtual void close();

    static EGbool useFilesystemPath;

    static EGbool dirExists(std::string dname);
    static EGbool fileExists(std::string fname);
    static EGbool makeDir(std::string dname);
    static EGbool listFiles(std::string dname, std::list<std::string> &files);

    static EGbool zipIsRegistered(EGResourcePtr zipRsrc);
    static EGbool registerZip(EGResourcePtr zipRsrc);
    static EGResourcePtr getZipResource(std::string rsrcpath);
    static std::string getPath(std::string rsrcpath);
    static EGResourcePtr getResource(std::string rsrcpath);
    static EGResourcePtr getFileAsResource(std::string filename);
    static std::string getTempDir();
    static std::string getHomeDir();
    static std::string getTempFile(std::string filename, std::string suffix);
};
