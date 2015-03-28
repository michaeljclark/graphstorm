#include "EG.h"
#include "EGGL.h"
#include "EGResource.h"
#include "EGOpenCL.h"

struct opencltest
{
  EGOpenCLPtr cl;
  EGOpenCLDeviceList cldevices;
  EGOpenCLContextPtr clctx;
  EGOpenCLCommandQueuePtr clcmdqueue;
  EGOpenCLProgramPtr cltestprog;

  opencltest()
  {
    cl = EGOpenCLPtr(new EGOpenCL());
    cldevices = cl->getDevices("gpu");
    clctx = cl->createContext(cldevices, false);
    clcmdqueue = clctx->createCommandQueue(clctx->getDevices().at(0), 0);
    cltestprog = clctx->createProgram(EGResource::getResource("Resources/cl.bundle/test.cl"));
  }


  void test_add()
  {
    int a[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int b[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int c[8];
    
    EGOpenCLBufferPtr abuffer = clctx->createBuffer(CL_MEM_READ_ONLY, sizeof(a), NULL);
    EGOpenCLBufferPtr bbuffer = clctx->createBuffer(CL_MEM_READ_ONLY, sizeof(b), NULL);
    EGOpenCLBufferPtr cbuffer = clctx->createBuffer(CL_MEM_WRITE_ONLY, sizeof(c), NULL);
    
    EGDebug("abuffer.size=%d bbuffer.size=%d cbuffer.size=%d\n", abuffer->getSize(), bbuffer->getSize(), cbuffer->getSize());

    EGOpenCLKernelPtr addkernel = cltestprog->getKernel("add");
    addkernel->setArg(0, abuffer);
    addkernel->setArg(1, bbuffer);
    addkernel->setArg(2, cbuffer);
    
    clcmdqueue->enqueueWriteBuffer(abuffer, true, 0, sizeof(a), a);
    clcmdqueue->enqueueWriteBuffer(bbuffer, true, 0, sizeof(b), b);
    clcmdqueue->enqueueNDRangeKernel(addkernel, EGOpenCLDimension(8));
    clcmdqueue->enqueueReadBuffer(cbuffer, true, 0, sizeof(c), c);
    
    EGbool ok = true;
    for (int i = 0; i < 8; i++) {
        ok &= (a[i] + b[i] == c[i]);
    }
    EGDebug("Add Test: %s\n", ok ? "Success": "Failed");
  }

  void test_write_float()
  {
    EGfloat a[1];

    EGOpenCLBufferPtr abuffer = clctx->createBuffer(CL_MEM_WRITE_ONLY, sizeof(a), NULL);

    EGOpenCLKernelPtr writefloatkernel = cltestprog->getKernel("write_float");
    writefloatkernel->setArg(0, abuffer);
    writefloatkernel->setArg(1, 7.0f);
    clcmdqueue->enqueueTask(writefloatkernel);
    clcmdqueue->enqueueReadBuffer(abuffer, true, 0, sizeof(a), a);

    EGDebug("Write Float Test: a=%f %s\n", a[0], a[0] == 7.0f ? "Success" : "Failed");
  }
};

int main(int argc, char **argv)
{

    EGResource::useFilesystemPath = true;

    opencltest test;
    test.test_add();
    test.test_write_float();

    return 0;
}
