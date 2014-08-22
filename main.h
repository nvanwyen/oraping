//
// main.h
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2004-2013 Metasystems Technologies Inc. (MTI)
// All rights reserved
//
// Distributed under the MTI Software License, Version 0.1.
//
// as defined by accompanying file MTI-LICENSE-0.1.info or
// at http://www.mtihq.com/license/MTI-LICENSE-0.1.info
//

#ifndef __MAIN_H
#define __MAIN_H

// c

// c++
#include <string>

// boost

// local

//
void usage();

//
bool is_numeric( char *val );
std::string env( std::string var );

// default SID (based on environment variables)
std::string sid( void );

//
int main( int argc, char** argv );

#endif // __MAIN_H
