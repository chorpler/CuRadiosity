/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#include "TriangleSet.h"

/**
 * @param  triangle
 */
void TriangleSet::add (Triangle* triangle ) {
	container_.push_back(*triangle);
}

/**
 * @param  triangle
 */
void TriangleSet::add (TriangleSet *pTset) {
	TriangleSet &tset = *pTset;
	for(size_t i=0; i< tset.count(); i++) {
		Triangle &t = tset[i];
		this->add(&t);
	}
}

/**
 */
size_t TriangleSet::count ( ) {
	return container_.size();
}


/**
 * @param  index
 */
Triangle& TriangleSet::operator[] (unsigned index ) {
	return container_[index];
}


/**
 * @return TriangleSet*
 * @param  triangle
 */
TriangleSet* TriangleSet::divide (Triangle* triangle ) {
	TriangleSet* tset = new TriangleSet;
	
	// Compute triangle line's centres
	Vertex *v = triangle->vertex;
	Vertex c[3];
	for (int i=0; i<3; i++)
		c[(i+2)%3] =
				centre(v[i], v[(i+1)%3]);
	
	// Add vertex triangles
	Triangle t = *triangle;
	for (int i=0; i<3; i++) {
		t.vertex[0] = v[i];
		t.vertex[1] = c[(i+2)%3];
		t.vertex[2] = c[(i+1)%3];
		tset-> add(&t);
	}
	
	// Add middle triangle
	for (int i=0; i<3; i++)
		t.vertex[i] = c[i];
	tset-> add(&t);
	
	return tset;
}
