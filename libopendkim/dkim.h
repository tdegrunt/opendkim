/*
**  Copyright (c) 2005-2009 Sendmail, Inc. and its suppliers.
**    All rights reserved.
**
**  Copyright (c) 2009, The OpenDKIM Project.  All rights reserved.
*/

#ifndef _DKIM_H_
#define _DKIM_H_

#ifndef lint
static char dkim_h_id[] = "@(#)$Id: dkim.h,v 1.12 2009/10/27 04:17:08 cm-msk Exp $";
#endif /* !lint */

/* system includes */
#include <sys/types.h>
#include <sys/param.h>
#include <stdbool.h>

/* openssl includes */
#include <openssl/sha.h>

/*
**  version -- 0xrrMMmmpp
**  
**  	rr == release number
**  	MM == major revision number
**  	mm == minor revision number
**  	pp == patch number
*/

#define	OPENDKIM_LIB_VERSION	0x01010200

#ifdef __STDC__
# ifndef __P
#  define __P(x)  x
# endif /* ! __P */
#else /* __STDC__ */
# ifndef __P
#  define __P(x)  ()
# endif /* ! __P */
#endif /* __STDC__ */

/* definitions */
#define DKIM_HDRMARGIN		75	/* "standard" header margin */
#define DKIM_MAXHEADER		4096	/* buffer for caching one header */
#define	DKIM_MAXHOSTNAMELEN	256	/* max. FQDN we support */
#define	DKIM_SIGNHEADER		"DKIM-Signature"
					/* DKIM signature header */

/* special DNS tokens */
#define	DKIM_DNSKEYNAME		"_domainkey"
					/* reserved DNS sub-zone */
#define	DKIM_DNSPOLICYNAME	"_adsp"	/* reserved DNS sub-zone */

/*
**  DKIM_STAT -- status code type
*/

typedef int DKIM_STAT;

#define	DKIM_STAT_OK		0	/* function completed successfully */
#define	DKIM_STAT_BADSIG	1	/* signature available but failed */
#define	DKIM_STAT_NOSIG		2	/* no signature available */
#define	DKIM_STAT_NOKEY		3	/* public key not found */
#define	DKIM_STAT_CANTVRFY	4	/* can't get domain key to verify */
#define	DKIM_STAT_SYNTAX	5	/* message is not valid syntax */
#define	DKIM_STAT_NORESOURCE	6	/* resource unavailable */
#define	DKIM_STAT_INTERNAL	7	/* internal error */
#define	DKIM_STAT_REVOKED	8	/* key found, but revoked */
#define	DKIM_STAT_INVALID	9	/* invalid function parameter */
#define	DKIM_STAT_NOTIMPLEMENT	10	/* function not implemented */
#define	DKIM_STAT_KEYFAIL	11	/* key retrieval failed */
#define	DKIM_STAT_CBREJECT	12	/* callback requested reject */
#define	DKIM_STAT_CBINVALID	13	/* callback gave invalid result */
#define	DKIM_STAT_CBTRYAGAIN	14	/* callback says try again later */
#define	DKIM_STAT_CBERROR	15	/* callback error */
#define	DKIM_STAT_MULTIDNSREPLY	16	/* multiple DNS replies */

/*
**  DKIM_CBSTAT -- callback status code type
*/

typedef int DKIM_CBSTAT;

#define	DKIM_CBSTAT_CONTINUE	0	/* continue */
#define	DKIM_CBSTAT_REJECT	1	/* reject */
#define	DKIM_CBSTAT_TRYAGAIN	2	/* try again later */
#define	DKIM_CBSTAT_NOTFOUND	3	/* requested record not found */
#define	DKIM_CBSTAT_ERROR	4	/* error requesting record */

/*
**  DKIM_SIGERROR -- signature errors
*/

typedef int DKIM_SIGERROR;

#define DKIM_SIGERROR_UNKNOWN		(-1)	/* unknown error */
#define DKIM_SIGERROR_OK		0	/* no error */
#define DKIM_SIGERROR_VERSION		1	/* unsupported version */
#define DKIM_SIGERROR_DOMAIN		2	/* invalid domain (d=/i=) */
#define DKIM_SIGERROR_EXPIRED		3	/* signature expired */
#define DKIM_SIGERROR_FUTURE		4	/* signature in the future */
#define DKIM_SIGERROR_TIMESTAMPS	5	/* x= < t= */
#define DKIM_SIGERROR_MISSING_C		6	/* c= missing */
#define DKIM_SIGERROR_INVALID_HC	7	/* c= invalid (header) */
#define DKIM_SIGERROR_INVALID_BC	8	/* c= invalid (body) */
#define DKIM_SIGERROR_MISSING_A		9	/* a= missing */
#define DKIM_SIGERROR_INVALID_A		10	/* a= invalid */
#define DKIM_SIGERROR_MISSING_H		11	/* h= missing */
#define DKIM_SIGERROR_INVALID_L		12	/* l= invalid */
#define DKIM_SIGERROR_INVALID_Q		13	/* q= invalid */
#define DKIM_SIGERROR_INVALID_QO	14	/* q= option invalid */
#define DKIM_SIGERROR_MISSING_D		15	/* d= missing */
#define DKIM_SIGERROR_EMPTY_D		16	/* d= empty */
#define DKIM_SIGERROR_MISSING_S		17	/* s= missing */
#define DKIM_SIGERROR_EMPTY_S		18	/* s= empty */
#define DKIM_SIGERROR_MISSING_B		19	/* b= missing */
#define DKIM_SIGERROR_EMPTY_B		20	/* b= empty */
#define DKIM_SIGERROR_CORRUPT_B		21	/* b= corrupt */
#define DKIM_SIGERROR_NOKEY		22	/* no key found in DNS */
#define DKIM_SIGERROR_DNSSYNTAX		23	/* DNS reply corrupt */
#define DKIM_SIGERROR_KEYFAIL		24	/* DNS query failed */
#define DKIM_SIGERROR_MISSING_BH	25	/* bh= missing */
#define DKIM_SIGERROR_EMPTY_BH		26	/* bh= empty */
#define DKIM_SIGERROR_CORRUPT_BH	27	/* bh= corrupt */
#define DKIM_SIGERROR_BADSIG		28	/* signature mismatch */
#define DKIM_SIGERROR_SUBDOMAIN		29	/* unauthorized subdomain */
#define DKIM_SIGERROR_MULTIREPLY	30	/* multiple records returned */
#define DKIM_SIGERROR_EMPTY_H		31	/* h= empty */
#define DKIM_SIGERROR_INVALID_H		32	/* h= missing req'd entries */
#define DKIM_SIGERROR_TOOLARGE_L	33	/* l= value exceeds body size */
#define DKIM_SIGERROR_MBSFAILED		34	/* "must be signed" failure */
#define	DKIM_SIGERROR_KEYVERSION	35	/* unknown key version */
#define	DKIM_SIGERROR_KEYUNKNOWNHASH	36	/* unknown key hash */
#define	DKIM_SIGERROR_KEYHASHMISMATCH	37	/* sig-key hash mismatch */
#define	DKIM_SIGERROR_NOTEMAILKEY	38	/* not an e-mail key */
#define	DKIM_SIGERROR_GRANULARITY	39	/* key granularity mismatch */
#define	DKIM_SIGERROR_KEYTYPEMISSING	40	/* key type missing */
#define	DKIM_SIGERROR_KEYTYPEUNKNOWN	41	/* key type unknown */
#define	DKIM_SIGERROR_KEYREVOKED	42	/* key revoked */
#define	DKIM_SIGERROR_KEYDECODE		43	/* key couldn't be decoded */

