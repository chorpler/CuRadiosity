/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#include "FormFactorEngine.h"
#include "TriangleSet.h"
#include "PatchRandomAccessEnumerator.h"
#include "PatchCacheLine.h"
#include <cuPrintf.cu>

#include <GL/glu.h>

#define EPSILON 0.000001

#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__)
	#include <windows.h>
	#include <wingdi.h>
	#include <GL/glext.h>
#else
	#include <X11/Xlib.h>
	#include <GL/glx.h>
#endif

#include <math.h>
#ifndef M_PI
	#define M_PI		3.14159265358979323846
#endif
#ifndef EDGE_L1
	#define EDGE_L1 FormFactorEngine::EDGE_LENGTH
#endif
#ifndef SCREEN_SIZE
	#define SCREEN_SIZE EDGE_L1*EDGE_L1*3
#endif

//#include <cutil.h>
//#include <cutil_math.h>
//#include <shrUtils.h>
#include <iostream>
#include <string>
//#include <book.h>

using namespace std;

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>

//typedef struct {
//	int edge;
//	double* elements;
//} Matrix;

static void HandleError( cudaError_t err,
                         const char *file,
                         int line ) {
    if (err != cudaSuccess) {
        printf( "%s in %s at line %d\n", cudaGetErrorString( err ),
                file, line );
        exit( EXIT_FAILURE );
    }
}
#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ ))


#define HANDLE_NULL( a ) {if (a == NULL) { \
                            printf( "Host memory failed in %s at line %d\n", \
                                    __FILE__, __LINE__ ); \
                            exit( EXIT_FAILURE );}}

template< typename T >
void swap( T& a, T& b ) {
    T t = a;
    a = b;
    b = t;
}


void* big_random_block( int size ) {
    unsigned char *data = (unsigned char*)malloc( size );
    HANDLE_NULL( data );
    for (int i=0; i<size; i++)
        data[i] = rand();

    return data;
}

int* big_random_block_int( int size ) {
    int *data = (int*)malloc( size * sizeof(int) );
    HANDLE_NULL( data );
    for (int i=0; i<size; i++)
        data[i] = rand();

    return data;
}

// ffcoefs initialization replacement kernel
__global__ void ffcoefs_init_better(Matrix device, int e1)
{
   //int i = blockIdx.x;
   //int j = threadIdx.x;
	int i = blockIdx.y * blockDim.y + threadIdx.y;
	int j = blockIdx.x * blockDim.x + threadIdx.x;
	int e2 = e1 * 2;
	unsigned tw = -e1 + i;
	unsigned th = -e1 + j;
	unsigned R = e2;
	double cw = cos(M_PI * tw/(double)R);
	double ch = cos(M_PI * th/(double)R);
	device.elements[i * device.edge + j] = cw*ch;
	//device.elements[i * device.edge + j] = i + 0.1;
}

// ffcoefs initialization replacement kernel
__global__ void ffcoefs_init(double** ffcoefs1, int e1)
{
   //int i = blockIdx.x;
   //int j = threadIdx.x;

	int i = blockIdx.y * blockDim.y + threadIdx.y;
	int j = blockIdx.x * blockDim.x + threadIdx.x;
	int e2 = e1 * 2;
	unsigned tw = -e1 + i;
	unsigned th = -e1 + j;
	unsigned R = e2;
	double cw = cos(M_PI * tw/(double)R);
	double ch = cos(M_PI * th/(double)R);
	ffcoefs1[i][j] = cw*ch;


	//for(int i=0; i<EDGE_2; i++)
	//{
	//	ffcoefs[i] = new double[EDGE_2]; 
	//	for(int j=0; j<EDGE_2; j++)
	//	{
	//		unsigned tw = -EDGE_1 + i;
	//		unsigned th = -EDGE_1 + j;
	//		unsigned R = EDGE_2;
	//		double cw = cos( M_PI * tw/(double)R );
	//		double ch = cos( M_PI * th/(double)R );
	//		ffcoefs[i][j] = cw*ch;
	//	}
	//}


   //...code that uses i and j....

}

