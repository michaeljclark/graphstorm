/*
 *  EGOpenCL.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include "EG.h"
#include "EGResource.h"

#include "EGOpenCL.h"

#if defined(__unix__) || defined(__linux__) || defined(__FreeBSD__) || \
    defined(__NetBSD__) || defined(__DragonFly__) || defined(__OpenBSD__)
#include <GL/glx.h>
#endif

static const char class_name[] = "EGOpenCL";

/* EGOpenCLPlatform */
 
EGOpenCLPlatform::EGOpenCLPlatform(cl_platform_id platformId, EGuint platformIndex)
    : platformId(platformId), platformIndex(platformIndex)
{
    EGOpenCL::getPlatformInfo(platformId, CL_PLATFORM_NAME, name);
    EGOpenCL::getPlatformInfo(platformId, CL_PLATFORM_VENDOR, vendor);
    EGOpenCL::getPlatformInfo(platformId, CL_PLATFORM_VERSION, version);
    EGOpenCL::getPlatformInfo(platformId, CL_PLATFORM_PROFILE, profile);
    EGOpenCL::getPlatformInfo(platformId, CL_PLATFORM_EXTENSIONS, extensions);
    
    parseExtensions();
}

EGOpenCLPlatform::~EGOpenCLPlatform() {}

void EGOpenCLPlatform::parseExtensions()
{
    size_t current;
    size_t next = -1;
    do {
        current = next + 1;
        next = extensions.find_first_of(" ", current);
        std::string extname = extensions.substr(current, next - current);
        std::transform(extname.begin(), extname.end(), extname.begin(), ::tolower);
        extensionSet.insert(extname);
    } while (next != std::string::npos);
}

EGbool EGOpenCLPlatform::hasExtension(std::string extname)
{
    std::transform(extname.begin(), extname.end(), extname.begin(), ::tolower);
    return (extensionSet.find(extname) != extensionSet.end());
}

void EGOpenCLPlatform::print()
{
    EGDebug("platform[%u].name                = %s\n", platformIndex, name.c_str());
    EGDebug("platform[%u].vendor              = %s\n", platformIndex, vendor.c_str());
    EGDebug("platform[%u].profile             = %s\n", platformIndex, profile.c_str());
    EGDebug("platform[%u].extensions          = %s\n", platformIndex, extensions.c_str());
    EGDebug("platform[%u].version             = %s\n", platformIndex, version.c_str());
}


/* EGOpenCLDevice */

EGOpenCLDevice::EGOpenCLDevice(EGOpenCLPlatformPtr platform, cl_device_id deviceId, EGuint deviceIndex)
    : platform(platform), deviceId(deviceId), deviceIndex(deviceIndex)
{
    EGOpenCL::getDeviceInfoType(deviceId, deviceType);
    EGOpenCL::getDeviceInfoString(deviceId, CL_DEVICE_NAME, name);
    EGOpenCL::getDeviceInfoString(deviceId, CL_DEVICE_VENDOR, vendor);
    EGOpenCL::getDeviceInfoString(deviceId, CL_DEVICE_VERSION, deviceVersion);
    EGOpenCL::getDeviceInfoString(deviceId, CL_DRIVER_VERSION, driverVersion);
    EGOpenCL::getDeviceInfoString(deviceId, CL_DEVICE_PROFILE, profile);
    EGOpenCL::getDeviceInfoString(deviceId, CL_DEVICE_EXTENSIONS, extensions);
    EGOpenCL::getDeviceInfoSize(deviceId, CL_DEVICE_PROFILING_TIMER_RESOLUTION, profilingTimerResolution);
    EGOpenCL::getDeviceInfoBool(deviceId, CL_DEVICE_AVAILABLE, available);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_ADDRESS_BITS, addressBits);
    EGOpenCL::getDeviceInfoBool(deviceId, CL_DEVICE_ENDIAN_LITTLE, littleEndian);
    EGOpenCL::getDeviceInfoFPConfig(deviceId, CL_DEVICE_SINGLE_FP_CONFIG, floatSingle);
    EGOpenCL::getDeviceInfoFPConfig(deviceId, CL_DEVICE_DOUBLE_FP_CONFIG, floatDouble);
    EGOpenCL::getDeviceInfoUlong(deviceId, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, globalMemCacheSize);
    EGOpenCL::getDeviceInfoMemCacheType(deviceId, globalMemCacheType);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, globalMemCacheLineSize);
    EGOpenCL::getDeviceInfoUlong(deviceId, CL_DEVICE_GLOBAL_MEM_SIZE, globalMemSize);
    EGOpenCL::getDeviceInfoBool(deviceId, CL_DEVICE_IMAGE_SUPPORT, imageSupport);
    EGOpenCL::getDeviceInfoSize(deviceId, CL_DEVICE_IMAGE2D_MAX_WIDTH, image2DmaxWidth);
    EGOpenCL::getDeviceInfoSize(deviceId, CL_DEVICE_IMAGE2D_MAX_HEIGHT, image2DmaxHeight);
    EGOpenCL::getDeviceInfoSize(deviceId, CL_DEVICE_IMAGE3D_MAX_WIDTH, image3DmaxWidth);
    EGOpenCL::getDeviceInfoSize(deviceId, CL_DEVICE_IMAGE3D_MAX_HEIGHT, image3DmaxHeight);
    EGOpenCL::getDeviceInfoSize(deviceId, CL_DEVICE_IMAGE3D_MAX_DEPTH, image3DmaxDepth);
    EGOpenCL::getDeviceInfoUlong(deviceId, CL_DEVICE_LOCAL_MEM_SIZE, localMemSize);
    EGOpenCL::getDeviceInfoLocalMemType(deviceId, localMemType);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_CLOCK_FREQUENCY, maxClockFrequency);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_COMPUTE_UNITS, maxComputeUnits);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_CONSTANT_ARGS, maxConstantArgs);
    EGOpenCL::getDeviceInfoUlong(deviceId, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, maxConstantBufferSize);
    EGOpenCL::getDeviceInfoUlong(deviceId, CL_DEVICE_MAX_MEM_ALLOC_SIZE, maxMemAllocSize);
    EGOpenCL::getDeviceInfoSize(deviceId, CL_DEVICE_MAX_PARAMETER_SIZE, maxParameterSize);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_READ_IMAGE_ARGS, maxReadImageArgs);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, maxWriteImageArgs);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_SAMPLERS, maxSamplers);
    EGOpenCL::getDeviceInfoSize(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, maxWorkGroupSize);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, maxWorkItemDim);
    EGOpenCL::getDeviceInfoSizeArray(deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, maxWorkItemSizes);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_MEM_BASE_ADDR_ALIGN, memBaseAddrAlign);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, minDataAlignSize);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, prefVecWidthChar);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, prefVecWidthShort);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, prefVecWidthInt);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, prefVecWidthLong);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, prefVecWidthFloat);
    EGOpenCL::getDeviceInfoUint(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, prefVecWidthDouble);
    
    parseExtensions();
}

