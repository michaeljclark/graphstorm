cmake_minimum_required (VERSION 2.8)

project (GraphStorm)

include_directories(src)

#
# Linux libraries and headers
#
IF (UNIX)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
find_library (OPENCL_LIBRARY OpenCL)
find_library (GLFW_LIBRARY glfw)
find_library (GLUT_LIBRARY glut)
find_library (GLU_LIBRARY GLU)
find_library (OPENGL_LIBRARY GL)
find_library (OPENCL_LIBRARY OpenCL)
find_library (JPEG_LIBRARY jpeg)
find_library (PNG_LIBRARY png)
find_library (ZLIB_LIBRARY z)
find_package(PkgConfig)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
pkg_check_modules(FREETYPE2 REQUIRED freetype2)
set (EXTRA_LIBS ${OPENCL_LIBRARY} ${GLU_LIBRARY} ${OPENGL_LIBRARY} ${CAIRO_LDFLAGS} ${HARFBUZZ_LDFLAGS} ${FREETYPE2_LDFLAGS} ${JPEG_LIBRARY} ${PNG_LIBRARY} ${ZLIB_LIBRARY})
include_directories (${OPENCL_INCLUDE_DIR})
add_definitions(${CAIRO_CFLAGS})
add_definitions(${HARFBUZZ_CFLAGS})
set(OPENCL 1)
ENDIF()

#
# OS X libraries and headers
#
IF (APPLE)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++0x")
find_library (SYSTEM_CONFIGURATION_LIBRARY SystemConfiguration)
find_library (COREFOUNDATION_LIBRARY CoreFoundation)
find_library (COCOA_LIBRARY Cocoa)
find_library (GLUT_LIBRARY GLUT)
find_library (GLFW_LIBRARY glfw)
find_library (OPENGL_LIBRARY OpenGL)
find_library (OPENCL_LIBRARY OpenCL)
find_library (AGL_LIBRARY AGL)
find_library (ZLIB_LIBRARY z)
set (EXTRA_LIBS ${SYSTEM_CONFIGURATION_LIBRARY} ${COREFOUNDATION_LIBRARY} ${COCOA_LIBRARY} ${OPENCL_LIBRARY} ${OPENGL_LIBRARY} ${OPENCL_LIBRARY} ${ZLIB_LIBRARY})
include_directories (${OPENCL_INCLUDE_DIR})
set(OPENCL 1)
ENDIF()

#
# Windows libraries and headers
#
IF (WIN32)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} /D _SECURE_SCL=0 /D _WINDOWS /D _CRT_SECURE_NO_WARNINGS /D FREEGLUT_STATIC")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG_INIT} /NODEFAULTLIB:freeglut_static.lib")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE_INIT} /NODEFAULTLIB:freeglut_static.lib")
find_library (UNISCRIBE usp10)
find_library (DIRECT_2D d2d1)
find_library (DIRECT_WRITE dwrite)
find_library (WINDOWS_CODECS windowscodecs)
find_library (GLUT_LIBRARY freeglut_static PATHS "C:/Users/mclark/src/freeglut-2.8.1/lib/x86")
set (GLUT_INCLUDE_DIR "C:/Users/mclark/src/freeglut-2.8.1/include")
find_library (OPENGL_LIBRARY OpenGL32)
#find_library (OPENCL_LIBRARY OpenCL PATHS "C:/Program Files (x86)/AMD APP SDK/2.9/lib/x86")
#set (OPENCL_INCLUDE_DIR "C:/Program Files (x86)/AMD APP SDK/2.9/include")
find_library (OPENCL_LIBRARY OpenCL PATHS "C:/Program Files (x86)/Intel/OpenCL SDK/4.6/lib/x86")
set (OPENCL_INCLUDE_DIR "C:/Program Files (x86)/Intel/OpenCL SDK/4.6/include")
set (EXTRA_LIBS ${UNISCRIBE} ${DIRECT_2D} ${DIRECT_WRITE} ${WINDOWS_CODECS} ${OPENCL_LIBRARY} ${OPENGL_LIBRARY} ${ZLIB_LIBRARY})
include_directories (${OPENCL_INCLUDE_DIR})
include_directories (${GLUT_INCLUDE_DIR})
set(OPENCL 1)
ENDIF()