// Kernel definition 
// newloop<<<gridStuff, blkStuff>>>(screenCUDA, cudaMap.keys, cudaMap.data);
__global__ void newloop(fakeMap cudaMap, unsigned char *screenCUDA, Matrix device, int e1)
{   
	unsigned char r,g,b;
	unsigned clr;
	int resH = 768;
	int i = blockIdx.y * blockDim.y + threadIdx.y;
	int j = blockIdx.x * blockDim.x + threadIdx.x;
	int w = i + 128;
	int h = j + 128;
	int rowLength = e1 * 2;
	int b1, b2, b3, b4;
	b1 = blockIdx.x;
	b2 = blockIdx.y;
	b3 = threadIdx.x;
	b4 = threadIdx.y;

	//for(w=128;w<(128+512);w++)
	//for(h=128;h<(128+512);h++)
	//{ 
	//	b = screen[ 3*(w*resH+h) +0];
	//	g = screen[ 3*(w*resH+h) +1];
	//	r = screen[ 3*(w*resH+h) +2];
	//	clr = ((unsigned)r)+((unsigned)g<<8)+((unsigned)b<<16);

	//	screen[ 3*(w*resH+h) +0] = (unsigned char)(b*ffcoefs[w-128][h-128]);
	//	screen[ 3*(w*resH+h) +1] = (unsigned char)(g*ffcoefs[w-128][h-128]);
	//	screen[ 3*(w*resH+h) +2] = (unsigned char)(r*ffcoefs[w-128][h-128]);

	//	(*ffmap2)[clr] += ffcoefs[w-128][h-128];
	//}


// Kommented out on 2012-09-26 temporarily
	//map<unsigned,double> *ffmap = new map<unsigned,double>();
	int base = 3 * (w * resH + h);
	int twoToOne = i * rowLength + j;
	//if(b1 == 0 && b2 == 0 && b3 == 0 && b4 == 0) {
		//cuPrintf("Screencuda blkX,blkY,thdX,thdY [%d,%d,%d,%d]: %d\n", b1, b2, b3, b4, screenCUDA[base]);
	b = screenCUDA[ base +0];
	g = screenCUDA[ base +1];
	r = screenCUDA[ base +2];
	clr = ((unsigned)r)+((unsigned)g<<8)+((unsigned)b<<16);
	double base2 = device.elements[ twoToOne ];
	//if(b1 == 0 && b2 == 0 && b3 == 0 && b4 == 0) {
	//	cuPrintf("Screencuda blkX,blkY,thdX,thdY [%d,%d,%d,%d]: %d\n", b1, b2, b3, b4, device.elements[twoToOne]);
	//}
	//if(base2 > 0.0001) {
		//cuPrintf("Device is: %f\n", base2);
	//}
	screenCUDA[base +0] = (unsigned char)(b * base2);
	screenCUDA[base +1] = (unsigned char)(g * base2);
	screenCUDA[base +2] = (unsigned char)(r * base2);
	cudaMap.keys[twoToOne] = clr;
	cudaMap.data[twoToOne] = base2;
	//if(b1 == 0 && b2 == 0 && b3 == 0 && b4 == 0)
		//cuPrintf("Screencuda[%d]: %d\n", i, screenCUDA[base]);
	//}
	////screen[ 3*(w*resH+h) +0] = (unsigned char)(b*ffcoefs[w-128][h-128]);
	////screen[ 3*(w*resH+h) +1] = (unsigned char)(g*ffcoefs[w-128][h-128]);
	////screen[ 3*(w*resH+h) +2] = (unsigned char)(r*ffcoefs[w-128][h-128]);
	//
	//((*ffmap)[clr]) += /*1*/ffcoefs1[w-128][h-128];


	//int i = threadIdx.x;
	//int j = threadIdx.y;
	//int scrSize = w * h * 3;
	////scra1 = cudaMalloc(
	//scra1[i][j] = 2;/*A[i][j] + B[i][j];*/
}


dim3 block(32, 16);
dim3 grid(32, 16);

//kernel<<<grids, blocks, 1>>>()


