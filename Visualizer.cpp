/*
* Copyright (C) 2007 TODO
*
* This file is part of rrv (Radiosity Renderer and Visualizer).
*
* rrv is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* rrv is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with rrv.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Visualizer.h"
#include "Triangle.h"
#include "TriangleSetExt.h"
#include "Scene.h"
#include <paramgl.h>
//#include <stdlib.h>
#include <fstream>
//#include <iostream>
//#include <unistd.h>
#include <stdio.h>
#include <time.h>

#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__) 
#include <GL/glext.h>
#include <GL/freeglut.h>
#else
#ifdef FREEGLUT
#include <GL/freeglut.h>
#endif
#endif

//void glPrint(int x, int y, const char *s, void *font)
//{
//	glRasterPos2f((GLfloat)x, (GLfloat)y);
//	int len = (int) strlen(s);
//	for (int i = 0; i < len; i++) {
//		glColor3f((GLfloat)1.0f, (GLfloat)1.0f, (GLfloat)1.0f);
//		glutBitmapCharacter(font, s[i]);
//	}
//}
//
//void glPrintShadowed(int x, int y, const char *s, void *font, float *color)
//{
//	glColor3f((GLfloat)1.0, (GLfloat)0.0, (GLfloat)0.0);
//	glPrint(x-1, y-1, s, font);
//
//	//glColor3fv((GLfloat *) color);
//	glColor3f((GLfloat)1.0, (GLfloat)1.0, (GLfloat)0.5);
//	glPrint(x, y, s, font);
//}


TriangleSetExt * Visualizer::patchSequenceEnumerator_;
TriangleSetExt* Visualizer::patchSequenceEnumerator2_;
Visualizer::TColorInput Visualizer::colorInput;

GLuint Visualizer::scene_;
GLuint Visualizer::scene2_;

int   Visualizer::xnew, Visualizer::ynew, Visualizer::znew;                  /* actual position */
int   Visualizer::xold, Visualizer::yold, Visualizer::zold;                  /* old position */
int   Visualizer::xx1, Visualizer::yy1, Visualizer::zz1;                     /* mouse position*/
int   Visualizer::mouseState;                        /* mouse button state */
float   Visualizer::xshift, Visualizer::yshift;                    /* shifting in space*/

float Visualizer::fov;                               /* field of view */
float Visualizer::near_plane;                        /* trim plain */
float Visualizer::far_plane;                         /* farther trim plain */
float Visualizer::line_width;                        /* width of line */
int   Visualizer::WindowWidth=700;                       /* width and height of window */
int   Visualizer::WindowHeight=700;
int   Visualizer::ObjectType;                        /* type of paint object */
int   Visualizer::Solid;                             /* fill or wireframe model */
vector<string> Visualizer::filelist;

int counter = 0;

bool  Visualizer::doScreenshot=false;
const char* Visualizer::screenshotFileName="screenshot.tga";

string Visualizer::file1;
string Visualizer::file2;
int Visualizer::currentFile;
int Visualizer::totalFiles;


string Visualizer::getFile(int which) {
	if(which == 1)
		return Visualizer::file1;
	else
		return Visualizer::file2;
}


void Visualizer::setFile(int which, string file_) {
	if(which == 1)
		Visualizer::file1 = file_;
	else
		Visualizer::file2 = file_;
}

void Visualizer::setFileList(vector<string> filelist_) {
	Visualizer::filelist = filelist_;
}

