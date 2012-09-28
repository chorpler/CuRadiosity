/*
 * CUDARadiosity 0.92a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#define CUDARADIOSITY 1
#define NDEBUG 1
#include "Scene.h"
#include "TriangleSetExt.h"
#include "Visualizer.h"
#include "VisualizeArguments.h"
#include "RadiosityRenderer.h"
#include "ConsoleProgressIndicator.h"
#include "ComputeArguments.h"

#include <string>
#include <time.h>

#ifndef NDEBUG
#	include <iostream>
#endif

#include <iomanip>
#include <regex>
#include <Windows.h>




// CUDA-C includes
#ifdef CUDARADIOSITY
// Shared Utilities (QA Testing)
#include <shrUtils.h>
#include <shrQATest.h>
#include <cuda.h>
#include <cuda_runtime_api.h>

int *pArgc = NULL;
char **pArgv = NULL;

// This function wraps the CUDA Driver API into a template function
template <class T>
inline void getCudaAttribute(T *attribute, CUdevice_attribute device_attribute, int device)
{
    CUresult error = 	cuDeviceGetAttribute( attribute, device_attribute, device );

    if( CUDA_SUCCESS != error) {
        fprintf(stderr, "cuSafeCallNoSync() Driver API error = %04d from file <%s>, line %i.\n",
                error, __FILE__, __LINE__);
        exit(-1);
    }
}
#endif


//using namespace std::tr1;


namespace {
	TriangleSetExt *globalTriangleSet= 0;
	
	// Destroy global triangle set after exit() call
	void destroyTriangleSet() {
#ifndef NDEBUG
		std::cout << ">>> Freeing memory ... " << std::flush;
#endif
		delete globalTriangleSet;
#ifndef NDEBUG
		std::cout << "ok" << std::endl;
#endif
	}
}

// Inline class definition for saving output
class ContinousSaver: public ProgressObserverMultiStep {
		Scene *scene_;
		RadiosityRenderer *renderer_;
		std::string nameBase_;
		int perStepFreq_;
		int perStepCounter_;
 		int stepSkip_;
	public:
		ContinousSaver(Scene *scene, RadiosityRenderer *renderer, std::string nameBase, int perStepFreq):
		  scene_(scene),
			  renderer_(renderer),
			  nameBase_(nameBase),
			  perStepFreq_(perStepFreq<<1),
			  perStepCounter_(0)/*,
								stepSkip_(1)*/
		  {
			  renderer->attach(this, perStepFreq);
		  }
		  virtual void updateStep() {
			  perStepCounter_ = 0;
			  perStepFreq_ >>= 1;
			  if (perStepFreq_ < 1)
				  perStepFreq_ = 1;
			  renderer_->detach(this);
			  renderer_->attach(this, perStepFreq_);
		  }
		  virtual void updatePerStepProgress() {
			  //if (1==perStepFreq_) {
				  //if (0==renderer_->currentStep()%stepSkip_)
					 // stepSkip_ <<= 1;
				  //else if (renderer_->currentStep()+1!=renderer_->stepCount())
 					//return;
 			//}
			std::ostringstream nameStream;
			nameStream
					<< nameBase_
					//<< "-step" << std::setw(4) << std::setfill('0') << renderer_->currentStep()
					<< "-step" << std::setw(5) << std::setfill('0') << renderer_->getTotalCount()
					//<< "-snapshot" << std::setw(3) << std::setfill('0') << perStepCounter_
					<< ".xml";
			//scene_->save(nameStream.str());
			perStepCounter_ ++;
			renderer_->incTotalCount();
			//std::cout << "  Saved " << renderer_->getTotalCount() << " files so far." << std::endl;
		}
};


// Function prototypes
int visualize(int argc, char* argv[]);
int compute(int argc, char* argv[]);
int GetFileList(const char* searchkey, vector<string> &list);
double diffclock(clock_t clock1, clock_t clock2);


int main(int argc, char *argv[]) {
	int retval = 0;

	pArgc = &argc;
	pArgv = argv;

    shrQAStart(argc, argv);

    shrSetLogFileName ("deviceQuery.txt");
    shrLog("%s Starting...\n\n", argv[0]); 
    shrLog(" CUDA Device Query (Runtime API) version (CUDART static linking)\n\n");
        
    int deviceCount = 0;
    cudaError_t error_id = cudaGetDeviceCount(&deviceCount);
    if (error_id != cudaSuccess) {
        shrLog( "cudaGetDeviceCount returned %d\n-> %s\n", (int)error_id, cudaGetErrorString(error_id) );
		shrQAFinishExit(*pArgc, (const char **)pArgv, QA_FAILED);
	}
	// This function call returns 0 if there are no CUDA capable devices.
	if (deviceCount == 0)
	{ 
		shrLog("There is no device supporting CUDA!\n");
		shrQAFinishExit(argc, (const char **)argv, QA_PASSED);
	} else {
		shrLog( "cuda devices: %d\n", deviceCount);
	}


	/* Run in Visualize mode if specified */
	if(regex_search(argv[0], regex(".*isualize.*")))
	{
		retval = visualize(argc, argv);
		
	}
	/* Run in compute mode if specified */
	else /*if (regex_search(argv[0], regex(".*ompute.*")))*/
	{
		retval = compute(argc, argv);
	}
	/* Complain if neither mode is specified */
	//else
	//{
	//	std::cout << "Cannot determine if program is in visualize or compute mode!" << std::endl;
	//	//nbody::main2(argc, argv);
	//	retval = 5;
	//}

	return retval;
}


