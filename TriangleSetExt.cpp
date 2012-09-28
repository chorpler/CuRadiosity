/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#include "TriangleSetExt.h"

/**
 * @param  triangle
 */
void TriangleSetExt::add (TriangleExt* triangle) {
	container_.push_back(*triangle);
}

void TriangleSetExt::add (TriangleSetExt *pTset) {
	TriangleSetExt &tset = *pTset;
	for(size_t i=0; i< tset.count(); i++) {
		TriangleExt &t = tset[i];
		this->add(&t);
	}
}

/**
 */
size_t TriangleSetExt::count ( ) {
	return container_.size();
}


/**
 * @param  index
 */
TriangleExt& TriangleSetExt::operator[] (unsigned index ) {
	return container_[index];
}