/**
* This method is call on redisplay of winodow
*/
void Visualizer::onDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* clear color buffer */
	glMatrixMode(GL_MODELVIEW);                   /* will be change of model matrix */
	glLoadIdentity();                             /* load unit matrix */

	glTranslatef(xshift, yshift, -50.0f);         /* shift of model farther from camera */
	glTranslatef(xshift, yshift, znew);
	glRotatef(ynew, 1.0, 0.0, 0.0);               /* rotation of object according mouser cursor */
	glRotatef(xnew, 0.0, 1.0, 0.0);
	//		glColor3f(1.0f, 1.0f, 1.0f);
	//glPrint(10, 10, "Testing 1-2-3", GLUT_BITMAP_TIMES_ROMAN_24);

	glCallList(scene_);                            /* draw my scene */
	//glColor3f(1.0f, 1.0f, 1.0f);
	//glPrint(10, 10, "Testing 1-2-3", GLUT_BITMAP_TIMES_ROMAN_24);

	glFlush();                                    /* do all directions */
	glutSwapBuffers();                            /* swap of buffer */

	//std::cout << "Called ondisplay " << counter++ << std::endl;
	//glColor3f(1.0f, 1.0f, 1.0f);
	//glPrint(10, 10, "Testing 1-2-3", GLUT_BITMAP_TIMES_ROMAN_24);
	//glRasterPos2f(10.0f, 10.0f);
	//const unsigned char* bob = (unsigned char*)"Testing 1-2-3";
	//glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, bob);
	if(doScreenshot)
	{
		//		sleep(1); // FIXME:remove this line 
		saveScreenshot(screenshotFileName);
		exit (0);
	}
}


/**
* Configuration system of coordinates according window size
*/
void Visualizer::onReshape(int w, int h)
{
	glViewport(0, 0, w, h);                       /* visibility area */
	glMatrixMode(GL_PROJECTION);                  /* projection matrix */
	glLoadIdentity();                             /* load matrix projection */
	gluPerspective(fov, (double)w/(double)h, near_plane, far_plane);
	glMatrixMode(GL_MODELVIEW);                   /* model matrix */
	WindowWidth = w;                              /* save window size */
	WindowHeight = h;
}


/**
* This method is call on press of key
*/
void Visualizer::onKeyboard(unsigned char key, int, int)
{
	if (key>='A' && key<='Z') key+='a'-'A';       /* convert upper char on lower */

	switch (key) {                                /* press key */
	case '1' :                                  	/* show vertex */
		glPolygonMode(GL_FRONT, GL_POINT);
		glPolygonMode(GL_BACK, GL_POINT);
		glutPostRedisplay();
		break;
	case '2' :                                  	/* show wireframe */
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		glutPostRedisplay();
		break;
	case '3' :                                  	/* show fill polygons */
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
		glutPostRedisplay();
		break;
	case '5' :                                  	/* decrease vision angle */
		if (fov>0) fov--;
		onReshape(WindowWidth, WindowHeight);
		glutPostRedisplay();
		break;
	case '6' :
		if (fov<180) fov++;                     /* increase vision angle */
		onReshape(WindowWidth, WindowHeight);
		glutPostRedisplay();
		break;
	case '7' :                                  	/* zoom in trim plain */
		if (line_width > 0) line_width -= 0.5;	
		glLineWidth(line_width);                
		glPointSize(line_width);                
		onReshape(WindowWidth, WindowHeight);
		glutPostRedisplay();
		break;
	case '8' :                                  	/* zoom out trim plain */
		if (line_width < 10) line_width += 0.5;
		glLineWidth(line_width);                
		glPointSize(line_width);                
		onReshape(WindowWidth, WindowHeight);
		glutPostRedisplay();
		break;
	case 's' :				    	/* print to bitmap - screenshot */
		saveScreenshot(screenshotFileName);
		break;
	case 27 :                                   	/* Escape */
	case 'q' :
		exit(0);                                /* exit */
		break;
	case 'm' :
		colorInput = INTERPOLATE_RADIOSITY;	/* interpolate radiosity */
		createCallList();
		glutPostRedisplay();
		cout << "Display mode: Interpolated Radiosity" << endl;
		break;
	case 'b' :
		colorInput = INTERPOLATE_RADIOSITY_RAW;	/* interpolate radiosity */
		createCallList();
		glutPostRedisplay();
		cout << "Display mode: Interpolated Radiosity Raw" << endl;
		break;
	case 'v' :
		colorInput = RADIOSITY;	/* radiosity */
		createCallList();
		glutPostRedisplay();
		cout << "Display mode: Radiosity" << endl;
		break;
	case 'n' :
		colorInput = RADIOSITY_LAST;	/* radiosity */
		createCallList();
		glutPostRedisplay();
		cout << "Display mode: Radiosity (Last)" << endl;
		break;
	case 'z' :
		colorInput = REFLECTIVITY;	/* reflectivity */
		createCallList();
		glutPostRedisplay();
		cout << "Display mode: Reflectivity" << endl;
		break;
	case 'x' :
		colorInput = EMISSION;	/* emission */
		createCallList();
		glutPostRedisplay();
		cout << "Display mode: Emission" << endl;
		break;
	case 'c' :
		colorInput = REFLECT_EMISS;	/* reflectivity + emission */
		createCallList();
		glutPostRedisplay();
		cout << "Display mode: Emission + Reflectivity" << endl;
		break;
	case 'f' :
		glutFullScreen();                      	/* fullscrean */
		break;
	case 'w' :
		glutReshapeWindow(WindowWidth, WindowHeight);              /* go back to window */
		break;
	default:
		break;
	}
}