/*
**  DKIM_CANON -- canonicalization method
*/

typedef int dkim_canon_t;

#define DKIM_CANON_UNKNOWN	(-1)	/* unknown method */
#define DKIM_CANON_SIMPLE	0	/* as specified in DKIM spec */
#define DKIM_CANON_RELAXED	1	/* as specified in DKIM spec */

#define DKIM_CANON_DEFAULT	DKIM_CANON_SIMPLE

/*
**  DKIM_SIGN -- signing method
*/

typedef int dkim_alg_t;

#define DKIM_SIGN_UNKNOWN	(-1)	/* unknown method */
#define DKIM_SIGN_RSASHA1	0	/* an RSA-signed SHA1 digest */
#ifdef SHA256_DIGEST_LENGTH
# define DKIM_SIGN_RSASHA256	1	/* an RSA-signed SHA256 digest */
#endif /* SHA256_DIGEST_LENGTH */

#ifdef SHA256_DIGEST_LENGTH
# define DKIM_SIGN_DEFAULT	DKIM_SIGN_RSASHA256
#else /* SHA256_DIGEST_LENGTH */
# define DKIM_SIGN_DEFAULT	DKIM_SIGN_RSASHA1
#endif /* SHA256_DIGEST_LENGTH */

/*
**  DKIM_QUERY -- query method
*/

typedef int dkim_query_t;

#define DKIM_QUERY_UNKNOWN	(-1)	/* unknown method */
#define DKIM_QUERY_DNS		0	/* DNS query method (per the draft) */
#define DKIM_QUERY_FILE		1	/* text file method (for testing) */

#define DKIM_QUERY_DEFAULT	DKIM_QUERY_DNS

/*
**  DKIM_PARAM -- known signature parameters
*/

typedef int dkim_param_t;

#define DKIM_PARAM_UNKNOWN	(-1)	/* unknown */
#define DKIM_PARAM_SIGNATURE	0	/* b */
#define DKIM_PARAM_SIGNALG	1	/* a */
#define DKIM_PARAM_DOMAIN	2	/* d */
#define DKIM_PARAM_CANONALG	3	/* c */
#define DKIM_PARAM_QUERYMETHOD	4	/* q */
#define DKIM_PARAM_SELECTOR	5	/* s */
#define DKIM_PARAM_HDRLIST	6	/* h */
#define DKIM_PARAM_VERSION	7	/* v */
#define DKIM_PARAM_IDENTITY	8	/* i */
#define DKIM_PARAM_TIMESTAMP	9	/* t */
#define DKIM_PARAM_EXPIRATION	10	/* x */
#define DKIM_PARAM_COPIEDHDRS	11	/* z */
#define DKIM_PARAM_BODYHASH	12	/* bh */
#define DKIM_PARAM_BODYLENGTH	13	/* l */

/*
**  DKIM_POLICY -- policies
*/

typedef int dkim_policy_t;

#define DKIM_POLICY_NONE	(-1)	/* none/undefined */
#define DKIM_POLICY_UNKNOWN	0	/* unknown */
#define DKIM_POLICY_ALL		1	/* all */
#define DKIM_POLICY_DISCARDABLE	2	/* discardable */

#define DKIM_POLICY_DEFAULT	DKIM_POLICY_UNKNOWN
#define DKIM_POLICY_DEFAULTTXT	"dkim=unknown"

/*
**  DKIM_PRESULT -- policy results
*/

#define DKIM_PRESULT_NONE		(-1)	/* none/undefined */
#define DKIM_PRESULT_NXDOMAIN		0	/* domain does not exist */
#define DKIM_PRESULT_AUTHOR		1	/* ADSP found for author */

/*
**  DKIM_MODE -- mode of a handle
*/

#define	DKIM_MODE_UNKNOWN	(-1)
#define	DKIM_MODE_SIGN		0
#define	DKIM_MODE_VERIFY	1

/*
**  DKIM_OPTS -- library-specific options
*/

typedef int dkim_opt_t;

#define DKIM_OP_GETOPT		0
#define	DKIM_OP_SETOPT		1

typedef int dkim_opts_t;

#define	DKIM_OPTS_FLAGS		0
#define	DKIM_OPTS_TMPDIR	1
#define	DKIM_OPTS_TIMEOUT	2
#define	DKIM_OPTS_SENDERHDRS	3
#define	DKIM_OPTS_SIGNHDRS	4
#define	DKIM_OPTS_VERSION	5	/* unused */
#define	DKIM_OPTS_QUERYMETHOD	6
#define	DKIM_OPTS_QUERYINFO	7
#define	DKIM_OPTS_FIXEDTIME	8
#define	DKIM_OPTS_SKIPHDRS	9
#define	DKIM_OPTS_ALWAYSHDRS	10
#define	DKIM_OPTS_SIGNATURETTL	11
#define	DKIM_OPTS_CLOCKDRIFT	12
#define	DKIM_OPTS_MUSTBESIGNED	13

