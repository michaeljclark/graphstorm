// See LICENSE for license details.

#pragma once

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <OpenCL/cl_gl_ext.h>
#else
#include <CL/cl.h>
#include <CL/cl_ext.h>
#include <CL/cl_gl.h>
#endif

class EGOpenCL;
typedef std::shared_ptr<EGOpenCL> EGOpenCLPtr;
class EGOpenCLPlatform;
typedef std::shared_ptr<EGOpenCLPlatform> EGOpenCLPlatformPtr;
typedef std::vector<EGOpenCLPlatformPtr> EGOpenCLPlatformList;
class EGOpenCLDevice;
typedef std::shared_ptr<EGOpenCLDevice> EGOpenCLDevicePtr;
typedef std::vector<EGOpenCLDevicePtr> EGOpenCLDeviceList;
class EGOpenCLContext;
typedef std::shared_ptr<EGOpenCLContext> EGOpenCLContextPtr;
typedef std::vector<EGOpenCLContextPtr> EGOpenCLContextList;
class EGOpenCLProgram;
typedef std::shared_ptr<EGOpenCLProgram> EGOpenCLProgramPtr;
typedef std::vector<EGOpenCLProgramPtr> EGOpenCLProgramList;
class EGOpenCLBuffer;
typedef std::shared_ptr<EGOpenCLBuffer> EGOpenCLBufferPtr;
class EGOpenCLKernel;
typedef std::shared_ptr<EGOpenCLKernel> EGOpenCLKernelPtr;
typedef std::map<std::string,EGOpenCLKernelPtr> EGOpenCLKernelMap;
class EGOpenCLEvent;
typedef std::shared_ptr<EGOpenCLEvent> EGOpenCLEventPtr;
class EGOpenCLCommandQueue;
typedef std::shared_ptr<EGOpenCLCommandQueue> EGOpenCLCommandQueuePtr;
typedef std::vector<EGOpenCLCommandQueuePtr> EGOpenCLCommandQueueList;
template <typename BufferType> class EGOpenCLBufferObject;


/* EGOpenCLPlatform */

class EGOpenCLPlatform
{
protected:
    friend class EGOpenCL;
    friend class EGOpenCLContext;
    
    std::set<std::string> extensionSet;

    cl_platform_id platformId;
    EGuint platformIndex;
    std::string name;
    std::string vendor;
    std::string version;
    std::string profile;
    std::string extensions;

    EGOpenCLPlatform(cl_platform_id platformId, EGuint platformIndex);

public:
    virtual ~EGOpenCLPlatform();
    
    void parseExtensions();
    EGbool hasExtension(std::string extname);

    cl_platform_id getPlatformId() { return platformId; }
    EGuint getPlatformIndex() { return platformIndex; }
    std::string getName() { return name; }
    std::string getVendor() { return vendor; }
    std::string getVersion() { return version; }
    std::string getProfile() { return profile; }
    std::string getExtensions() { return extensions; }

    void print();
};


/* EGOpenCLDevice */

class EGOpenCLDevice
{
protected:
    friend class EGOpenCL;
    friend class EGOpenCLPlatform;
    friend class EGOpenCLContext;
    friend class EGOpenCLProgram;
    friend class EGOpenCLCommandQueue;
    
    std::set<std::string> extensionSet;
    
