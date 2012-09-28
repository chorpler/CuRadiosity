/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#include "PatchRandomAccessEnumerator.h"
#include "PatchSequenceEnumerator.h"
#include "Entity.h"
#include "EntitySet.h"

PatchRandomAccessEnumerator* PatchRandomAccessEnumerator::create(Entity *fromEntity) {
  return createGeneric(fromEntity);
}

PatchRandomAccessEnumerator* PatchRandomAccessEnumerator::create(EntitySet *fromEntitySet) {
  return createGeneric(fromEntitySet);
}

PatchRandomAccessEnumerator::PatchRandomAccessEnumerator (
		PatchSequenceEnumerator *pe)
{
	Triangle *t;
	for(int i=0; 0!= (t=pe->nextPatch()); i++)
		container_[i] = t;
}

/**
 * @return int
 */
int PatchRandomAccessEnumerator::count ( ) {
	return container_.size();
}


/**
 * @param index
 */
Triangle& PatchRandomAccessEnumerator::operator[] (unsigned index ) {
	Triangle *t = container_[index];
	return *t;
}