EGOpenCLDevice::~EGOpenCLDevice() {}

void EGOpenCLDevice::parseExtensions()
{
    size_t current;
    size_t next = -1;
    do {
        current = next + 1;
        next = extensions.find_first_of(" ", current);
        std::string extname = extensions.substr(current, next - current);
        std::transform(extname.begin(), extname.end(), extname.begin(), ::tolower);
        extensionSet.insert(extname);
    } while (next != std::string::npos);
}

EGbool EGOpenCLDevice::hasExtension(std::string extname)
{
    std::transform(extname.begin(), extname.end(), extname.begin(), ::tolower);
    return (extensionSet.find(extname) != extensionSet.end());
}

std::string EGOpenCLDevice::deviceTypeString()
{
    switch (deviceType) {
        case CL_DEVICE_TYPE_CPU: return "CL_DEVICE_TYPE_CPU";
        case CL_DEVICE_TYPE_GPU: return "CL_DEVICE_TYPE_GPU";
        case CL_DEVICE_TYPE_ACCELERATOR: return "CL_DEVICE_TYPE_ACCELERATOR";
        case CL_DEVICE_TYPE_DEFAULT: return "CL_DEVICE_TYPE_DEFAULT";
        default: return "UNKNOWN";
    }
}

std::string EGOpenCLDevice::globalMemCacheTypeString()
{
    switch (globalMemCacheType) {
        case CL_NONE: return "CL_NONE";
        case CL_READ_ONLY_CACHE: return "CL_READ_ONLY_CACHE";
        case CL_READ_WRITE_CACHE: return "CL_READ_WRITE_CACHE";
        default: return "UNKNOWN";
    }
}

std::string EGOpenCLDevice::localMemTypeString()
{
    switch (localMemType) {
        case CL_LOCAL: return "CL_LOCAL";
        case CL_GLOBAL: return "CL_GLOBAL";
        default: return "UNKNOWN";
    }
}

std::string EGOpenCLDevice::fpconfigString(cl_device_fp_config fpConfig)
{
    std::stringstream ss;
    if (fpConfig & CL_FP_DENORM) ss << "+CL_FP_DENORM";
    if (fpConfig & CL_FP_INF_NAN) ss << "+CL_FP_INF_NAN";
    if (fpConfig & CL_FP_ROUND_TO_NEAREST) ss << "+CL_FP_ROUND_TO_NEAREST";
    if (fpConfig & CL_FP_ROUND_TO_ZERO) ss << "+CL_FP_ROUND_TO_ZERO";
    if (fpConfig & CL_FP_FMA) ss << "+CL_FP_FMA ";
    return ss.str();
}

std::string EGOpenCLDevice::sizeArrayString(std::vector<size_t> &sizeArray)
{
    std::stringstream ss;
    ss << "[";
    for (std::vector<size_t>::iterator si = sizeArray.begin(); si != sizeArray.end(); si++) {
        if (si != sizeArray.begin()) ss << ",";
        ss << *si;
    }
    ss << "]";
    return ss.str();
}

void EGOpenCLDevice::print()
{
    EGDebug("device[%u].name                     = %s\n", deviceIndex, name.c_str());
    EGDebug("device[%u].vendor                   = %s\n", deviceIndex, vendor.c_str());
    EGDebug("device[%u].type                     = %s\n", deviceIndex, deviceTypeString().c_str());
    EGDebug("device[%u].profile                  = %s\n", deviceIndex, profile.c_str());
    EGDebug("device[%u].extensions               = %s\n", deviceIndex, extensions.c_str());
    EGDebug("device[%u].deviceVersion            = %s\n", deviceIndex, deviceVersion.c_str());
    EGDebug("device[%u].driverVersion            = %s\n", deviceIndex, driverVersion.c_str());
    EGDebug("device[%u].available                = %s\n", deviceIndex, available ? "TRUE" : "FALSE");
    EGDebug("device[%u].addressBits              = %u\n", deviceIndex, addressBits);
    EGDebug("device[%u].littleEndian             = %s\n", deviceIndex, available ? "TRUE" : "FALSE");
    EGDebug("device[%u].floatSingle              = %s\n", deviceIndex, fpconfigString(floatSingle).c_str());
    EGDebug("device[%u].floatDouble              = %s\n", deviceIndex, fpconfigString(floatDouble).c_str());
    EGDebug("device[%u].globalMemCacheSize       = %lu\n", deviceIndex, globalMemCacheSize);
    EGDebug("device[%u].globalMemCacheType       = %s\n", deviceIndex, globalMemCacheTypeString().c_str());
    EGDebug("device[%u].globamMemCacheLineSize   = %u\n", deviceIndex, globalMemCacheLineSize);
    EGDebug("device[%u].globalMemSize            = %lu\n", deviceIndex, globalMemSize);
    EGDebug("device[%u].imageSupport             = %s\n", deviceIndex, imageSupport ? "TRUE" : "FALSE");
    EGDebug("device[%u].image2DmaxWidth          = %u\n", deviceIndex, (EGuint)image2DmaxWidth);
    EGDebug("device[%u].image2DmaxHeight         = %u\n", deviceIndex, (EGuint)image2DmaxHeight);
    EGDebug("device[%u].image3DmaxWidth          = %u\n", deviceIndex, (EGuint)image3DmaxWidth);
    EGDebug("device[%u].image3DmaxHeight         = %u\n", deviceIndex, (EGuint)image3DmaxHeight);
    EGDebug("device[%u].image3DmaxDepth          = %u\n", deviceIndex, (EGuint)image3DmaxDepth);
    EGDebug("device[%u].localMemSize             = %lu\n", deviceIndex, localMemSize);
    EGDebug("device[%u].localMemType             = %s\n", deviceIndex, localMemTypeString().c_str());
    EGDebug("device[%u].maxClockFrequency        = %u\n", deviceIndex, maxClockFrequency);
    EGDebug("device[%u].maxComputeUnits          = %u\n", deviceIndex, maxComputeUnits);
    EGDebug("device[%u].maxConstantArgs          = %u\n", deviceIndex, maxConstantArgs);
    EGDebug("device[%u].maxConstantBufferSize    = %lu\n", deviceIndex, maxConstantBufferSize);
    EGDebug("device[%u].maxMemAllocSize          = %lu\n", deviceIndex, maxMemAllocSize);
    EGDebug("device[%u].maxParameterSize         = %u\n", deviceIndex, (EGuint)maxParameterSize);
    EGDebug("device[%u].maxReadImageArgs         = %u\n", deviceIndex, maxReadImageArgs);
    EGDebug("device[%u].maxWriteImageArgs        = %u\n", deviceIndex, maxWriteImageArgs);
    EGDebug("device[%u].maxSamplers              = %u\n", deviceIndex, maxSamplers);
    EGDebug("device[%u].maxWorkGroupSize         = %u\n", deviceIndex, (EGuint)maxWorkGroupSize);
    EGDebug("device[%u].maxWorkItemDim           = %u\n", deviceIndex, maxWorkItemDim);
    EGDebug("device[%u].maxWorkItemSizes         = %s\n", deviceIndex, sizeArrayString(maxWorkItemSizes).c_str());
    EGDebug("device[%u].memBaseAddrAlign         = %u\n", deviceIndex, memBaseAddrAlign);
    EGDebug("device[%u].minDataAlignSize         = %u\n", deviceIndex, minDataAlignSize);
    EGDebug("device[%u].prefVecWidthChar         = %u\n", deviceIndex, prefVecWidthChar);
    EGDebug("device[%u].prefVecWidthShort        = %u\n", deviceIndex, prefVecWidthShort);
    EGDebug("device[%u].prefVecWidthInt          = %u\n", deviceIndex, prefVecWidthInt);
    EGDebug("device[%u].prefVecWidthLong         = %u\n", deviceIndex, prefVecWidthLong);
    EGDebug("device[%u].prefVecWidthFloat        = %u\n", deviceIndex, prefVecWidthFloat);
    EGDebug("device[%u].prefVecWidthDouble       = %u\n", deviceIndex, prefVecWidthDouble);
    EGDebug("device[%u].profilingTimerResolution = %u\n", deviceIndex, (EGuint)profilingTimerResolution);
}


