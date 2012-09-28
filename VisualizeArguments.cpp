/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#include <iostream>
#include <iomanip>
#include "VisualizeArguments.h"

#define PARAMETRS 3
#define KEYNUM 1
using namespace std;

// Constructors/Destructors
//  

const char* VisualizeArguments::NAMES[PARAMETRS]  = { "filein", 
						      "screenshot",
						      "fileout",};
							


const char* VisualizeArguments::descripts_[PARAMETRS]= { "Input file name, REQUIRED", 
							 "Save screenshot",
							 "File to save screenshot", };

const bool  VisualizeArguments::reqs_[PARAMETRS]= { true, 
						    false,
						    false,};

const char* VisualizeArguments::defaults_[PARAMETRS] = { "", 
							 "0",
							 "screenshot.tga",};

const char* VisualizeArguments::keys_[KEYNUM] = { "M", };

const char* VisualizeArguments::kdesc_[KEYNUM] = { "model from xml, reflectivity is used in color property", };

VisualizeArguments::VisualizeArguments ( ) 
{
	initialize();
}

VisualizeArguments::~VisualizeArguments ( ) { }

/**
 * @return char*
 */
char* VisualizeArguments::getFileIn ( ) 
{
	return getCstringValue( string( NAMES[ 0 ] ) );
}

/* 
 * Screenshot capability disabled for now.
 * -- David Sargeant, 2011-12-05
 *
 */
bool VisualizeArguments::getScreenshot( )
{
	bool result = true;
        char* val = getCstringValue( NAMES[ 1 ] );

        if( !strcmp( val, "1" ) || !strcmp( val, "on") || !strcmp( val, "true") )
        {
                result = true;
        }
        else if ( !strcmp( val, "0") || !strcmp( val, "off") || !strcmp( val, "false") )
        {
                result = false;
        }
        else
        {
                std::cerr << "Bad Screenshot value. Usable values is :  0 1 || on off || true false" << endl;
        }

        return result;
}

/**
 * @return char*
 */
char* VisualizeArguments::getScreenshotFile ( ) 
{
	return getCstringValue( string( NAMES[ 2 ] ) );
}


/**
 * Initialize members
 */
void VisualizeArguments::initialize ( ) 
{
	this->setDescription("Visualize radiosity-computed or original scene.\nAuthors:\tDavid Sargeant, Patrick Gradie, Michael Rogers\nDate:\t\t2012-04-08");
	this->setVersion("0.87a");

	for( int i = 0; i < PARAMETRS; i++ )
	{
		this->addArgument( NAMES[i], descripts_[i], reqs_[i], defaults_[i] );
	}
}

/**
 * Show help for controls
 */
void VisualizeArguments::showHelpAppendix()
{
	cout << "Controls: " << endl;

	for( int i = 0; i < KEYNUM; i++ )
	{
		cout << "  " << setw(10) << left << keys_[i] << kdesc_[i] << endl; 
	}
}