    EGOpenCLPlatformPtr platform;
    cl_device_id deviceId;
    EGuint deviceIndex;
    cl_device_type deviceType;
    std::string name;
    std::string vendor;
    std::string deviceVersion;
    std::string driverVersion;
    std::string profile;
    std::string extensions;
    cl_bool available;
    cl_uint addressBits;
    cl_bool littleEndian;
    cl_device_fp_config floatSingle;
    cl_device_fp_config floatDouble;
    cl_ulong globalMemCacheSize;
    cl_device_mem_cache_type globalMemCacheType;
    cl_uint globalMemCacheLineSize;
    cl_ulong globalMemSize;
    cl_bool imageSupport;
    size_t image2DmaxWidth;
    size_t image2DmaxHeight;
    size_t image3DmaxWidth;
    size_t image3DmaxHeight;
    size_t image3DmaxDepth;
    cl_ulong localMemSize;
    cl_device_local_mem_type localMemType;
    cl_uint maxClockFrequency;
    cl_uint maxComputeUnits;
    cl_uint maxConstantArgs;
    cl_ulong maxConstantBufferSize;
    cl_ulong maxMemAllocSize;
    size_t maxParameterSize;
    cl_uint maxReadImageArgs;
    cl_uint maxWriteImageArgs;
    cl_uint maxSamplers;
    size_t maxWorkGroupSize;
    cl_uint maxWorkItemDim;
    std::vector<size_t> maxWorkItemSizes;
    cl_uint memBaseAddrAlign;
    cl_uint minDataAlignSize;
    cl_uint prefVecWidthChar;
    cl_uint prefVecWidthShort;
    cl_uint prefVecWidthInt;
    cl_uint prefVecWidthLong;
    cl_uint prefVecWidthFloat;
    cl_uint prefVecWidthDouble;
    size_t profilingTimerResolution;

    EGOpenCLDevice(EGOpenCLPlatformPtr platform, cl_device_id deviceId, EGuint deviceIndex);

public:
    virtual ~EGOpenCLDevice();
    
    void parseExtensions();
    EGbool hasExtension(std::string extname);

    std::string deviceTypeString();
    std::string globalMemCacheTypeString();
    std::string localMemTypeString();
    std::string fpconfigString(cl_device_fp_config fpConfig);
    std::string sizeArrayString(std::vector<size_t> &sizeArray);
    
    EGOpenCLPlatformPtr getPlatform() { return platform; }
    cl_device_id getDeviceId() { return deviceId; }
    EGuint getDeviceIndex() { return deviceIndex; }
    cl_device_type getDeviceType() { return deviceType; }
    std::string getName() { return name; }
    std::string getVendor() { return vendor; }
    std::string getDeviceVersion() { return deviceVersion; }
    std::string getDriverVersion() { return driverVersion; }
    std::string getProfile() { return profile; }
    std::string getExtensions() { return extensions; }
    cl_bool isAvailable() { return available; }
    cl_uint getAddressBits() { return addressBits; }
    cl_bool isLittleEndian() { return littleEndian; }
    cl_device_fp_config getFloatConfig() { return floatSingle; }
    cl_device_fp_config getDoubleConfig() { return floatDouble; }
    cl_ulong getGlobalMemCacheSize() { return globalMemCacheSize; }
    cl_device_mem_cache_type getGlobalMemCacheType() { return globalMemCacheType; }
    cl_uint getGlobalMemCacheLineSize() { return globalMemCacheLineSize; }
    cl_ulong getGlobalMemSize() { return globalMemSize; }
    cl_bool hasImageSupport() { return imageSupport; }
    size_t getImage2DmaxWidth() { return image2DmaxWidth; }
    size_t getImage2DmaxHeight() { return image2DmaxHeight; }
    size_t getImage3DmaxWidth() { return image3DmaxWidth; }
    size_t getImage3DmaxHeight() { return image3DmaxHeight; }
    size_t getImage3DmaxDepth() { return image3DmaxDepth; }
    cl_ulong getLocalMemSize() { return localMemSize; }
    cl_device_local_mem_type getLocalMemType() { return localMemType; }
    cl_uint getMaxClockFrequency() { return maxClockFrequency; }
    cl_uint getMaxComputeUnits() { return maxComputeUnits; }
    cl_uint getMaxConstantArgs() { return maxConstantArgs; }
    cl_ulong getMaxConstantBufferSize() { return maxConstantBufferSize; }
    cl_ulong getMaxMemAllocSize() { return maxMemAllocSize; }
    size_t getMaxParameterSize() { return maxParameterSize; }
    cl_uint getMaxReadImageArgs() { return maxReadImageArgs; }
    cl_uint getMaxWriteImageArgs() { return maxWriteImageArgs; }
    cl_uint getMaxSamplers() { return maxSamplers; }
    size_t getMaxWorkGroupSize() { return maxWorkGroupSize; }
    cl_uint getMaxWorkItemDim() { return maxWorkItemDim; }
    std::vector<size_t> getMaxWorkItemSizes() { return maxWorkItemSizes; }
    cl_uint getMemBaseAddrAlign() { return memBaseAddrAlign; }
    cl_uint getMinDataAlignSize() { return minDataAlignSize; }
    cl_uint getPrefVecWidthChar() { return prefVecWidthChar; }
    cl_uint getPrefVecWidthShort() { return prefVecWidthShort; }
    cl_uint getPrefVecWidthInt() { return prefVecWidthInt; }
    cl_uint getPrefVecWidthLong() { return prefVecWidthLong; }
    cl_uint getPrefVecWidthFloat() { return prefVecWidthFloat; }
    cl_uint getPrefVecWidthDouble() { return prefVecWidthDouble; }
    size_t getProfilingTimerResolution() { return profilingTimerResolution; }