/* EGOpenCLContext */

void EGOpenCLContext::notify(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
    EGError("%s:%s %s\n", class_name, __func__, errinfo);
}

#if !defined(__APPLE__)
typedef cl_int (*clGetGLContextInfoKHR_func)(const cl_context_properties * /* properties */,
                      cl_gl_context_info            /* param_name */,
                      size_t                        /* param_value_size */,
                      void *                        /* param_value */,
                      size_t *                      /* param_value_size_ret */);
#endif


EGOpenCLContext::EGOpenCLContext(EGOpenCLDeviceList devices, EGbool requestGlSharing) : clDevices(devices), clContext(0)
{
 	cl_int ret;

    if (devices.size() == 0) return;
    
    EGbool hasSharing = false;
    cl_device_id *cl_device_arr = new cl_device_id[devices.size()];
    EGOpenCLPlatformPtr platform;
    for (size_t i = 0; i < devices.size(); i++) {
        if (!platform) platform = devices[i]->platform;
        hasSharing |= (devices[i]->hasExtension("cl_apple_gl_sharing") || devices[i]->hasExtension("cl_khr_gl_sharing"));
        cl_device_arr[i] = devices[i]->deviceId;
    }
    
    glSharing = false;
    cl_context_properties * cl_ctx_properties = nullptr;
    
    if (hasSharing && requestGlSharing)
    {
        #if defined (EGL_VERSION)
            cl_context_properties properties[] = {
                CL_GL_CONTEXT_KHR, (cl_context_properties) eglGetCurrentContext(),
                CL_EGL_DISPLAY_KHR, (cl_context_properties) eglGetCurrentDisplay(),
                CL_CONTEXT_PLATFORM, (cl_context_properties) platform->platformId,
                0
            };
            cl_ctx_properties = new cl_context_properties[7];
            memcpy(cl_ctx_properties, properties, sizeof(properties));
        #elif defined (__APPLE__)
            cl_context_properties properties[] = {
                CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
                (cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext()),
                0
            };
            cl_ctx_properties = new cl_context_properties[3];
            memcpy(cl_ctx_properties, properties, sizeof(properties));
        #elif defined(__linux__)
            cl_context_properties properties[] = {
                CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
                CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
                CL_CONTEXT_PLATFORM, (cl_context_properties) platform->platformId,
                0
            };
            cl_ctx_properties = new cl_context_properties[7];
            memcpy(cl_ctx_properties, properties, sizeof(properties));
        #elif defined(_WIN32)
            cl_context_properties properties[] = {
                CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
                CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
                CL_CONTEXT_PLATFORM, (cl_context_properties) platform->platformId,
                0
            };
            cl_ctx_properties = new cl_context_properties[7];
            memcpy(cl_ctx_properties, properties, sizeof(properties));
        #endif

        // create context
        clContext = clCreateContext(cl_ctx_properties, (cl_uint)devices.size(), cl_device_arr, &notify, this, &ret);
        if (ret != CL_SUCCESS) {
            EGError("%s:%s clCreateContext failed: ret=%d\n", class_name, __func__, ret);
            clContext = 0;
        }

        // fetch subset of devices compatible with the current context
        size_t glcl_devices_size = 0;
#if defined (__APPLE__)
        ret = clGetGLContextInfoAPPLE(clContext, CGLGetCurrentContext(), CL_CGL_DEVICE_FOR_CURRENT_VIRTUAL_SCREEN_APPLE, 0, nullptr, &glcl_devices_size);
        if (ret != CL_SUCCESS) {
            EGError("%s:%s clGetGLContextInfoAPPLE(CL_CGL_DEVICE_FOR_CURRENT_VIRTUAL_SCREEN_APPLE) \n", class_name, __func__, ret);
        }
#else
#if defined (CL_VERSION_1_2)
        clGetGLContextInfoKHR_func clGetGLContextInfoKHR = (clGetGLContextInfoKHR_func)clGetExtensionFunctionAddressForPlatform(platform->platformId, "clGetGLContextInfoKHR");
#else
        clGetGLContextInfoKHR_func clGetGLContextInfoKHR = (clGetGLContextInfoKHR_func)clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
#endif
        if (clGetGLContextInfoKHR) {
            ret = clGetGLContextInfoKHR(cl_ctx_properties, CL_DEVICES_FOR_GL_CONTEXT_KHR, 0, nullptr, &glcl_devices_size);
            if (ret != CL_SUCCESS) {
                EGError("%s:%s clGetGLContextInfoKHR(CL_DEVICES_FOR_GL_CONTEXT_KHR)\n", class_name, __func__, ret);
            }
        }
#endif
        size_t clgl_device_count = glcl_devices_size / sizeof(cl_device_id);
        if (ret == CL_SUCCESS && clgl_device_count > 0) {
            // Find number of CL capable devices in the current GL context
            // Fetch device ids of CL capable devices in the current GL context
            cl_device_id *clgl_device_arr = new cl_device_id[clgl_device_count];
#if defined (__APPLE__)
                ret = clGetGLContextInfoAPPLE(clContext, CGLGetCurrentContext(), CL_CGL_DEVICE_FOR_CURRENT_VIRTUAL_SCREEN_APPLE,
                                              clgl_device_count * sizeof(cl_device_id), clgl_device_arr, nullptr);
                if (ret != CL_SUCCESS) {
                    EGError("%s:%s clGetGLContextInfoAPPLE(CL_CGL_DEVICE_FOR_CURRENT_VIRTUAL_SCREEN_APPLE)\n", class_name, __func__, ret);
                }
#else
                ret = clGetGLContextInfoKHR(properties, CL_DEVICES_FOR_GL_CONTEXT_KHR,
                                            clgl_device_count * sizeof(cl_device_id), clgl_device_arr, nullptr);
                if (ret != CL_SUCCESS) {
                    EGError("%s:%s clGetGLContextInfoKHR(CL_DEVICES_FOR_GL_CONTEXT_KHR)\n", class_name, __func__, ret);
                }
#endif
            if (ret == CL_SUCCESS) {
                
                // Filter device list to those that can share
                bool recreate_context = false;
                for (EGOpenCLDeviceList::iterator di = clDevices.begin(); di != clDevices.end();) {
                    bool found_in_current_context = false;
                    for (size_t i = 0; i < clgl_device_count; i++) {
                        found_in_current_context |= ((*di)->deviceId == clgl_device_arr[i]);
                    }
                    if (found_in_current_context) {
                        di++;
                    } else {
                        di = clDevices.erase(di);
                        recreate_context = true;
                    }
                }

                for (size_t i = 0; i < clDevices.size(); i++) {
                    cl_device_arr[i] = clDevices[i]->deviceId;
                    EGDebug("%s:%s OpenGL/OpenCL sharing capable device: %s\n", class_name, __func__, clDevices[i]->name.c_str());
                }

                if (recreate_context) {
                    ret = clReleaseContext(clContext);
                    if (ret != CL_SUCCESS) {
                        EGError("%s:%s clReleaseContext failed: ret=%d\n", class_name, __func__, ret);
                    }
                    clContext = 0;
                    delete [] cl_device_arr;
                    cl_device_arr = new cl_device_id[clDevices.size()];
                    EGOpenCLPlatformPtr platform;
                    for (size_t i = 0; i < clDevices.size(); i++) {
                        cl_device_arr[i] = clDevices[i]->deviceId;
                    }
                    EGDebug("%s:%s recreating OpenCL context with subset of OpenGL/OpenCL sharing capable devices\n", class_name, __func__, clDevices.size());
                    clContext = clCreateContext(cl_ctx_properties, (cl_uint)clDevices.size(), cl_device_arr, &notify, this, &ret);
                    if (ret != CL_SUCCESS) {
                        EGError("%s:%s clCreateContext failed: ret=%d\n", class_name, __func__, ret);
                        clContext = 0;
                    }
                }
                
                if (clContext) {
                    EGDebug("%s:%s Enabled OpenGL Sharing (context compatible devices)\n", class_name, __func__);
                    glSharing = true;
                }
            } else {
                EGError("%s:%s No OpenCL capable devices in the current GL context\n", class_name, __func__);
            }
        } else if (clContext) {
            // Attempt to use all devices
            EGDebug("%s:%s Enabled OpenGL Sharing\n", class_name, __func__);
            glSharing = true;
        }
    }

    if (!clContext) {
        clContext = clCreateContext(cl_ctx_properties, (cl_uint)clDevices.size(), cl_device_arr, &notify, this, &ret);
    }
    
    if (cl_device_arr) delete [] cl_device_arr;
    if (cl_ctx_properties) delete [] cl_ctx_properties;
    
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clCreateContext failed: ret=%d\n", class_name, __func__, ret);
        clContext = 0;
    }
}