#define	DKIM_LIBFLAGS_NONE		0x000
#define	DKIM_LIBFLAGS_TMPFILES		0x001
#define	DKIM_LIBFLAGS_KEEPFILES		0x002
#define	DKIM_LIBFLAGS_SIGNLEN		0x004
#define DKIM_LIBFLAGS_CACHE		0x008
#define DKIM_LIBFLAGS_ZTAGS		0x010
#define DKIM_LIBFLAGS_DELAYSIGPROC	0x020
#define DKIM_LIBFLAGS_EOHCHECK		0x040
#define DKIM_LIBFLAGS_ACCEPTV05		0x080
#define DKIM_LIBFLAGS_FIXCRLF		0x100
#define DKIM_LIBFLAGS_ACCEPTDK		0x200

#define	DKIM_LIBFLAGS_DEFAULT		DKIM_LIBFLAGS_NONE

#define DKIM_REP_DEFREJECT	1001
#define DKIM_REP_ROOT		"al.dkim-reputation.org"

/*
**  DKIM_DNSSEC -- results of DNSSEC queries
*/

#define DKIM_DNSSEC_UNKNOWN	(-1)
#define DKIM_DNSSEC_BOGUS	0
#define DKIM_DNSSEC_INSECURE	1
#define DKIM_DNSSEC_SECURE	2

/*
**  DKIM_LIB -- library handle
*/

struct dkim_lib;
typedef struct dkim_lib DKIM_LIB;

/*
**  DKIM -- DKIM context
*/

struct dkim;
typedef struct dkim DKIM;

/*
**  DKIM_SIGKEY_T -- private/public key (unencoded)
*/

typedef unsigned char * dkim_sigkey_t;

/*
**  DKIM_SIGINFO -- signature information for use by the caller
*/

struct dkim_siginfo;
typedef struct dkim_siginfo DKIM_SIGINFO;

#define DKIM_SIGFLAG_IGNORE		0x01
#define DKIM_SIGFLAG_PROCESSED		0x02
#define DKIM_SIGFLAG_PASSED		0x04
#define DKIM_SIGFLAG_TESTKEY		0x08
#define DKIM_SIGFLAG_NOSUBDOMAIN	0x10

#define DKIM_SIGBH_UNTESTED		(-1)
#define DKIM_SIGBH_MATCH		0
#define DKIM_SIGBH_MISMATCH		1

/*
**  DKIM_PSTATE -- policy query state
*/

struct dkim_pstate;
typedef struct dkim_pstate DKIM_PSTATE;

/*
**  DKIM_HDRDIFF -- header differences
*/

struct dkim_hdrdiff
{
	u_char *		hd_old;
	u_char *		hd_new;
};

/*
**  PROTOTYPES
*/

/*
**  DKIM_SET_TRUST_ANCHOR -- set trust anchor file path
**
**  Parameters:
**  	lib -- DKIM_LIB handle to update
**  	tafile -- trust anchor file path
** 
**  Return value:
**  	A DKIM_STAT constant.
*/

extern DKIM_STAT dkim_set_trust_anchor __P((DKIM_LIB *lib, char *tafile));

/*
**  DKIM_INIT -- initialize the DKIM package
**
**  Parameters:
**  	None.
**
**  Return value:
**  	A DKIM_STAT value.
*/

extern DKIM_LIB *dkim_init __P((void *(*mallocf)(void *closure, size_t nbytes),
                                void (*freef)(void *closure, void *p)));

/*
**  DKIM_CLOSE -- shut down the DKIM package
**
**  Parameters:
**  	lib -- DKIM_LIB handle to shut down
**
**  Return value:
**  	None.
*/

extern void dkim_close __P((DKIM_LIB *lib));

/*
**  DKIM_SIGN -- make a new DKIM context for signing
**
**  Parameters:
**  	libhandle -- library handle, returned by dkim_init()
**  	id -- an opaque printable string for identifying this message, suitable
**  	      for use in logging or debug output; may not be NULL
**  	memclosure -- memory closure, for use by user-provided malloc/free
**  	secretkey -- pointer to secret key data to use; if NULL, it will be
**  	             obtained from disk
**  	selector -- selector being used to sign
**  	domain -- domain on behalf of which we're signing
**  	hdr_canon_alg -- canonicalization algorithm to use for headers;
**  	                 one of the DKIM_CANON_* macros, or -1 for default
**  	body_canon_alg -- canonicalization algorithm to use for body;
**  	                  one of the DKIM_CANON_* macros, or -1 for default
**  	sign_alg -- signing algorithm to use; one of the DKIM_SIGN_* macros,
**  	            or -1 for default
**  	length -- number of bytes of the body to sign (-1 == all)
**  	statp -- pointer to a DKIM_STAT which is updated by this call
**
**  Return value:
**  	A newly-allocated DKIM handle, or NULL on failure.  "statp" will be
**  	updated.
*/

extern DKIM *dkim_sign __P((DKIM_LIB *libhandle, const char *id,
                            void *memclosure, const dkim_sigkey_t secretkey,
                            const char *selector, const char *domain,
                            dkim_canon_t hdr_canon_alg,
                            dkim_canon_t body_canon_alg,
                            dkim_alg_t sign_alg,
                            off_t length, DKIM_STAT *statp));

/*
**  DKIM_VERIFY -- make a new DKIM context for verifying
**
**  Parameters:
**  	libhandle -- library handle, returned by dkim_init()
**  	id -- an opaque printable string for identifying this message, suitable
**  	      for use in logging or debug output; may not be NULL
**  	memclosure -- memory closure, for use by user-provided malloc/free
**  	statp -- pointer to a DKIM_STAT which is updated by this call
**
**  Return value:
**  	A newly-allocated DKIM handle, or NULL on failure.  "statp" will be
**  	updated.
*/

extern DKIM *dkim_verify __P((DKIM_LIB *libhandle, const char *id,
                              void *memclosure, DKIM_STAT *statp));