    void print();
};


/* EGOpenCLContext */

class EGOpenCLContext
{
protected:
    friend class EGOpenCL;
    friend class EGOpenCLProgram;
    friend class EGOpenCLBuffer;
    friend class EGOpenCLCommandQueue;
    
    EGOpenCLDeviceList clDevices;
    cl_context clContext;
    EGbool glSharing;
    EGOpenCLCommandQueueList cmdqueues;
    std::map<std::string,EGOpenCLProgramPtr> programCache;
    
#ifdef _WIN32
    static void WINAPI notify(const char *errinfo, const void *private_info, size_t cb, void *user_data);
#else
    static void notify(const char *errinfo, const void *private_info, size_t cb, void *user_data);
#endif

    EGOpenCLContext(EGOpenCLDeviceList devices, EGbool requestGlSharing);

public:
    virtual ~EGOpenCLContext();
    
    EGbool glSharingIsEnabled() { return glSharing; }
    cl_context getContext() { return clContext; }
    EGOpenCLDeviceList& getDevices() { return clDevices; }
    EGOpenCLProgramPtr createProgram(EGResourcePtr src, std::string options = "");
    EGOpenCLCommandQueuePtr createCommandQueue(EGOpenCLDevicePtr device, cl_command_queue_properties properties = 0);
    EGOpenCLBufferPtr createBuffer(cl_mem_flags flags, size_t size, void *host_ptr);
    EGOpenCLBufferPtr createBufferFromGLBuffer(cl_mem_flags flags, EGuint glbuffer);
};


/* EGOpenCLProgram */

class EGOpenCLProgram
{
protected:
    friend class EGOpenCLContext;
    
    EGOpenCLContext *context;
    EGResourcePtr src;
    std::string options;
    cl_program clProgram;
    EGOpenCLKernelMap kernels;

    EGOpenCLProgram(EGOpenCLContext *context, EGResourcePtr src, std::string options);

public:
    virtual ~EGOpenCLProgram();

    EGOpenCLKernelPtr getKernel(std::string name);
};


/* EGOpenCLLocalMemParam */

struct EGOpenCLLocalMemParam
{
    size_t size;
    
    EGOpenCLLocalMemParam(size_t size) : size(size) {}
};


/* EGOpenCLParam */

struct EGOpenCLParam
{
    const size_t size;
    const void* param;
    
    EGOpenCLParam(const size_t size, const void* param) : size(size), param(param) {}
};


/* EGOpenCLKernel */

class EGOpenCLKernel
{
protected:
    friend class EGOpenCLProgram;
    friend class EGOpenCLCommandQueue;
    
    EGOpenCLProgram *program;
    cl_kernel clKernel;
    std::string name;

    EGOpenCLKernel(EGOpenCLProgram *program, cl_kernel clKernel, std::string name);

public:
    virtual ~EGOpenCLKernel();
    
    void setArg(cl_uint arg_index, EGOpenCLLocalMemParam mem_size);
    void setArg(cl_uint arg_index, EGOpenCLParam paramval);
    void setArg(cl_uint arg_index, cl_mem memval);
    void setArg(cl_uint arg_index, cl_int intval);
    void setArg(cl_uint arg_index, cl_float floatval);
    void setArg(cl_uint arg_index, cl_double doubleval);
    void setArg(cl_uint arg_index, EGOpenCLBufferPtr &buffer);
    