int visualize(int argc, char* argv[]) {
	std::cout << "Running in Visualizer mode..." << std::endl;
	//if(strcmp(argv[0], 
	VisualizeArguments va;
	va.parseArguments( argc, argv);
	string infile = va.getFileIn();

	//if (va.getScreenshot())
	//	// Create screenshot
	//	Visualizer::takeScreenshot(va.getScreenshotFile(), ::globalTriangleSet);

	//else
		// Initiate visualizer

	vector<string> list;
	string iFiles = infile + "-step*.xml";

    int count = GetFileList(iFiles.c_str(), list);
	if(count > 1)
	{
		Visualizer::setFileList(list);
		for(unsigned int i = 0; i < list.size(); i++)
		{
			cout << list[i] << endl;
		}
		infile = list[list.size() - 1];
	}

	{
		Scene scene;
		scene.load(infile);
		::globalTriangleSet = scene.dumpForVis();

		// Leaving this block should "release" Scene object from stack
	}

	// Clean-up after exit() call
	std::atexit(destroyTriangleSet);	

		Visualizer::visualize((std::string("Visualize - ") + infile).c_str(), ::globalTriangleSet);
		Visualizer::setFile(1, infile);
		Visualizer::setFile(2, infile);
		Visualizer::setTotalFileNum(list.size());
		Visualizer::setCurrentFileNum(list.size() - 1);


	// NOTE: This line is never executed
	return 0;
}

int compute(int argc, char* argv[]) {
	std::cout << "Running in Computate mode..." << std::endl;
	//std::string fileToView;
	
	ComputeArguments *args = new ComputeArguments();
	args->parseArguments( argc, argv );

	/* if (1>=argc)
	return 1; */

	std::string fileName(args->getFileIn());

	// Load scene
	Scene scene;

	cout << "Loading file " << fileName << "..." << endl;
	try
	{
		scene.load(args->getFileIn());
	}
	catch( XML::XMLException &e )
	{
		std::cerr << e.what() << std::endl;
		exit( 0 );
	}
	
	cout << "Loading finished!" << endl;
	//fileToView = args->getFileOut();
	
	// Acceleration
	//scene.applyEmission();

	clock_t begin = clock();
	// Patch division
	//scene.divide(DIVIDE_TO);
	scene.divide(args->getDivide());

	//scene.save("scene-div3.xml");

	// Compute radiosity
	RadiosityRenderer *renderer =
		scene.createRadiosityRenderer(args->getSteps(), args->getTreshold(), args->getCache()*1048576);
	//scene.createRadiosityRenderer(STEP_COUNT, FORM_FACTOR_TRESHOLD, MAX_CACHE_SIZE);

	ConsoleProgressIndicator *progressIndicator=
		new ConsoleProgressIndicator(renderer);

	ContinousSaver *saver=
		new ContinousSaver(&scene, renderer, fileName, args->getSaves());
	//new ContinousSaver(&scene, renderer, fileName, PER_STEP_SAVE_INITIAL);

	renderer->compute();
	clock_t end = clock();
	double elapsedtime = diffclock(end,begin);
	std::cout << "Time elapsed: " << elapsedtime << "ms" << std::endl;
	int totes = renderer->getTotalCount();

	delete saver;
	delete progressIndicator;
	delete renderer;

	//scene.save(fileName+"-final.xml");
	scene.save(args->getFileOut());

	std::string fileToView(args->getFileOut());
	::globalTriangleSet = scene.dumpForVis();
	
	// Clean-up after exit() call
	std::atexit(destroyTriangleSet);	

	//if (va.getScreenshot())
	//	// Create screenshot
	//	Visualizer::takeScreenshot(va.getScreenshotFile(), ::globalTriangleSet);

	//else
	// Initiate visualizer
	Visualizer::setFile(1, fileName);
	Visualizer::setFile(2, fileToView);
	Visualizer::setTotalFileNum(totes);
	Visualizer::setCurrentFileNum(totes);
	Visualizer::visualize((std::string("Visualize - ") + fileToView).c_str(), ::globalTriangleSet);

	delete args;

	return 0;
}


int GetFileList(const char *searchkey, std::vector<std::string> &list)
{
    WIN32_FIND_DATA fd;
    HANDLE h = FindFirstFile(searchkey,&fd);

	if(h == INVALID_HANDLE_VALUE)
    {
        return 0; // no files found
    }

    while(1)
    {
        list.push_back(fd.cFileName);
        if(FindNextFile(h, &fd) == FALSE)
            break;
    }

    return list.size();
}


double diffclock(clock_t clock1, clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
	return diffms;
}
