/*
**  Copyright (c) 2007-2009 Sendmail, Inc. and its suppliers.
**	All rights reserved.
**
**  Copyright (c) 2009, The OpenDKIM Project.  All rights reserved.
**
**  $Id: opendkim-ar.h,v 1.1 2009/07/16 20:59:11 cm-msk Exp $
*/

#ifndef _OPENDKIM_AR_H_
#define _OPENDKIM_AR_H_

#ifndef lint
static char opendkim_ar_h_id[] = "@(#)$Id: opendkim-ar.h,v 1.1 2009/07/16 20:59:11 cm-msk Exp $";
#endif /* !lint */

/* system includes */
#include <sys/types.h>

/* opendkim includes */
#include "opendkim.h"

/* limits */
#define	MAXARESULTS	16
#define	MAXPROPS	16
#define	MAXAVALUE	256

/* ARES_METHOD_T -- type for specifying an authentication method */
typedef int ares_method_t;

#define	ARES_METHOD_UNKNOWN	(-1)
#define	ARES_METHOD_AUTH	0
#define	ARES_METHOD_DKIM	1
#define	ARES_METHOD_DOMAINKEYS	2
#define	ARES_METHOD_SENDERID	3
#define	ARES_METHOD_SPF		4
#define	ARES_METHOD_DKIMADSP	5
#define	ARES_METHOD_IPREV	6

/* ARES_RESULT_T -- type for specifying an authentication result */
typedef int ares_result_t;

#define	ARES_RESULT_UNDEFINED	(-1)
#define	ARES_RESULT_PASS	0
#define	ARES_RESULT_HARDFAIL	1
#define	ARES_RESULT_SOFTFAIL	2
#define	ARES_RESULT_NEUTRAL	3
#define	ARES_RESULT_TEMPERROR	4
#define	ARES_RESULT_PERMERROR	5
#define	ARES_RESULT_NONE	6
#define ARES_RESULT_FAIL	7
#define ARES_RESULT_POLICY	8
#define ARES_RESULT_NXDOMAIN	9
#define ARES_RESULT_SIGNED	10
#define ARES_RESULT_UNKNOWN	11
#define ARES_RESULT_DISCARD	12

/* ARES_PTYPE_T -- type for specifying an authentication property */
typedef int ares_ptype_t;

#define	ARES_PTYPE_UNKNOWN	(-1)
#define	ARES_PTYPE_SMTP		0
#define	ARES_PTYPE_HEADER	1
#define	ARES_PTYPE_BODY		2
#define	ARES_PTYPE_POLICY	3

/* RESULT structure -- a single result */
struct result
{
	int		result_props;
	ares_method_t	result_method;
	ares_result_t	result_result;
	ares_ptype_t	result_ptype[MAXPROPS];
	unsigned char	result_reason[MAXAVALUE + 1];
	unsigned char	result_property[MAXPROPS][MAXAVALUE + 1];
	unsigned char	result_value[MAXPROPS][MAXAVALUE + 1];
};

/* AUTHRES structure -- the entire header parsed */
struct authres
{
	int		ares_count;
	unsigned char	ares_host[DKIM_MAXHOSTNAMELEN + 1];
	unsigned char	ares_version[MAXAVALUE + 1];
	struct result	ares_result[MAXARESULTS];
};

/*
**  ARES_PARSE -- parse an Authentication-Results: header, return a
**                structure containing a parsed result
**
**  Parameters:
**  	hdr -- NULL-terminated contents of an Authentication-Results:
**  	       header field
**  	ar -- a pointer to a (struct authres) loaded by values after parsing
**  
**  Return value:
**  	0 on success, -1 on failure.
*/

extern int ares_parse __P((u_char *hdr, struct authres *ar));

#endif /* _OPENDKIM_AR_H_ */