EGOpenCLContext::~EGOpenCLContext()
{
    if (clContext) {
        cl_int ret = clReleaseContext(clContext);
        if (ret != CL_SUCCESS) {
            EGError("%s:%s clReleaseContext failed: ret=%d\n", class_name, __func__, ret);
        }
        clContext = 0;
    }
}

EGOpenCLProgramPtr EGOpenCLContext::createProgram(EGResourcePtr src, std::string options)
{
    std::string cacheKey = options + std::string(",") + src->getPath();
    std::map<std::string,EGOpenCLProgramPtr>::iterator cacheEnt = programCache.find(cacheKey);
    if (cacheEnt != programCache.end()) {
        return (*cacheEnt).second;
    } else {
        EGOpenCLProgramPtr program(new EGOpenCLProgram(this, src, options));
        programCache.insert(std::pair<std::string,EGOpenCLProgramPtr>(cacheKey, program));
        return program;
    }
}

EGOpenCLCommandQueuePtr EGOpenCLContext::createCommandQueue(EGOpenCLDevicePtr device, cl_command_queue_properties properties)
{
    EGOpenCLCommandQueuePtr cmdqueue(new EGOpenCLCommandQueue(this, device, properties));
    cmdqueues.push_back(cmdqueue);
    return cmdqueue;
}

EGOpenCLBufferPtr EGOpenCLContext::createBuffer(cl_mem_flags flags, size_t size, void *host_ptr)
{
    return EGOpenCLBufferPtr(new EGOpenCLBuffer(this, flags, size, host_ptr));
}

EGOpenCLBufferPtr EGOpenCLContext::createBufferFromGLBuffer(cl_mem_flags flags, EGuint glbuffer)
{
 	cl_int ret;
    cl_mem clBuffer = clCreateFromGLBuffer(clContext, flags, glbuffer, &ret);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clCreateFromGLBuffer failed: ret=%d\n", class_name, __func__, ret);
        return EGOpenCLBufferPtr();
    } else {
        return EGOpenCLBufferPtr(new EGOpenCLBuffer(clBuffer));
    }
}


/* EGOpenCLProgram */

