//
// main.cpp
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

// c
#include <stdlib.h>
#include <string.h>
#include <time.h>

// c++
#include <iostream>

// boost

// local
#include "release.h"
#include "main.h"
#include "tns.h"
#include "ora.h"
#include "exp.h"

using namespace std;
using namespace mti;

//
void usage()
{
    //      0.........1.........2.........3.........4.........5.........6.........7.........8
    cout << "Usage oraping TNS [COUNT]" << endl << endl;

    cout << "  where TNS is an entry or fully qualifed description and COUNT is the number of" << endl
         << "  ping its (default 1) or 0 (zero) for unlimited"   << endl << endl;

    cout << "  examples: $ oraping orcl"                                << endl << endl;

    cout << "            $ oraping //server.com:5521/orcl 3"            << endl << endl;

    cout << "            $ oraping \"(description = \\"                         << endl;
    cout << "                           (address = \\"                          << endl;
    cout << "                               (protocol = tcp) \\"                << endl;
    cout << "                               (host = server.com) \\"             << endl;
    cout << "                               (port = 1521)) \\"                  << endl;
    cout << "                           (connect_data = \\"                     << endl;
    cout << "                               (server = dedicated) \\"            << endl;
    cout << "                               (service_name = orcl)))\""  << endl << endl;
}

//
bool is_numeric( char *val )
{
    bool ok = true;

    //
    int sz = strlen( val );
    int it = 0;

    //
    while ( it < sz )
    {
        //
        if ( !isdigit( val[ it ] ) )
        {
            //
            ok = false;
            break;
        }

        //
        it++;

    }

    //
    return ok;
}

//
string env( string var )
{
	string val;

#if !defined(DOS) && !defined(_WIN32) && !defined(WINSOCK)
    char* tmp = ::getenv( var.c_str() );
	val = string( ( ( tmp == NULL ) ? "" : tmp ) );
#else
	char* tmp = NULL;
	size_t sz;

	::getenv_s( &sz, NULL, 0, var.c_str() );

	if ( sz > 0 )
	{
		tmp = (char*)::malloc( sz * sizeof(char) );

		if ( tmp )
		{
			::getenv_s( &sz, tmp, sz, var.c_str() );
			val = string( tmp );

			::free( tmp );
		}
	}
#endif

	return val;
}

//
string sid( void )
{
    string val;

    //
    val = env( "TWO_TASK" );

    //    
    if ( val.length() == 0 )
    {
        //
        val = env( "ORACLE_SID" );

        //
        if ( val.length() > 0 )
            cout << "using environment variable ORACLE_SID for TNS\n" << endl;
    }
    else
        cout << "using environment variable TWO_TASK for TNS\n" << endl;

    //
    return val;
}

//
int main( int argc, char** argv )
{
    int rc = 1;
    string dbs;
    int itr = 0;

    //
    version();
    copyright();

    //
    if ( ( argc > 1 ) && ( argc < 4 ) )
    {
        //
        switch ( argc )
        {
            case 2:
                {
                    if ( is_numeric( argv[ 1 ] ) )
                    {
                        itr = ::atoi( argv[ 1 ] );
                        dbs = sid();

                        if ( dbs.length() == 0 )
                        {
                            usage();
                            return rc;
                        }
                    }
                    else
                    {
                        dbs = string( argv[ 1 ] );
                        itr = 1;
                    }
                }
                break;

            case 3:
                dbs = string( argv[ 1 ] );
                itr = ::atoi( argv[ 2 ] );
                break;

            default:
                usage();
                return rc;
        }
    }
    else
    {
        dbs = sid();

        if ( dbs.length() == 0 )
        {
            usage();
            return rc;
        }
        else
            itr = 1;
    }

    clock_t time;
    int ms = 0;
    bool ok = false;

	try
	{
		tns tns( dbs.c_str() );

		//
		cout << "Using: " << tns.tnsnames() << endl;

		//
		if ( tns.sqlnet().length() > 0 )
			cout << "       " << tns.sqlnet() << endl;

		//
		if ( tns.ldap().length() > 0 )
			cout << "       " << tns.ldap() << endl;

		//
		cout << endl;

		//
		cout << "Resolved with " << tns.type() << endl;

		//
		if ( tns.name().length() > 0 )
			cout << tns.name() << " = ";

		//
		cout << tns.desc() << endl << endl;

        rc = 0;
		for ( int i = 0; i < itr; ++i )
		{
			time = clock();

			try
			{
				ora( dbs.c_str() ).attach();
				ok = true;
			}
			catch ( mti::exp& x )
			{
                rc++;
				ok = false;
				cerr << x.what();
			}

			//
			ms = (int)double( clock() - time ) / CLOCKS_PER_SEC * 1000;
			cout << ( ( ok ) ? "OK (" : "(" ) << ms << " ms)" << endl;
		}
	}
	catch ( mti::exp& x )
	{
		cerr << x.what() << endl;
	}

	//
	cout << endl;
    return rc;
}
