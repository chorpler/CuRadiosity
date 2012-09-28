/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#include "XMLNames.h"

const char* XML::XMLNames::TAGS[] = {
	"scene",
	"head",
	"body",
	"objectdef",
	"cuboid",
	"barrel",
	"globe",
	"teapot",
	"triangleset",
	"triangle",
	"vertex",
	"rotate",
	"scale",
	"shear",
	"translate",
	"object",
	"trianglenext" };

const char* XML::XMLNames::ATTRIBUTES[] = {
	"name",
	"emission",
	"reflectivity",
	"radiosity",
	"x",
	"y",
	"z",
	"dx",
	"dy",
	"dz",
	"angle",
	"file",
	"spec",
	"refr",
	"refl"};
