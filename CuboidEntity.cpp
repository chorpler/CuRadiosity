/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#include "CuboidEntity.h"
#include <iostream>

using namespace XML;
/**
 * @param  from
 */
void CuboidEntity::impl_deserialize (XMLNode *from ) {
	setName(from,"cuboid");
	this->polygonize();
}

/**
 */
void CuboidEntity::polygonize ( ) {
	const size_t PLANE_COUNT = 6;
	const size_t QUAD_VERTEX_COUNT = 4;
	Vertex tpl[PLANE_COUNT][QUAD_VERTEX_COUNT] = {{
			// left plane
			Vertex(-0.5, -0.5, -0.5),
			Vertex(-0.5, -0.5, +0.5),
			Vertex(-0.5, +0.5, +0.5),
			Vertex(-0.5, +0.5, -0.5),
		}, {
			// right plane
			Vertex(+0.5, -0.5, -0.5),
			Vertex(+0.5, +0.5, -0.5),
			Vertex(+0.5, +0.5, +0.5),
			Vertex(+0.5, -0.5, +0.5),
		}, {
			// down plane
			Vertex(-0.5, -0.5, -0.5),
			Vertex(+0.5, -0.5, -0.5),
			Vertex(+0.5, -0.5, +0.5),
			Vertex(-0.5, -0.5, +0.5),
		}, {
			// up plane
			Vertex(-0.5, +0.5, -0.5),
			Vertex(-0.5, +0.5, +0.5),
			Vertex(+0.5, +0.5, +0.5),
			Vertex(+0.5, +0.5, -0.5),
		}, {
			// rear plane
			Vertex(-0.5, -0.5, -0.5),
			Vertex(-0.5, +0.5, -0.5),
			Vertex(+0.5, +0.5, -0.5),
			Vertex(+0.5, -0.5, -0.5),
		}, {
			// front plane
			Vertex(-0.5, -0.5, +0.5),
			Vertex(+0.5, -0.5, +0.5),
			Vertex(+0.5, +0.5, +0.5),
			Vertex(-0.5, +0.5, +0.5),
		}
	};
	for(size_t i=0; i<PLANE_COUNT; i++)
		this->addQuad(tpl[i]);
}

void CuboidEntity::addQuad(Vertex vertex[4]) {
	// Triangle initialization
	Triangle t;
	setTriangleProperties( t );
	
	// Add triangle #1
	for (int i=0; i<3; i++)
		t.vertex[i] = vertex[i];
	this->addTriangle(&t);
	
	// Add triangle #2
	t.vertex[1] = vertex[2];
	t.vertex[2] = vertex[3];
	this->addTriangle(&t);
}
