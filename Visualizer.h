/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#ifndef GLUTWINDOW_H
#define GLUTWINDOW_H

/**
 * @file Visualizer.h
 * @brief Class Visualizer - scene visualization
 * @author xhefka00
 * @date 2007-11-17
 */

#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__) 
	#include <windows.h>
#else
#endif

#include <GL/glut.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
using namespace std;

#ifndef BUILDING_DOX
class TriangleSetExt;
#endif

/**
 * @brief Scene visualization
 */
class Visualizer{
	public:
		/**
		 * @attention This method ends using exit() function and never returns!
		 * @brief Initiate interactive scene visualization
		 * @param title Title of visualizer's window
		 * @param te Filled patch container with computed patch and vertex colors.
		 */
		static void visualize(const char *title, TriangleSetExt *te);	/* visualize of scene in GLUT window */
		
		/**
		 * Using TGA image format to save screenshot.
		 * @attention This method ends using exit() function and never returns!
		 * @brief Take screenshot from default camera position and save to file.
		 * @param fileName Name of file to save screenshot to. Existing file will be replaced.
		 * @param te Filled patch container with computed patch and vertex colors.
		 */
		static void takeScreenshot(char *fileName, TriangleSetExt *te);		/* do screenshot of scene to file .tga */
	
		/**
		 * get a file name
		 * @brief Return a filename, either of the filein our fileout.
		 * @param which 1 to get filein name, 2 for fileout name.
		 */
		static string getFile(int which);

		/**
		 * set a file name
		 * @brief Save a filename, either of the filein our fileout.
		 * @param which An integer to specify which file it is -- 1 for filein, 2 for fileout.
		 * @param file_ The file name to save.
		 */
		static void setFile(int which, string file_);


		static int getCurrentFileNum(void);
		static int getTotalFileNum(void);
		static void setCurrentFileNum(int filenum_);
		static void setTotalFileNum(int filenum_);
		static void setFileList(std::vector<std::string> filelist_);

	private:
		typedef enum { INTERPOLATE_RADIOSITY, INTERPOLATE_RADIOSITY_RAW, RADIOSITY_LAST, RADIOSITY, REFLECTIVITY, EMISSION, REFLECT_EMISS } TColorInput;
	
		static TriangleSetExt * patchSequenceEnumerator_;
		static TriangleSetExt* patchSequenceEnumerator2_;
		static TColorInput colorInput;
	
		static GLuint scene_;
		static GLuint scene2_;
	
		static int   xnew, ynew, znew;                 	/* actual position */
		static int   xold, yold, zold;                 	/* old position */
		static int   xx1, yy1, zz1;                   	/* mouse position*/
		static int   mouseState;                     	/* mouse button state */
		static float   xshift, yshift;                  	/* shifting in space*/
	
		static float fov;                              	/* field of view */
		static float near_plane;                      	/* trim plain */
		static float far_plane;                       	/* farther trim plain */
		static float point_size;                      	/* size of point */
		static float line_width;                      	/* width of line */
		static int   WindowWidth;                     	/* width and height of window */
		static int   WindowHeight;
		static int   ObjectType;                      	/* type of paint object */
		static int   Solid;                             /* fill or wireframe model */
		
		static bool doScreenshot;
		static const char * screenshotFileName;

		static string file1;
		static string file2;
		static int currentFile;
		static int totalFiles;
		static std::vector<std::string> filelist;
	
		// callbacks
		static void onDisplay(void);
		static void onReshape(int w, int h);
		static void onKeyboard(unsigned char key, int x, int y);
		static void onSpecial(int key, int, int);
		static void onMouseClick(int button, int state, int x, int y);
		static void onMouseMotion(int x, int y);
		static void mouseWheel(int button, int dir, int x, int y);
	
		static void start(TriangleSetExt * patchEnumerator, TColorInput colorInput);
	
		Visualizer();
	
		static void createWindow(const char * title);						
		static void saveScreenshot(const char * filename);
		static void createCallList(void);
		static void changeScene(int sceneNumber);
};

#endif
