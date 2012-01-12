/// Includes correct OpenGL ES headers for corresponding platforms.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#ifndef OPENGL_H_
#define OPENGL_H_

#include "Platform.h"

#if defined(RAINBOW_ANDROID)
#	include <GLES/gl.h>
#elif defined(RAINBOW_IOS)
#	include <OpenGLES/ES1/gl.h>
#elif defined(RAINBOW_WIN)
#	ifndef WIN32
#		define WIN32
#	endif
#	include <GL/GLee.h>
#else
#	define GL_GLEXT_PROTOTYPES 1
#	include <GL/gl.h>
#endif

#endif