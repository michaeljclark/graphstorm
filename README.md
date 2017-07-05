GraphStorm
==========

GraphStorm is an OpenCL accelerated force-directed graph layout engine intended
for layout and visualization of large graphs. GraphStorm includes a simple
OpenGL accelerated 2D UI toolkit.

![Graphstorm Screenshot 1](/screenshot-1.png)

![Graphstorm Screenshot 2](/screenshot-2.png)

![Graphstorm Screenshot 3](/screenshot-3.png)

### Build dependencies ###

  * OpenGL 3.x
  * OpenCL SDK
    * (Windows/Linux) NVidia GPU Computing SDK
    * (Windows/Linux) AMD APP SDK
    * (Windows/Linux) Intel SDK for OpenCL 
    * (Mac OS X) OpenCL.framework (included in Mac OS X 10.7.x and above)
  * GLUT - to build GraphStormGLUT
    * (Windows/Linux) freeglut 2.8.1 <http://freeglut.sourceforge.net/>
    * (Mac OS X) GLUT.framework (included in Mac OS X)
  * GLFW - to build GraphstormGLFW

### Mac build Instructions ###

  * To build GraphStormGLUT open macosx/GraphStormGLUT.xcodeproj

### Windows build Instructions ###

  * Download and build freeglut_static target in freeglut 2.8.1
  * To build GraphStormGLUT open win32/GraphStormGLUT.vcxproj
  * Set target to "Release"
  * Edit C++ -> General -> Additional Include Directories
    * Set header include path for freeglut and your chosen OpenCL SDK
  * Edit Linker -> General -> Additional Library Directories
    * Set library include path for freeglut and your chosen OpenCL SDK

### Alternative build using cmake (Windows, Mac or Linux) ###

  * Install GLFW and GLUT as indicated above
  * Edit CMakeLists.txt to set location of GLUT and OpenCL libraries and includes
    * ```find_library (GLUT_LIBRARY freeglut_static PATHS "<freeglut_lib_path>")```
    * ```set (GLUT_INCLUDE_DIR "<freeglut_include_path>")```
    * ```find_library (OPENCL_LIBRARY OpenCL PATHS "<opencl_lib_path>")```
    * ```set (OPENCL_INCLUDE_DIR "<opencl_include_path>")```
  * Open a terminal (on Windows use Visual Studio x86 Native Tools Command Prompt)
    * ```cd graphstorm``` ```mkdir build``` ```cd build``` ```ccmake ..```
  * Open generated project file or run make (depending on cmake configuration)

### License ###

GraphStorm is BSD licensed, see LICENSE.
