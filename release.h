//
// release.h
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

#ifndef __RELEASE_HPP
#define __RELEASE_HPP

#include <stdio.h>

//
#define APPLICATION         "Oracle Ping (oraping)"
#define VERSION             "1.0.0.1"
#define BUILD               "1"
#define CORPORATION         "Metasystems Technologies Inc. (MTI)"
#define COPYWRITE           "Copyright (c) 2004, 2014, MTI.  All rights reserved."

//
void version()
{
    ::fprintf( stdout, "\n%s - Version: %s, Build: %s\n",
                       APPLICATION,
                       VERSION,
                       BUILD );
}

//
void copyright()
{
    ::fprintf( stdout, "\n%s\n%s\n\n",
                       CORPORATION,
                       COPYWRITE );
}

#endif // __RELEASE_HPP