/**
* Method is call on press special key
*/
void Visualizer::onSpecial(int key, int, int)
{
	int curScene = getCurrentFileNum();
	switch(key)
	{
	case GLUT_KEY_UP:
		if (yshift>-50) yshift--;                       /* shift up */
		onReshape(WindowWidth, WindowHeight);
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		if (yshift<50) yshift++;                       /* shift down */
		onReshape(WindowWidth, WindowHeight);
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		//if (xshift<50) xshift++;                       /* shift left */
		//onReshape(WindowWidth, WindowHeight);
		//Visualizer::
		curScene--;
		if(curScene < 0)
			curScene = 0;
		else
		{
			cout << "Changing to " << curScene << "..." << endl;
			changeScene(curScene);
		}
		createCallList();
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		//if (xshift>-50) xshift--;                       /* shift right */
		//onReshape(WindowWidth, WindowHeight);
		curScene++;
		if(curScene > totalFiles)
			curScene = totalFiles;            
		else
		{
			cout << "Changing to " << curScene << "..." << endl;
			changeScene(curScene);
		}
		createCallList();
		glutPostRedisplay();
		break;
	}	
}


/**
* Method is call on press mouse button
*/
void Visualizer::onMouseClick(int button, int, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {               /* left button activate of rotation */
		if (mouseState == GLUT_DOWN) {          /* on press */
			mouseState = 1;                 /* set state for function motion */
			xx1 = x;                        /* remember position of mouse cursor */
			yy1 = y;
		} else {                                /* GLUT_UP */
			mouseState = 0;                 /* normal state */
			xold = xnew;                    /* remember new start */
			yold = ynew;
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {              /* right mouse button activating shift */
		if (mouseState == GLUT_DOWN) {          /* pri stlaceni */
			mouseState = 2;                 /* nastaveni pro funkci motion */
			zz1 = y;                        /* remember position of mouse cursor */
		}
		else {
			mouseState = 0;
			zold = znew;                    /* remember new start */
		}
	}
	glutPostRedisplay();                            /* redraw scene */
}

/**
* Method is call on mouse motion
*/
void Visualizer::onMouseMotion(int x, int y)
{
	if (mouseState == 1) {                          /* state of position */
		xnew = xold+x-xx1;                      /* count new position */
		ynew = yold+y-yy1;
		glutPostRedisplay();                    /* redraw scene */
	}
	if (mouseState == 2) {                          /* shift state */
		znew = zold+y-zz1;                      /* count new position */
		glutPostRedisplay();                    /* redraw scene */
	}
}

/**
* Method is called on mouse wheel motion
*/
void Visualizer::mouseWheel(int button, int dir, int x, int y)
{
	int mod = glutGetModifiers();
	if (dir > 0)
	{
		if(mod == GLUT_ACTIVE_SHIFT)
		{
			fov += 10.0f;
		} else {
			//windSpeed = windSpeed + 10.0f;
			fov += 1.0f;
		}
		if(fov > 180.0f)
			fov = 180.0f;
		//	A = 1e-3f;

		//cout << printf("A: %f\n", A);
		//reshape(512,512);
		// Zoom in
	}
	else
	{
		// Zoom out
		if(mod == GLUT_ACTIVE_SHIFT)
		{
			if(fov > 0.0f)
				fov -= 10.0f;
			else
				fov = 0.0f;
		} else {
			if(fov > 0.0f)
				fov -= 1.0f;
			else
				fov = 0.0f;
		}
		//printf("A: %f\n", A);
	}
	//cutilDeviceReset();

	//reshape(512,512);
	//std::cout << "fov: " << fov << std::endl;
	onReshape(WindowWidth, WindowHeight);
	glutPostRedisplay();
}

/**
* Method for create a window
*/
void Visualizer::createWindow(const char * title)
{
	xnew=0; ynew=0; znew=0;                   /* actual position */
	xold=0; yold=0; zold=0;                   /* old position */
	xx1=0; yy1=0; zz1=0;                      /* mouse position*/
	mouseState=0;                             /* mouse button state */
	xshift=0; yshift=0;                       /* shifting in space*/

	xnew=0; ynew=0; znew=0;                   /* actual position */
	xold=0; yold=0; zold=0;                   /* old position */
	xx1=0; yy1=0; zz1=0;                      /* mouse position*/
	mouseState=0;                             /* mouse button state */
	xshift=0; yshift=0;                       /* shifting in space*/

	fov=45.0;                                 /* field of view */
	near_plane=1;                             /* trim plain */
	far_plane=1000.0;                         /* farther trim plain */
	line_width=1.0;                           /* width of line */
	WindowWidth=1024;                         /* width and height of window */
	WindowHeight=768;
	ObjectType=0;                             /* type of paint object */
	Solid=1;                                  /* fill or wireframe model */

	char * dummy_argv[1];
	dummy_argv[0] = const_cast<char *>("run");
	int dummy_argc = 1;

	glutInit(&dummy_argc, dummy_argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE ); /* graphic mod of window */

	glutInitWindowSize(WindowWidth,WindowHeight); /* original window size */
	glutInitWindowPosition(10,10);                /* original window position */
	glutCreateWindow(title);                      /* create window */

	glutDisplayFunc(onDisplay);                   /* set function for redisplay */
	glutReshapeFunc(onReshape);                   /* set function for change of size window */
	glutKeyboardFunc(onKeyboard);                 /* set function for press key */
	glutSpecialFunc(onSpecial);
	glutMouseFunc(onMouseClick);                  /* set function for press mouse button */
	glutMotionFunc(onMouseMotion);                /* set function for mouse move */
	glutMouseWheelFunc(mouseWheel);               /* set function for mouse wheel */
	glClearColor(0.0, 0.0, 0.0, 0.0);             /* color for clearing of color-buffer */
	glClearDepth(1.0f);                           /* color for clearing of z-buffer */
	glEnable(GL_DEPTH_TEST);                      /* configure function for testing value in z-buffer */
	glDepthFunc(GL_LESS);                         /* select function */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); /* improvement display */

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);             /* configure draw of fill polygons */
	//glCullFace(GL_FRONT);

	glLineWidth(line_width);                      /* line width */
	glPointSize(line_width);

	//glEnable(GL_MULTISAMPLE);
#ifdef FREEGLUT
#ifdef GLUT_AUX
	int *sampleNumbers = NULL;
	int sampleNumbersSize = 0;
	sampleNumbers = glutGetModeValues(GLUT_MULTISAMPLE, &sampleNumbersSize);
	if(sampleNumbers != NULL)
	{
		glutSetOption(GLUT_MULTISAMPLE, sampleNumbers[sampleNumbersSize - 1]);
		printf("Multisampling with %i samples.\n", sampleNumbers[sampleNumbersSize - 1]);
		free(sampleNumbers);
	}
	else
	{
		printf("Multisampling is not available.\n");
	}
#endif
#endif

	scene_ = glGenLists(1);                        /* get number of calllist */
	cout << "glgenlists(1) is " << scene_ << endl;
	createCallList();                             /* initialization */

	glutMainLoop();
}


