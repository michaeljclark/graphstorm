/*
 *  EGResource.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGTime.h"
#include "EGResource.h"
#include "EGZipFile.h"

#include <fcntl.h>
#include <sys/stat.h>

#if defined (_WIN32)
#include <io.h>
#include <Windows.h>
#include <Shlobj.h>
#include <KnownFolders.h>
#pragma comment(lib, "Shell32.lib")
#define stat _stat
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/mman.h>
#endif

#if defined (__APPLE__)
#include "NSStubs.h"
#include <sys/xattr.h>
#include <dlfcn.h>
#endif

static char class_name[] = "EGResource";

// global containing zipfiles mapped into the resource space
EGZipFileList EGResource::zipFiles;


/* EGResourceZipFile */

class EGResourceZipFile : public EGResource
{
private:
    EGZipFileEntryPtr zipFileEntry;
    EGoffset seekOffset;

public:
    EGResourceZipFile(EGZipFileEntryPtr zipFileEntry);
    ~EGResourceZipFile();
    
    EGbool open(EGint mode);
    EGoffset read(void *buf, EGsize buflen);
    const void* getBuffer();
    EGoffset getLength();
    EGint asFileDescriptor(EGoffset *outStart, EGoffset *outLength);
    EGoffset seek(EGoffset offset, int whence);
    void close();
};

EGResourceZipFile::EGResourceZipFile(EGZipFileEntryPtr zipFileEntry) : EGResource(zipFileEntry->filename), zipFileEntry(zipFileEntry), seekOffset(0) {}

EGResourceZipFile::~EGResourceZipFile() {}

EGbool EGResourceZipFile::open(EGint mode)
{
    if (zipFileEntry->mMethod != 0) {
        EGError("%s:%s cannot open compressed zip entry\n", class_name, __func__);
        return true;
    }
    
    EGResourcePtr zipRsrc = zipFileEntry->zipFile->getZipResource();
    return zipRsrc->open(0);
}

EGoffset EGResourceZipFile::read(void *buf, EGsize buflen)
{
    if (open(0)) {
        return -1;
    }
    
    EGResourcePtr zipRsrc = zipFileEntry->zipFile->getZipResource();
    const EGubyte *zipBuf = (const EGubyte *)zipRsrc->getBuffer();
    if (!zipBuf) {
        EGError("%s:%s couldn't get zip buffer\n", class_name, __func__);
        return -1;
    }
    
    EGoffset bytesRead = (EGoffset)buflen;
    if (bytesRead > (EGoffset)zipFileEntry->mUncompressedLength - seekOffset) {
        bytesRead = zipFileEntry->mUncompressedLength - seekOffset;
    }
    if (bytesRead < 0) {
        bytesRead = 0;
    }
    if (bytesRead > 0) {
        memcpy(buf, zipBuf + zipFileEntry->mOffset + seekOffset, bytesRead);
        seekOffset += bytesRead;
    }
    return bytesRead;
}

const void* EGResourceZipFile::getBuffer()
{
    if (zipFileEntry->mMethod != 0) {
        EGError("%s:%s cannot get buffer for compressed zip entry\n", class_name, __func__);
        return NULL;
    }
    
    EGResourcePtr zipRsrc = zipFileEntry->zipFile->getZipResource();
    const EGubyte *zipBuf = (const EGubyte *)zipRsrc->getBuffer();
    if (!zipBuf) {
        EGError("%s:%s couldn't get zip buffer\n", class_name, __func__);
        return NULL;
    }
    return (const void*)(zipBuf + zipFileEntry->mOffset);
}

EGoffset EGResourceZipFile::getLength()
{
    return zipFileEntry->mUncompressedLength;
}

EGint EGResourceZipFile::asFileDescriptor(EGoffset *outStart, EGoffset *outLength)
{
    if (zipFileEntry->mMethod != 0) {
        EGError("%s:%s cannot get file descriptor for compressed zip entry\n", class_name, __func__);
        return true;
    }
    
    EGResourcePtr zipRsrc = zipFileEntry->zipFile->getZipResource();
    EGoffset zipOutStart, zipOutLength;
    EGint fd = zipRsrc->asFileDescriptor(&zipOutStart, &zipOutLength);
    
    if (outStart) {
        *outStart = zipFileEntry->mOffset;
    }
    if (outLength) {
        *outLength = zipFileEntry->mUncompressedLength;
    }
    return fd;
}

