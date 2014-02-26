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
#include <time.h>

// c++
#include <iostream>

// boost

// local
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
         << "  ping iterations (default 1) or 0 (zero) for unlimited"   << endl << endl;

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
int main( int argc, char** argv )
{
    int itr = 0;

    //
    version();
    copyright();

    //
    if ( ( argc > 1 ) && ( argc < 4 ) )
    {
        if ( argc == 3 )
            itr = ::atoi( argv[ 2 ] );
        else
            itr = 1;
    }
    else
    {
        usage();
        return 1;
    }

    clock_t time;
    int ms = 0;
    bool ok = false;

    tns tns( argv[ 1 ] );

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

    for ( int i = 0; i < itr; ++i )
    {
        time = clock();

        try
        {
            ora( argv[ 1 ] ).attach();
            ok = true;
        }
        catch ( mti::exp& x )
        {
            ok = false;
            cerr << x.what();
        }

        //
        ms = (int)double( clock() - time ) / CLOCKS_PER_SEC * 1000;
        cout << ( ( ok ) ? "OK (" : "(" ) << ms << " ms)" << endl;
    }

    //
    cout << endl;
    return 0;
}
