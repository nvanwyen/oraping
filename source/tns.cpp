//
// tns.cpp
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
#if defined(DOS) || defined(_WIN32) || defined(WINSOCK)
#pragma comment( lib, "oci.lib" )
#endif

// c
#include <stdlib.h>
#include <string.h>

#if !defined(DOS) && !defined(_WIN32) && !defined(WINSOCK)
#include <sys/stat.h>
#else
#include <windows.h>
#endif

// c++
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>

// boost

// local
#include "main.h"
#include "tns.h"
#include "exp.h"
#include "ldp.h"

//
#define ATTR_IDX_CN                 0
#define ATTR_IDX_DESC               1
#define ATTR_IDX_ALIAS              2

#define ATTR_VAL_CN                 "cn"
#define ATTR_VAL_DESC               "orclnetdescstring"
#define ATTR_VAL_ALIAS              "aliasedobjectname"

//
#ifndef ETC_DIR
#if !defined(DOS) && !defined(_WIN32) && !defined(WINSOCK)
#define ETC_DIR                     "/etc"
#else
#define ETC_DIR                     "\\"
#endif
#endif // ETC_DIR

//
#ifndef TNSNAMES_FILE
#define TNSNAMES_FILE               "tnsnames.ora"
#endif // TNSNAMES_FILE

//
#ifndef SQLNET_FILE
#define SQLNET_FILE                 "sqlnet.ora"
#endif // SQLNET_FILE

//
#ifndef LDAP_FILE
#define LDAP_FILE                   "ldap.ora"
#endif // LDAP_FILE

//
#ifndef WHITESPACE
#define WHITESPACE                  " \t"
#endif // WHITESPACE

//
#ifndef LDAP_ERROR
#define LDAP_ERROR                  -1
#endif // LDAP_ERROR

//
#ifndef LDAP_TIME_LIMIT
#define LDAP_TIME_LIMIT              0
#endif // LDAP_TIME_LIMIT

//
#ifndef MISSING_MESG_FILE
#if !defined(DOS) && !defined(_WIN32) && !defined(WINSOCK)
#define MISSING_MESG_FILE           "LDAP Method encountered, but the $ORACLE_HOME/ldap/mesg/ldapus.msb is missing!"
#else
#define MISSING_MESG_FILE           "LDAP Method encountered, but the %ORACLE_HOME%\\ldap\\mesg\\ldapus.msb is missing!"
#endif // !define...
#endif // MISSING_MESG_FILE

using namespace std;
using namespace mti;

//
string tns::trim( string str )
{
    return rtrim( ltrim( str ) );
}

//
string tns::ltrim( string str )
{
    size_t pos = str.find_first_not_of( WHITESPACE );
    return ( pos == string::npos ) ? "" : str.substr( pos );
}

//
string tns::rtrim( string str )
{
    size_t pos = str.find_last_not_of( WHITESPACE );
    return ( pos == string::npos ) ? "" : str.substr( 0, pos + 1 );
}

//
string tns::ucase( string str )
{
    transform( str.begin(), str.end(),str.begin(), ::toupper );
    return str;
}

//
string tns::split( string str, int idx, string tok /*= ","*/ )
{
    int   itm = 0;
    char* dat;

#if !defined(DOS) && !defined(_WIN32) && !defined(WINSOCK)
    //
    dat = ::strtok( &str[ 0 ], tok.c_str() );

    //
    while( dat != NULL )
    {
        if ( ++itm > idx )
            break;

        dat = ::strtok( NULL, tok.c_str() );
    }
#else
	char *nxt = NULL;

	dat = ::strtok_s( &str[ 0 ], tok.c_str(), &nxt );

	 while ( dat != NULL )
	 {
        if ( ++itm > idx )
            break;

		dat = ::strtok_s( NULL, tok.c_str(), &nxt );
	 }
#endif

	return trim( string( ( dat == NULL ) ? "" : dat ) );
}

