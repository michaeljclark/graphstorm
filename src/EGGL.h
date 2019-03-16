// See LICENSE for license details.

#pragma once

#if defined(__APPLE__)

/* Apple OS X / iPhone OS */
#include <Availability.h>

#if TARGET_OS_IPHONE
/* iPhone OS */
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
typedef double GLdouble;
#include "glu.h"
#include "ug.h"

#elif TARGET_OS_MAC

/* Mac OS X */
#include <OpenGL/glext.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#elif ANDROID

/* Android */
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
typedef double GLdouble;
#include "glu.h"
#include "ug.h"

#elif __native_client__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
typedef double GLdouble;
#define GL_DOUBLE GL_FLOAT
#include "glu.h"
#ifndef NACL
#include "ug.h"
#endif

#elif defined(__unix__) || defined(__linux__) || defined(__FreeBSD__) || \
	  defined(__NetBSD__) || defined(__DragonFly__) || defined(__OpenBSD__)

/* Linux/Unix */
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>

#elif defined(WIN32)

/* Windows */
#include "GLee.h"
#include <GL/gl.h>
#include <GL/glu.h>

#endif

#if defined (_WIN32)
#define GLU_CALLBACK (GLvoid (__stdcall *) ())
#else
#define GLU_CALLBACK (GLvoid(*)())
#endif

#if defined(__APPLE__)
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#endif

#define USE_VAO 1