/**
* @param destPatch Index of destination patch.
* @param cacheLine Pointer to target cache line.
*/
void FormFactorEngine::fillCacheLine(int destPatch, PatchCacheLine *cacheLine)
{
	PatchRandomAccessEnumerator &patchSet = *patchEnumerator_;

	renderFullScene(destPatch);
	map<unsigned,double> *ffmap = getFF();

	//float S = 256.0f*256.0f+4*256.0f*128.0f;
	float S = 196608.0f;
	
	map<unsigned,double>::iterator iter;
	for(iter = ffmap->begin(); iter != ffmap->end(); iter++ ) {
		if(iter->first != 0xffffff)
		{
				cacheLine->addPatch(iter->first, 2.0*iter->second/S);
			// this condition is due to not offscreen rendering!!!
			if(static_cast<unsigned>(patchSet.count()) > iter->first)
				cacheLine->addPatch(iter->first, 2.0*iter->second/S);
		}
	}
	
	delete ffmap;
}

map<unsigned,double> *FormFactorEngine::getFF()
{
	printf("Now in getFF.\n");
	//map<unsigned,double> *ffmap = new map<unsigned,double>();

	// We can't create a map in CUDA space, so let's create two arrays and use them as a pseudo-map.
	// One will be an unsigned array and one will be a double array.
	//unsigned *map1 = new unsigned[size];
	//double *map2 = new double[size];

	fakeMap cudaMap, localMap;

	//map<unsigned,double> *ffmap_cuda;

	unsigned resW = FormFactorEngine::EDGE_LENGTH;
	unsigned resH = FormFactorEngine::EDGE_LENGTH;

	int screenDataSize = resW * resH * 3;

	//map<unsigned,double> *dev1;
	localMap.keys = (unsigned*) malloc(sizeof(unsigned) * EDGE_2 * EDGE_2);
	localMap.data = (double*) malloc(sizeof(double) * EDGE_2 * EDGE_2);
	HANDLE_ERROR( cudaMalloc( &cudaMap.keys, sizeof(unsigned) * EDGE_2 * EDGE_2) );
	HANDLE_ERROR( cudaMalloc( &cudaMap.data, sizeof(double) * EDGE_2 * EDGE_2) );
	
	//unsigned resW = 1536;
	//unsigned resH = 1536;
	
	unsigned char *screen = new unsigned char[screenDataSize];
	unsigned char *screenCUDA;
	unsigned char *screenHost = new unsigned char[screenDataSize];

	unsigned w, h, clr;

	unsigned char r,g,b;

	glReadPixels(0, 0, resW, resH, GL_BGR, GL_UNSIGNED_BYTE, screen); 

	// CUDAIZE STARTING HERE
	HANDLE_ERROR( cudaMalloc( &screenCUDA, sizeof(unsigned char) * screenDataSize ) );

	HANDLE_ERROR( cudaMemcpy( screenCUDA, screen, sizeof(unsigned char) * screenDataSize, cudaMemcpyHostToDevice) );
	dim3 gridStuff(16, 16);
	dim3 blkStuff(32, 32);
	// void newloop(fakeMap cudaMap, unsigned char *screenCUDA, Matrix device, int e1)
	//cudaPrintfInit();
	newloop<<<blkStuff, gridStuff>>>(cudaMap, screenCUDA, device, EDGE_1);
	//cudaPrintfDisplay(stdout, true);
	//cudaPrintfEnd();
	HANDLE_ERROR(cudaMemcpy( localMap.keys, cudaMap.keys, sizeof(unsigned) * EDGE_2 * EDGE_2, cudaMemcpyDeviceToHost));
	HANDLE_ERROR(cudaMemcpy( localMap.data, cudaMap.data, sizeof(double) * EDGE_2 * EDGE_2, cudaMemcpyDeviceToHost));
	HANDLE_ERROR(cudaMemcpy( screenHost, screenCUDA, sizeof(unsigned char) * screenDataSize, cudaMemcpyDeviceToHost));

	//for(int a = 0; a < screenDataSize; a++) {
	//	if(screen[a] != screenHost[a]) {
	//		printf("Screen[%d]: %d. Cuda: %d.\n", a, screen[a], screenHost[a]);
	//	}
	//}

	//std::cout << "Done with CUDA memcpy!" << endl;
	// After kernel loopa executes, the fakeMap will have the 

	map<unsigned,double> *ffmap = new map<unsigned,double>();
	map<unsigned,double> *ffmap2 = new map<unsigned,double>();
	for(int i = 0; i < EDGE_2 * EDGE_2; i++) {
		(*ffmap)[localMap.keys[i]] += localMap.data[i];
		//if(i%10000 == 0)printf("Localmap key %d, + %f, now %f.\n", localMap.keys[i], localMap.data[i], (*ffmap)[localMap.keys[i]]);
	}

	//int w, h;
	//for(w=128;w<(128+512);w++)
	//	for(h=128;h<(128+512);h++)
	//	{ 
	//		b = screen[ 3*(w*resH+h) +0];
	//		g = screen[ 3*(w*resH+h) +1];
	//		r = screen[ 3*(w*resH+h) +2];
	//		clr = ((unsigned)r)+((unsigned)g<<8)+((unsigned)b<<16);

	//		screen[ 3*(w*resH+h) +0] = (unsigned char)(b*ffcoefs[w-128][h-128]);
	//		screen[ 3*(w*resH+h) +1] = (unsigned char)(g*ffcoefs[w-128][h-128]);
	//		screen[ 3*(w*resH+h) +2] = (unsigned char)(r*ffcoefs[w-128][h-128]);

	//		((*ffmap2)[clr]) += ffcoefs[w-128][h-128];
	//	}

		//map<unsigned,double>::iterator it;
		////for(int i = 0; i < ffmap2.size(); i++) {
		//int i = 0;
		//printf("ffmap2 size: %d.  ffmap size: %d.\n", ffmap2->size(), ffmap->size());
		//for(it = ffmap2->begin(); it != ffmap2->end(); it++) {
		//	unsigned key = it->first;
		//	double dat = it->second;
		//	//if((ffmap2[i] - (*ffmap)[i]) > EPSILON) {
		//	if(((*ffmap2)[key] - (*ffmap)[key]) > EPSILON) {
		//		printf("%05d: CUDA ffmap[%d]: %f.  Real: %f. Iterator: %f.\n", i, key, (*ffmap)[key], (*ffmap2)[key], dat);
		//	}
		//	i++;
		//}
	//HANDLE_ERROR( cudaMemcpy( (void*)ffmap, (const void*)ffmap_cuda, sizeof(map<unsigned,double>) * sizeof(ffmap_cuda), cudaMemcpyDeviceToHost) );

	//HANDLE_ERROR( cudaMemcpy( (void*)screenCUDA, (const void*)screen, sizeoYeahf(unsigned char) * sizeof(screen), cudaMemcpyHostToDevice) );


		//glDrawPixels(resW, resH, GL_BGR, GL_UNSIGNED_BYTE, screen);

	//cudaFree(screenCUDA);
	//cudaFree(device.elements);
	//cudaFree(cudaMap.keys);
	//cudaFree(cudaMap.data);
	delete[] screen;
	//delete[] localMap.keys;
	//delete[] localMap.data;
	//delete &cudaMap;
	//delete &localMap;
	//delete screenHost;
	//delete screenCUDA;

	return ffmap;

}