    template<typename T>
    void setArgsR(const cl_uint arg_index, T &&argv)
    {
        setArg(arg_index, std::forward<T>(argv));
    }

    template<typename T, typename... Args>
    void setArgsR(const cl_uint arg_index, T &&argv, Args&&... args)
    {
        setArgsR(arg_index, std::forward<T>(argv));
        setArgsR(arg_index + 1, args...);
    }

    template<typename... Args>
    void setArgs(Args&&... args)
    {
        setArgsR(0, args...);
    }
};


/* EGOpenCLBuffer */

class EGOpenCLBuffer
{
protected:
    friend class EGOpenCLContext;
    friend class EGOpenCLKernel;
    friend class EGOpenCLCommandQueue;
    
    EGOpenCLContext *context;
    cl_mem clBuffer;
    cl_mem_flags flags;
    size_t size;
  	void *host_ptr;
    
    EGOpenCLBuffer(cl_mem clBuffer);
    EGOpenCLBuffer(EGOpenCLContext *context, cl_mem_flags flags, size_t size, void *host_ptr);
    
public:
    virtual ~EGOpenCLBuffer();
    
    cl_mem_object_type getType();
    cl_mem_flags getFlags();
    size_t getSize();
};


/* EGOpenCLBufferList */

struct EGOpenCLBufferList : public std::vector<EGOpenCLBufferPtr>
{
    EGOpenCLBufferList() {}
    
    EGOpenCLBufferList(const EGOpenCLBufferPtr &buffer1)
    {
        push_back(buffer1);
    }
    
    EGOpenCLBufferList(const EGOpenCLBufferPtr &buffer1, const EGOpenCLBufferPtr &buffer2)
    {
        push_back(buffer1);
        push_back(buffer2);
    }

    EGOpenCLBufferList(const EGOpenCLBufferPtr &buffer1, const EGOpenCLBufferPtr &buffer2, const EGOpenCLBufferPtr &buffer3)
    {
        push_back(buffer1);
        push_back(buffer2);
        push_back(buffer3);
    }

    EGOpenCLBufferList(const EGOpenCLBufferPtr &buffer1, const EGOpenCLBufferPtr &buffer2, const EGOpenCLBufferPtr &buffer3, const EGOpenCLBufferPtr &buffer4)
    {
        push_back(buffer1);
        push_back(buffer2);
        push_back(buffer3);
        push_back(buffer4);
    }
};


/* EGOpenCLProfilingInfo */

struct EGOpenCLProfilingInfo
{
    cl_ulong queued;
    cl_ulong submit;
    cl_ulong start;
    cl_ulong end;
    
    EGOpenCLProfilingInfo(cl_ulong queued, cl_ulong submit, cl_ulong start, cl_ulong end);
    
    std::string toString();
};


/* EGOpenCLEvent */

class EGOpenCLEvent
{
protected:
    friend class EGOpenCLCommandQueue;
    
    cl_event clEvent;
    
    EGOpenCLEvent(cl_event clEvent);

public:
    virtual ~EGOpenCLEvent();
    
    void wait();
    
    EGOpenCLProfilingInfo getProfilingInfo();
    cl_int getExecutionStatus();
};


/* EGOpenCLEventList */

struct EGOpenCLEventList : public std::vector<EGOpenCLEventPtr>
{
    EGOpenCLEventList() {}

    EGOpenCLEventList(const EGOpenCLEventPtr &event1)
    {
        push_back(event1);
    }

    EGOpenCLEventList(const EGOpenCLEventPtr &event1, const EGOpenCLEventPtr &event2)
    {
        push_back(event1);
        push_back(event2);
    }
};


/* EGOpenCLDimension */

struct EGOpenCLDimension : public std::vector<size_t>
{
    EGOpenCLDimension() {}
    
    EGOpenCLDimension(const size_t dim1)
    {
        push_back(dim1);
    }
    
    EGOpenCLDimension(const size_t dim1, const size_t dim2)
    {
        push_back(dim1);
        push_back(dim2);
    }
};


/* EGOpenCLCommandQueue */

