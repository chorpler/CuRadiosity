/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */


#ifndef PROGRESSOBSERVERMULTISTEP_H
#define PROGRESSOBSERVERMULTISTEP_H

/**
 * @file ProgressObserverMultiStep.h
 * @brief Class ProgressObserverMultiStep - Observer for progress subject (usually RadiosityRenderer).
 * @author xdudka00
 * @date 2007-11-18
 */

/**
 * @brief Observer for multi-step progress subject. Design pattern observer.
 */
class ProgressObserverMultiStep
{
	public:
		virtual ~ProgressObserverMultiStep() { }
		
		/**
		 * @brief Notification about starting new step.
		 */
		virtual void updateStep();


		/**
		 * @brief Notification about subject's progress.
		 */
		virtual void updatePerStepProgress();
};

#endif // PROGRESSOBSERVERMULTISTEP_H
