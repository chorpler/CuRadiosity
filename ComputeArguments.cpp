/*
 * CUDARadiosity 0.87a
 * Copyright 2012 by David Sargeant, Patrick Gradie, Michael Rogers
 * 
 * Based on code from rrv (Radiosity Renderer and Visualizer) by TODO
 * Distributed under GPL (see <http://www.gnu.org/licenses/>)
 * 
 */

#include "ComputeArguments.h"
#define PARAMETRS 8
using namespace std;

// Constructors/Destructors
//  

const char* ComputeArguments::NAMES[PARAMETRS] 	  = {	"filein",
						"fileout",
						"treshold",
						"cache",
						"divide",
						"steps",
						"saves",
						"normalize",
						}; 

const char* ComputeArguments::descripts_[PARAMETRS]= {	"Input file name, REQUIRED",
						"Output file name, default is output.xml",
						"Form factor treshold value, default is 0.0",
						"Form factor cache size in MiB, default is 1024",
						"Size of largest acceptable patch after patch division, default is 1.0",
						"Number of steps, default is 32",
						"Number of continous saves in 1st step, default is 8",
						"'on' lightness is normalized, 'off' ligthness is truncated, default is 'on'",
						 };

const bool  ComputeArguments::reqs_[PARAMETRS]= { 	true,
						false,
						false,
						false,
						false,
						false,
						false,
						false,
						};

const char* ComputeArguments::defaults_[PARAMETRS] = { 	"inscene.xml",
						"output.xml",
						"0.0",
						"1024",
						"50",
						"10",
						"8",
						"1",
						};

//const char* ComputeArguments::defaults_[PARAMETRS] = { 	"",
//						"output.xml",
//						"0.0",
//						"1024",
//						"1",
//						"32",
//						"8",
//						"1",
//						};

ComputeArguments::ComputeArguments ( ) 
{
	this->initialize();
}

ComputeArguments::~ComputeArguments ( ) { }

// Other methods
//  


/**
 * @return float
 */
float ComputeArguments::getTreshold ( ) 
{
	float res = getValue<float>( string( NAMES[ ArgTreshold ] ) );

	if( 0 > res )
	{
		std::cerr << "Treshold cant be less than 0" << endl;
		return 0.0;
	}

	return res;
}


/**
 * @return float
 */
float ComputeArguments::getDivide ( ) 
{
	float res = getValue<float>( string( NAMES[ ArgDivide ] ) );
	
	if( 0 >= res )
	{
		std::cerr << "Divide must be greater than 0" << endl;
		return 1.0;
	}

	return res;
}


/**
 * @return int
 */
int ComputeArguments::getSteps ( ) 
{
	int res = getValue<int>( string( NAMES[ ArgSteps ] ) );
	
	if( 0 >= res )
	{
		std::cerr << "Steps must be greater than 0" << endl;
		return 32;
	}

	return res;
}


/**
 * @return int
 */
int ComputeArguments::getSaves ( ) 
{
	int res = getValue<int>( string( NAMES[ ArgSaves ] ) ); 

	if( 0 > res )
	{
		std::cerr << "Saves cant be less than 0" << endl;
		return 8;
	}

	return res;
}


/**
 * @return long
 */
long ComputeArguments::getCache ( ) 
{
	long res = getValue<long>( string( NAMES[ ArgCache ] ) );

 	if( 0 > res ) 
	{
		std::cerr << "Cache cant be less than 0" << endl; 
		return 1024l;
	}
	
	return res;
}


/**
 * @return char*
 */
char* ComputeArguments::getFileIn ( ) 
{
	return getCstringValue( string( NAMES[ ArgFileIn ] ) );
}


/**
 * @return char*
 */
char* ComputeArguments::getFileOut ( ) 
{
	return getCstringValue( string( NAMES[ ArgFileOut ] ) );
}


/**
 * @return bool
 */
bool ComputeArguments::getNormalize ( ) 
{
	bool result = true;
	char* val = getCstringValue( NAMES[ ArgNormalize ] );

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
		std::cerr << "Bad Normalize value. Usable values is :  0 1 || on off || true false" << endl;
	}

	return result;
}


/**
 * Initialize object.
 */
void ComputeArguments::initialize ( ) 
{
	this->setDescription("Compute scene radiosity.\nAuthors:\tDavid Sargeant, Patrick Gradie, Michael Rogers\nDate:\t\t2012-01-28");
	this->setVersion("0.87a");

	for( int i = 0; i < PARAMETRS; i++ )
	{
		this->addArgument( NAMES[i], descripts_[i], reqs_[i], defaults_[i] );
	}
}