//
bool tns::mesg()
{
    // The file ldapus.msb must be located in the $ORACLE_HOME/ldap/mesg/
    // directory which more than likely on an Instant Client installation will
    // not exist, but you can find a copy, as a courtesy, in the
    // <oraping_source>/mesg/ directory here within

    // The message file (ldapus.msb) in the correct directory before oraping
    // will search LDAP for TNS Descriptions
    //
    // Example:
    //     sudo mkdir -p $ORACLE_HOME/ldap/mesg/
    //     sudo cp ldapus.msb.11.2.0.3 $ORACLE_HOME/ldap/mesg/
    //     ( cd $ORACLE_HOME/ldap/mesg/ ; sudo ln -s ldapus.msb.11.2.0.3 ldapus.msb )

    // Looking for the message file $ORACLE_HOME/ldap/mesg/ldapus.msb
#if !defined(DOS) && !defined(_WIN32) && !defined(WINSOCK)
    return exists( env( "ORACLE_HOME" ) + "/ldap/mesg/ldapus.msb" );
#else
    return exists( env( "ORACLE_HOME" ) + "\\ldap\\mesg\\ldapus.msb" );
#endif
}

//
size_t tns::add( string name, string desc, entry::origin type )
{
    return add( entry( name, desc, type ) );
}

size_t tns::add( entry ent )
{
    //
    entries_.push_back( ent );

    //
    return entries_.size();
}

//
tns::entry tns::resolve( string name, string desc )
{
    entry e;

    //
    for ( item itm = entries_.begin(); itm != entries_.end(); ++ itm )
    {
        //
        if ( format( itm->name ) == format( name ) )
        {
            e.name = itm->name;
            e.desc = itm->desc;
            e.type = itm->type;

            break;
        }
        else
        {
            if ( format( itm->desc ) == format( name ) )
            {
                e.name = itm->name;
                e.desc = itm->desc;
                e.type = itm->type;

                break;
            }
        }
    }

    //
    if ( e.desc.length() == 0 )
    {
        e.desc = name;
        e.type = entry::EZCONNECT;
    }

    e.name = format( e.name );
    e.desc = format( e.desc );

    return e;
}

//
void tns::reconcile( string ent )
{

    //
    string desc;

    //
    try
    {
        //
        tnsnames_ = find( TNSNAMES_FILE );
    }
    catch ( exp& x )
    {
        // ... do nothing
        if ( x.code() == EXP_MISSING )
            sqlnet_ = "";
        else
            throw exp( "File [" + string( TNSNAMES_FILE ) + "] not found!", EXP_OPEN );
    }

    //
    try
    {
        //
        sqlnet_ = find( SQLNET_FILE );
    }
    catch ( exp& x )
    {
        // ... do nothing
        if ( x.code() == EXP_MISSING )
            sqlnet_ = "";
        else
            throw exp( "File [" + string( SQLNET_FILE ) + "] not found!", EXP_OPEN );
    }

    //
    try
    {
        //
        ldap_ = find( LDAP_FILE );
    }
    catch ( exp& x )
    {
        // ... do nothing
        if ( x.code() == EXP_MISSING )
            ldap_ = "";
        else
            throw exp( "File [" + string( LDAP_FILE ) + "] not found!", EXP_OPEN );
    }

    //
    if ( load_methods() > 0 )
    {
        //
        typedef vector<string>::iterator itype;

        //
        for ( itype i = methods_.begin(); i != methods_.end(); ++i )
        {
            //
            if ( ( *i == "TNSNAMES" ) || ( *i == "EZCONNECT" ) )
			{
				try
				{
					load_tnsnames();
				}
				catch ( exp& x )
				{
					throw exp( x.what(), x.code() );
				}
			}

			if ( *i == "LDAP" )
			{
				try
				{
					load_ldap();
				}
				catch ( exp& x )
				{
					throw exp( x.what(), x.code() );
				}
			}

            if ( *i == "ONAMES" )
                throw exp( "Unsupported naming type [" + (*i) + "]!", EXP_UNSUPPORTED );
        }
    }
    else
	{
		try
		{
			load_tnsnames();
		}
		catch ( exp& x )
		{
			cerr << "Could not load TNS Names file: " << x.what() << endl;
			throw x;
		}
	}

    //
    entry_ = resolve( ent, resolve( ent ) );
}

