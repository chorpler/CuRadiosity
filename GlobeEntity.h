/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#ifndef GLOBEENTITY_H
#define GLOBEENTITY_H

/**
 * @file GlobeEntity.h
 * @brief Class GlobeEntity
 * @author xbarin02, xfilak01
 * @date 2007-11-17
 */

#include "Entity.h"

/**
 * This entity has sphere as base shape. Radius of this sphere is 0.5
 * and center is in Vertex(0.0, 0.0, 0.0). See documentation
 * of Entity class for more information about transformations, patch division, etc.
 * @brief Globe entity.
 */
class GlobeEntity : public Entity {
protected:

		/**
		 * @brief  Read entity properties and set its in object
		 * @param  from XMLNode which has tagname globe
		 */
		virtual void impl_deserialize (XMLNode *from );

private:
		void polygonize ( );
		void addQuad(Vertex vertex[4]);
};

#endif // GLOBEENTITY_H
