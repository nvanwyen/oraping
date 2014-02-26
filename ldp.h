//
// ldp.h
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

// this is a "ldap.h" header, which is limited to only
// those API used by oraping. It is included here because
// Oracle does not provide one with the baseline Instant
// Client distribution, albeit the API are avaiable in the
// libclntsh.so library and the OpenLDAP header conflicts
// with the Oracle libraries (e.g. you cannot use the standard
// ldap library in the same process space as the oracle client
// because of symbol collisions)
//
// the complete Oracle "ldap.h" file can be found in the full
// client and database distrubutions in the directory
//    $ORACLE_HOME/ldap/public/
// or as part of the OID SDK distribution

#ifndef __LDP_H
#define __LDP_H

#include <stdlib.h>

//
#define LDAP_SUCCESS                0x0000
#define LDAP_OPERATIONS_ERROR       0x0001

//
/* search scopes */
#define LDAP_SCOPE_BASE             0x0000
#define LDAP_SCOPE_ONELEVEL         0x0001
#define LDAP_SCOPE_SUBTREE          0x0002

//
#define LDAP_OPT_TIMELIMIT          0x0004

// not part of Oracle
#define LDAP_OPT_TIMEOUT            0x5002  /* default timeout */
#define LDAP_OPT_NETWORK_TIMEOUT    0x5005  /* socket level timeout */