__global__ void addSomething(int a, int b, int *c) {
	*c = a + b;
}

/**
 * @param  patchIterator
 */
FormFactorEngine::FormFactorEngine (PatchRandomAccessEnumerator *patchEnumerator):
	patchEnumerator_(patchEnumerator)
{
	createGLWindow();
	//ffcoefs = new double*[EDGE_2];
	ffcoefs = (double**)malloc(sizeof(double*) * EDGE_2);
	//double** ffcoefs_cuda;
	size_t pitch;
	int n;
	int *dev_n;
	std::cout << "Starting some CUDA stuff!" << endl;
	double* devPtr;
	//Matrix host, device;
	host.edge = EDGE_2;
	device.edge = EDGE_2;
	size_t size = EDGE_2 * EDGE_2 * sizeof(double);

	//HANDLE_ERROR( cudaMalloc( (void**)&dev_n, sizeof(int) ) );
	//HANDLE_ERROR( cudaMalloc( (void**)&ffcoefs_cuda, sizeof(double*) * EDGE_2 * EDGE_2 ) );
	//HANDLE_ERROR( cudaMallocPitch( &devPtr, &pitch,sizeof(double)*EDGE_2, EDGE_2) );
	HANDLE_ERROR( cudaMalloc( &device.elements, size ));

	host.elements = (double*)malloc(size);

	std::cout << "Malloc'ed some CUDA stuff (and a local array, hooray)! Calling a loop!" << endl;

	/*for(int i = 0; i < EDGE_2; i++)
		ffcoefs[i] = new double[EDGE_2];*/
	dim3 initgrid(32,32);
	dim3 initblk(16,16);
	//ffcoefs_init<<<initgrid, initblk>>>(ffcoefs_cuda, EDGE_1);
	//ffcoefs_init_better<<<initblk, initgrid>>>(device, EDGE_1);
	ffcoefs_init_better<<<initgrid, initblk>>>(device, EDGE_1);
	//addSomething<<<1,1>>>(2, 7, dev_n);

	std::cout << "Done looping, now we're copying back!" << endl;

	HANDLE_ERROR( cudaMemcpy( host.elements, device.elements, size, cudaMemcpyDeviceToHost) );
	//HANDLE_ERROR( cudaMemcpy( &ffcoefs, ffcoefs_cuda, sizeof(double*) * EDGE_2 * EDGE_2, cudaMemcpyDeviceToHost) );
	//HANDLE_ERROR( cudaMemcpy( &n, dev_n, sizeof(int), cudaMemcpyDeviceToHost) );

	//std::cout << "Done with cudaMemcpy!" << endl;
	 
	//cudaFree(device.elements);
	//printf("Hey, look, 2+7=%d!\n(That just indicates excitement, not factorial. [For Patrick])\n", n);
	//cudaFree(dev_n);

	//std::cout << "Done with cudaFree!" << endl;

	//double** ffcoefs_local = (double**) malloc(sizeof(double*) * EDGE_2);

	//for(int i=0; i<EDGE_2; i++)
	//{
	//	ffcoefs_local[i] = new double[EDGE_2]; 
	//	for(int j=0; j<EDGE_2; j++)
	//	{
	//		unsigned tw = -EDGE_1 + i;
	//		unsigned th = -EDGE_1 + j;
	//		unsigned R = EDGE_2;
	//		double cw = cos( M_PI * tw/(double)R );
	//		double ch = cos( M_PI * th/(double)R );
	//		ffcoefs_local[i][j] = cw*ch;
	//	}
	//}

	//ffcoefs = ffcoefs_local;
	
	//for(int i=0; i < EDGE_2; i++) {
	//	ffcoefs[i] = new double[EDGE_2];
	//	for(int j=0; j < EDGE_2; j++) {
	//		ffcoefs[i][j] = host.elements[i*EDGE_2+j];
	//		if((ffcoefs[i][j] - ffcoefs_local[i][j] > EPSILON) || ((i%100+j== 34))) // && (i % 50 == 0))
	//			printf("Cuda [%d, %d]: %f. Real: %f.\n", i, j, ffcoefs[i][j], ffcoefs_local[i][j]);
	//	}
	//}

	std::cout << "ffcoefs is now full of good stuff." << endl;
	//HANDLE_ERROR( cudaMemcpy( (void*)screenCUDA, (const void*)screen, sizeof(unsigned char) * sizeof(screen), cudaMemcpyHostToDevice) );

	//for(int i=0; i<EDGE_2; i++)
	//{
	//	ffcoefs[i] = new double[EDGE_2]; 
	//	for(int j=0; j<EDGE_2; j++)
	//	{
	//		unsigned tw = -EDGE_1 + i;
	//		unsigned th = -EDGE_1 + j;
	//		unsigned R = EDGE_2;
	//		double cw = cos( M_PI * tw/(double)R );
	//		double ch = cos( M_PI * th/(double)R );
	//		ffcoefs[i][j] = cw*ch;
	//	}
	//}
}