EGOpenCLProgram::EGOpenCLProgram(EGOpenCLContext *context, EGResourcePtr src, std::string options) : context(context), src(src), options(options)
{
    const char* src_strings[] = { (const char*)src->getBuffer() };
    const size_t src_lengths[] = { (size_t)src->getLength() };
 	cl_int ret;
    clProgram = clCreateProgramWithSource(context->clContext, 1, src_strings, src_lengths, &ret);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clCreateProgramWithSource failed: ret=%d\n", class_name, __func__, ret);
        return;
    } else {
        EGDebug("%s:%s created program: %s\n", class_name, __func__, src->getBasename().c_str());
    }

    cl_device_id *cl_device_arr = new cl_device_id[context->clDevices.size()];
    for (size_t i = 0; i < context->clDevices.size(); i++) {
        cl_device_arr[i] = context->clDevices[i]->deviceId;
    }

    ret = clBuildProgram(clProgram, (cl_uint)context->clDevices.size(), cl_device_arr, options.c_str(), NULL, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clBuildProgram failed: ret=%d\n", class_name, __func__, ret);
    } else {
        EGDebug("%s:%s built program: %s\n", class_name, __func__, src->getBasename().c_str());
    }

    delete [] cl_device_arr;

    for (size_t i = 0; i < context->clDevices.size(); i++) {
        size_t build_log_size;
        ret = clGetProgramBuildInfo(clProgram, context->clDevices[i]->deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_size);
        if (ret != CL_SUCCESS) {
            EGError("%s:%s clGetProgramBuildInfo failed: ret=%d\n", class_name, __func__, ret);
            continue;
        }
        char *build_log = new char[build_log_size + 1];
        build_log[build_log_size] = '\0';
        ret = clGetProgramBuildInfo(clProgram, context->clDevices[i]->deviceId, CL_PROGRAM_BUILD_LOG, build_log_size, build_log, NULL);
        if (ret != CL_SUCCESS) {
            EGError("%s:%s clGetProgramBuildInfo failed: ret=%d\n", class_name, __func__, ret);
        } else {
            EGDebug("%s:%s device[%u] program build log = [%s]\n", class_name, __func__, context->clDevices[i]->deviceIndex, build_log);
        }
        delete [] build_log;
    }
    
    cl_uint  num_kernels;
    ret = clCreateKernelsInProgram(clProgram, 0, NULL, &num_kernels);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clCreateKernelsInProgram failed: ret=%d\n", class_name, __func__, ret);
        return;
    }
    
    cl_kernel *kernel_arr = new cl_kernel[num_kernels];
    ret = clCreateKernelsInProgram(clProgram, num_kernels, kernel_arr, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clCreateKernelsInProgram failed: ret=%d\n", class_name, __func__, ret);
        delete [] kernel_arr;
        return;
    }
    for (size_t i = 0; i < num_kernels; i++) {
        size_t kernel_name_size;
        ret = clGetKernelInfo(kernel_arr[i], CL_KERNEL_FUNCTION_NAME, 0, NULL, &kernel_name_size);
        if (ret != CL_SUCCESS) {
            EGError("%s:%s clGetKernelInfo failed: ret=%d\n", class_name, __func__, ret);
            continue;
        }
        char *kernel_name = new char[kernel_name_size + 1];
        kernel_name[kernel_name_size] = '\0';
        ret = clGetKernelInfo(kernel_arr[i], CL_KERNEL_FUNCTION_NAME, kernel_name_size, kernel_name, NULL);
        if (ret != CL_SUCCESS) {
            EGError("%s:%s clGetKernelInfo failed: ret=%d\n", class_name, __func__, ret);
        } else {
            EGDebug("%s:%s created kernel: %s\n", class_name, __func__, kernel_name);
            EGOpenCLKernelPtr kernel(new EGOpenCLKernel(this, kernel_arr[i], kernel_name));
            kernels.insert(std::pair<std::string,EGOpenCLKernelPtr>(kernel_name, kernel));
        }
        delete [] kernel_name;
    }
    delete [] kernel_arr;
}

EGOpenCLProgram::~EGOpenCLProgram()
{
    EGDebug("%s:%s released program: %s\n", class_name, __func__, src->getBasename().c_str());
    clReleaseProgram(clProgram);
}

EGOpenCLKernelPtr EGOpenCLProgram::getKernel(std::string name)
{
    EGOpenCLKernelMap::iterator ki = kernels.find(name);
    if (ki != kernels.end()) {
        return (*ki).second;
    } else {
        return EGOpenCLKernelPtr();
    }
}


/* EGOpenCLKernel */

EGOpenCLKernel::EGOpenCLKernel(EGOpenCLProgram *program, cl_kernel clKernel, std::string name) : program(program), clKernel(clKernel), name(name)
{
}

EGOpenCLKernel::~EGOpenCLKernel()
{
    clReleaseKernel(clKernel);
}

void EGOpenCLKernel::setArg(cl_uint arg_index, EGOpenCLLocalMemParam mem_size)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, mem_size.size, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clSetKernelArg failed: ret=%d\n", class_name, __func__, ret);
    }
}

void EGOpenCLKernel::setArg(cl_uint arg_index, EGOpenCLParam paramval)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, paramval.size, paramval.param);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clSetKernelArg failed: ret=%d\n", class_name, __func__, ret);
    }
}

void EGOpenCLKernel::setArg(cl_uint arg_index, cl_mem memval)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, sizeof(cl_mem), &memval);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clSetKernelArg failed: ret=%d\n", class_name, __func__, ret);
    }
}

void EGOpenCLKernel::setArg(cl_uint arg_index, cl_int intval)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, sizeof(cl_int), &intval);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clSetKernelArg failed: ret=%d\n", class_name, __func__, ret);
    }
}

void EGOpenCLKernel::setArg(cl_uint arg_index, cl_float floatval)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, sizeof(cl_float), &floatval);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clSetKernelArg failed: ret=%d\n", class_name, __func__, ret);
    }
}

void EGOpenCLKernel::setArg(cl_uint arg_index, cl_double doubleval)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, sizeof(cl_double), &doubleval);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clSetKernelArg failed: ret=%d\n", class_name, __func__, ret);
    }
}

void EGOpenCLKernel::setArg(cl_uint arg_index, EGOpenCLBufferPtr &buffer)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, sizeof(cl_mem), &buffer->clBuffer);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clSetKernelArg failed: ret=%d\n", class_name, __func__, ret);
    }
}


/* EGOpenCLBuffer */

EGOpenCLBuffer::EGOpenCLBuffer(cl_mem clBuffer) : clBuffer(clBuffer) {}

EGOpenCLBuffer::EGOpenCLBuffer(EGOpenCLContext *context, cl_mem_flags flags, size_t size, void *host_ptr)
{
    cl_int ret;
    clBuffer = clCreateBuffer(context->clContext, flags, size, host_ptr, &ret);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clCreateBuffer failed: ret=%d\n", class_name, __func__, ret);
    }
}

EGOpenCLBuffer::~EGOpenCLBuffer()
{
    clReleaseMemObject(clBuffer);
}

cl_mem_object_type EGOpenCLBuffer::getType()
{
    cl_mem_object_type type;
    cl_int ret = clGetMemObjectInfo (clBuffer, CL_MEM_SIZE, sizeof(cl_mem_object_type), &type, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetMemObjectInfo failed: ret=%d\n", class_name, __func__, ret);
    }
    return type;
}

cl_mem_flags EGOpenCLBuffer::getFlags()
{
    cl_mem_flags flags;
    cl_int ret = clGetMemObjectInfo (clBuffer, CL_MEM_SIZE, sizeof(cl_mem_flags), &flags, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetMemObjectInfo failed: ret=%d\n", class_name, __func__, ret);
    }
    return flags;
}

size_t EGOpenCLBuffer::getSize()
{
    size_t size;
    cl_int ret = clGetMemObjectInfo (clBuffer, CL_MEM_SIZE, sizeof(size_t), &size, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetMemObjectInfo failed: ret=%d\n", class_name, __func__, ret);
    }
    return size;
}

/* EGOpenCLProfilingInfo */

EGOpenCLProfilingInfo::EGOpenCLProfilingInfo(cl_ulong queued, cl_ulong submit, cl_ulong start, cl_ulong end)
    : queued(queued), submit(submit), start(start), end(end) {}

std::string EGOpenCLProfilingInfo::toString()
{
    std::stringstream ss;
    ss << "totaltime = " << std::left << std::setw(9) << std::setfill(' ') << (end - queued)
        << " ns, queuedtime = " << std::left << std::setw(9) << std::setfill(' ') << (start - queued)
        << " ns, runtime = " << std::left << std::setw(9) << std::setfill(' ') << (end - start) << " ns";
    return ss.str();
}


/* EGOpenCLEvent */

EGOpenCLEvent::EGOpenCLEvent(cl_event clEvent) : clEvent(clEvent) {}