/**
* Visualization
*/
void Visualizer::visualize(const char* setTitle, TriangleSetExt * patchSequenceEnumerator)
{
	std::cout << "There were " << totalFiles << " total steps, " << file1 << " and " << file2 << "." << std::endl;
	patchSequenceEnumerator_ = patchSequenceEnumerator;
	createWindow(setTitle);
}

/**
*  Screenshot
*/
void Visualizer::takeScreenshot(char * filename, TriangleSetExt * patchSequenceEnumerator)
{
	doScreenshot = true;
	screenshotFileName = filename;

	patchSequenceEnumerator_ = patchSequenceEnumerator;	
	createWindow("Screenshot");
}

/**
* Method for save Screenshot of scene to *.tga
*/
void Visualizer::saveScreenshot(const char * filename)
{
	int resolutionW = WindowWidth;
	int resolutionH = WindowHeight;

	//char filename[]="screenshot.tga"; 
	int size=resolutionW*resolutionH*4; 

	unsigned char *screen = (unsigned char*)malloc(size*sizeof(unsigned char)); 

	FILE *fout; 
	int i, j; 
	int width=resolutionW; 
	int height=resolutionH; 
	unsigned char tgaHeader[18]={0x00,
		0x00, 
		0x02, //truecolor 
		0x00, 0x00, 
		0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, //position
		0x00, 0x00, 0x00, 0x00, //width and height 
		0x20, //bpp 
		0x20}; //orientation

	memcpy(tgaHeader+12, &width, 2); 
	memcpy(tgaHeader+14, &height, 2); 

	// reading pixels in glut window
	glReadPixels(0, 0, resolutionW, resolutionH, GL_BGRA_EXT, GL_UNSIGNED_BYTE, screen); 

	// open file
	if((fout=fopen(filename, "wb")) == NULL)
		std::cerr << "Error: Can't open file \n";

	// write to file
	if (fout) 
	{ 
		fwrite(tgaHeader, 18, 1, fout); 

		for (j=resolutionH; j>0; j--) 
		{ 
			for (i=0; i<resolutionW*4; i++) 
			{ 
				fputc(screen[((j*resolutionW-resolutionW)*4)+i], fout); 
			} 
		} 
		fclose(fout); 
	} 

	free(screen); 
	screen=NULL; 
}