EGoffset EGResourceZipFile::seek(EGoffset offset, int whence)
{
    switch(whence) {
        case 0: seekOffset = offset; break; /* SEEK_SET */
        case 1: seekOffset = seekOffset + offset; break; /* SEEK_CUR */
        case 2: seekOffset = zipFileEntry->mUncompressedLength + offset; break; /* SEEK_END */
    }
    if (seekOffset > (EGoffset)zipFileEntry->mUncompressedLength) {
        seekOffset = zipFileEntry->mUncompressedLength;
    }
    if (seekOffset < 0) {
        seekOffset = 0;
    }
    return seekOffset;
}

void EGResourceZipFile::close()
{
    EGResource::close();
    EGResourcePtr zipRsrc = zipFileEntry->zipFile->getZipResource();
    seekOffset = 0;
}


#if defined(_WIN32)

/* EGResourceWindowsFile */

class EGResourceWindowsFile : public EGResource
{
private:
    HANDLE fh;
    HANDLE fmaph;
    EGoffset length;
    void *mapaddr;

public:
    EGResourceWindowsFile(std::string path);
    ~EGResourceWindowsFile();

    EGbool open(EGint mode);
    EGoffset read(void *buf, EGsize buflen);
    const void* getBuffer();
    EGoffset getLength();
    EGint asFileDescriptor(EGoffset *outStart, EGoffset *outLength);
    EGoffset seek(EGoffset offset, int whence);
    void close();
};

EGResourceWindowsFile::EGResourceWindowsFile(std::string path) : EGResource(path), fh(INVALID_HANDLE_VALUE), fmaph(INVALID_HANDLE_VALUE), length(-1), mapaddr(NULL) {}

EGResourceWindowsFile::~EGResourceWindowsFile()
{
    close();
}