#
# Bundled third party libraries on Emscripten, Native Client or Windows
#
IF (LINUX OR WIN32)
add_subdirectory("third_party/tess" "libs/tess")
add_subdirectory("third_party/zlib" "libs/zlib")
add_subdirectory("third_party/jpeg" "libs/jpeg")
add_subdirectory("third_party/png" "libs/png")
add_subdirectory("third_party/freetype" "libs/freetype")
add_subdirectory("third_party/pixman" "libs/pixman")
add_subdirectory("third_party/cairo" "libs/cairo")
add_subdirectory("third_party/harfbuzz" "libs/harfbuzz")
include_directories (third_party/tess)
include_directories (third_party/zlib)
include_directories (third_party/png)
include_directories (third_party/jpeg)
include_directories (third_party/freetype/include)
include_directories (third_party/freetype/include/freetype/config)
include_directories (third_party/cairo/src)
include_directories (third_party/harfbuzz/src)
add_definitions( -DFT_CONFIG_CONFIG_H=\"ftconfig.h\" )
add_definitions( -DFT_CONFIG_MODULES_H=\"ftmodule.h\" )
ENDIF ()

#
# graphlib
#
set (graphlib_SOURCES src/EG.cc src/EGCamera.cc src/EGClipboardAndroid.cc src/EGClipboardEmscripten.cc src/EGClipboardNACL.cc src/EGClipboardWindows.cc src/EGClipboardX11.cc  src/EGEvent.cc src/EGGraph.cc src/EGGraphData.cc src/EGGraphLayoutFactory.cc src/EGGraphTest.cc src/EGImage.cc src/EGImageDecompressor.cc src/EGImageJPEG.cc src/EGImagePNG.cc src/EGLight.cc src/EGMath.cc src/EGKeyEvent.cc src/EGMouseEvent.cc src/EGObject.cc src/EGBaseObject.cc src/EGProperties.cc src/EGRenderApi.cc src/EGRenderApiES2.cc src/EGResource.cc src/EGFont.cc src/EGText.cc src/EGTextCairo.cc src/EGTextMac.cc src/EGTextGDI.cc src/EGTextDWrite.cc src/EGTextRendererES2.cc src/EGTrackBall.cc src/EGUtil.cc src/EGZipFile.cc src/EWBox.cc src/EWButton.cc src/EWCheckBox.cc src/EWColorPicker.cc src/EWComboBox.cc src/EWContainer.cc src/EWContext.cc src/EWEntry.cc src/EWFrame.cc src/EWGrid.cc src/EWIcon.cc src/EWLabel.cc src/EWLayout.cc src/EWList.cc src/EWProgressBar.cc src/EWScrollBar.cc src/EWSlider.cc src/EWSpinner.cc src/EWSpinBox.cc src/EWTable.cc src/EWToggleButton.cc src/EWWidget.cc src/EWWidgetDefaults.cc src/EWWidgetRenderer.cc src/EWWidgetRendererES2.cc src/EWWindow.cc)
set (graphlib_HEADERS src/EG.h src/EGBase.h src/EGCamera.h src/EGColor.h src/EGCombination.h src/EGEvent.h src/EGGL.h src/EGGraph.h src/EGGraphData.h src/EGGraphEdgeLabelRenderer.h src/EGGraphEdgeRenderer.h src/EGGraphEdgeRendererES2.h src/EGGraphFloydWarshall.h src/EGGraphLayout.h src/EGGraphLayoutFactory.h src/EGGraphModifier.h src/EGGraphNodeLabelRenderer.h src/EGGraphNodeRenderer.h src/EGGraphNodeRendererES2.h src/EGGraphRendererParams.h src/EGGraphTest.h src/EGImage.h src/EGIndexArray.h src/EGLight.h src/EGMath.h src/EGKeyEvent.h src/EGMouseEvent.h src/EGObject.h src/EGBaseObject.h src/EGProperties.h src/EGPointRegionOctree.h src/EGPointRegionQuadtree.h src/EGPointRegionTree.h src/EGPointRegionTreeArrayCL.h src/EGPointRegionTreeMonolithicCL.h src/EGPointRegionTreeStructCL.h src/EGRenderApi.h src/EGRenderApiES2.h src/EGRenderBatch.h src/EGRenderBatchES2.h src/EGRenderBatchText.h src/EGResource.h src/EGSpringLayout.h src/EGSpringLayoutCL.h src/EGFont.h src/EGText.h src/EGTextRenderer.h src/EGTextRendererES2.h src/EGThread.h src/EGTime.h src/EGTrackBall.h src/EGTree.h src/EGTreeRenderer.h src/EGTreeRendererES2.h src/EGBoundsRenderer.h src/EGBoundsRendererES2.h src/EGVector.h src/EGVertexArray.h src/EGViewport.h src/EGUtil.h src/EGZipFile.h src/EWBox.h src/EWButton.h src/EWCheckBox.h src/EWColorPicker.h src/EWComboBox.h src/EWContainer.h src/EWContext.h src/EWEntry.h src/EWFrame.h src/EWGrid.h src/EWIcon.h src/EWLabel.h src/EWLayout.h src/EWList.h src/EWProgressBar.h src/EWScrollBar.h src/EWSlider.h src/EWSpinner.h src/EWSpinBox.h src/EWTable.h src/EWToggleButton.h src/EWWidget.h src/EWWidgetDefaults.h src/EWWidgetRenderer.h src/EWWidgetRendererES2.h src/EWWindow.h)

IF (APPLE)
set (graphlib_SOURCES ${graphlib_SOURCES} src/NSStubs.mm src/EGClipboardMac.mm src/EGClipboardIOS.mm)
set (graphlib_HEADERS ${graphlib_HEADERS} src/NSStubs.h)
ENDIF ()

IF (WIN32)
set (graphlib_SOURCES ${graphlib_SOURCES} src/GLee.c)
set (graphlib_HEADERS ${graphlib_HEADERS} src/GLee.h)
ENDIF ()

IF (OPENCL)
set (graphlib_SOURCES ${graphlib_SOURCES} src/EGOpenCL.cc)
set (graphlib_HEADERS ${graphlib_HEADERS} src/EGOpenCL.h src/EGGraphEdgeRendererES2CL.h src/EGGraphNodeRendererES2CL.h)
ENDIF ()

add_library (graphlib ${graphlib_SOURCES})

#
# graphapp
#
set (graphapp_SOURCES src/EGGraphApp.cc src/EGGraphAppUI.cc)
set (graphapp_HEADERS src/EGGraphApp.h src/EGGraphAppUI.h)

add_library (graphapp ${graphapp_SOURCES})


#
# Mac bundle resources
#
IF (APPLE)
file(GLOB_RECURSE UI_BUNDLE Resources/ui.bundle/*)
file(GLOB_RECURSE METADATA_BUNDLE Resources/metadata.bundle/*)
file(GLOB_RECURSE FONTS_BUNDLE Resources/fonts.bundle/*)
file(GLOB_RECURSE CL_BUNDLE Resources/cl.bundle/*)
file(GLOB_RECURSE GLES2_BUNDLE Resources/gles2.bundle/*)
set (RESOURCE_BUNDLES ${UI_BUNDLE} ${METADATA_BUNDLE} ${FONTS_BUNDLE} ${CL_BUNDLE} ${GLES2_BUNDLE})
FOREACH(_file ${RESOURCE_BUNDLES})
	GET_FILENAME_COMPONENT(_file_dir ${_file} PATH)
	FILE(RELATIVE_PATH _file_dir ${CMAKE_CURRENT_SOURCE_DIR} ${_file_dir})
	SET_SOURCE_FILES_PROPERTIES( ${_file} PROPERTIES MACOSX_PACKAGE_LOCATION ${_file_dir})
ENDFOREACH(_file)
ENDIF ()


#
# Emscripten Targets
#
IF (EMSCRIPTEN)
  SET (CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} --preload-file Resources/cl.bundle --preload-file Resources/fonts.bundle --preload-file Resources/gles2.bundle --preload-file Resources/metadata.bundle --preload-file Resources/ui.bundle")

  set (graphstorm_SOURCES src/EGGraphStormGLUT.cc)
  set (graphstorm_HEADERS src/EGGraphStormGLUT.h)
  add_executable (graphstorm.html ${graphstorm_SOURCES})
  target_link_libraries (graphstorm.html graphapp graphlib harfbuzz cairo pixman freetype png jpeg tess zlib)
  add_custom_command(TARGET graphstorm.html PRE_BUILD COMMAND ln -sf ../Resources)

  set (graphstormwidgets_SOURCES src/EGGraphStormWidgets.cc src/EGGraphStormWidgetsGLUT.cc)
  set (graphstormwidgets_HEADERS src/EGGraphStormWidgets.h src/EGGraphStormWidgetsGLUT.h)
  add_executable (graphstormwidgets.html ${graphstormwidgets_SOURCES})
  target_link_libraries (graphstormwidgets.html graphlib harfbuzz cairo pixman freetype png jpeg tess zlib)
  add_custom_command(TARGET graphstormwidgets.html PRE_BUILD COMMAND ln -sf ../Resources)
ENDIF ()

#
# OS X, Windows and Linux Targets
#
IF (APPLE OR WIN32 OR UNIX)

  # gswidgets
  set (gswidgets_SOURCES src/EGGraphStormWidgets.cc src/EGGraphStormWidgetsGLUT.cc)
  set (gswidgets_HEADERS src/EGGraphStormWidgets.h src/EGGraphStormWidgetsGLUT.h)
  set (gswidgets_SOURCES ${gswidgets_SOURCES} ${RESOURCE_BUNDLES})
  add_executable (gswidgets WIN32 MACOSX_BUNDLE ${gswidgets_SOURCES})
  target_link_libraries (gswidgets graphlib ${EXTRA_LIBS} ${GLUT_LIBRARY})
  IF (WIN32)
    target_link_libraries (gswidgets png jpeg tess zlib)
  ENDIF ()
  
  # gsglut
  set (gsglut_SOURCES src/EGGraphStormGLUT.cc)
  set (gsglut_HEADERS src/EGGraphStormGLUT.h)
  set (gsglut_SOURCES ${gsglut_SOURCES} ${RESOURCE_BUNDLES})
  add_executable (gsglut WIN32 MACOSX_BUNDLE ${gsglut_SOURCES})
  target_link_libraries (gsglut graphapp graphlib ${EXTRA_LIBS} ${GLUT_LIBRARY})
  IF (WIN32)
    target_link_libraries (gsglut png jpeg tess zlib)
  ENDIF ()

  # gsglfw
  set (gsglfw_SOURCES src/EGGraphStormGLFW.cc)
  set (gsglfw_HEADERS src/EGGraphStormGLFW.h)
  set (gsglfw_SOURCES ${gsglfw_SOURCES} ${RESOURCE_BUNDLES})
  add_executable (gsglfw WIN32 MACOSX_BUNDLE ${gsglfw_SOURCES})
  target_link_libraries (gsglfw graphapp graphlib ${EXTRA_LIBS} ${GLFW_LIBRARY})
  IF (WIN32)
    target_link_libraries (gsglfw png jpeg tess zlib)
  ENDIF ()

  IF (OPENCL)
    # opencltest
    set (opencltest_SOURCES tests/opencltest.cc)
    add_executable (opencltest ${opencltest_SOURCES})
    target_link_libraries (opencltest graphlib ${EXTRA_LIBS})
  ENDIF (OPENCL)

  # testobject
  set (testobject_SOURCES tests/testobject.cc)
  add_executable (testobject ${testobject_SOURCES})
  target_link_libraries (testobject graphlib ${EXTRA_LIBS})

ENDIF ()