FormFactorEngine::~FormFactorEngine()
{
#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__) 
#else
	XCloseDisplay(dpy);
#endif
	for(int i=0; i<EDGE_2; i++)
		delete[] ffcoefs[i];
	delete[] ffcoefs;
}

#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
#else
#endif

void FormFactorEngine::createGLWindow()
{
#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__)
	HINSTANCE hInstance = NULL;

	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc= (WNDPROC)WndProc;
	wcex.cbClsExtra= 0;
	wcex.cbWndExtra= 0;
	wcex.hInstance= hInstance;
	wcex.hIcon= 0;
	wcex.hCursor= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName= 0;
	wcex.lpszClassName= "RadiosityRenderWindowClass";
	wcex.hIconSm= 0;
	
	RegisterClassEx(&wcex);
	
	HWND hWnd = CreateWindow("RadiosityRenderWindowClass", "Radiosity render window", 
		WS_POPUP, 0, 0, EDGE_LENGTH, EDGE_LENGTH, NULL, NULL, hInstance, NULL);
	
	HDC hDC = GetDC( hWnd );
	HGLRC  hglrc;
	
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;
    ZeroMemory( &pfd, sizeof( pfd ) );
    pfd.nSize = sizeof( pfd );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                  PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat( hDC, &pfd );
    SetPixelFormat( hDC, iFormat, &pfd );
	
	hglrc = wglCreateContext (hDC);
	wglMakeCurrent (hDC, hglrc);
	
	//ShowWindow(hWnd, SW_SHOW);
	//UpdateWindow(hWnd);
#else
	int attributeList[]={
		GLX_RGBA, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 16,
		None};

	dpy = XOpenDisplay(getenv("DISPLAY"));
	if(!dpy)
	{
		cerr << "Cannot open display!" << endl;
		exit(-1);
	}

	int screen;
	screen = DefaultScreen(dpy);

	XVisualInfo *vi;
	vi = glXChooseVisual(dpy, screen, attributeList);
	if(!vi)
	{
		cerr << "Cannot find visual with desired attributes!" << endl;
		exit(-1);
	}

	GLXContext cx;
	cx = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	if(!cx)
	{
		cerr << "Cannot create context!" << endl;
		exit(-1);
	}

	win = XCreateSimpleWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, EDGE_LENGTH,  EDGE_LENGTH, 0, 0, 0);

	XFree(vi);

	XMapWindow(dpy, win);

	XStoreName(dpy, win, "Computing form factors...");

	XSync(dpy,false);

	XEvent ev;
        memset(&ev,0,sizeof(ev)); 
        ev.type=ClientMessage; 
        ev.xclient.type = ClientMessage; 
        ev.xclient.message_type = XInternAtom(dpy, "_NET_WM_STATE", 0); 
        ev.xclient.display=dpy; 
        ev.xclient.window=win; 
        ev.xclient.format=32; 
        ev.xclient.data.l[0]= 1; 
        ev.xclient.data.l[1]=XInternAtom(dpy, "_NET_WM_STATE_STAYS_ON_TOP", 0);
        XLockDisplay(dpy); 
        XSendEvent(dpy, XDefaultRootWindow(dpy), 0, SubstructureRedirectMask | SubstructureNotifyMask, &ev); 

        ev.xclient.data.l[1]=XInternAtom(dpy, "_NET_WM_STATE_ABOVE", 0);
        XSendEvent(dpy, XDefaultRootWindow(dpy), 0, SubstructureRedirectMask | SubstructureNotifyMask, &ev); 
        XUnlockDisplay(dpy);

	XSync(dpy,false);

	glXMakeCurrent(dpy, win, cx);