/*
**  DKIM_HEADER -- process a header
**
**  Parameters:
**  	dkim -- a DKIM handle previously returned by dkim_sign() or
**  	        dkim_verify()
**  	hdr -- the header to be processed, in canonical format
**  	len -- number of bytes to process starting at "hdr"
**
**  Return value:
**  	A DKIM_STAT value.
*/

extern DKIM_STAT dkim_header __P((DKIM *dkim, u_char *hdr, size_t len));

/*
**  DKIM_EOH -- identify end of headers
**
**  Parameters:
**  	dkim -- a DKIM handle previously returned by dkim_sign() or
**  	        dkim_verify()
**
**  Return value:
**  	A DKIM_STAT value.  DKIM_STAT_NOSIG will be returned if we're
**  	validating a signature but no DKIM signature was found in the headers.
*/

extern DKIM_STAT dkim_eoh __P((DKIM *dkim));

/*
**  DKIM_BODY -- process a body chunk
**
**  Parameters:
**  	dkim -- a DKIM handle previously returned by dkim_sign() or
**  	        dkim_verify()
**  	buf -- the body chunk to be processed, in canonical format
**  	len -- number of bytes to process starting at "hdr"
**
**  Return value:
**  	A DKIM_STAT value.
*/

extern DKIM_STAT dkim_body __P((DKIM *dkim, u_char *buf, size_t len));

/*
**  DKIM_CHUNK -- process a message chunk
**
**  Parameters:
**  	dkim -- DKIM handle
**  	buf -- data to process
**  	buflen -- number of bytes at "buf" to process
**
**  Return value:
**  	A DKIM_STAT_* constant.
*/

extern DKIM_STAT dkim_chunk __P((DKIM *dkim, u_char *buf, size_t buflen));

/*
**  DKIM_EOM -- identify end of body
**
**  Parameters:
**  	dkim -- a DKIM handle previously returned by dkim_sign() or
**  	        dkim_verify()
**  	testkey -- TRUE iff the a matching key was found but is marked as a
**  	           test key (returned)
**
**  Return value:
**  	A DKIM_STAT value.
*/

extern DKIM_STAT dkim_eom __P((DKIM *dkim, _Bool *testkey));

/*
**  DKIM_KEY_SYNTAX -- process a key record parameter set for valid syntax
**
**  Parameters:
**  	dkim -- DKIM context in which this is performed
**  	str -- string to be scanned
**  	len -- number of bytes available at "str"
**
**  Return value:
**  	A DKIM_STAT constant.
*/

extern DKIM_STAT dkim_key_syntax __P((DKIM *dkim, u_char *str, size_t len));

/*
**  DKIM_POLICY_SYNTAX -- process a policy record parameter set
**                        for valid syntax
**
**  Parameters:
**  	dkim -- DKIM context in which this is performed
**  	str -- string to be scanned
**  	len -- number of bytes available at "str"
**
**  Return value:
**  	A DKIM_STAT constant.
*/

extern DKIM_STAT dkim_policy_syntax __P((DKIM *dkim, u_char *str, size_t len));

/*
**  DKIM_SIG_SYNTAX -- process a signature parameter set for valid syntax
**
**  Parameters:
**  	dkim -- DKIM context in which this is performed
**  	str -- string to be scanned
**  	len -- number of bytes available at "str"
**
**  Return value:
**  	A DKIM_STAT constant.
*/

extern DKIM_STAT dkim_sig_syntax __P((DKIM *dkim, u_char *str, size_t len));

/*
**  DKIM_GETCACHESTATS -- retrieve cache statistics
**
**  Parameters:
**  	queries -- number of queries handled (returned)
**  	hits -- number of cache hits (returned)
**  	expired -- number of expired hits (returned)
**
**  Return value:
**  	DKIM_STAT_OK -- statistics returned
**  	DKIM_STAT_NOTIMPLEMENT -- function not implemented
**
**  Notes:
**  	Any of the parameters may be NULL if the corresponding datum
**  	is not of interest.
*/

extern DKIM_STAT dkim_getcachestats __P((u_int *queries, u_int *hits,
                                         u_int *expired));

/*
**  DKIM_FLUSH_CACHE -- purge expired records from the database, reclaiming
**                      space for use by new data
**
**  Parameters:
**  	lib -- DKIM library whose cache should be flushed
**
**  Return value:
**  	-1 -- caching is not in effect
**  	>= 0 -- number of flushed records
*/

extern int dkim_flush_cache __P((DKIM_LIB *lib));

/*
**  DKIM_MINBODY -- return number of bytes still expected
**
**  Parameters:
**  	dkim -- DKIM handle
**
**  Return value:
**  	0 -- all canonicalizations satisfied
**  	ULONG_MAX -- at least one canonicalization wants the whole message
**  	other -- bytes required to satisfy all canonicalizations
*/

extern u_long dkim_minbody __P((DKIM *dkim));

/*
**  DKIM_GETSIGLIST -- retrieve the list of signatures
**
**  Parameters:
**  	dkim -- DKIM handle
**   	sigs -- pointer to a vector of DKIM_SIGINFO pointers (updated)
**   	nsigs -- pointer to an integer to receive the pointer count (updated)
**
**  Return value:
**  	A DKIM_STAT_* constant.
*/

extern DKIM_STAT dkim_getsiglist __P((DKIM *dkim, DKIM_SIGINFO ***sigs,
                                      int *nsigs));

/*
**  DKIM_GETSIGNATURE -- retrieve the "final" signature
**
**  Parameters:
**  	dkim -- DKIM handle
**
**  Return value:
**  	Pointer to a DKIM_SIGINFO handle which is the one libopendkim will
**  	use to return a "final" result; NULL if none could be determined.
*/

extern DKIM_SIGINFO *dkim_getsignature __P((DKIM *dkim));

/*
**  DKIM_GETSIGHDR -- compute and return a signature header for a message
**
**  Parameters:
**  	dkim -- a DKIM handle previously returned by dkim_sign()
**  	buf -- buffer into which to write the signature
**  	len -- number of bytes available at "buf"
**  	initial -- width of the first line
**
**  Return value:
**  	A DKIM_STAT value.
*/