EGOpenCLEvent::~EGOpenCLEvent()
{
    clReleaseEvent(clEvent);
}

void EGOpenCLEvent::wait()
{
    cl_int ret = clWaitForEvents(1, &clEvent);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clWaitForEvents failed: ret=%d\n", class_name, __func__, ret);
    }
}

EGOpenCLProfilingInfo EGOpenCLEvent::getProfilingInfo()
{
    cl_ulong queued = 0, submit = 0, start = 0, end = 0;
    clGetEventProfilingInfo(clEvent, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued, NULL);
    clGetEventProfilingInfo(clEvent, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit, NULL);
    clGetEventProfilingInfo(clEvent, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    clGetEventProfilingInfo(clEvent, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    return EGOpenCLProfilingInfo(queued, submit, start, end);
}

cl_int EGOpenCLEvent::getExecutionStatus()
{
    cl_int eventInfo;
    cl_int ret = clGetEventInfo(clEvent, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), &eventInfo, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetEventInfo failed: ret=%d\n", class_name, __func__, ret);
    }
    return eventInfo;
}


/* EGOpenCLCommandQueue */

EGOpenCLCommandQueue::EGOpenCLCommandQueue(EGOpenCLContext *context, EGOpenCLDevicePtr device, cl_command_queue_properties properties)
    : context(context), device(device), properties(properties), printProfilingInfo(false)
{
 	cl_int ret;
    clCommandQueue = clCreateCommandQueue(context->clContext, device->deviceId, properties, &ret);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clCreateCommandQueue failed: ret=%d\n", class_name, __func__, ret);
    }
}

EGOpenCLCommandQueue::~EGOpenCLCommandQueue()
{
    EGDebug("%s:%s released command queue\n", class_name, __func__);
    clReleaseCommandQueue(clCommandQueue);
}

void EGOpenCLCommandQueue::finish()
{
    clFinish(clCommandQueue);
}

EGOpenCLEventPtr EGOpenCLCommandQueue::enqueueTask(EGOpenCLKernelPtr &kernel,
                                                   EGOpenCLEventList eventWaitList)
{
    cl_event *wait_list = NULL;
    if (eventWaitList.size()) {
        wait_list = new cl_event[eventWaitList.size()];
        for (size_t i = 0; i < eventWaitList.size(); i++) {
            wait_list[i] = eventWaitList.at(i)->clEvent;
        }
    }

    cl_event clEvent;
    cl_int ret = clEnqueueTask(clCommandQueue, kernel->clKernel, (cl_uint)eventWaitList.size(), wait_list, &clEvent);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clEnqueueTask failed: ret=%d\n", class_name, __func__, ret);
        return EGOpenCLEventPtr();
    } else {
        EGOpenCLEvent *event = new EGOpenCLEvent(clEvent);
        if (printProfilingInfo) {
            event->wait();
            EGDebug("%-45s task   %-30s : %s\n", __func__, kernel->name.c_str(), event->getProfilingInfo().toString().c_str());
        }
        return EGOpenCLEventPtr(event);
    }
}

EGOpenCLEventPtr EGOpenCLCommandQueue::enqueueNDRangeKernel(EGOpenCLKernelPtr &kernel,
                                                            const EGOpenCLDimension &globalWorkSize,
                                                            EGOpenCLEventList eventWaitList)
{
    return enqueueNDRangeKernel(kernel, globalWorkSize, EGOpenCLDimension(), eventWaitList);
}

EGOpenCLEventPtr EGOpenCLCommandQueue::enqueueNDRangeKernel(EGOpenCLKernelPtr &kernel,
                                                            const EGOpenCLDimension &globalWorkSize,
                                                            const EGOpenCLDimension &localWorkSize,
                                                            EGOpenCLEventList eventWaitList)
{
    cl_event clEvent;
    size_t *global_work_size = NULL, *local_work_size = NULL;
    cl_event *wait_list = NULL;
    
    global_work_size= new size_t[globalWorkSize.size()];
    for (size_t i = 0; i < globalWorkSize.size(); i++) {
        global_work_size[i] = globalWorkSize[i];
    }
    
    if (localWorkSize.size() > 0) {
        local_work_size = new size_t[localWorkSize.size()];
        for (size_t i = 0; i < localWorkSize.size(); i++) {
            local_work_size[i] = localWorkSize[i];
        }
    }

    if (eventWaitList.size()) {
        wait_list = new cl_event[eventWaitList.size()];
        for (size_t i = 0; i < eventWaitList.size(); i++) {
            wait_list[i] = eventWaitList.at(i)->clEvent;
        }
    }
    
    cl_int ret = clEnqueueNDRangeKernel(clCommandQueue, kernel->clKernel, (cl_uint)globalWorkSize.size(),
                                        NULL, global_work_size, local_work_size, (cl_uint)eventWaitList.size(), wait_list, &clEvent);
    
    delete [] global_work_size;
    if (local_work_size) {
        delete [] local_work_size;
    }
    if (wait_list) {
        delete [] wait_list;
    }
    
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clEnqueueNDRangeKernel failed: ret=%d\n", class_name, __func__, ret);
        return EGOpenCLEventPtr();
    } else {
        EGOpenCLEvent *event = new EGOpenCLEvent(clEvent);
        if (printProfilingInfo) {
            event->wait();
            EGDebug("%-45s kernel %-30s : %s\n", __func__, kernel->name.c_str(), event->getProfilingInfo().toString().c_str());
        }
        return EGOpenCLEventPtr(event);
    }
}

EGOpenCLEventPtr EGOpenCLCommandQueue::enqueueReadBuffer(EGOpenCLBufferPtr &buffer, cl_bool blocking_read, size_t offset, size_t cb, void *ptr,
                                                         EGOpenCLEventList eventWaitList)
{
    cl_event clEvent;
    cl_event *wait_list = NULL;

    if (eventWaitList.size()) {
        wait_list = new cl_event[eventWaitList.size()];
        for (size_t i = 0; i < eventWaitList.size(); i++) {
            wait_list[i] = eventWaitList.at(i)->clEvent;
        }
    }

    cl_int ret = clEnqueueReadBuffer(clCommandQueue, buffer->clBuffer, blocking_read, offset, cb, ptr, (cl_uint)eventWaitList.size(), wait_list, &clEvent);

    if (wait_list) {
        delete [] wait_list;
    }

    if (ret != CL_SUCCESS) {
        EGError("%s:%s clEnqueueReadBuffer failed: ret=%d\n", class_name, __func__, ret);
        return EGOpenCLEventPtr();
    } else {
        EGOpenCLEvent *event = new EGOpenCLEvent(clEvent);
        if (printProfilingInfo) {
            event->wait();
            EGDebug("%-83s : %s\n", __func__, event->getProfilingInfo().toString().c_str());
        }
        return EGOpenCLEventPtr(event);
    }
}