// 		static int currentFile;
//		static int totalFiles;

//
int Visualizer::getCurrentFileNum() {
	return currentFile;
}

//
void Visualizer::setCurrentFileNum(int fileNum_) {
	currentFile = fileNum_;
}

int Visualizer::getTotalFileNum() {
	return totalFiles;
}

void Visualizer::setTotalFileNum(int fileNum_) {
	totalFiles = fileNum_;
}

/**
* Method draw triangles in patchEnumerator to scene
*/
void Visualizer::start(TriangleSetExt *pTset, TColorInput colorInput)
{
	TriangleSetExt &tset = *pTset;
	std::cout << "Triangles: " << tset.count() << std::endl;
	float dx = 0.0f, dy = 0.0f;
	glBegin(GL_TRIANGLES);
	for(unsigned int j=0; j<tset.count(); j++) {
		TriangleExt &te = tset[j];
		for(int i=0; i<3; i++) {
			//std::cout << "Now taking care of triangle " << j << "." << i << "..." << std::endl;
			Color &color = te.vertexColor[i];
			Color &color2 = te.vertexColorRaw[i];
			Vertex &vertex = te.vertex[i];

			switch (colorInput)
			{
			case INTERPOLATE_RADIOSITY:
				//std::cout << "Interpolate_radiosity " << color << " " << vertex << std::endl;
				//if(te.vertexColor[i] == Color(0.0f,0.0f,0.0f))
				//{
				//	te.vertexColor[i].r = 0.1;
				//	te.vertexColor[i].g = 0.1;
				//	te.vertexColor[i].b = 0.1;
				//}
				//if(j == 15 && i == 0)
				//{
				//	std::cout << "Tri" << j << "-" << i << ": " << te.vertex[i] << std::endl;
				//	dx = te.vertex[i].x;
				//	dy = te.vertex[i].y;
				//	te.vertexColor[i].r *= 1.2;
				//	te.vertexColor[i].g *= 1.2;
				//	te.vertexColor[i].b *= 1.2;

				//	//te.vertex[i].x += 1.0;
				//	//te.vertex[i].y += 1.0;
				//	te.vertex[i].z += 1.0;
				//}
				glColor3f(
					color.r,
					color.g,
					color.b);
				//glPrint(te.vertex[i].x, te.vertex[i].y, "test", GLUT_BITMAP_8_BY_13);
				break;

			case INTERPOLATE_RADIOSITY_RAW:
				//std::cout << "Interpolate_radiosity_raw" << std::endl;
				glColor3f(
					color2.r,
					color2.g,
					color2.b);
				break;

			case RADIOSITY_LAST:
				//std::cout << "radiosity_last" << std::endl;
				glColor3f(
					te.radiosityLast.r,
					te.radiosityLast.g,
					te.radiosityLast.b);
				break;

			case RADIOSITY:
				//std::cout << "radiosity" << std::endl;
				te.radiosity.r *= 1.1;
				te.radiosity.g *= 1.1;
				te.radiosity.b *= 1.1;

				glColor3f(
					te.radiosity.r,
					te.radiosity.g,
					te.radiosity.b);
				break;

			case REFLECTIVITY:
				//std::cout << "reflectivity" << std::endl;
				glColor3f(
					te.reflectivity.r, 
					te.reflectivity.g, 
					te.reflectivity.b);
				break;

			case EMISSION:
				//std::cout << "emission" << std::endl;
				glColor3f(
					te.emission.r, 
					te.emission.g, 
					te.emission.b);

				break;

			case REFLECT_EMISS:
				//std::cout << "reflect_emiss" << std::endl;
				glColor3f(
					te.reflectivity.r + te.emission.r, 
					te.reflectivity.g + te.emission.g, 
					te.reflectivity.b + te.emission.b);

				break;
			}
			glVertex3f(
				vertex.x,
				vertex.y,
				vertex.z);
		}
	}	
	glEnd();
	glFlush();                                    /* do all directions */
	glutSwapBuffers();                            /* swap of buffer */

	//char tmp1[6];
	//int digits = 0;
	//
	//const char* tmp2 = (const char*) tmp1;
	//Vertex v1, v2, v3, bc;
	//for(unsigned int j=0; j<tset.count(); j++) {
	//	TriangleExt &te = tset[j];
	//	digits = sprintf(tmp1, "T%02d", j);
	//	v1 = te.vertex[0];
	//	v2 = te.vertex[1];
	//	v3 = te.vertex[2];
	//	bc.x = (v1.x + v2.x + v3.x) / 3;
	//	bc.y = (v1.y + v2.y + v3.y) / 3;
	//	bc.z = (v1.z + v2.z + v3.z) / 3;
	//	glColor3f(1.0f, 1.0f, 0.5f);
	//	//glPrint(te.vertex[0].x, te.vertex[0].y + 2, te.vertex[0].z, tmp2, GLUT_BITMAP_HELVETICA_10);
	//	glPrint(bc.x, bc.y, bc.z, tmp2, GLUT_BITMAP_HELVETICA_10);
	//}
}