extern DKIM_STAT dkim_getsighdr __P((DKIM *dkim, u_char *buf, size_t len,
                                     size_t initial));

/*
**  DKIM_GETSIGHDR_D -- compute and return a signature header for a message,
**                      but do it dynamically
**
**  Parameters:
**  	dkim -- a DKIM handle previously returned by dkim_sign()
**  	initial -- width of the first line
**  	buf -- location of generated header (returned)
**  	len -- number of bytes available at "buf" (returned)
**
**  Return value:
**  	A DKIM_STAT value.
*/

extern DKIM_STAT dkim_getsighdr_d __P((DKIM *dkim, size_t initial,
                                       u_char **buf, size_t *len));

/*
**  DKIM_SIG_HDRSIGNED -- retrieve the header list from a signature
**
**  Parameters:
**  	sig -- DKIM_SIGINFO handle
**  	hdr -- header name to find
**
**  Return value:
**  	TRUE iff "sig" had a header list in it and the header "hdr"
**  	appeared in that list.
*/

extern _Bool dkim_sig_hdrsigned __P((DKIM_SIGINFO *sig, char *hdr));

/*
**  DKIM_SIG_GETDNSSEC -- retrieve DNSSEC results for a signature
**
**  Parameters:
**  	sig -- DKIM_SIGINFO handle
**
**  Return value:
**  	A DKIM_DNSSEC_* constant.
*/

extern u_int dkim_sig_getdnssec __P((DKIM_SIGINFO *sig));

/*
**  DKIM_SIG_GETREPORTINFO -- retrieve reporting information from a key
**
**  Parameters:
**  	dkim -- DKIM handle
**  	sig -- DKIM_SIGINFO handle
**  	hfd -- canonicalized header descriptor (or NULL) (returned)
**  	bfd -- canonicalized body descriptor (or NULL) (returned)
**  	addr -- address buffer (or NULL)
**  	addrlen -- size of addr
**  	fmt -- format buffer (or NULL)
**  	fmtlen -- size of fmt
**  	opts -- options buffer (or NULL)
**  	optslen -- size of opts
**  	smtp -- SMTP prefix buffer (or NULL)
**  	smtplen -- size of smtp
**  	interval -- requested report interval (or NULL)
**
**  Return value:
**  	A DKIM_STAT_* constant.
*/

extern DKIM_STAT dkim_sig_getreportinfo __P((DKIM *dkim, DKIM_SIGINFO *sig,
                                             int *hfd, int *bfd,
                                             u_char *addr, size_t addrlen,
                                             u_char *fmt, size_t fmtlen,
                                             u_char *opts, size_t optslen,
                                             u_char *smtp, size_t smtplen,
                                             u_int *interval));

/*
**  DKIM_SIG_GETIDENTITY -- retrieve identity of the signer
**
**  Parameters:
**  	dkim -- DKIM handle
**  	sig -- DKIM_SIGINFO handle (or NULL to choose final one)
**  	val -- destination buffer
**  	vallen -- size of destination buffer
**
**  Return value:
**  	A DKIM_STAT_* constant.
*/

extern DKIM_STAT dkim_sig_getidentity __P((DKIM *dkim, DKIM_SIGINFO *sig,
                                           char *val, size_t vallen));

/*
**  DKIM_SIG_GETCANONLEN -- report number of (canonicalized) body bytes that
**                          were signed
**
**  Parameters:
**  	dkim -- a DKIM handle previously returned by dkim_sign() or
**  	        dkim_verify()
**  	sig -- a DKIM_SIGINFO handle
**  	msglen -- total size of the message body (returned)
**  	canonlen -- total number of canonicalized bytes (returned)
**  	signlen -- restricted signature length (returned)
**
**  Return value:
**  	A DKIM_STAT value.
**
**  Notes:
**  	msglen or canonlen can be NULL if that information is not of interest
**  	to the caller.
*/

extern DKIM_STAT dkim_sig_getcanonlen __P((DKIM *dkim, DKIM_SIGINFO *sig,
                                           off_t *msglen, off_t *canonlen,
                                           off_t *signlen));

/*
**  DKIM_OPTIONS -- set/get options
**
**  Parameters:
**  	dkimlib -- DKIM library handle
**  	op -- operation (DKIM_OPT_GET or DKIM_OPT_SET)
**  	opt -- which option (a DKIM_OPTS_* constant)
**  	ptr -- value (in or out)
**  	len -- bytes available at "ptr"
**
**  Return value:
**  	A DKIM_STAT value.
*/

extern DKIM_STAT dkim_options __P((DKIM_LIB *dkimlib, int op, dkim_opts_t opt,
                                   void *ptr, size_t len));

/*
**  DKIM_SIG_GETFLAGS -- retreive signature handle flags
**
**  Parameters:
**  	sig -- DKIM_SIGINFO handle
**
**  Return value:
**  	An unsigned integer which is a bitwise-OR of the DKIM_SIGFLAG_*
**  	constants currently set in the provided handle.
*/

extern unsigned int dkim_sig_getflags __P((DKIM_SIGINFO *sig));

/*
**  DKIM_SIG_GETBH -- retreive signature handle "bh" test state
**
**  Parameters:
**  	sig -- DKIM_SIGINFO handle
**
**  Return value:
**  	An unsigned integer which is one of the DKIM_SIGBH_* constants
**  	indicating the current state of "bh" evaluation of the signature.
*/

extern unsigned int dkim_sig_getbh __P((DKIM_SIGINFO *sig));

/*
**  DKIM_SIG_GETKEYSIZE -- retreive key size after verifying
**
**  Parameters:
**  	sig -- DKIM_SIGINFO handle
**  	bits -- size of the key in bits (returned)
**
**  Return value:
**  	A DKIM_STAT value.
*/

extern DKIM_STAT dkim_sig_getkeysize __P((DKIM_SIGINFO *sig,
                                          unsigned int *bits));

/*
**  DKIM_SIG_GETSIGNALG -- retreive signature algorithm after verifying
**
**  Parameters:
**  	sig -- DKIM_SIGINFO handle
**  	alg -- a DKIM_SIGN_* value (returned)
**
**  Return value:
**  	A DKIM_STAT value.
*/

