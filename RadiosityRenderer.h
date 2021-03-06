/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#ifndef RADIOSITYRENDERER_H
#define RADIOSITYRENDERER_H

/**
 * @file RadiosityRenderer.h
 * @brief Class RadiosityRenderer - radiosity rendering engine.
 * @author xdudka00
 * @date 2007-11-18
 */

#include "Triangle.h"
#include "ProgressSubjectMultiStep.h"

class PatchSequenceEnumerator;
class PatchRandomAccessEnumerator;
class PatchCache;

/**
 * @brief Radiosity rendering engine.
 */
class RadiosityRenderer: public ProgressSubjectMultiStep {
	public:
		/**
		 * @param patchEnumerator Instance of PatchSequenceEnumerator - can be @b deleted immediately ater construction.
		 * @param stepCount Count of radiosity computation steps.
		 * @param formFactorTreshold Pair of patches with smaller form factor than formFactorTreshold will be ignored.
		 * @param maxCacheSize Maximum size of patch cache (in bytes).
		 * @note Maximum cache size is raw size (estimated). The real cache size can be greater.
		 */
		RadiosityRenderer(PatchSequenceEnumerator *patchEnumerator, int stepCount, float formFactorTreshold, long maxCacheSize);
		~RadiosityRenderer();
		
		/**
		 * @brief Return total count of radiosity computation steps.
		 */
		virtual int stepCount() const;

		/**
		 * @brief Return number of current step - always in range <0, stepCount()-1>.
		 */
		virtual int currentStep() const;

		/**
		 * @brief Return total count of patches maintained by RadiosityRenderer.
		 */
		virtual int patchCount() const;

		/**
		 * @brief Return number of current patch computed by RadiosityRenderer - always in range <0, patchCount()-1>.
		 */
		virtual int currentPatch() const;
		
		/**
		 * @brief Return current total computational count.
		 * @copydoc PatchCache::getTotalCount()
		 */
		virtual int getTotalCount() const;

		/**
		 * @brief Increment total computational count by 1.
		 */
		int incTotalCount();

		/**
		 * @brief Set total computational count.
		 */
		void setTotalCount(int stc);

		/**
		 * @brief Initiate radiosity rendering progress.
		 */
		void compute();
		
		/**
		 * @brief Optional color normalization.
		 * @attention This method can't be called before compute()!
		 */
		void normalize();
		
		/**
		 * @brief Return current patch cache size.
		 * @copydoc PatchCache::cacheRawSize()
		 */
		long int cacheRawSize() const;
		
		


	private:
		int stepCount_;
		int currentStep_;
		int currentPatch_;
		PatchRandomAccessEnumerator *patchEnumerator_;
		int patchCount_;
		int totalCount_;
		PatchCache *patchCache_;
		
	private:
		float colorPeak_;
		void updateColorPeak(const Color &c) {
			if (colorPeak_ < c.r)
				colorPeak_ = c.r;
			if (colorPeak_ < c.g)
				colorPeak_ = c.g;
			if (colorPeak_ < c.b)
				colorPeak_ = c.b;
		}
		void normalize(Color &dest) {
				float ratio = 1/colorPeak_;
				dest.r *= ratio;
				dest.g *= ratio;
				dest.b *= ratio;
		}
		void computeStep();
};

#endif // RADIOSITYRENDERER_H
