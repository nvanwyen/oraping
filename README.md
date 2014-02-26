oraping
=======

Oracle TNS*Ping (tnsping) command line alternative, designed to work with Oracle Instant Client on Linux and Windows

The precompiled directory contains builds for common targets, in case you do
not want to build them yourself from the source.

The directory structure is as follows ...

    |-- ldap
    |   `-- mesg
    |       `-- ldapus.msb
    |-- linux
    |   `-- oraping.3.10.25-gentoo
    |-- README
    `-- windows
        |-- win32
        |   `-- oraping.exe
        `-- win64
            `-- oraping.exe

The Windows releases are based on the 32 and 64 bit distributions and the
Linux build is based only on the 64 bit distribution. While the Linux build
was created for Gentoo Linux, it should work on any Linux distribution that
support the 3.10.x 64 bit architecture and instant client.

The ldap/mesg directory contains the "ldapus.msb" file, from the 11.2.0.3
Oracle Client release and is included here only because the OCI library
(libclntsh.so on Linux and oci.dll on Windows) must have this file in the
$ORACLE_HOME/ldap/mesg/ directory if TNS information is being used in the OID

The Oracle Ping (oraping) utility supports the TNSNAMES, LDAP and EZCONNECT
options for name/description resolution (ONAMES are not supported). It uses the
OCI API OCIServerAttach to verify the connectivity of the server, not a dummy
user/password combination (as some solutions do). The reason for this is simple,
security. A server attach call does not imply an "Unsuccessful" login attempt
and therefore will not generate an audit trail showing an attempt to connect to
database by session.

Finally, because Oracle ping is a simple utility, created quickly, the error
messages returned from the Oracle Client are ORA-* messages and are not
translated to TNS-* messages.

Source
--------------------------------------------------------------------------------
All source code for Oracle Ping is included, along with project and solution
files for both Make, Code::Blocks and Visual Studio (2010).