extern DKIM_STAT dkim_sig_getsignalg __P((DKIM_SIGINFO *sig, dkim_alg_t *alg));

/*
**  DKIM_SIG_GETSIGNTIME -- retreive signature timestamp after verifying
**
**  Parameters:
**  	sig -- DKIM_SIGINFO handle
**  	when -- timestamp on the signature (returned)
**
**  Return value:
**  	A DKIM_STAT value.
*/

extern DKIM_STAT dkim_sig_getsigntime __P((DKIM_SIGINFO *sig, time_t *when));

/*
**  DKIM_SIG_GETSELECTOR -- retrieve selector used to generate the signature
**
**  Parameters:
**  	sig -- DKIM_SIGINFO handle from which to retrieve selector
**
**  Return value:
**  	Selector found in the signature.
*/

extern unsigned char *dkim_sig_getselector __P((DKIM_SIGINFO *sig));

/*
**  DKIM_SIG_GETDOMAIN -- retrieve signing domain after verifying
**
**  Parameters:
**  	sig -- DKIM_SIGINFO handle
**
**  Return value:
**  	Pointer to the signing domain.
*/

extern unsigned char *dkim_sig_getdomain __P((DKIM_SIGINFO *sig));

/*
**  DKIM_SIG_GETCANONS -- retrieve canonicaliztions after verifying
**
**  Parameters:
**  	sig -- DKIM_SIGINFO handle
**
**  Return value:
**  	DKIM_STAT_OK -- success
*/

extern DKIM_STAT dkim_sig_getcanons __P((DKIM_SIGINFO *sig, dkim_canon_t *hdr,
                                         dkim_canon_t *body));

/*
**  DKIM_SET_USER_CONTEXT -- set DKIM handle user context
**
**  Parameters:
**  	dkim -- DKIM signing handle
**  	ctx -- user context pointer to store
**
**  Parameters:
**  	A DKIM_STAT_* constant.
*/

extern DKIM_STAT dkim_set_user_context __P((DKIM *dkim, const void *ctx));

/*
**  DKIM_GET_USER_CONTEXT -- retrieve DKIM handle user context
**
**  Parameters:
**  	dkim -- DKIM signing handle
**
**  Parameters:
**  	User context pointer.
*/

extern const void *dkim_get_user_context __P((DKIM *dkim));

/*
**  DKIM_GET_MSGDATE -- retrieve value extracted from the Date: header
**
**  Parameters:
**  	dkim -- DKIM handle
**
**  Return value:
**  	time_t representing the value in the Date: header of the message,
**  	or 0 if no such header was found or the value in it was unusable
*/

extern time_t dkim_get_msgdate __P((DKIM *dkim));

/*
**  DKIM_GETMODE -- return the mode (signing, verifying, etc.) of a handle
**
**  Parameters:
**  	dkim -- DKIM handle
**
**  Return value:
**  	A DKIM_MODE_* constant.
*/

extern int dkim_getmode __P((DKIM *dkim));

/*
**  DKIM_GETDOMAIN -- retrieve policy domain from a DKIM context
**
**  Parameters:
**  	dkim -- DKIM handle
**
**  Return value:           
**  	Pointer to the domain used for policy checking (if any) or NULL if
**  	no domain could be determined.
*/

extern u_char *dkim_getdomain __P((DKIM *dkim));

/*
**  DKIM_SET_SIGNER -- set DKIM signature's signer
**
**  Parameters:
**  	dkim -- DKIM signing handle
**  	signer -- signer to store
**
**  Parameters:
**  	A DKIM_STAT_* constant.
*/

extern DKIM_STAT dkim_set_signer __P((DKIM *dkim, const char *signer));

/*
**  DKIM_SET_DNS_CALLBACK -- set the DNS wait callback
**
**  Parameters:
**  	libopendkim -- DKIM library handle
**  	func -- function to call; should take an opaque context pointer
**  	interval -- how often to call back
**
**  Return value:
**  	DKIM_STAT_OK -- success
**  	DKIM_STAT_INVALID -- invalid use
**  	DKIM_STAT_NOTIMPLEMENT -- underlying resolver doesn't support callbacks
*/

extern DKIM_STAT dkim_set_dns_callback __P((DKIM_LIB *libopendkim,
                                            void (*func)(const void *context),
                                            unsigned int interval));

/*
**  DKIM_SET_KEY_LOOKUP -- set the key lookup function
**
**  Parameters:
**  	libopendkim -- DKIM library handle
**  	func -- function to call
**
**  Return value:
**  	DKIM_STAT_OK
*/

extern DKIM_STAT dkim_set_key_lookup __P((DKIM_LIB *libopendkim,
                                          DKIM_CBSTAT (*func)(DKIM *dkim,
                                                              DKIM_SIGINFO *sig,
                                                              u_char *buf,
                                                              size_t buflen)));

/*
**  DKIM_SET_POLICY_LOOKUP -- set the policy lookup function
**
**  Parameters:
**  	libopendkim -- DKIM library handle
**  	func -- function to call
**
**  Return value:
**  	DKIM_STAT_OK
*/

extern DKIM_STAT dkim_set_policy_lookup __P((DKIM_LIB *libopendkim,
                                             DKIM_CBSTAT (*func)(DKIM *dkim,
                                                                 u_char *query,
                                                                 _Bool excheck,
                                                                 u_char *buf,
                                                                 size_t buflen,
                                                                 int *qstat)));

/*
**  DKIM_SET_SIGNATURE_HANDLE -- set the signature handle creator function
**
**  Parameters:
**  	libopendkim -- DKIM library handle
**  	func -- function to call
**
**  Return value:
**  	Pointer to the user-side handle thus created, or NULL.
*/

extern DKIM_STAT dkim_set_signature_handle __P((DKIM_LIB *libopendkim,
                                                void * (*func)(void *closure)));

/*
**  DKIM_SET_SIGNATURE_HANDLE_FREE -- set the signature handle destroyer
**                                    function
**
**  Parameters:
**  	libopendkim -- DKIM library handle
**  	func -- function to call
**
**  Return value:
**  	None.
*/

