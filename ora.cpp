//
// ora.cpp
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

//
#if defined(DOS) || defined(WINDOWS) || defined(WIN32) || defined(_WIN32) || defined(WINSOCK)
#pragma comment( lib, "oci.lib" )
#endif

// c

// c++
#include <iostream>
#include <algorithm>

// boost

// local
#include "ora.h"

using namespace mti;

//
int ora::initialize()
{
    sword rc = OCI_SUCCESS;

    //
    if ( ( rc = ::OCIInitialize( OCI_DEFAULT, NULL, NULL, NULL, NULL ) ) == OCI_SUCCESS )
    {
        //
        if ( ( rc = ::OCIEnvInit( (OCIEnv **) &env_, OCI_DEFAULT, (size_t) 0, (dvoid**) 0 ) ) == OCI_SUCCESS )
        {
            //
            if ( ( rc = ::OCIEnvCreate( &env_, OCI_DEFAULT, NULL, NULL, NULL, NULL, 0, NULL ) ) == OCI_SUCCESS )
            {
                //
                if ( ( rc = ::OCIHandleAlloc( (dvoid*) env_, (dvoid**) &err_, OCI_HTYPE_ERROR,  (size_t) 0, (dvoid**) 0 ) ) != OCI_SUCCESS )
                    throw exp ( "OCI error handle allocation failed", rc );

                if ( ( rc = ::OCIHandleAlloc( (dvoid*) env_, (dvoid**) &srv_, OCI_HTYPE_SERVER, (size_t) 0, (dvoid**) 0 ) ) != OCI_SUCCESS )
                    throw exp ( "OCI server handle allocation failed", rc );
            }
            else
                throw exp ( "OCI Environment creation failed", rc );
        }
        else
            throw exp ( "OCI Environment initialize failed", rc );
    }
    else
        throw exp ( "OCI initialize failed", rc );

    return rc;
}

//
int ora::attach()
{
    sword rc = OCI_SUCCESS;

    //
    if ( ! initialized() )
        initialize();

    //
    if ( tns_.length() > 0 )
    {
        //
        if ( ( rc = (sword)::OCIServerAttach( srv_, err_, (text*)tns_.c_str(),  (sb4)tns_.length(), 0 ) ) != OCI_SUCCESS )
        {
            //
            if ( chk().length() > 0 )
            {
                throw exp ( msg_, val_ );
            }
            else
                throw exp ( "Failed to attach to server", rc );
        }
    }
    else
        throw exp ( "Empty TNS Names entry not allowed", EXP_EMPTY );

    return rc;
}

int ora::detach()
{
    if ( srv_ )
    {
        //
        (void)::OCIServerDetach( srv_, err_, OCI_DEFAULT );

        // OCIHandleFree
        if ( srv_ )
            (void)::OCIHandleFree( (dvoid*) srv_, OCI_HTYPE_SERVER );

        // OCIHandleFree
        if ( err_ )
            (void)::OCIHandleFree( (dvoid*) err_, OCI_HTYPE_ERROR );

        env_ = NULL;
        err_ = NULL;
        srv_ = NULL;
    }

    return OCI_SUCCESS;
}

//
string ora::chk()
{
    //
    if ( err_ )
    {
        text buf[ 1024 ] = { '\0' };

        //
        (void) ::OCIErrorGet( (dvoid*) err_, (ub4) 1, (text*) NULL, &val_, buf, (ub4) sizeof( buf ), (ub4) OCI_HTYPE_ERROR );

        //
        if ( buf[ 0 ] )
        {
            msg_ = string( (const char*) buf );

            replace( msg_.begin(), msg_.end(), '\n', ' ' );
        }
        else
            msg_ = "";
    }

    return msg_;
}

//
void ora::chk( sword stat )
{
    //
    if ( stat == OCI_SUCCESS) return;

    //
    switch ( stat )
    {
        case OCI_ERROR:
            throw exp ( chk(), EXP_ERROR );
            break;

        case OCI_INVALID_HANDLE:
            throw exp ( "OCI_INVALID_HANDLE", EXP_ERROR );
            break;

        default:
            throw exp ( "Unknown exception", val_ );
            break;
    }
}
