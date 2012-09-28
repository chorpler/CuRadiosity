/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#ifndef XMLWRITER_H
#define XMLWRITER_H

#include "xmlParser.h"

namespace XML
{
	class XMLWriter
	{
		public:
			XMLWriter();

			void addEntity( XMLNode entityNode );
			
			void writeToFile( const char* fileName );

			static XMLSTR copyString( XMLCSTR from );

		private:
			XMLCSTR objectName;

			XMLNode root_;
			XMLNode definition_;
			XMLNode objectdef_;
			XMLNode instantiate_;
			XMLNode object_;			
	};
}

#endif
