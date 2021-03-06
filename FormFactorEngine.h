/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#ifndef FORMFACTORENGINE_H
#define FORMFACTORENGINE_H

/**
 * @file FormFactorEngine.h
 * @brief Class FormFactorEngine - compute form factors.
 * @author xbarin02
 * @date 2007-11-17
 */

#include <map>
#include <vector>

#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__)
#	include <windows.h>
#else
#	include <X11/Xlib.h>
#endif

#include <GL/glu.h>
#include "Triangle.h"
//#include <cuda.h>
//#include <cuda_runtime.h>
//#include <device_launch_parameters.h>

//#include <book.h>

using namespace std;

#ifndef BUILDING_DOX
class Vector;
class PatchRandomAccessEnumerator;
class PatchCacheLine;
#endif


typedef struct {
	int edge;
	double* elements;
} Matrix;

typedef struct {
	unsigned* keys;
	double* data;
} fakeMap;


/**
 * @brief Compute all form factors for any destination patch.
 */
class FormFactorEngine {
public:
		
		/**
		 * @brief Constructor, create window, OpenGL context and pre-compute some temp variables.
		 * @param patchEnumerator
		 */
		FormFactorEngine (PatchRandomAccessEnumerator *patchEnumerator);
			
		/**
		 * @brief Compute form factors for destination patch destPatch and store these factors into cache line.
		 * @param destPatch Index of destination patch.
		 * @param cacheLine Pointer to target cache line.
		 */
		void fillCacheLine(int destPatch, PatchCacheLine *cacheLine);

private:
	PatchRandomAccessEnumerator *patchEnumerator_;
	Matrix host, device;

#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__) 
#else
		Display *dpy;
		Window win;
#endif
		void createGLWindow();

		void drawScene();
		void renderViewport(const GLint x, const GLint y, const Vertex &c, const Vertex &at, const Vector &up);
		void renderFullScene(int dest);
		map<unsigned,double> *getFF();

public:
		/**
		 * @brief Destructor, close window, free memory.
		 */
		~FormFactorEngine();

		static const int EDGE_1 = 256;	 ///< size (in pixels) of hemi-cube edge
		static const int EDGE_2 = 2*EDGE_1;	///< EDGE_1 * 2 (size of important area in hemicube)
		static const int EDGE_LENGTH = 3*EDGE_1;	 ///< size (pixels) of render viewport

private:
		double **ffcoefs;
};

#endif // FORMFACTORENGINE_H