//
string tns::env( string var )
{
    // defer to main.h
    return ::env( var );
}

//
bool tns::exists( string file )
{
    bool ok = false;

#if !defined(DOS) && !defined(_WIN32) && !defined(WINSOCK)
	struct stat stat;

    //
    if ( ::stat( file.c_str(), &stat ) == 0 )
    {
        if ( S_ISREG( stat.st_mode ) || S_ISLNK( stat.st_mode ) )
            ok = true;
    }
#else
	WIN32_FIND_DATA dat;
	HANDLE hdl = NULL;
	wstring tmp = wstring( file.begin(), file.end() );


	if ( ( hdl = ::FindFirstFile( tmp.c_str(), &dat ) ) != INVALID_HANDLE_VALUE )
	{
		::FindClose( hdl );
		ok = true;
	}
	else
	{
		ok = false;
	}
#endif // !define...

	//
    return ok;
}

//
string tns::find( string name )
{
    string file;

    // search order ...
    //
    //    1) $TNS_ADMIN/tnsnames.ora
    //    2) $ORACLE_HOME/network/admin/tnsnames.ora
    //    3) /etc/oracle/tnsnames.ora
    //    4) $HOME/.tnsnames.ora

    // $TNS_ADMIN/<file>
    //
    if ( env( "TNS_ADMIN" ).length() > 0 )
    {
        //
#if !defined(DOS) && !defined(_WIN32) && !defined(WINSOCK)
        if ( exists( env( "TNS_ADMIN" ) + "/" + name ) )
            file = env( "TNS_ADMIN" ) + "/" + name;
#else
        if ( exists( env( "TNS_ADMIN" ) + "\\" + name ) )
            file = env( "TNS_ADMIN" ) + "\\" + name;
#endif
	}

    // $ORACLE_HOME/network/admin/<file>
    //
    if ( file.length() == 0 )
    {
        if ( env( "ORACLE_HOME" ).length() > 0 )
        {
            //
#if !defined(DOS) && !defined(_WIN32) && !defined(WINSOCK)
            if ( exists( env( "ORACLE_HOME" ) + "/" + name ) )
                file = env( "ORACLE_HOME" ) + "/" + name;
#else
            if ( exists( env( "ORACLE_HOME" ) + "\\" + name ) )
                file = env( "ORACLE_HOME" ) + "\\" + name;
#endif
		}
    }

    // /etc/oracle/<file>
    //
    if ( file.length() == 0 )
    {
        //
#if !defined(DOS) && !defined(_WIN32) && !defined(WINSOCK)
        if ( exists( string( ETC_DIR ) + "/" + name ) )
            file = string( ETC_DIR ) + "/" + name;
#else
        if ( exists( string( ETC_DIR ) + "\\" + name ) )
            file = string( ETC_DIR ) + "\\" + name;
#endif
	}

    // $HOME/.<file>
    //
    if ( file.length() == 0 )
    {
        if ( env( "HOME" ).length() > 0 )
        {
            //
#if !defined(DOS) && !defined(_WIN32) && !defined(WINSOCK)
            if ( exists( env( "HOME" ) + "/." + name ) )
                file = env( "HOME" ) + "/." + name;
#else
            if ( exists( env( "HOME" ) + "\\." + name ) )
                file = env( "HOME" ) + "\\." + name;
#endif
		}
    }

    // not found ...
    //
    if ( file.length() == 0 )
        throw exp( "File [" + name + "] not found!", EXP_MISSING );

    //
    return file;
}