EGOpenCLEventPtr EGOpenCLCommandQueue::enqueueWriteBuffer(EGOpenCLBufferPtr &buffer, cl_bool blocking_write, size_t offset, size_t cb, const void *ptr,
                                                          EGOpenCLEventList eventWaitList)
{
    cl_event clEvent;
    cl_event *wait_list = NULL;

    if (eventWaitList.size()) {
        wait_list = new cl_event[eventWaitList.size()];
        for (size_t i = 0; i < eventWaitList.size(); i++) {
            wait_list[i] = eventWaitList.at(i)->clEvent;
        }
    }

    cl_int ret = clEnqueueWriteBuffer(clCommandQueue, buffer->clBuffer, blocking_write, offset, cb, ptr, (cl_uint)eventWaitList.size(), wait_list, &clEvent);

    if (wait_list) {
        delete [] wait_list;
    }

    if (ret != CL_SUCCESS) {
        EGError("%s:%s clEnqueueWriteBuffer failed: ret=%d\n", class_name, __func__, ret);
        return EGOpenCLEventPtr();
    } else {
        EGOpenCLEvent *event = new EGOpenCLEvent(clEvent);
        if (printProfilingInfo) {
            event->wait();
            EGDebug("%-83s : %s\n", __func__, event->getProfilingInfo().toString().c_str());
        }
        return EGOpenCLEventPtr(event);
    }
}

EGOpenCLEventPtr EGOpenCLCommandQueue::enqueueAcquireGLObjects(EGOpenCLBufferList bufferList, EGOpenCLEventList eventWaitList)
{
    cl_event clEvent;
    cl_mem *buffer_list = NULL;
    cl_event *wait_list = NULL;
    
    if (bufferList.size()) {
        buffer_list = new cl_mem[bufferList.size()];
        for (size_t i = 0; i < bufferList.size(); i++) {
            buffer_list[i] = bufferList.at(i)->clBuffer;
        }
    }

    if (eventWaitList.size()) {
        wait_list = new cl_event[eventWaitList.size()];
        for (size_t i = 0; i < eventWaitList.size(); i++) {
            wait_list[i] = eventWaitList.at(i)->clEvent;
        }
    }
    
    cl_int ret = clEnqueueAcquireGLObjects(clCommandQueue, (cl_uint)bufferList.size(), buffer_list, (cl_uint)eventWaitList.size(), wait_list, &clEvent);
    
    if (buffer_list) {
        delete [] buffer_list;
    }
    
    if (wait_list) {
        delete [] wait_list;
    }

    if (ret != CL_SUCCESS) {
        EGError("%s:%s clEnqueueAcquireGLObjects failed: ret=%d\n", class_name, __func__, ret);
        return EGOpenCLEventPtr();
    } else {
        EGOpenCLEvent *event = new EGOpenCLEvent(clEvent);
        if (printProfilingInfo) {
            event->wait();
            EGDebug("%-83s : %s\n", __func__, event->getProfilingInfo().toString().c_str());
        }
        return EGOpenCLEventPtr(event);
    }
}

EGOpenCLEventPtr EGOpenCLCommandQueue::enqueueReleaseGLObjects(EGOpenCLBufferList bufferList, EGOpenCLEventList eventWaitList)
{
    cl_event clEvent;
    cl_mem *buffer_list = NULL;
    cl_event *wait_list = NULL;
    
    if (bufferList.size()) {
        buffer_list = new cl_mem[bufferList.size()];
        for (size_t i = 0; i < bufferList.size(); i++) {
            buffer_list[i] = bufferList.at(i)->clBuffer;
        }
    }
    
    if (eventWaitList.size()) {
        wait_list = new cl_event[eventWaitList.size()];
        for (size_t i = 0; i < eventWaitList.size(); i++) {
            wait_list[i] = eventWaitList.at(i)->clEvent;
        }
    }
    
    cl_int ret = clEnqueueReleaseGLObjects(clCommandQueue, (cl_uint)bufferList.size(), buffer_list, (cl_uint)eventWaitList.size(), wait_list, &clEvent);
    
    if (buffer_list) {
        delete [] buffer_list;
    }
    
    if (wait_list) {
        delete [] wait_list;
    }
    
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clEnqueueReleaseGLObjects failed: ret=%d\n", class_name, __func__, ret);
        return EGOpenCLEventPtr();
    } else {
        EGOpenCLEvent *event = new EGOpenCLEvent(clEvent);
        if (printProfilingInfo) {
            event->wait();
            EGDebug("%-83s : %s\n", __func__, event->getProfilingInfo().toString().c_str());
        }
        return EGOpenCLEventPtr(event);
    }
}


/* EGOpenCL */

EGOpenCL::EGOpenCL() : platforms()
{
    if (!init()) {
        EGError("%s:%s initialization failed\n", class_name, __func__);
    }
}

EGOpenCL::~EGOpenCL() {}

EGbool EGOpenCL::getPlatformIDs(cl_platform_id* &platforms, cl_uint &num_platforms)
{
    cl_int ret;
    
    // find number of platforms
    ret = clGetPlatformIDs(0, NULL, &num_platforms);
	if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetPlatformIDs failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }
	EGDebug("%s:%s found %d platforms\n", class_name, __func__, num_platforms);
    
    // get platform ids
	platforms = new cl_platform_id[num_platforms];
	ret = clGetPlatformIDs(num_platforms, platforms, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetPlatformIDs failed: ret=%d\n", class_name, __func__, ret);
        delete [] platforms;
        platforms = NULL;
        return false;
    }
    
    return true;
}

EGbool EGOpenCL::getPlatformInfo(cl_platform_id platform, cl_platform_info param_name, std::string &info)
{
    cl_int ret;
    size_t param_value_size;
    char *param_value;
    
    // get platform info size
    ret = clGetPlatformInfo(platform, param_name, 0, NULL, &param_value_size);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetPlatformInfo failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }
    
    // get platform info
    param_value = new char[param_value_size+1];
    ret = clGetPlatformInfo(platform, param_name, param_value_size, (void*)param_value, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetPlatformInfo failed: ret=%d\n", class_name, __func__, ret);
        delete [] param_value;
        return false;
    }
    
    // null terminate and copy to string
    param_value[param_value_size] = '\0';
    info = param_value;
    delete [] param_value;
    
    return true;
}

EGbool EGOpenCL::getDeviceIDs(cl_platform_id platform, cl_device_type device_type, cl_device_id* &devices, cl_uint &num_devices)
{
    cl_int ret;
    
    // find number of devices
    ret = clGetDeviceIDs(platform, device_type, 0, NULL, &num_devices);
	if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceIDs failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }
	EGDebug("%s:%s found %d devices\n", class_name, __func__, num_devices);
    
    // get device ids
	devices = new cl_device_id[num_devices];
	ret = clGetDeviceIDs(platform, device_type, num_devices, devices, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceIDs failed: ret=%d\n", class_name, __func__, ret);
        delete [] devices;
        devices = NULL;
        return false;
    }
    
    return true;
}

