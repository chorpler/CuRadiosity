/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#ifndef TRIANGLESETEXT_H
#define TRIANGLESETEXT_H

/**
 * @file TriangleSetExt.h
 * @brief Class TriangleSetExt - extension to TriangleSet to carry per-vertex colors.
 * @author xdudka00
 * @date 2007-11-18
 */

#include "Triangle.h"

#include <vector>

/**
 * @brief Extension to Triangle to carry per-vertex colors.
 */
struct TriangleExt: Triangle {
	Color vertexColor[3];			///< per-vertex colors
	Color vertexColorRaw[3];	///< per-vertex colors after
	
	/**
	 * @brief backward compatibility with Triangle structure.
	 * @param t Reference to Triangle object to construct from.
	 */
	TriangleExt(const Triangle &t):
		Triangle(t)
	{
		for(int i=0; i<3; i++)
			vertexColorRaw[i] = vertexColor[i] = t.radiosity;
	}
};

/**
 * @brief Extension to TriangleSet to carry per-vertex colors.
 */
class TriangleSetExt {
	public:
		/**
		 * @brief @copydoc TriangleSet::add(Triangle*)
		 */
		void add (TriangleExt* triangle);
		
		/**
		 * @brief @copydoc TriangleSet::add(TriangleSet*)
		 */
		void add (TriangleSetExt* tset);
		
		/**
		 * @brief @copydoc TriangleSet::count()
		 */
		size_t count ( );

		/**
		 * @brief @copydoc TriangleSet::operator[](unsigned)
		 */
		TriangleExt& operator[] (unsigned index);

	private:
		typedef std::vector<TriangleExt> TContainer;
		TContainer container_;

};

#endif // TRIANGLESETEXT_H