//
size_t tns::load_methods()
{
    //
    if ( sqlnet_.length() > 0 )
    {
        //
        ifstream ifs;

        try
        {
            string buf; // line buffer

            //
            ifs.open( sqlnet_.c_str(), ios::in );

            //
            while ( getline( ifs, buf ) )
            {
                size_t pos;

                //
                buf = trim( buf );

                //
                if ( buf.substr( 0, 1 ) == "#" )
                    continue;

                //
                if ( ( pos = ucase( buf ).find( "NAMES.DIRECTORY_PATH", 0 ) ) != string::npos )
                {
                    size_t lpos = buf.find_first_of( "(" );

                    if ( lpos != string::npos )
                    {
                        size_t rpos = buf.find_first_of( ")", lpos + 1 );

                        if ( rpos != string::npos )
                        {
                            int i = 0;
                            string path = buf.substr( lpos + 1, rpos - ( lpos + 1 ) );
                            string itm = split( path, i );

                            while ( ( itm = split( path, i++ ) ) != "" )
                                methods_.push_back( ucase( itm ) );
                        }
                    }
                }
            }
        }
        catch ( exception& x )
        {
            throw exp( "File [" + sqlnet_ + "] read error: "  + string( x.what() ), EXP_OPEN );
        }
    }

    //
    if ( methods_.size() == 0 )
    {
        methods_.push_back( "TNSNAMES" );
        methods_.push_back( "EZCONNECT" );
    }

    return methods_.size();
}

//
size_t tns::load_tnsnames()
{
    //
    if ( entries_.size() == 0 )
    {
        //
        if ( tnsnames_.length() > 0 )
        {
            //
            ifstream ifs;

            try
            {
                string buf; // line buffer

                string ent; // entry
                string dsc; // description

                bool flg;   // new item flag
                int  lvl;   // parenthisis level

                //
                ifs.open( tnsnames_.c_str(), ios::in );

                //
                flg = true;
                lvl = 0;

                //
                while ( std::getline( ifs, buf ) )
                {
                    //
                    buf = trim( buf );

                    //
                    if ( trim( buf ) == string( "" ) )
                    {
                        // reset on empty line
                        ent = dsc = ""; flg = true; lvl = 0;
                        continue;
                    }

                    //
                    if ( buf.substr( 0, 1 ) == "#" )
                    {
                        // reset on comment line
                        ent = dsc = ""; flg = true; lvl = 0;
                        continue;
                    }

                    //
                    for ( size_t i = 0; i < buf.length(); ++i )
                    {
                        //
                        if ( buf[ i ] == ' ' )
                            continue;

                        //
                        if ( flg )
                        {
                            //
                            if ( buf[ i ] != '=' )
                            {
                                if ( ( buf[ i ] == ' ' ) || ( buf[ i ] == '\t' ) )
                                    continue;

                                ent += buf[ i ];
                            }
                            else
                                flg = false;    // start working on the description
                        }
                        else
                        {
                            //
                            if ( ( dsc.length() == 0 ) && ( buf[ i ] == '(' ) )
                            {
                                // started the description
                                dsc += buf[ i ]; ++lvl;
                            }
                            else
                            {
                                //
                                if ( buf[ i ] == '(' ) ++lvl;
                                if ( buf[ i ] == ')' ) --lvl;

                                //
                                dsc += buf[ i ];

                                //
                                if ( lvl == 0 )
                                {
                                    //
                                    flg = true; // next new entry
                                    add( format( ent ), format( dsc ), entry::TNSNAMES );
                                }
                            }
                        }
                    }
                }
            }
            catch ( exception& x )
            {
                throw exp( "File [" + tnsnames_ + "] read error: "  + string( x.what() ), EXP_OPEN );
            }
        }
        else
            throw exp( "TNS Names file [tnsnames.ora] not found or is invalid", EXP_MISSING );
    }

    //
    return entries_.size();
}

