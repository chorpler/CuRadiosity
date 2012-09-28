/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#include "EntitySet.h"
#include "PatchSequenceEnumerator.h"
#include "PatchRandomAccessEnumerator.h"
#include "TriangleSetExt.h"

/**
 * @param  entity
 */
void EntitySet::add (Entity *entity) {
	container_.push_back(*entity);
}

/**
 * @param  size
 */
void EntitySet::divide (float size ) {
	TContainer::iterator i;
	for(i= container_.begin(); i!= container_.end(); i++) {
		Entity &e = *i;
		e.divide(size);
	}
}

/**
 * @return PatchSequenceEnumerator*
 */
PatchSequenceEnumerator* EntitySet::createPatchSequenceEnumerator ( ) {
	IndirectPatchSequenceEnumerator *pe= new IndirectPatchSequenceEnumerator;
	
	TContainer::iterator i;
	for(i= container_.begin(); i!=container_.end(); i++) {
		Entity &e = *i;
		pe->add(e.createPatchSequenceEnumerator());
	}
	
	return pe;
}

size_t EntitySet::count()
{
	return container_.size();
}

Entity& EntitySet::operator[]( unsigned index )
{
	return container_[index];
}

TriangleSetExt* EntitySet::computeVertexColors() {
	TriangleSetExt *tset = new TriangleSetExt;
	
	TContainer::iterator i;
	for(i= container_.begin(); i!=container_.end(); i++) {
		Entity &e = *i;
		tset->add(e.computeVertexColors());
	}
	
	return tset;
}

