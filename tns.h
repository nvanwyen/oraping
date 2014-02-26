//
// tns.h
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

#ifndef __TNS_H
#define __TNS_H

// c

// c++
#include <stdexcept>
#include <string>
#include <vector>

// boost

// local
#include "exp.h"

//
using namespace std;

//
namespace mti {
//
class tns
{
    public:
        //
        class entry
        {
            public:
                //
                enum origin { UNKNOWN, TNSNAMES, LDAP, EZCONNECT };

                //
                string name;
                string desc;
                origin type;

                entry() : name( "" ), desc( "" ), type( UNKNOWN )
                {}

                entry( string n, string d, int t = UNKNOWN )
                {
                    name = n;
                    desc = d;
                    type = (origin) t;
                }

            protected:
            private:
        };

        //
        typedef vector<entry>   entries;
        typedef vector<string>  methods;

        typedef vector<entry>::iterator  item;
        typedef vector<string>::iterator method;


        //
        tns() {};
        tns( string ent ) { reconcile( ent ); };

        ~tns() {}

        //
        void reconcile( string ent );

        //
        string name() { return entry_.name; }

        //
        string desc() { return entry_.desc; }

        //
        string type()
        {
            string t;

            switch ( entry_.type )
            {
                case entry::TNSNAMES:
                    t = "TNSNAMES";
                    break;

                case entry::LDAP:
                    t = "LDAP";
                    break;

                case entry::EZCONNECT:
                default:
                    t = "EZCONNECT";
                    break;
            }

            return t;
        }

        //
        string tnsnames() { return tnsnames_; }
        string sqlnet()   { return sqlnet_; }
        string ldap()     { return ldap_; }

        entries tns_entries() { return entries_; }
        methods tns_methods() { return methods_; }

    protected:
    private:
        //
        entry entry_;

        // files
        string tnsnames_;
        string sqlnet_;
        string ldap_;

        // ldap.ora
        string host_;                   // ldap host
        string port_;                   // ldap port
        string pssl_;                   // ldaps port
        string root_;                   // root context

        // sqlnet.ora
        methods methods_;               // connection type (from sqlnet.ora)

        //
        entries entries_;               // list of all entries

        //
        string trim( string str );
        //
        string ltrim( string str );
        string rtrim( string str );

        //
        string ucase( string str );

        //
        string split( string str, int idx, string tok = "," );

        //
        bool mesg();

        //
        size_t add( string name, string desc, entry::origin type );
        size_t add( entry ent );

        //
        entry resolve( string name, string desc );

        //
        string env( string var );       // get the value of a environment variable
        bool exists( string file );     // test if a file exists
        string find( string name );     // find a file based on the TNS search order

        //
        size_t load_tnsnames();
        size_t load_methods();
        size_t load_ldap();

        //
        bool resolve_directory();

        //
        string format( string str );

        //
        string resolve( string ent );
        string compare( string dsc );
};
}
#endif // __TNS_H
