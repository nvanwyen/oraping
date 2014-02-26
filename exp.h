//
// exp.h
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

#ifndef __EXP_H
#define __EXP_H

// c
#include <stdio.h>

// c++
#include <string>
#include <stdexcept>

// boost

// local

//
using namespace std;

#define EXP_SUCCESS             0x00
#define EXP_UNKNOWN             0x01
#define EXP_LIBRARY             0x02
#define EXP_SYMBOL              0x03
#define EXP_ERROR               0x04
#define EXP_EMPTY               0x05
#define EXP_MISSING             0x06
#define EXP_OPEN                0x07
#define EXP_UNSUPPORTED         0x08
#define EXP_UNRESOLVED          0x09

//
namespace mti {
//
class exp : public exception
{
    //
    public:
        //
        exp() : what_( "" ), code_( EXP_UNKNOWN ) {}
        exp( string msg )          : what_( msg ), code_( EXP_UNKNOWN ) {}
        exp( string msg, int num ) : what_( msg ), code_( num )         {}

        //
        ~exp() throw() {}

        //
        virtual string message() throw()
        {
            return what();
        }

        //
        virtual const char* what() const throw()
        {
            return what_.c_str();
        }

        //
        virtual int code()
        {
            return code_;
        }

    //
    private:
        //
        string what_;
        int    code_;
};
}
#endif // __EXP_H
