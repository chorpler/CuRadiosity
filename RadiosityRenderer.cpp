/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#include "RadiosityRenderer.h"
#include "FormFactorEngine.h"
#include "PatchRandomAccessEnumerator.h"
#include "PatchCache.h"

#include <iostream>

RadiosityRenderer::RadiosityRenderer(
	PatchSequenceEnumerator *patchEnumerator,
	int stepCount,
	float formFactorTreshold,
	long maxCacheSize
	):
stepCount_(stepCount),
	currentStep_(0),
	currentPatch_(0),
	colorPeak_(0.0)
{
	patchEnumerator_ = new PatchRandomAccessEnumerator(patchEnumerator);
	patchCount_ = patchEnumerator_->count();
	totalCount_ = 0;
	
	// Create patch cache
	patchCache_ = new PatchCache(patchEnumerator_, formFactorTreshold, maxCacheSize);

// #ifndef NDEBUG
	std::cout << "--- Count of patch: " << patchCount_ << std::endl;
	std::cout << "--- FormFactor treshold: " << formFactorTreshold << std::endl;
	std::cout << "--- Max cache raw size: " << maxCacheSize/(1024*1024) << " MB" << std::endl;
// #endif
}

RadiosityRenderer::~RadiosityRenderer() {
	delete patchCache_;
	delete patchEnumerator_;
}

/**
	*/
int RadiosityRenderer::stepCount() const {
	return stepCount_;
}


/**
	* @return int
	*/
int RadiosityRenderer::currentStep() const {
	return currentStep_;
}

/**
	*/
int RadiosityRenderer::patchCount() const {
	return patchCount_;
}


/**
	*/
int RadiosityRenderer::currentPatch() const {
	return currentPatch_;
}

long int RadiosityRenderer::cacheRawSize() const {
	return patchCache_->cacheRawSize();
}

//int getTotalCount();
//int incTotalCount();
//void setTotalCount(int stc);

int RadiosityRenderer::getTotalCount() const {
	return totalCount_;
}

int RadiosityRenderer::incTotalCount() {
	totalCount_++;
	return totalCount_;
}

void RadiosityRenderer::setTotalCount(int stc) {
	totalCount_ = stc;
}


void RadiosityRenderer::compute() {
	cout << "We'll be doing " << stepCount_ << " steps." << endl;

	for(currentStep_=0; currentStep_<stepCount_; currentStep_++) {
		notifyStep();
		this->computeStep();
	}
}

/**
 */
void RadiosityRenderer::computeStep() {
	
	PatchRandomAccessEnumerator &patch = *patchEnumerator_;
	
	// Initialize radiosityLast tag of Triangle objects
	for(int i=0; i<patchCount_; i++) {
		Triangle &dest = patch[i];
		dest.radiosityLast = dest.radiosity;
	}
	
	// Compute new step
	for(currentPatch_=0; currentPatch_<patchCount_; currentPatch_++) {
		notifyPerStepProgress();
		
		// Compute radiosity
		Triangle &dest = patch[currentPatch_];
		Color &rad = dest.radiosity;
		//rad.r = 100;
		//rad.g = 100;
		//rad.b = 24;
		rad = patchCache_->totalRadiosity(currentPatch_);
		rad *= dest.reflectivity;
		rad += dest.emission;
		//
		updateColorPeak(rad);
	}
}

void RadiosityRenderer::normalize() {
// #ifndef NDEBUG
	std::cout << ">>> Normalizing ... " << std::flush;
// #endif

	PatchRandomAccessEnumerator &patch = *patchEnumerator_;
		
	for(int i=0; i<patchCount_; i++)
		normalize(patch[i].radiosity);

// #ifndef NDEBUG
	std::cout << "ok" << std::endl;
// #endif
}