class EGOpenCLCommandQueue
{
protected:
    friend class EGOpenCLContext;
    
    EGOpenCLContext *context;
    EGOpenCLDevicePtr device;
    cl_command_queue_properties properties;
    cl_command_queue clCommandQueue;
    EGbool printProfilingInfo;

    EGOpenCLCommandQueue(EGOpenCLContext *context, EGOpenCLDevicePtr device, cl_command_queue_properties properties);

public:
    virtual ~EGOpenCLCommandQueue();
    
    void finish();
    EGOpenCLDevicePtr getDevice() { return device; }
    cl_command_queue getCommandQueue() { return clCommandQueue; }
    EGbool profilingIsEnabled() { return properties & CL_QUEUE_PROFILING_ENABLE; }
    void setPrintProfilingInfo(EGbool printProfilingInfo) { this->printProfilingInfo = printProfilingInfo; }
    EGOpenCLEventPtr enqueueTask(EGOpenCLKernelPtr &kernel, EGOpenCLEventList eventWaitList = EGOpenCLEventList());
    EGOpenCLEventPtr enqueueNDRangeKernel(EGOpenCLKernelPtr &kernel, const EGOpenCLDimension &globalWorkSize,
         const EGOpenCLDimension &localWorkSize, EGOpenCLEventList eventWaitList = EGOpenCLEventList());
    EGOpenCLEventPtr enqueueNDRangeKernel(EGOpenCLKernelPtr &kernel, const EGOpenCLDimension &globalWorkSize,
        EGOpenCLEventList eventWaitList = EGOpenCLEventList());
    EGOpenCLEventPtr enqueueReadBuffer(EGOpenCLBufferPtr &buffer, cl_bool blocking_read, size_t offset, size_t cb, void *ptr,
        EGOpenCLEventList eventWaitList = EGOpenCLEventList());
    EGOpenCLEventPtr enqueueWriteBuffer(EGOpenCLBufferPtr &buffer, cl_bool blocking_write, size_t offset, size_t cb, const void *ptr,
        EGOpenCLEventList eventWaitList = EGOpenCLEventList());
    EGOpenCLEventPtr enqueueAcquireGLObjects(EGOpenCLBufferList bufferList, EGOpenCLEventList eventWaitList = EGOpenCLEventList());
    EGOpenCLEventPtr enqueueReleaseGLObjects(EGOpenCLBufferList bufferList, EGOpenCLEventList eventWaitList = EGOpenCLEventList());

    template<typename BufferType>
    void copyBuffersToDevice(BufferType &buf);
    
    template<typename BufferType, typename... Args>
    void copyBuffersToDevice(BufferType &buf, Args&&... args);

    template<typename BufferType>
    void copyBuffersToHost(BufferType &buf);
    
    template<typename BufferType, typename... Args>
    void copyBuffersToHost(BufferType &buf, Args&&... args);
};


/* EGOpenCLBufferObject */

template <typename BufferType>
class EGOpenCLBufferObject
{
private:
    size_t size;
    cl_mem_flags flags;
    BufferType *host_buf;
    EGOpenCLBufferPtr cl_buf;
    
public:
    EGOpenCLBufferObject() : size(0), host_buf(nullptr), cl_buf() {}
    ~EGOpenCLBufferObject() { clear(); }
    
    void clear()
    {
        cl_buf = EGOpenCLBufferPtr();
        if (host_buf) {
            delete [] host_buf;
            host_buf = nullptr;
        }
    }

    const size_t getSize() const { return sizeof(BufferType) * size; }
    operator BufferType*() { return host_buf; }
    operator EGOpenCLBufferPtr&() { return cl_buf; }

    void setSize(EGOpenCLContextPtr clctx, size_t size, cl_mem_flags flags = CL_MEM_READ_WRITE)
    {
        if (host_buf && cl_buf && this->size == size && this->flags == flags) return;
        if (host_buf) delete [] host_buf;
        this->size = size;
        this->flags = flags;
        host_buf = new BufferType[size];
        cl_buf = clctx->createBuffer(flags, sizeof(BufferType) * size, NULL);
    }
    