extern DKIM_STAT dkim_set_signature_handle_free __P((DKIM_LIB *libopendkim,
                                                     void (*func)(void *closure,
                                                                  void *user)));

/*
**  DKIM_SET_SIGNATURE_TAGVALUES -- set the signature handle populator function
**
**  Parameters:
**  	libopendkim -- DKIM library handle
**  	func -- function to call
**
**  Return value:
**  	DKIM_STAT_OK
*/

extern DKIM_STAT dkim_set_signature_tagvalues __P((DKIM_LIB *libopendkim,
                                                   void (*func)(void *user,
                                                                dkim_param_t pcode,
                                                                const u_char *param,
                                                                const u_char *value)));

/*
**  DKIM_SET_PRESCREEN -- set the prescreen function
**
**  Parameters:
**  	libopendkim -- DKIM library handle
**  	func -- function to call
**
**  Return value:
**  	DKIM_STAT_OK
*/

extern DKIM_STAT dkim_set_prescreen __P((DKIM_LIB *libopendkim,
                                         DKIM_CBSTAT (*func)(DKIM *dkim,
                                                             DKIM_SIGINFO **sigs,
                                                             int nsigs)));

/*
**  DKIM_SET_FINAL -- set the final processing function
**
**  Parameters:
**  	libopendkim -- DKIM library handle
**  	func -- function to call
**
**  Return value:
**  	DKIM_STAT_OK
*/

extern DKIM_STAT dkim_set_final __P((DKIM_LIB *libopendkim,
                                     DKIM_CBSTAT (*func)(DKIM *dkim,
                                                         DKIM_SIGINFO **sigs,
                                                         int nsigs)));

/*
**  DKIM_SIG_GETCONTEXT -- get user-specific context from a DKIM_SIGINFO
**
**  Parameters:
**  	siginfo -- a pointer to a DKIM_SIGINFO
**
**  Return value:
**  	The user-provided pointer stored in the named "siginfo", or NULL
**  	if none was ever set.
*/

extern void *dkim_sig_getcontext __P((DKIM_SIGINFO *siginfo));

/*
**  DKIM_SIG_GETERROR -- get error code from a DKIM_SIGINFO
**
**  Parameters:
**  	siginfo -- a pointer to a DKIM_SIGINFO
**
**  Return value:
**  	A DKIM_SIGERROR_* constant.
*/

extern int dkim_sig_geterror __P((DKIM_SIGINFO *siginfo));

/*
**  DKIM_SIG_GETERRORSTR -- translate a DKIM_SIGERROR into a string
**
**  Parameters:
**  	sigerr -- a DKIM_SIGERROR constant
**
**  Return value:
**  	A pointer to a human-readable string translation of "sigerr", or NULL
**  	if no such translation exists.
*/

extern const char *dkim_sig_geterrorstr __P((DKIM_SIGERROR sigerr));

/*
**  DKIM_SIG_IGNORE -- mark a signature referenced by a DKIM_SIGINFO with
**                     an "ignore" flag
**
**  Parameters:
**  	siginfo -- pointer to a DKIM_SIGINFO to update
**
**  Return value:
**  	None.
*/

extern void dkim_sig_ignore __P((DKIM_SIGINFO *siginfo));

/*
**  DKIM_POLICY -- parse policy associated with the sender's domain
**
**  Parameters:
**  	dkim -- DKIM handle
**  	pcode -- discovered policy (returned)
**  	pstate -- state, for re-entrancy (updated; can be NULL)
**
**  Return value:
**  	A DKIM_STAT_* constant.
*/

extern DKIM_STAT dkim_policy __P((DKIM *dkim, dkim_policy_t *pcode,
                                  DKIM_PSTATE *pstate));

/*
**  DKIM_POLICY_GETDNSSEC -- retrieve DNSSEC results for a policy
**
**  Parameters:
**  	dkim -- DKIM handle
**
**  Return value:
**  	A DKIM_DNSSEC_* constant.
*/

extern u_int dkim_policy_getdnssec __P((DKIM *dkim));

/*
**  DKIM_POLICY_GETREPORTINFO -- retrieve reporting information from policy
**
**  Parameters:
**  	dkim -- DKIM handle
**  	addr -- address buffer (or NULL)
**  	addrlen -- size of addr
**  	fmt -- format buffer (or NULL)
**  	fmtlen -- size of fmt
**  	opts -- options buffer (or NULL)
**  	optslen -- size of opts
**  	smtp -- SMTP prefix buffer (or NULL)
**  	smtplen -- size of smtp
**  	interval -- requested report interval (or NULL)
**
**  Return value:
**  	A DKIM_STAT_* constant.
*/

extern DKIM_STAT dkim_policy_getreportinfo __P((DKIM *dkim,
                                                u_char *addr, size_t addrlen,
                                                u_char *fmt, size_t fmtlen,
                                                u_char *opts, size_t optslen,
                                                u_char *smtp, size_t smtplen,
                                                u_int *interval));

/*
**  DKIM_SIG_PROCESS -- process a signature
**
**  Parameters:
**  	dkim -- DKIM handle
**  	sig -- DKIM_SIGINFO handle
**
**  Return value:
**  	A DKIM_STAT_* constant.
*/

extern DKIM_STAT dkim_sig_process __P((DKIM *dkim, DKIM_SIGINFO *sig));

/*
**  DKIM_FREE -- release resources associated with a DKIM handle
**
**  Parameters:
**  	dkim -- a DKIM handle previously returned by dkim_sign() or
**  	        dkim_verify()
**
**  Return value:
**  	A DKIM_STAT value.
*/

extern DKIM_STAT dkim_free __P((DKIM *dkim));

/*
**  DKIM_GETERROR -- return any stored error string from within the DKIM
**                   context handle
**
**  Parameters:
**  	dkim -- DKIM handle from which to retrieve an error string
**
**  Return value:
**  	A pointer to the stored string, or NULL if none was stored.
*/

extern const char *dkim_geterror __P((DKIM *dkim));

/*
**  DKIM_GETRESULTSTR -- translate a DKIM_STAT_* constant to a string
**
**  Parameters:
**      result -- DKIM_STAT_* constant to translate
**
**  Return value:
**      Pointer to a text describing "result", or NULL if none exists
*/