#ifdef __cplusplus
extern "C" {
#endif

//
typedef struct seqorset {
    unsigned int     sos_clen;
    unsigned int     sos_tag;
    char            *sos_first;
    char            *sos_ptr;
    struct seqorset *sos_next;
} Seqorset;
#define NULLSEQORSET    ((Seqorset *) 0)

#define SOS_STACK_SIZE  8 /* depth of the pre-allocated sos structure stack */

//
typedef int (*BERTranslateProc)( char **bufp,
                                 unsigned int *buflenp,
                                 int free_input );

//
typedef struct berelement {
    char            *ber_buf;
    char            *ber_ptr;
    char            *ber_end;
    struct seqorset *ber_sos;
    unsigned int     ber_tag;
    unsigned int     ber_len;
    int              ber_usertag;
    char             ber_options;
#define LBER_USE_DER            0x01
#define LBER_USE_INDEFINITE_LEN 0x02
#define LBER_TRANSLATE_STRINGS  0x04
    char            *ber_rwptr;
    BERTranslateProc ber_encode_translate_proc;
    BERTranslateProc ber_decode_translate_proc;
    int              ber_flags;
    int              ber_sos_stack_posn;
    Seqorset         ber_sos_stack[SOS_STACK_SIZE];
} BerElement;
#define NULLBER ((BerElement *) 0)

//
/* sgsluns - private network endpoint type
 */
struct sgsluns
{
    int   sock_sgsluns;      /* socket identifier */
    long  state_sgsluns;     /* sgsluns state flag (SGSLUNS_STATE_xxx) */
    char  saddr_sgsluns[32]; /* inet address of other end of this connection
                              * in the form A.B.C.D where A,B,C, and D are
                              * base 256 notation integers. eg. "192.0.0.1"
                              */
};
typedef struct sgsluns sgsluns;

//
typedef struct sockbuf {
#ifndef MACOS
    int     sb_sd;
#else /* MACOS */
    void        *sb_sd;
#endif /* MACOS */
    BerElement  sb_ber;

    int       sb_naddr;                 /* > 0 implies using CLDAP (UDP) */
    void     *sb_useaddr;               /* pointer to sockaddr to use next */
    void     *sb_fromaddr;              /* pointer to message source sockaddr */
    void    **sb_addrs;                 /* actually an array of pointers to
                                           sockaddrs */

    int     sb_options;                 /* to support copying ber elements */
#define LBER_TO_FILE            0x01    /* to a file referenced by sb_fd   */
#define LBER_TO_FILE_ONLY       0x02    /* only write to file, not network */
#define LBER_MAX_INCOMING_SIZE  0x04    /* impose limit on incoming stuff  */
#define LBER_NO_READ_AHEAD      0x08    /* read only as much as requested  */
    int      sb_fd;
    int      sb_max_incoming;
    void    *sb_nzenv;                  /* nzos environment pointer */
    void    *sb_sslContext;
    int      sb_sslauth;                /* type of ssl authentication */
    int      sb_sslflag;
    int      ssl_sd;
    sgsluns *sb_endp;                   /* socket end point */
} Sockbuf;
#define READBUFSIZ  8192

/* structure for returning a sequence of octet strings + length */
struct berval {
    unsigned int    bv_len;
    char        *bv_val;
};

//
/*
 * This structure represents both ldap messages and ldap responses.
 * These are really the same, except in the case of search responses,
 * where a response has multiple messages.
 */
typedef struct ldapmsg {
    int             lm_msgid;   /* the message id */
    int             lm_msgtype; /* the message type */
    BerElement     *lm_ber;     /* the ber encoded message contents */
    struct ldapmsg *lm_chain;   /* for search - next msg in the resp */
    struct ldapmsg *lm_next;    /* next response */
    unsigned int    lm_time;    /* used to maintain cache */
} LDAPMessage;
#define NULLMSG ((LDAPMessage *) NULL)

//
/*
 * structure for client cache
 */
#define LDAP_CACHE_BUCKETS  31  /* cache hash table size */
typedef struct ldapcache {
    LDAPMessage *lc_buckets[LDAP_CACHE_BUCKETS];    /* hash table */
    LDAPMessage *lc_requests;                       /* unfulfilled reqs */
    int          lc_timeout;                        /* request timeout */
    int          lc_maxmem;                         /* memory to use */
    int          lc_memused;                        /* memory in use */
    int          lc_enabled;                        /* enabled? */
    unsigned int lc_options;                        /* options */
#define LDAP_CACHE_OPT_CACHENOERRS  0x00000001
#define LDAP_CACHE_OPT_CACHEALLERRS 0x00000002
}  LDAPCache;
#define NULLLDCACHE ((LDAPCache *)NULL)

/*
 * structures for ldap getfilter routines
 */

//
typedef struct ldap_filt_info {
    char                  *lfi_filter;
    char                  *lfi_desc;
    int                    lfi_scope;               /* LDAP_SCOPE_BASE, etc */
    int                    lfi_isexact;             /* exact match filter? */
    struct ldap_filt_info *lfi_next;
} LDAPFiltInfo;

//
typedef struct ldap_filt_list {
    char                  *lfl_tag;
    char                  *lfl_pattern;
    char                  *lfl_delims;
    LDAPFiltInfo          *lfl_ilist;
    struct ldap_filt_list *lfl_next;
} LDAPFiltList;

//
#define LDAP_FILT_MAXSIZ    1024

//
typedef struct ldap_filt_desc {
    LDAPFiltList    *lfd_filtlist;
    LDAPFiltInfo    *lfd_curfip;
    LDAPFiltInfo     lfd_retfi;
    char             lfd_filter[ LDAP_FILT_MAXSIZ ];
    char            *lfd_curval;
    char            *lfd_curvalcopy;
    char            **lfd_curvalwords;
    char            *lfd_filtprefix;
    char            *lfd_filtsuffix;
} LDAPFiltDesc;

//
typedef struct ldapcontrol {
    char          *ldctl_oid;
    struct berval  ldctl_value;
    char           ldctl_iscritical;
} LDAPControl, *PLDAPControl;

//
typedef struct ldappend {
    void            *lp_sema;       /* semaphore to post */
    int              lp_msgid;      /* message id */
    LDAPMessage     *lp_result;     /* result storage */
    struct ldappend *lp_prev;       /* previous pending */
    struct ldappend *lp_next;       /* next pending */
} LDAPPend;

//
/* structure for holding credential information - used
 * to bind to referred server when chasing referrals
 */
typedef struct ldapcreds {
       char    *binddn;
       char    *passwd;
       int      authmethod;
       char    *proxyDN;
} LDAPCreds;

//
/*
 * structure representing an ldap connection
 */
typedef struct ldap {
    Sockbuf  ld_sb;                     /* socket descriptor & buffer */
    char    *ld_host;
    int      ld_version;
    char     ld_lberoptions;
    int      ld_deref;
#define LDAP_DEREF_NEVER        0
#define LDAP_DEREF_SEARCHING    1
#define LDAP_DEREF_FINDING      2
#define LDAP_DEREF_ALWAYS       3

    int      ld_timelimit;
    int      ld_sizelimit;
#define LDAP_NO_LIMIT           0

    LDAPFiltDesc *ld_filtd;             /* from getfilter for ufn searches */
    char         *ld_ufnprefix;         /* for incomplete ufn's */

    int           ld_errno;
    char         *ld_error;
    char         *ld_matched;
    int           ld_msgid;

    /* do not mess with these */
#ifdef LDAP_REFERRALS
    LDAPRequest * ld_requests;          /* list of outstanding requests */
#else /* LDAP_REFERRALS */
    LDAPMessage  *ld_requests;          /* list of outstanding requests */
#endif /* LDAP_REFERRALS */
    LDAPMessage  *ld_responses;         /* list of outstanding responses */
    int          *ld_abandoned;         /* array of abandoned requests */
    char          ld_attrbuffer[100];
    LDAPCache    *ld_cache;             /* non-null if cache is initialized */
    char         *ld_cldapdn;           /* DN used in connectionless search */

    /* it is OK to change these next four values directly */
    int           ld_cldaptries;        /* connectionless search retry count */
    int           ld_cldaptimeout;      /* time between retries */
    int           ld_refhoplimit;       /* limit on referral nesting */
    unsigned int  ld_options;           /* boolean options */
#ifdef LDAP_DNS
#define LDAP_OPT_DNS        0x00000001  /* use DN & DNS */
#endif /* LDAP_DNS */

#define LDAP_BITOPT_REFERRALS   0x80000000
#define LDAP_BITOPT_SSL         0x40000000
#define LDAP_BITOPT_DNS         0x20000000
#define LDAP_BITOPT_RESTART     0x10000000
#define LDAP_BITOPT_RECONNECT   0x08000000
#define LDAP_BITOPT_ASYNC       0x04000000

    /* do not mess with the rest though */
    char            *ld_defhost;        /* full name of default server */
    int              ld_defport;        /* port of default server */
    BERTranslateProc ld_lber_encode_translate_proc;
    BERTranslateProc ld_lber_decode_translate_proc;
#ifdef LDAP_REFERRALS
    LDAPConn    *    ld_defconn;        /* default connection */
    LDAPConn    *    ld_conns;          /* list of server connections */
    void        *    ld_selectinfo;     /* platform specifics for select */
    int            (*ld_rebindproc)( struct ldap *ld,
                                     char **dnp,
                                     char **passwdp,
                                     int *authmethodp,
                                     int freeit );
    /* routine to get info needed for re-bind */
#endif /* LDAP_REFERRALS */

    /* V3 ldap controls */
    LDAPControl     **ld_servercontrol;
    LDAPControl     **ld_clientcontrol;

    /* Pending results */
    LDAPPend        *ld_pend;           /* list of pending results */

    void            *gsluctx;           /* gslu context handle */
    /* client side referral cache-usage enable/disable option */
    int             ld_refcache_enabled;
    /* current status of the refcache */
    int             ld_refcache_status;
    void            *ld_refcache;       /* client side referral cache */
    void            *ld_wrk_selectinfo;
    LDAPCreds       *ld_creds;          /* credential information */
} LDAP;

#if !defined(MACOS) && !defined(DOS) && !defined(_WIN32) && !defined(WINSOCK)
#include <sys/time.h>
#endif

//
LDAP *ldap_init( char *defhost,
                 int defport );

//
int ldap_set_option( LDAP *ld,
                     int option,
                     void *optdata );

//
int ldap_simple_bind_s( LDAP *ld,
                        char *who,
                        char *passwd );

//
int ldap_search_ext_s( LDAP *ld,
                       char *base,
                       int scope,
                       char *filter,
                       char **attrs,
                       int attrsonly,
                       LDAPControl **serverctrls,
                       LDAPControl **clientctrls,
                       struct timeval *timeoutp,
                       int sizelimit,
                       LDAPMessage **res);

//
int ldap_result( LDAP *ld,
                 int msgid,
                 int all,
                 struct timeval *timeout,
                 LDAPMessage **result );

//
int ldap_count_entries( LDAP *ld,
                        LDAPMessage *chain );

//
int ldap_count_messages( LDAP *ld,
                         LDAPMessage *chain );

//
LDAPMessage *ldap_first_entry( LDAP *ld,
                               LDAPMessage *chain );

//
LDAPMessage *ldap_next_entry( LDAP *ld,
                              LDAPMessage *entry );

//
char *ldap_first_attribute( LDAP *ld,
                            LDAPMessage *entry,
                            BerElement **ber );

//
char *ldap_next_attribute( LDAP *ld,
                           LDAPMessage *entry,
                           BerElement *ber );

//
char **ldap_get_values( LDAP *ld,
                        LDAPMessage *entry,
                        char *target );

//
int ldap_msgfree( LDAPMessage *lm );

//
void ldap_memfree( void *p );

//
void ldap_value_free( char **vals );

//
int ldap_sort_entries( LDAP *ld,
                       LDAPMessage **chain,
                       char *attr,
                       int (*cmp)() );

//
#ifdef __LDAP_SORT_VALUES
#define __LDAP_SORT_VALUES

//
typedef int lcomp( const char* left,
                   const char* right );

// not part of the Oracle library
int ldap_sort_values( LDAP  *ld,
                      char  **vals,
                      lcomp *cmp )
{
    int sz = 0;

    for ( int i = 0; vals[ i ] != NULL; i++ )
        ++sz;

    qsort( vals, sz, sizeof( char* ), cmp );

    return sz;
}

#endif // __LDAP_SORT_VALUES

//
char *ldap_get_dn( LDAP *ld,
                   LDAPMessage *entry );

//
char *ldap_err2string( int err );

//
void ber_free( BerElement *ber,
               int freebuf );

/*
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_init"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_set_option"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_simple_bind_s"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_search_ext_s"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_result"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_count_entries"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_count_messages"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_first_entry"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_next_entry"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_first_attribute"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_next_attribute"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_get_values"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_msgfree"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_memfree"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_value_free"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_sort_entries"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_sort_values"    # as expeced not in th Oracle library
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_get_dn"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ldap_err2string"
    objdump -T $ORACLE_HOME/lib64/libclntsh.so | egrep -w "ber_free"
*/

#ifdef __cplusplus
}
#endif
#endif // __LDP_H