    void copyToDevice(EGOpenCLCommandQueuePtr clcmdqueue)
    {
        clcmdqueue->enqueueWriteBuffer(cl_buf, true, 0, sizeof(BufferType) * size, host_buf);
    }

    void copyToHost(EGOpenCLCommandQueuePtr clcmdqueue)
    {
        clcmdqueue->enqueueReadBuffer(cl_buf, true, 0, sizeof(BufferType) * size, host_buf);
    }

    void copyToDevice(EGOpenCLCommandQueue* clcmdqueue)
    {
        clcmdqueue->enqueueWriteBuffer(cl_buf, true, 0, sizeof(BufferType) * size, host_buf);
    }
    
    void copyToHost(EGOpenCLCommandQueue* clcmdqueue)
    {
        clcmdqueue->enqueueReadBuffer(cl_buf, true, 0, sizeof(BufferType) * size, host_buf);
    }
};


/* EGOpenCLCommandQueue */

template<typename BufferType>
void EGOpenCLCommandQueue::copyBuffersToDevice(BufferType &buf)
{
    buf.copyToDevice(this);
}

template<typename BufferType, typename... Args>
void EGOpenCLCommandQueue::copyBuffersToDevice(BufferType &buf, Args&&... args)
{
    buf.copyToDevice(this);
    copyBuffersToDevice(args...);
}

template<typename BufferType>
void EGOpenCLCommandQueue::copyBuffersToHost(BufferType &buf)
{
    buf.copyToDevice(this);
}

template<typename BufferType, typename... Args>
void EGOpenCLCommandQueue::copyBuffersToHost(BufferType &buf, Args&&... args)
{
    buf.copyToHost(this);
    copyBuffersToHost(args...);
}


/* EGOpenCL */

class EGOpenCL
{
protected:
    friend class EGOpenCLPlatform;
    friend class EGOpenCLDevice;
    friend class EGOpenCLContext;
    
    EGOpenCLPlatformList platforms;
    EGOpenCLDeviceList devices;
    EGOpenCLContextList contexts;

    static EGbool getPlatformIDs(cl_platform_id* &platforms, cl_uint &num_platforms);
    static EGbool getPlatformInfo(cl_platform_id platform, cl_platform_info param_name, std::string &info);
    static EGbool getDeviceIDs(cl_platform_id platform, cl_device_type device_type, cl_device_id* &devices, cl_uint &num_devices);
    static EGbool getDeviceInfoType(cl_device_id device, cl_device_type &deviceType);
    static EGbool getDeviceInfoMemCacheType(cl_device_id device, cl_device_mem_cache_type &memCacheType);
    static EGbool getDeviceInfoLocalMemType(cl_device_id device, cl_device_local_mem_type &localMemType);
    static EGbool getDeviceInfoFPConfig(cl_device_id device, cl_device_info param_name, cl_device_fp_config &devicefpConfig);
    static EGbool getDeviceInfoBool(cl_device_id device, cl_device_info param_name, cl_bool &boolVal);
    static EGbool getDeviceInfoUint(cl_device_id device, cl_device_info param_name, cl_uint &uintVal);
    static EGbool getDeviceInfoUlong(cl_device_id device, cl_device_info param_name, cl_ulong &ulongVal);
    static EGbool getDeviceInfoSize(cl_device_id device, cl_device_info param_name, size_t &sizeVal);
    static EGbool getDeviceInfoSizeArray(cl_device_id device, cl_device_info param_name, std::vector<size_t> &sizeArray);
    static EGbool getDeviceInfoString(cl_device_id device, cl_device_info param_name, std::string &info);
    
public:
    EGOpenCL();
    virtual ~EGOpenCL();
    
    EGbool init();
    EGOpenCLPlatformList& getPlatforms() { return platforms; }
    EGOpenCLDeviceList& getDevices() { return devices; }
    EGOpenCLContextList& getContexts() { return contexts; }
    EGOpenCLDeviceList getDevices(std::string deviceMatch);
    EGOpenCLContextPtr createContext(std::string deviceMatch, EGbool requestGlSharing = false);
    EGOpenCLContextPtr createContext(EGOpenCLDeviceList deviceList, EGbool requestGlSharing = false);
};