//
bool tns::resolve_directory()
{
    //
    if ( ( host_.length() == 0 ) )
    {
        //
        if ( ldap_.length() > 0 )
        {
            //
            ifstream ifs;

            try
            {
                string buf; // line buffer

                //
                ifs.open( ldap_.c_str(), ios::in );

                //
                while ( getline( ifs, buf ) )
                {
                    size_t pos;

                    //
                    buf = trim( buf );

                    //
                    if ( buf.substr( 0, 1 ) == "#" )
                        continue;

                    //
                    if ( ( pos = ucase( buf ).find( "DIRECTORY_SERVERS", 0 ) ) != string::npos )
                    {
                        size_t lpos = buf.find_first_of( "(" );

                        if ( lpos != string::npos )
                        {
                            size_t rpos = buf.find_first_of( ")", lpos + 1 );

                            if ( rpos != string::npos )
                            {
                                string dat = buf.substr( lpos + 1, rpos - ( lpos + 1 ) );

                                host_ = split( dat, 0, ":" );
                                port_ = split( dat, 1, ":" );
                                pssl_ = split( dat, 2, ":" );
                            }
                        }
                    }

                    //
                    if ( ( pos = ucase( buf ).find( "DEFAULT_ADMIN_CONTEXT", 0 ) ) != string::npos )
                    {
                        size_t lpos = buf.find_first_of( "\"" );

                        if ( lpos != string::npos )
                        {
                            size_t rpos = buf.find_first_of( "\"", lpos + 1 );

                            if ( rpos != string::npos )
                                root_ = trim( buf.substr( lpos + 1, rpos - ( lpos + 1 ) ) );
                        }
                    }

                    //
                    if ( ( pos = ucase( buf ).find( "DIRECTORY_SERVER_TYPE", 0 ) ) != string::npos )
                    {
                        size_t lpos = buf.find_first_of( "=" );

                        if ( lpos != string::npos )
                        {
                            string typ = ucase( trim( buf.substr( lpos + 1 ) ) );

                            if ( typ != "OID" )
                                throw exp( "Unsupported directory type [" + typ + "] encountered", EXP_UNSUPPORTED );
                        }
                    }
                }
            }
            catch ( exception& x )
            {
                throw exp( "File [" + sqlnet_ + "] read error: "  + string( x.what() ), EXP_OPEN );
            }
        }
    }

    return ( host_.length() > 0 );
}

