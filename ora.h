//
// ora.h
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

#ifndef __ORA_H
#define __ORA_H

// c
#include <oci.h>

// c++
#include <string>

// boost

// local
#include "exp.h"

//
using namespace std;

//
namespace mti {
//
class ora
{
    public:
        //
        ora()             { env_ = NULL; err_ = NULL; srv_ = NULL; };
        ora( string tns ) { env_ = NULL; err_ = NULL; srv_ = NULL; tns_ = tns; };

        //
        virtual ~ora()    { detach(); };

        //
        int attach();
        int detach();

    protected:
    private:
        //
        bool initialized() { return ( (env_ != NULL) && (err_ != NULL) && (srv_ != NULL) ); }

        //
        int initialize();

        //
        string chk();
        void chk( sword stat );

        //
        string tns_;

        //
        OCIEnv*    env_;
        OCIError*  err_;
        OCIServer* srv_;

        //
        string msg_;
        sb4    val_;
};
}
#endif // __ORA_H