#endif
	// this should be in some event ;-)
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, EDGE_LENGTH, EDGE_LENGTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective((double)90.0, (double)EDGE_LENGTH/(double)EDGE_LENGTH, (double)1e-3, (double)50.0);
	glMatrixMode(GL_MODELVIEW);
}

class Vector;
inline float operator* (const Vector &u, const Vector &v);

#ifndef NDEBUG
inline std::ostream& operator<< (std::ostream &out, const Vector &vect);
#endif
	
class Vector {
	public:
		// tohle by mohlo byt pozdeji private
		float dx,dy,dz;

	public:
		static Vector fromTriangle(Triangle &t) {
			return Vector(
					t.vertex[0],
		 			t.vertex[1],
					t.vertex[2]
					);
		}
		
		static float cos(const Vector &u, const Vector &v) {
			return (u*v)/(u.size()*v.size());
		}
		
		float& operator[](int i) {
			if(0==i)
				return dx;
			if(1==i)
				return dy;
			if(2==i)
				return dz;
			return(dx);
		}
		
		const float& operator[](int i) const {
			if(0==i)
				return dx;
			if(1==i)
				return dy;
			if(2==i)
				return dz;
			return(dx);
		}
		
		Vector(const float dx, const float dy, const float dz)
		{
			this->dx = dx;
			this->dy = dy;
			this->dz = dz;
		}

		Vector(const Vertex &a, const Vertex &b) {
			dx = a.x - b.x;
			dy = a.y - b.y;
			dz = a.z - b.z;
		}