/**
* Method for create calllist
*/
void Visualizer::createCallList(void)
{
	glNewList(scene_,GL_COMPILE);

	start(patchSequenceEnumerator_, colorInput);

	glEndList();
}

/**
* Load previous file
*/
void Visualizer::changeScene(int newScene)
{
	int new1 = newScene - 1;
	std::ostringstream streamName;
	totalFiles = filelist.size();
	
	
	if(newScene == totalFiles)
	{
		streamName << file2;
	} else {
		streamName << file1 << "-step" << std::setw(5) << std::setfill('0') << new1 << ".xml";
	}

	std::cout << "Supposed to switch to scene " << streamName.str() << ", " << newScene << " of " << totalFiles << "." << std::endl;
	
	cout << "Going to change to " << streamName.str() << "..." << endl;
	if(newScene < 1 || newScene > totalFiles)
	{
		std::cout << "Cannot change to file " << streamName.str() << ".  Probably doesn't exist." << std::endl;
		return;
	}

	Scene scene2;

	try
	{
		scene2.load(streamName.str());
	}
	catch( XML::XMLException &e )
	{
		std::cerr << "Error switching to scene " << streamName << "!" << std::endl;
		std::cerr << e.what() << std::endl;
		exit( 0 );
	}

	std::cout << "Supposed to switch to scene " << streamName.str() << ", " << newScene << " of " << totalFiles << "." << std::endl;
	patchSequenceEnumerator_ = scene2.dumpForVis();

	//glNewList(scene2_, GL_COMPILE);

	//start(patchSequenceEnumerator_, colorInput);

	//glEndList();

	setCurrentFileNum(newScene);
	//std::cout << "Switched to scene " << streamName << ", " << newScene << " of " << totalFiles << "." << std::endl;
	return;
}