EGbool EGOpenCL::getDeviceInfoType(cl_device_id device, cl_device_type &deviceType)
{
    cl_int ret = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(cl_device_type), (void*)&deviceType, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceInfo failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }    
    return true;
}

EGbool EGOpenCL::getDeviceInfoMemCacheType(cl_device_id device, cl_device_mem_cache_type &memCacheType)
{
    cl_int ret = clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, sizeof(cl_device_mem_cache_type), (void*)&memCacheType, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceInfo failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }
    return true;
}

EGbool EGOpenCL::getDeviceInfoLocalMemType(cl_device_id device, cl_device_local_mem_type &localMemType)
{
    cl_int ret = clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(cl_device_local_mem_type), (void*)&localMemType, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceInfo failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }
    return true;
}

EGbool EGOpenCL::getDeviceInfoFPConfig(cl_device_id device, cl_device_info param_name, cl_device_fp_config &devicefpConfig)
{
    cl_int ret = clGetDeviceInfo(device, param_name, sizeof(cl_device_fp_config), (void*)&devicefpConfig, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceInfo failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }    
    return true;
}

EGbool EGOpenCL::getDeviceInfoBool(cl_device_id device, cl_device_info param_name, cl_bool &boolVal)
{
    cl_int ret = clGetDeviceInfo(device, param_name, sizeof(cl_bool), (void*)&boolVal, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceInfo failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }
    return true;
}

EGbool EGOpenCL::getDeviceInfoUint(cl_device_id device, cl_device_info param_name, cl_uint &uintVal)
{
    cl_int ret = clGetDeviceInfo(device, param_name, sizeof(cl_uint), (void*)&uintVal, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceInfo failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }    
    return true;
}

EGbool EGOpenCL::getDeviceInfoUlong(cl_device_id device, cl_device_info param_name, cl_ulong &ulongVal)
{
    cl_int ret = clGetDeviceInfo(device, param_name, sizeof(cl_ulong), (void*)&ulongVal, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceInfo failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }    
    return true;
}

EGbool EGOpenCL::getDeviceInfoSize(cl_device_id device, cl_device_info param_name, size_t &sizeVal)
{
    cl_int ret = clGetDeviceInfo(device, param_name, sizeof(size_t), (void*)&sizeVal, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceInfo failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }
    return true;
}

EGbool EGOpenCL::getDeviceInfoSizeArray(cl_device_id device, cl_device_info param_name, std::vector<size_t> &sizeArray)
{
    cl_int ret;
    size_t param_value_size;
    size_t *param_value;
    
    // get device info string size
    ret = clGetDeviceInfo(device, param_name, 0, NULL, &param_value_size);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceInfo failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }
    
    // get device info string
    size_t nelem = param_value_size / sizeof(size_t);
    param_value = new size_t[nelem];
    ret = clGetDeviceInfo(device, param_name, param_value_size, (void*)param_value, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceInfo failed: ret=%d\n", class_name, __func__, ret);
        delete [] param_value;
        return false;
    }
    
    // copy values to array
    for (size_t i = 0; i < nelem; i++) {
        sizeArray.push_back(param_value[i]);
    }
    delete [] param_value;

    return true;
}

EGbool EGOpenCL::getDeviceInfoString(cl_device_id device, cl_device_info param_name, std::string &info)
{
    cl_int ret;
    size_t param_value_size;
    char *param_value;
    
    // get device info string size
    ret = clGetDeviceInfo(device, param_name, 0, NULL, &param_value_size);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceInfo failed: ret=%d\n", class_name, __func__, ret);
        return false;
    }
    
    // get device info string
    param_value = new char[param_value_size+1];
    ret = clGetDeviceInfo(device, param_name, param_value_size, (void*)param_value, NULL);
    if (ret != CL_SUCCESS) {
        EGError("%s:%s clGetDeviceInfo failed: ret=%d\n", class_name, __func__, ret);
        delete [] param_value;
        return false;
    }
    
    // null terminate and copy to string
    param_value[param_value_size] = '\0';
    info = param_value;
    delete [] param_value;
    
    return true;
}

EGbool EGOpenCL::init()
{
    cl_uint num_platforms;
    cl_uint num_devices;
	cl_platform_id *cl_platform_arr;
    cl_device_id *cl_device_arr;
    
    // read platform ids
    if (!getPlatformIDs(cl_platform_arr, num_platforms)) return false;
    
    // read platform info
    for (EGuint platformIndex = 0; platformIndex < num_platforms; platformIndex++) {
        EGOpenCLPlatformPtr platform(new EGOpenCLPlatform(cl_platform_arr[platformIndex], platformIndex));
        platforms.push_back(platform);
        //platform->print();
        
        // read device ids
        if (!getDeviceIDs(cl_platform_arr[platformIndex], CL_DEVICE_TYPE_ALL, cl_device_arr, num_devices)) goto fail_platforms;
        
        // read device info
        for (EGuint deviceIndex = 0; deviceIndex < num_devices; deviceIndex++) {
            EGOpenCLDevicePtr device(new EGOpenCLDevice(platform, cl_device_arr[deviceIndex], deviceIndex));
            devices.push_back(device);
            //device->print();
        }
        
        delete [] cl_device_arr;
    }
    
    delete [] cl_platform_arr;
    return true;
    
fail_platforms:
    delete [] cl_platform_arr;
    return false;
}

EGOpenCLDeviceList EGOpenCL::getDevices(std::string deviceMatch)
{
    EGOpenCLDeviceList deviceList;
    EGOpenCLPlatformPtr platform;
    for (EGOpenCLDeviceList::iterator di = devices.begin(); di != devices.end(); di++) {
        EGOpenCLDevicePtr device = *di;
        if (!device->available) continue;
        if (platform && device->platform != platform) continue; // make sure all devices are on the same platform
        if (deviceMatch == "any") {
            deviceList.push_back(device);
            break;
        } else if (deviceMatch == "gpu" && device->deviceType == CL_DEVICE_TYPE_GPU) {
            deviceList.push_back(device);
        } else if (deviceMatch == "cpu" && device->deviceType == CL_DEVICE_TYPE_CPU) {
            deviceList.push_back(device);
        } else if (deviceMatch == "all") {
            platform = device->platform;
            deviceList.push_back(device);
        } else if (deviceMatch == device->name) {
            platform = device->platform;
            deviceList.push_back(device);
        }
    }
    return deviceList;
}

EGOpenCLContextPtr EGOpenCL::createContext(std::string deviceMatch, EGbool requestGlSharing)
{
    return createContext(getDevices(deviceMatch), requestGlSharing);
}

EGOpenCLContextPtr EGOpenCL::createContext(EGOpenCLDeviceList deviceList, EGbool requestGlSharing)
{
    if (deviceList.size() == 0) {
        EGError("%s:%s no OpenCL devices present\n", class_name, __func__);
        return EGOpenCLContextPtr();
    }
    EGOpenCLContextPtr context(new EGOpenCLContext(deviceList, requestGlSharing));
    contexts.push_back(context);
    return context;
}