extern const char *dkim_getresultstr __P((DKIM_STAT result));

/*
**  DKIM_GETPRESULT -- retrieve policy result
**
**  Parameters:
**  	dkim -- DKIM handle from which to get policy result
**
**  Return value:
**  	DKIM policy check result.
*/

extern int dkim_getpresult __P((DKIM *dkim));

/*
**  DKIM_GETPRESULTSTR -- retrieve policy result string
**
**  Parameters:
**  	presult -- policy result code to translate
**
**  Return value:
**  	Pointer to text that describes "presult".
*/

extern const char *dkim_getpresultstr __P((int presult));

/*
**  DKIM_GETPOLICYSTR -- retrieve sender policy string
**
**  Parameters:
**  	policy -- policy code to translate
**
**  Return value:
**  	Pointer to text that describes "policy".
*/

extern const char *dkim_getpolicystr __P((int policy));

/*
**  DKIM_OHDRS -- extract and decode original headers
**
**  Parameters:
**  	dkim -- DKIM handle
**  	sig -- DKIM_SIGINFO handle
**  	ptrs -- user-provided array of pointers to header strings (updated)
**  	pcnt -- number of pointers available (updated)
**
**  Return value:
**  	A DKIM_STAT_* constant.
*/

extern DKIM_STAT dkim_ohdrs __P((DKIM *dkim, DKIM_SIGINFO *sig, char **ptrs,
                                 int *pcnt));

/*
**  DKIM_DIFFHEADERS -- compare original headers with received headers
**
**  Parameters:
**  	dkim -- DKIM handle
**  	maxcost -- maximum "cost" of changes to be reported
**  	ohdrs -- original headers, presumably extracted from a "z" tag
**  	nohdrs -- number of headers at "ohdrs" available
**  	out -- pointer to an array of struct dkim_hdrdiff objects (updated)
** 	nout -- counter of handles returned (updated)
**
**  Return value:
**  	A DKIM_STAT_* constant.
**
**  Side effects:
**  	A series of DKIM_HDRDIFF handles is allocated and must later be
**  	destroyed.
*/

extern DKIM_STAT dkim_diffheaders __P((DKIM *dkim, int maxcost, char **ohdrs,
                                       int nohdrs, struct dkim_hdrdiff **out,
                                       int *nout));

/*
**  DKIM_GETPARTIAL -- return a DKIM handle's "body length tag" flag
**
**  Parameters:
**  	dkim -- DKIM handle
**
**  Return value:
**  	True iff the signature is to include a body length tag
*/

extern _Bool dkim_getpartial __P((DKIM *dkim));

/*
**  DKIM_SETPARTIAL -- set the DKIM handle to sign using the DKIM body length
**                     tag (l=)
**
**  Parameters:
**  	dkim -- DKIM handle
**  	value -- new flag value
**
**  Return value:
**  	DKIM_STAT_OK
*/

extern DKIM_STAT dkim_setpartial __P((DKIM *dkim, _Bool value));

/*
**  DKIM_SET_MARGIN -- set the margin to use when generating signatures
**
**  Parameters:
**      dkim -- DKIM handle
**      value -- new margin value
**
**  Return value:
**      DKIM_STAT_INVALID -- "dkim" referenced a verification handle, or
**  	                     "value" was negative
**      DKIM_STAT_OK -- otherwise
*/

extern DKIM_STAT dkim_set_margin __P((DKIM *dkim, int value));

/*
**  DKIM_GET_REPUTATION -- query reputation service about a signature
**
**  Parameters:
**  	dkim -- DKIM handle
**  	sig -- DKIM_SIGINFO handle
**  	qroot -- query root
**  	rep -- integer reputation (returned)
**
**  Return value:
**  	DKIM_STAT_OK -- "rep" now contains a reputation
**  	DKIM_STAT_NOKEY -- no reputation data available
**  	DKIM_STAT_CANTVRFY -- data retrieval error of some kind
**  	DKIM_STAT_INTERNAL -- internal error of some kind
**  	DKIM_STAT_NOTIMPLEMENT -- not implemented
*/

extern DKIM_STAT dkim_get_reputation __P((DKIM *dkim, DKIM_SIGINFO *sig,
                                          char *qroot, int *rep));

/*
**  RFC2822_MAILBOX_SPLIT -- extract the userid and host from a structured
**                           header
**
**  Parameters:
**  	addr -- the header to parse; see RFC2822 for format
**  	user -- local-part of the parsed header (returned)
**  	domain -- domain part of the parsed header (returned)
**
**  Return value:
**  	0 on success; other on error (see source)
*/

extern int rfc2822_mailbox_split __P((char *addr, char **user, char **domain));

/*
**  DKIM_SSL_VERSION -- return the version of the OpenSSL library against
**                      which this library was compiled
**
**  Parameters:
**  	None.
**
**  Return value:
**  	The OPENSSL_VERSION_NUMBER constant as defined by OpenSSL.
*/

extern unsigned long dkim_ssl_version __P((void));

/*
**  DKIM_LIBFEATURE -- check for a library feature
**
**  Parameters:
**  	lib -- DKIM_LIB handle
**  	fc -- feature code
**
**  Return value:
**  	TRUE iff the library was compiled with the requested feature
*/

#define DKIM_FEATURE_DIFFHEADERS	0
#define DKIM_FEATURE_DKIM_REPUTATION	1
#define DKIM_FEATURE_PARSE_TIME		2
#define DKIM_FEATURE_QUERY_CACHE	3
#define DKIM_FEATURE_SHA256		4
#define DKIM_FEATURE_ASYNC_DNS		5
#define DKIM_FEATURE_DNSSEC		6

#define	DKIM_FEATURE_MAX		6

extern _Bool dkim_libfeature __P((DKIM_LIB *lib, u_int fc));

/* default list of sender headers */
extern const u_char *dkim_default_senderhdrs[];

/* list of headers that should be signed, per RFC4871 section 5.5 */
extern const u_char *dkim_should_signhdrs[]; 

/* list of headers that should not be signed, per RFC4871 section 5.5 */
extern const u_char *dkim_should_not_signhdrs[];

#endif /* ! _DKIM_H_ */
