/*
**  Copyright (c) 2009, The OpenDKIM Project.  All rights reserved.
*/

#ifndef _DKIM_INTERNAL_H_
#define _DKIM_INTERNAL_H_

#ifndef lint
static char dkim_internal_h_id[] = "@(#)$Id: dkim-internal.h,v 1.2 2009/10/22 19:33:48 cm-msk Exp $";
#endif /* !lint */

/* libopendkim includes */
#include "dkim.h"

/* the basics */
#ifndef NULL
# define NULL	0
#endif /* ! NULL */
#ifndef FALSE
# define FALSE	0
#endif /* ! FALSE */
#ifndef TRUE
# define TRUE	1
#endif /* ! TRUE */
#ifndef MAXPATHLEN
# define MAXPATHLEN		256
#endif /* ! MAXPATHLEN */

#ifndef ULONG_MAX
# define ULONG_MAX		0xffffffffL
#endif /* ! ULONG_MAX */
#ifndef ULLONG_MAX
# define ULLONG_MAX		0xffffffffffffffffLL
#endif /* ! ULLONG_MAX */

#ifndef MIN
# define MIN(x,y)		((x) < (y) ? (x) : (y))
#endif /* ! MIN */
#ifndef MAX
# define MAX(x,y)		((x) > (y) ? (x) : (y))
#endif /* ! MAX */

#ifdef __STDC__
# ifndef __P
#  define __P(x)  x
# endif /* ! __P */
#else /* __STDC__ */
# ifndef __P
#  define __P(x)  ()
# endif /* ! __P */
#endif /* __STDC__ */

/* limits, macros, etc. */
#define	BUFRSZ			1024	/* base temp buffer size */
#define	BASE64SIZE(x)		(((x + 2) / 3) * 4)
					/* base64 encoding growth ratio */
#define MAXADDRESS		256	/* biggest user@host we accept */
#define	MAXBUFRSZ		65536	/* max temp buffer size */
#define MAXCNAMEDEPTH		3	/* max. CNAME recursion we allow */
#define MAXHEADERS		32768	/* buffer for caching headers */
#define MAXLABELS		16	/* max. labels we allow */
#define MAXPOLICYDEPTH		1	/* policy recursion we allow */
#define MAXTAGNAME		8	/* biggest tag name */

#define DKIM_MAXHEADER		4096	/* buffer for caching one header */
#define	DKIM_MAXHOSTNAMELEN	256	/* max. FQDN we support */

/* defaults */
#define	DEFERRLEN		64	/* default error string length */
#define	DEFTMPDIR		"/var/tmp"
					/* default temporary directory */

/* version */
#define	DKIM_VERSION_KEY	"DKIM1"	/* current version token for keys */
#define	DKIM_VERSION_SIG	"1"	/* current version token for sigs */
#define	DKIM_VERSION_SIGOLD	"0.5"	/* old version token for sigs */

/* headers */
#define	DKIM_DATEHEADER		"Date"	/* Date: header */
#define	DKIM_FROMHEADER		"From"	/* From: header */

#define	DKIM_DATEHEADER_LEN	(sizeof(DKIM_DATEHEADER) - 1)
#define	DKIM_FROMHEADER_LEN	(sizeof(DKIM_FROMHEADER) - 1)
#define	DKIM_SIGNHEADER_LEN	(sizeof(DKIM_SIGNHEADER) - 1)
/*
**  DKIM_KEY -- known key parameters
*/

typedef int dkim_key_t;

#define	DKIM_KEY_VERSION	0	/* v */
#define	DKIM_KEY_GRANULARITY	1	/* g */
#define	DKIM_KEY_ALGORITHM	2	/* a */
#define	DKIM_KEY_NOTES		3	/* n */
#define	DKIM_KEY_DATA		4	/* p */
#define	DKIM_KEY_SERVICE	5	/* s */
#define	DKIM_KEY_FLAGS		6	/* t */

/*
**  DKIM_PPARAM -- policy parameters
*/

typedef int dkim_pparam_t;

#define DKIM_PPARAM_POLICY	0	/* dkim */
#define DKIM_PPARAM_FLAGS	1	/* t */
#define DKIM_PPARAM_REPORTADDR	2	/* r */

/*
**  DKIM_PFLAG -- policy flags
*/

#define	DKIM_PFLAG_TEST		0x01	/* t */
#define	DKIM_PFLAG_NOSUBDOMAIN	0x02	/* s */

#define DKIM_PFLAG_DEFAULT	0x00

/*
**  DKIM_SETTYPE -- types of sets
*/

typedef int dkim_set_t;

#define	DKIM_SETTYPE_ANY	(-1)
#define	DKIM_SETTYPE_SIGNATURE	0
#define	DKIM_SETTYPE_POLICY	1
#define	DKIM_SETTYPE_KEY	2
#ifdef _FFR_VBR
# define DKIM_SETTYPE_VBRINFO	3
#endif /* _FFR_VBR */

/*
**  DKIM_HASHTYPE -- types of hashes
*/

#define DKIM_HASHTYPE_UNKNOWN	(-1)
#define DKIM_HASHTYPE_SHA1	0
#ifdef SHA256_DIGEST_LENGTH
# define DKIM_HASHTYPE_SHA256	1
#endif /* SHA256_DIGEST_LENGTH */

/*
**  DKIM_KEYTYPE -- types of keys
*/

#define	DKIM_KEYTYPE_UNKNOWN	(-1)
#define	DKIM_KEYTYPE_RSA	0

/*
**  DKIM_SET -- a set of parameters and values
*/

struct dkim_set;
typedef struct dkim_set DKIM_SET;

/*
**  DKIM_PLIST -- a parameter/value pair, as a linked list
*/

struct dkim_plist;
typedef struct dkim_plist DKIM_PLIST;

/*
**  DKIM_KEY -- contents of a published key record
*/

struct dkim_key;
typedef struct dkim_key DKIM_KEY;

/*
**  DKIM_POLICY -- contents of a published policy record
*/

struct dkim_policy;
typedef struct dkim_policy DKIM_POLICY;

/*
**  DKIM_CANON -- canonicalization
*/

struct dkim_canon;
typedef struct dkim_canon DKIM_CANON;

/* prototypes */
extern DKIM_STAT dkim_process_set __P((DKIM *, dkim_set_t, u_char *, size_t,
                                       void *, _Bool));
extern DKIM_STAT dkim_siglist_setup __P((DKIM *));

#endif /* ! _DKIM_INTERNAL_H_ */