		Vector(const Vector &u, const Vector &v) { // normalovy
			dx = u[1]*v[2] - v[1]*u[2];
			dy = u[2]*v[0] - v[2]*u[0];
			dz = u[0]*v[1] - v[0]*u[1];
		}
		
		Vector(const Vertex &a, const Vertex &b, const Vertex &c) {
			Vector u(b,a);
			Vector v(c,a);
			dx = u[1]*v[2] - v[1]*u[2];
			dy = u[2]*v[0] - v[2]*u[0];
			dz = u[0]*v[1] - v[0]*u[1];
		}
		
		float size() const {
			return sqrtf( dx*dx + dy*dy + dz*dz );
		}
/*
		Vertex operator+ (const Vertex &v) {
			return Vertex(v.x+dx, v.y+dy, v.z+dz);
		}
*/
};

inline float operator* (const Vector &u, const Vector &v) {
	return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}
#ifndef NDEBUG
inline std::ostream& operator<< (std::ostream &out, const Vector &vect) {
	out << "vect(" <<
			vect[0] << ", " <<
			vect[1] << ", " <<
			vect[2] << ")";
	return out;
}
#endif

inline Vertex operator+ (const Vertex &v, const Vector &n) {
	return Vertex(v.x+n.dx, v.y+n.dy, v.z+n.dz);
}

void FormFactorEngine::drawScene()
{
	PatchRandomAccessEnumerator &patchSet = *patchEnumerator_;
	glBegin(GL_TRIANGLES);
	for(unsigned i=0;i< static_cast<unsigned>(patchSet.count());i++)
	{
		Triangle &t = patchSet[i];
		glColor3ub((i), (i>>8),(i>>16));
		for(int j=0;j<3;j++)
			glVertex3f(t.vertex[j].x,t.vertex[j].y,t.vertex[j].z);
	}
	glEnd();
}

/**
* @param eye Position of camera
* @param center Direction of camera
* @param up Up vector of camera
*/
void gluLookAtV(const Vertex &eye, const Vertex &center, const Vector &up)
{
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.dx, up.dy, up.dz);
}

/**
* @param x Offset from left
* @param y Offset from bottom
* @param c Position of camera
* @param at Direction of camera
* @param up Up vector of camera
*/
void FormFactorEngine::renderViewport(const GLint x, const GLint y, const Vertex &c, const Vertex &at, const Vector &up)
{
	glViewport(x,y, 256,256);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (double)EDGE_LENGTH/(double)EDGE_LENGTH, 1e-3, 50);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAtV( c, at, up );
	drawScene();
}

/**
* @param dest Index of destination patch.
*/
void FormFactorEngine::renderFullScene(int dest)
{
	PatchRandomAccessEnumerator &patchSet = *patchEnumerator_;

	// clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// destination triangle
	Triangle &t0 = patchSet[dest];

	// center of this triangle
	Vertex c = Triangle::centerOf(t0);
	
	// normal vector and inverse normal vector of this triangle
	Vector norm = Vector::fromTriangle(t0);
	Vector norm_m(-norm.dx,-norm.dy,-norm.dz);
	
	Vector side(norm, Vector(1,2,3));
	if(0 == side.size())
	    side = Vector(norm, Vector(1,1,1)); // a neudelat Vector::operator= ??
	Vector side_m(-side.dx,-side.dy,-side.dz);
	
	// side vectors
	Vector vctD(/*t0.vertex[0], t0.vertex[1]*/side);
	Vector vctC(/*t0.vertex[1], t0.vertex[0]*/side_m);
	Vector vctA( vctD, norm );
	Vector vctB( vctC, norm );
	
	// points for directions of camera (top and 4 side)
	Vertex at = c+norm;
	Vertex atA = c+vctA;
	Vertex atB = c+vctB;
	Vertex atC = c+vctC;
	Vertex atD = c+vctD;
	
	// top
	renderViewport(256, 256, c, at, vctA);
	
	// 1. side
	renderViewport(256, 512, c, atA, norm_m);
	
	// opposite side
	renderViewport(256, 0, c, atB, norm);
	
	// left side
	renderViewport(0, 256, c, atC, vctA);
	
	// right side
	renderViewport(512, 256, c, atD, vctA);
	
	// rendercc
	glFlush();

	// only due to doublebuffering
#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__) 
#else
	glXSwapBuffers( dpy, win );
#endif
}
