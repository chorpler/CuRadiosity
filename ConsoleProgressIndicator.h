/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#ifndef CONSOLEPROGRESSINDICATOR_H
#define CONSOLEPROGRESSINDICATOR_H

/**
 * @file ConsoleProgressIndicator.h
 * @brief Class ConsoleProgressIndicator - Console progress indicator for RadiosityRenderer.
 * @author davidpsargeant
 * @date 2012-04-16
 */


#include "ProgressObserverMultiStep.h"

class RadiosityRenderer;

/**
 * @brief Console progress indicator for RadiosityRenderer.
 */
class ConsoleProgressIndicator: public ProgressObserverMultiStep
{
	public:
		ConsoleProgressIndicator(RadiosityRenderer *);
		~ConsoleProgressIndicator();
		
		virtual void updateStep();
		virtual void updatePerStepProgress();
		
	private:
		RadiosityRenderer *renderer_;
		int counter_;
};

#endif // CONSOLEPROGRESSINDICATOR_H