//
size_t tns::load_ldap()
{
    if ( ldap_.length() > 0 )
    {
        if ( mesg() )
        {
            string url;
            int rc = LDAP_SUCCESS;

            try
            {
                if ( resolve_directory() )
                {
                    LDAP *ld;

                    //
                    url = "ldap://" + host_ + ":" + port_;

                    //
                    if ( ( ld = ::ldap_init( (char*)host_.c_str(), ::atoi( port_.c_str() ) ) ) != NULL )
                    {
                        // use the first element for sorting
                        const char* attrs[]  = { ATTR_VAL_CN, ATTR_VAL_DESC, ATTR_VAL_ALIAS, NULL };
                        const char  filter[] = "(|(|(objectclass=orclNetService)(objectclass=orclService))(objectclass=orclNetServiceAlias))";
                        struct timeval tv    = { 1, 0 };
                        LDAPMessage* res     = NULL;

                        if ( ( rc = ::ldap_simple_bind_s( ld, NULL, NULL ) ) != LDAP_SUCCESS )
                            throw exp ( "LDAP anonymous bind failed: " + string( ::ldap_err2string( rc ) ), rc );

                        //
                        if ( ( rc = ::ldap_search_ext_s( ld,
                                                         (char*)((root_.length() == 0 ) ? "" : root_.c_str()),
                                                         LDAP_SCOPE_SUBTREE,
                                                         (char*)filter,
                                                         (char**)attrs,
                                                         0,
                                                         NULL,
                                                         NULL,
                                                         &tv,
                                                         LDAP_NO_LIMIT,
                                                         &res ) ) == LDAP_SUCCESS )
                        {
                            LDAPMessage* ent = NULL;
                            BerElement*  ber = NULL;

                            //
                            ::ldap_sort_entries( ld, &res, (char*)attrs[ ATTR_IDX_CN ], (int(*)())::strcmp );

                            //
                            for ( ent = ::ldap_first_entry( ld, res );
                                  ent != NULL;
                                  ent = ::ldap_next_entry( ld, ent ) )
                            {
                                char* dn   = NULL;
                                char* attr = NULL;
                                entry itm;

                                //
                                itm.type = entry::LDAP;

                                //
                                if ( ( dn = ::ldap_get_dn( ld, ent ) ) != NULL )
                                {
                                    //
                                    for ( attr = ::ldap_first_attribute( ld, ent, &ber );
                                          attr != NULL;
                                          attr = ::ldap_next_attribute( ld, ent, ber ) )
                                    {
                                        char** vals;

                                        if ( ( vals = ::ldap_get_values( ld, ent, attr ) ) != NULL )
                                        {
                                            //
                                            // (*sortval)( ld, vals, ::strcmp );

                                            //
                                            for ( int idx = 0; vals[ idx ] != NULL; idx++ )
                                            {
                                                //
                                                if ( format( string( attr ) ) == format( string( ATTR_VAL_CN ) ) )
                                                {
                                                    if ( itm.name.length() == 0 )
                                                        itm.name = format( vals[ idx ] );
                                                }

                                                if ( format( string( attr ) ) == format( string( ATTR_VAL_ALIAS ) ) )
                                                    itm.name = format( vals[ idx ] );

                                                if ( format( string( attr ) ) == format( string( ATTR_VAL_DESC ) ) )
                                                    itm.desc = format( vals[ idx ] );
                                            }

                                            //
                                            ::ldap_value_free( vals );
                                        }

                                        //
                                        ::ldap_memfree( attr );
                                    }

                                    ::ldap_memfree( dn );
                                }
                                else
                                    cerr << "NULL DN encountered" << endl;

                                //
                                ::ber_free( ber, 0 );

                                //
                                if ( ( itm.name.length() > 0  ) && ( itm.desc.length() > 0 ) )
                                    add( itm );
                            }
                        }
                        else
                            throw exp ( "LDAP search failed: " + string( ::ldap_err2string( rc ) ), rc );
                    }
                    else
                    {
                        throw exp ( "LDAP initialization failed", rc );
                    }
                }
                else
                    throw exp ( "Unresolved directory information", EXP_UNRESOLVED );
            }
            catch ( exp& x )
            {
                // report but don;t throw, for now
                cerr << url << " - " << x.what() << " [" << rc << "]" << endl << endl;
            }
        }
        else
            cerr << MISSING_MESG_FILE << endl << endl;
    }

    //
    return 0;
}

//
string tns::format( string str )
{
    str.erase( remove( str.begin(), str.end(), ' ' ),  str.end() );
    str.erase( remove( str.begin(), str.end(), '\r' ), str.end() );
    str.erase( remove( str.begin(), str.end(), '\n' ), str.end() );
    str.erase( remove( str.begin(), str.end(), '\t' ), str.end() );

    return trim( ucase( str ) );
}

//
string tns::resolve( string ent )
{
    string dsc;

    //
    ent = format( ent );

    //
    for ( item itm = entries_.begin(); itm != entries_.end(); ++itm )
    {
        //
        if ( ent == itm->name )
        {
            dsc = itm->desc;
            break;
        }
    }

    return dsc;
}

//
string tns::compare( string dsc )
{
    string ent;

    //
    dsc = format( dsc );

    //
    for ( item itm = entries_.begin(); itm != entries_.end(); ++itm )
    {
        //
        if ( dsc == itm->desc )
        {
            ent = itm->name;
            break;
        }
    }

    return ent;
}