EGbool EGResourceWindowsFile::open(EGint mode)
{
    if (fh != INVALID_HANDLE_VALUE) {
        return false;
    }
    if ((fh = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
        errcode = GetLastError();
        EGError("%s:%s CreateFile: errcode=%d: %s\n", class_name, __func__, errcode, path.c_str());
        return true;
    }
    return false;
}

EGoffset EGResourceWindowsFile::read(void *buf, EGsize buflen)
{
    if (open(0)) {
        return -1;
    }
    DWORD bytesRead;
    if (!ReadFile(fh, buf, (DWORD)buflen, &bytesRead, NULL)) {
        errcode = GetLastError();
        EGError("%s:%s ReadFile: errcode=%d\n", class_name, __func__, errcode);
        return -1;
    }
    return bytesRead;
}

const void* EGResourceWindowsFile::getBuffer()
{
    if (mapaddr) {
        return mapaddr;
    }
    if (open(0)) {
        return NULL;
    }
    if (getLength() <= 0) {
        return NULL;
    }
    if ((fmaph = CreateFileMapping(fh, NULL, PAGE_READONLY, 0, 0, NULL)) == INVALID_HANDLE_VALUE) {
        errcode = GetLastError();
        EGError("%s:%s CreateFileMapping: errcode=%d\n", class_name, __func__, errcode);
        return NULL;
    }
    void *addr;
    if ((addr = MapViewOfFile(fmaph, FILE_MAP_READ, 0, 0, 0)) == NULL) {
        errcode = GetLastError();
        EGError("%s:%s MapViewOfFile: errcode=%d\n", class_name, __func__, errcode);
        return NULL;
    } else {
        EGDebug("%s:%s MapViewOfFile: file=%s addr=%p\n", class_name, __func__, getBasename().c_str(), addr);
        return (mapaddr = addr);
    }
}

EGoffset EGResourceWindowsFile::getLength()
{
    if (length != -1) {
        return length;
    }

    if (open(0)) {
        return -1;
    }

    struct _FILE_STANDARD_INFO finfo;
    if (!GetFileInformationByHandleEx(fh, FileStandardInfo, &finfo, sizeof(finfo))) {
        errcode = GetLastError();
        return -1;
    }

    return (length = finfo.EndOfFile.LowPart);
}

EGint EGResourceWindowsFile::asFileDescriptor(EGoffset *outStart, EGoffset *outLength)
{
    if (open(0)) {
        return -1;
    }
    if (outStart) {
        *outStart = 0;
    }
    if (outLength) {
        *outLength = getLength();
    }
    return _open_osfhandle((intptr_t)fh, _O_RDONLY);
}

EGoffset EGResourceWindowsFile::seek(EGoffset offset, int whence)
{
    if (open(0)) {
        return -1;
    }
    int moveMethod;
    switch(whence) {
        case 0: moveMethod = FILE_BEGIN;   break; /* SEEK_SET */
        case 1: moveMethod = FILE_CURRENT; break; /* SEEK_CUR */
        case 2: moveMethod = FILE_END;     break; /* SEEK_END */
    }

    EGoffset off = SetFilePointer(fh, offset, 0, moveMethod);
    if (off == INVALID_SET_FILE_POINTER) {
        errcode = GetLastError();
        return -1;
    }
    return off;
}

void EGResourceWindowsFile::close()
{
    EGResource::close();

    if (mapaddr) {
        if (!UnmapViewOfFile(mapaddr)) {
            errcode = GetLastError();
            EGError("%s:%s UnmapViewOfFile: errcode=%d\n", class_name, __func__, errcode);
        } else {
            EGDebug("%s:%s UnmapViewOfFile: file=%s\n", class_name, __func__, getBasename().c_str());
        }
        mapaddr = NULL;
    }

    if (fmaph != INVALID_HANDLE_VALUE) {
        CloseHandle(fmaph);
        fmaph = INVALID_HANDLE_VALUE;
    }
    
    if (fh != INVALID_HANDLE_VALUE) {
        CloseHandle(fh);
        fh = INVALID_HANDLE_VALUE;
        length = -1;
    }
}

#else

/* EGResourceUnixFile */

class EGResourceUnixFile : public EGResource
{
private:
    int fd;
    EGoffset length;
    void *mapaddr;

public:
    EGResourceUnixFile(std::string path);
    ~EGResourceUnixFile();

    EGbool open(EGint mode);
    EGoffset read(void *buf, EGsize buflen);
    const void* getBuffer();
    EGoffset getLength();
    EGint asFileDescriptor(EGoffset *outStart, EGoffset *outLength);
    EGoffset seek(EGoffset offset, int whence);
    void close();
};

EGResourceUnixFile::EGResourceUnixFile(std::string path) : EGResource(path), fd(-1), length(-1), mapaddr(NULL) {}

EGResourceUnixFile::~EGResourceUnixFile()
{
    close();
}

EGbool EGResourceUnixFile::open(EGint mode)
{
    if (fd != -1) {
        return false;
    }
    if ((fd = ::open(path.c_str(), O_RDONLY)) < 0) {
        errcode = errno;
        errmsg = strerror(errno);
        return true;
    }
    return false;
}

EGoffset EGResourceUnixFile::read(void *buf, EGsize buflen)
{
    if (open(0)) {
        return -1;
    }
    EGoffset bytesRead = ::read(fd, buf, buflen);
    if (bytesRead < 0) {
        errcode = errno;
        errmsg = strerror(errno);
    }
    return bytesRead;
}

const void* EGResourceUnixFile::getBuffer()
{
    if (mapaddr) {
        return mapaddr;
    }
    if (open(0)) {
        return NULL;
    }
    if (getLength() <= 0) {
        return NULL;
    }
    void *addr;
    if ((addr = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        EGError("%s:%s mmap: %s\n", class_name, __func__, strerror(errno));
        return NULL;
    } else {
        EGDebug("%s:%s mmap: file=%s addr=%p\n", class_name, __func__, getBasename().c_str(), addr);
        return (mapaddr = addr);
    }
}

EGoffset EGResourceUnixFile::getLength()
{
    if (length != -1) {
        return length;
    }

    if (open(0)) {
        return -1;
    }

    struct stat statbuf;
    if (fstat(fd, &statbuf) < 0) {
        return -1;
    }

    return (length = statbuf.st_size);
}

EGint EGResourceUnixFile::asFileDescriptor(EGoffset *outStart, EGoffset *outLength)
{
    if (open(0)) {
        return -1;
    }
    if (outStart) {
        *outStart = 0;
    }
    if (outLength) {
        *outLength = getLength();
    }
    return fd;
}

EGoffset EGResourceUnixFile::seek(EGoffset offset, int whence)
{
    if (open(0)) {
        return -1;
    }
    EGoffset off = lseek(fd, offset, whence);
    if (off < 0) {
        errcode = errno;
        errmsg = strerror(errno);
    }
    return off;
}

void EGResourceUnixFile::close()
{
    EGResource::close();

    if (mapaddr) {
        if (munmap(mapaddr, length) < 0) {
            EGError("%s:%s munmap %s\n", class_name, __func__, strerror(errno));
        } else {
            EGDebug("%s:%s munmap: file=%s\n", class_name, __func__, getBasename().c_str());
        }
        mapaddr = NULL;
    }
    
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
        length = -1;
    }
}

#endif


/* EGResource */

EGbool EGResource::useFilesystemPath = false;

EGResource::EGResource(std::string path) : sbuf(""), sbufOffset(0), path(path), errmsg("unknown"), errcode(0) {}

EGResource::~EGResource() {}

std::string EGResource::getPath()
{
    return path;
}

std::string EGResource::getBasename()
{
#if defined (_WIN32)
    size_t slashoffset = path.find_last_of('\\');
#else
    size_t slashoffset = path.find_last_of('/');
#endif
    if (slashoffset == std::string::npos) {
        return path;
    } else {
        return path.substr(slashoffset + 1);
    }
    return path;
}

std::string EGResource::getErrorMessage()
{
    return errmsg;
}

EGint EGResource::getErrorCode()
{
    return errcode;
}

EGbool EGResource::copyToPath(std::string destpath)
{
    EGint destfd;
    EGoffset bytesWritten = 0;
    EGoffset len;
    EGubyte buf[4096];

    EGoffset fileLength = getLength();
    if (fileLength == -1) {
        EGError("%s:%s error opening resource for copy: %s\n", class_name, __func__, getPath().c_str());
        return true;
    }

#if defined (_WIN32)
    if ((destfd = _open(destpath.c_str(), O_RDWR | O_CREAT, 0600)) < 0) {
#else
    if ((destfd = ::open(destpath.c_str(), O_RDWR | O_CREAT, 0600)) < 0) {
#endif
        errcode = errno;
        errmsg = strerror(errno);
        EGError("%s:%s error opening resource copy destination %s: %s\n", class_name, __func__, destpath.c_str(), errmsg.c_str());
        return true;
    }

    while ((len = read(buf, sizeof(buf))) > 0) {
#if defined (_WIN32)
        if (_write(destfd, buf, len) != len) {
#else
        if (write(destfd, buf, len) != len) {
#endif
            errcode = errno;
            errmsg = strerror(errno);
#if defined (_WIN32)
            _close(destfd);
#else
            ::close(destfd);
#endif
            remove(destpath.c_str());
            EGError("%s:%s short write during copy: %s -> %s\n",
                    class_name, __func__, path.c_str(), destpath.c_str());
            return true;
        }
        bytesWritten += len;
    }
#if defined (_WIN32)
    _close(destfd);
#else
    ::close(destfd);
#endif

    if (bytesWritten != fileLength) {
        errcode = EIO;
        errmsg = "incomplete copy";
        EGError("%s:%s incomplete copy: (length=%d written=%d) %s -> %s\n", class_name, __func__, fileLength, bytesWritten, path.c_str(), destpath.c_str());
        return true;
    }

    return false;
}

char* EGResource::readLine(char *buf, EGsize buflen)
{
    if (buflen == 0) return NULL;

    // periodically refill buffer
    if ((sbuf.size() - sbufOffset) < buflen) {
        EGoffset len;
        char rdbuf[4096];
        while((len = read(rdbuf, sizeof(rdbuf))) > 0) {
            sbuf.append(rdbuf, len);
            if (sbuf.size() - sbufOffset >= buflen) break;
        }
    }

    // return EOF if there is no data remaining
    if (sbuf.size() - sbufOffset <= 0) {
        sbuf.resize(0);
        sbufOffset = 0;
        return NULL;
    }

    // read a line from the buffer
    size_t nextOffset, sizeToCopy = 0;
    if ((nextOffset = sbuf.find_first_of("\r\n", sbufOffset)) != std::string::npos) {
        sizeToCopy = nextOffset - sbufOffset;
    } else {
        sizeToCopy = sbuf.size() - sbufOffset;
    }
    if (sizeToCopy > buflen - 1) {
        sizeToCopy = buflen - 1;
    }
    if (sizeToCopy > 0) {
        memcpy(buf, sbuf.data() + sbufOffset, sizeToCopy);
    }
    buf[sizeToCopy] = '\0';
    if (nextOffset + 1 < sbuf.size() && sbuf.data()[nextOffset] == '\r' && sbuf.data()[nextOffset + 1] == '\n') {
        sbufOffset += (sizeToCopy + 2);
    } else {
        sbufOffset += (sizeToCopy + 1);
    }

    // periodically trim buffer
    if (sbufOffset >= 4096 && sbufOffset <= sbuf.size()) {
        sbuf.erase(0, sbufOffset);
        sbufOffset = 0;
    }

    return buf;
}

void EGResource::close()
{
    sbuf.resize(0);
    sbufOffset = 0;
}

EGbool EGResource::dirExists(std::string dname)
{
    struct stat buf;
    if (stat(dname.c_str(), &buf)) {
        return false;
    }
    return ((buf.st_mode & S_IFDIR) != 0);
}

EGbool EGResource::fileExists(std::string fname)
{
    struct stat buf;
    if (stat(fname.c_str(), &buf)) {
        return false;
    }
    return ((buf.st_mode & S_IFREG) != 0);
}

EGbool EGResource::makeDir(std::string dname)
{
    if (dirExists(dname)) {
        return true;
    }
#if defined (_WIN32)
    if (!CreateDirectoryA(dname.c_str(), NULL)) {
        EGError("%s:%s *** Error: failed to make directory: %s\n", class_name, __func__, dname.c_str());
        return false;
    }
#else
    if (mkdir(dname.c_str(), 0777) < 0) {
        EGError("%s:%s *** Error: failed to make directory: %s\n", class_name, __func__, dname.c_str());
        return false;
    }
#endif
    else {
        EGDebug("%s:%s Made directory: %s\n", class_name, __func__, dname.c_str());
        return true;
    }
}

EGbool EGResource::listFiles(std::string dname, std::list<std::string> &files)
{
#if ! defined (_WIN32)
    DIR* dirp;
    struct dirent *dp;
    struct stat buf;
    
    if ((dirp = opendir(dname.c_str())) == NULL) {
        EGDebug("%s:%s *** Error: failed to open directory: %s\n", class_name, __func__, dname.c_str());
        return false;
    }
    while ((dp = readdir(dirp)) != NULL) {
        std::string fname = dname + std::string(DIR_SEPARATOR) + std::string(dp->d_name);
        if (!stat(fname.c_str(), &buf) && buf.st_mode & S_IFREG) {
            files.push_back(std::string(dp->d_name));
        }
    }
    closedir(dirp);
#endif
    return true;
}

EGResourcePtr EGResource::getFileAsResource(std::string filename)
{
#if defined (_WIN32)
    std::replace(filename.begin(), filename.end(), '/', '\\');
    return EGResourcePtr(new EGResourceWindowsFile(filename));
#else
    return EGResourcePtr(new EGResourceUnixFile(filename));
#endif
}

EGbool EGResource::zipIsRegistered(EGResourcePtr zipRsrc)
{
    for(EGZipFileList::iterator zli = zipFiles.begin(); zli != zipFiles.end(); zli++) {
        EGZipFilePtr zipFile = *zli;
        if (zipFile->getZipResource()->getPath() == zipRsrc->getPath()) {
            return true;
        }
    }
    return false;
}

EGbool EGResource::registerZip(EGResourcePtr zipRsrc)
{
    // check zipfile is not already registered
    if (zipIsRegistered(zipRsrc)) {
        EGDebug("%s:%s zip file %s already registered\n", zipRsrc->getPath().c_str());
        return false;
    }
    // register zip file
    EGZipFilePtr zipFile = EGZipFilePtr(new EGZipFile(zipRsrc));
    zipFiles.push_back(zipFile);
    return true;
}

EGResourcePtr EGResource::getZipResource(std::string rsrcpath)
{
    EGZipFileEntryPtr matchZipFileEntry;
    
    // walk through zip files and find the last matching entry
    for(EGZipFileList::iterator zli = zipFiles.begin(); zli != zipFiles.end(); zli++) {
        EGZipFilePtr zipFile = *zli;
        EGZipFileEntryPtr zipFileEntry = zipFile->getZipEntry(rsrcpath);
        if (zipFileEntry) {
            matchZipFileEntry = zipFileEntry;
        }
    }
    if (matchZipFileEntry) {
        return EGResourcePtr(new EGResourceZipFile(matchZipFileEntry));
    } else {
        return EGResourcePtr();
    }
}


#if defined (__APPLE__)

std::string EGResource::getPath(std::string rsrcpath)
{    
    if (useFilesystemPath) {
        return rsrcpath;
    }
    
    // Seperate the resource path into path components
    CFStringRef rsrcStringRef = CFStringCreateWithCString
        (kCFAllocatorDefault, rsrcpath.c_str(), kCFStringEncodingMacRoman);
    CFArrayRef components = CFStringCreateArrayBySeparatingStrings
        (kCFAllocatorDefault, rsrcStringRef, CFSTR("/"));
    
    // See if we have a bundle
    CFStringRef bundleName = NULL;
    CFMutableStringRef bundlePath = NULL;
    CFStringRef lastComponent = NULL;
    for (int i=0; i < CFArrayGetCount(components); i++) {
        CFStringRef pathcomp = (CFStringRef)CFArrayGetValueAtIndex(components, i);
        if (bundleName) {
            // Accumulate path after bundle
            CFStringAppend(bundlePath, CFSTR("/"));
            CFStringAppend(bundlePath, pathcomp);
        } else {
            // See if we have a bundle in our path
            if (CFStringHasSuffix(pathcomp, CFSTR(".bundle"))) {
                bundleName = pathcomp;
                bundlePath = CFStringCreateMutable(kCFAllocatorDefault, 0);
            } else if (i == CFArrayGetCount(components) - 1) {
                // otherwise stash lastComponent
                lastComponent = pathcomp;
            }
        }
    }
    
    // Get name and type of resource
    CFBundleRef mainBundle;
    CFURLRef fsURL;
    CFStringRef nameStringRef, typeStringRef, fsPathStringRef;
    
    if (bundleName) {
        CFRange result = CFStringFind(bundleName, CFSTR("."), kCFCompareBackwards);
        nameStringRef = CFStringCreateWithSubstring(kCFAllocatorDefault, bundleName,
                                                    CFRangeMake(0, result.location));
        typeStringRef = CFStringCreateWithSubstring(kCFAllocatorDefault, bundleName,
                                                    CFRangeMake(result.location + 1, CFStringGetLength(bundleName) - result.location - 1));        
    } else {
        CFRange result = CFStringFind(lastComponent, CFSTR("."), kCFCompareBackwards);
        nameStringRef = CFStringCreateWithSubstring(kCFAllocatorDefault, lastComponent,
                                                    CFRangeMake(0, result.location));
        typeStringRef = CFStringCreateWithSubstring(kCFAllocatorDefault, lastComponent,
                                                    CFRangeMake(result.location + 1, CFStringGetLength(lastComponent) - result.location - 1));        
    }

    // Create path to resource
    std::string filepath;
    mainBundle = CFBundleGetMainBundle();
    fsURL = CFBundleCopyResourceURL(mainBundle, nameStringRef, typeStringRef, NULL);
    if (fsURL) {
        fsPathStringRef = CFURLCopyFileSystemPath(fsURL, kCFURLPOSIXPathStyle);
        filepath = std::string(CFStringGetCStringPtr(fsPathStringRef, CFStringGetFastestEncoding(fsPathStringRef)));
        if (bundleName) {
            filepath += std::string(CFStringGetCStringPtr(bundlePath, CFStringGetFastestEncoding(bundlePath)));
        }
        CFRelease(fsURL);
        CFRelease(fsPathStringRef);
    }
    if (bundlePath) {
        CFRelease(bundlePath);
    }
    CFRelease(nameStringRef);
    CFRelease(typeStringRef);
    CFRelease(rsrcStringRef);
    CFRelease(components);
    
    return filepath;
}

EGResourcePtr EGResource::getResource(std::string rsrcpath)
{
    EGResourcePtr zipRsrc = getZipResource(rsrcpath);
    if (zipRsrc) {
        return zipRsrc;
    }
    std::string path = getPath(rsrcpath);
    return EGResourcePtr(new EGResourceUnixFile(path));
}

std::string EGResource::getTempDir()
{
    static std::string tempDir;
    if (tempDir.size() == 0) {
        return (tempDir = NSStubs::getTempDir());
    }
    return tempDir;
}

std::string EGResource::getHomeDir()
{
    static std::string homeDir;
    if (homeDir.size() == 0) {
        return (homeDir = NSStubs::getHomeDir());
    }
    return homeDir;
}

typedef Boolean (*funcptr_CFURLSetResourcePropertyForKey)(CFURLRef url, CFStringRef key, CFTypeRef propertyValue, CFErrorRef *error);

#elif defined (_WIN32)

std::string EGResource::getPath(std::string rsrcpath)
{
    char path[MAX_PATH];
    static std::string topDir;

    GetModuleFileNameA(NULL, path, MAX_PATH);

    // locate top directory by searching upwards from exe dir for presence of the 'Resources' directory
    if (!topDir.length()) {
        std::string moduleDir = path;
        size_t last = moduleDir.length();
        while (last >= 0) {
            size_t offset;
            if ((offset = moduleDir.find_last_of("/\\", last)) == std::string::npos) {
                break;
            }
            std::string dir = moduleDir.substr(0, offset);
            if (dirExists(dir + std::string(DIR_SEPARATOR) + std::string("Resources"))) {
                EGDebug("%s:%s found Resources directory in : %s\n", class_name, __func__, dir.c_str());
                topDir = dir;
                break;
            }
            EGDebug("%s:%s searching for Resources directory in: %s\n", class_name, __func__, dir.c_str());
            last = offset - 1;
        }
    }
    std::string finalPath = topDir + std::string(DIR_SEPARATOR) + rsrcpath;
    std::replace(finalPath.begin(), finalPath.end(), '/', '\\');
    return finalPath;
}

EGResourcePtr EGResource::getResource(std::string rsrcpath)
{
    EGResourcePtr zipRsrc = getZipResource(rsrcpath);
    if (zipRsrc) {
        return zipRsrc;
    }
    std::string path = getPath(rsrcpath);
    return EGResourcePtr(new EGResourceWindowsFile(path));
}

std::string EGResource::getTempDir()
{
    wchar_t wpath[MAX_PATH] = { 0 };
    char path[MAX_PATH*2];
    
    GetTempPathW(MAX_PATH, wpath);
    if (!WideCharToMultiByte(CP_UTF8, 0, wpath, -1, path, MAX_PATH*2, NULL, NULL)) {
        EGError("%s:%s WideCharToMultiByte failed\n", class_name, __func__);
        return std::string();
    }
    return path;
}

std::string EGResource::getHomeDir()
{
    PWSTR wpath;
    char path[MAX_PATH*2];
    if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &wpath) != S_OK) {
        EGError("%s:%s SHGetKnownFolderPath failed\n", class_name, __func__);
        return std::string();
    }
    if (!WideCharToMultiByte(CP_UTF8, 0, wpath, -1, path, MAX_PATH*2, NULL, NULL)) {
        EGError("%s:%s WideCharToMultiByte failed\n", class_name, __func__);
        return std::string();
    }
    return std::string(path);
}

#else

std::string EGResource::getPath(std::string rsrcpath)
{
#if __native_client__
    return std::string("/") + rsrcpath;
#else
    return std::string("./") + rsrcpath;
#endif
}

EGResourcePtr EGResource::getResource(std::string rsrcpath)
{
    EGResourcePtr zipRsrc = getZipResource(rsrcpath);
    if (zipRsrc) {
        return zipRsrc;
    }
    std::string path = getPath(rsrcpath);
    return EGResourcePtr(new EGResourceUnixFile(path));
}

std::string EGResource::getTempDir()
{
    return std::string("/tmp");
}

std::string EGResource::getHomeDir()
{
    return std::string(getenv("HOME"));
}
        
#endif
                       
std::string EGResource::getTempFile(std::string filename, std::string suffix)
{
    size_t npos;
    std::string tmpfilename;
    if ((npos = filename.find_last_of("/")) != std::string::npos) {
        tmpfilename = EGResource::getTempDir() + filename.substr(npos) + suffix;
    } else if ((npos = filename.find_last_of("\\")) != std::string::npos) {
        tmpfilename = EGResource::getTempDir() + filename.substr(npos) + suffix;
    } else {
        tmpfilename = EGResource::getTempDir() + std::string(DIR_SEPARATOR) + filename + suffix;
    }
    return tmpfilename;
}    
