/*
**  Copyright (c) 2007 Sendmail, Inc. and its suppliers.
**    All rights reserved.
**
**  Copyright (c) 2009, The OpenDKIM Project.  All rights reserved.
*/

#ifndef _VBR_H_
#define _VBR_H_

#ifndef lint
static char vbr_h_id[] = "@(#)$Id: vbr.h,v 1.1 2009/07/16 19:12:04 cm-msk Exp $";
#endif /* !lint */

/* strings */
#define	VBR_ALL			"all"
#define	VBR_INFOHEADER		"VBR-Info"
#define	VBR_PREFIX		"_vouch"

/* definitions */
#define	VBR_MAXHEADER		1024
#define	VBR_MAXHOSTNAMELEN	256

/* return codes */
typedef int VBR_STAT;

#define VBR_STAT_OK		0
#define VBR_STAT_INVALID	1
#define VBR_STAT_DNSERROR	2
#define VBR_STAT_NORESOURCE	3

/* types */
struct vbr_handle;
typedef struct vbr_handle VBR;

/* prototypes */

/*
**  VBR_INIT -- initialize a VBR handle
**
**  Parameters:
**  	caller_mallocf -- caller-provided memory allocation function
**  	caller_freef -- caller-provided memory release function
**  	closure -- memory closure to pass to the above when used
**
**  Return value:
**  	A new VBR handle suitable for use with other VBR functions, or
**  	NULL on failure.
**  
**  Side effects:
**  	Strange radar returns at Indianapolis ARTCC.
*/

extern VBR * vbr_init __P((void *(*caller_mallocf)(void *closure,
                                                   size_t nbytes),
                           void (*caller_freef)(void *closure, void *p),
                           void *closure));

/*
**  VBR_CLOSE -- shut down a VBR instance
**
**  Parameters:
**  	vbr -- VBR handle to shut down
**
**  Return value:
**  	None.
*/

extern void vbr_close __P((VBR *));

/*
**  VBR_GETERROR -- return any stored error string from within the VBR
**                  context handle
**
**  Parameters:
**  	vbr -- VBR handle from which to retrieve an error string
**
**  Return value:
**  	A pointer to the stored string, or NULL if none was stored.
*/

extern const char *vbr_geterror __P((VBR *));

/*
**  VBR_GETHEADER -- generate and store the VBR-Info header
**
**  Parameters:
**  	vbr -- VBR handle, created by vbr_init()
**  	hdr -- header buffer
**  	len -- number of bytes available at "hdr"
**
**  Return value:
**  	STAT_OK -- success
**  	STAT_NORESOURCE -- "hdr" was too short
*/

extern VBR_STAT vbr_getheader __P((VBR *, char *, size_t));

/*
**  VBR_SETCERT -- store the VBR certifiers of this message
**
**  Parameters:
**  	vbr -- VBR handle, created by vbr_init()
**  	cert -- certifiers string
**
**  Return value:
**  	None (yet).
*/

extern void vbr_setcert __P((VBR *, char *));

/*
**  VBR_SETTYPE -- store the VBR type of this message
**
**  Parameters:
**  	vbr -- VBR handle, created by vbr_init()
**  	type -- type string
**
**  Return value:
**  	None (yet).
*/

extern void vbr_settype __P((VBR *, char *));

/*
**  VBR_SETDOMAIN -- declare the sender's domain
**
**  Parameters:
**  	vbr -- VBR handle, created by vbr_init()
**  	cert -- certifiers string
**
**  Return value:
**  	None (yet).
*/

extern void vbr_setdomain __P((VBR *, char *));

/*
**  VBR_TRUSTEDCERTS -- set list of trusted certifiers
**
**  Parameters:
**  	vbr -- VBR handle, created by vbr_init()
**  	certs -- NULL terminted vector of trusted certifier names
**
**  Return value:
**  	None (yet).
*/

extern void vbr_trustedcerts __P((VBR *, char **));

/*
**  VBR_QUERY -- query the vouching servers for results
**
**  Parameters:
**  	vbr -- VBR handle, created by vbr_init()
**  	res -- result string (one of "fail", "pass"); returned
**  	cert -- name of the certifier that returned a "pass"; returned
**
**  Return value:
**  	VBR_STAT_OK -- able to determine a result
**  	VBR_STAT_INVALID -- vbr_trustedcerts(), vbr_settype() and
**  	                     vbr_setcert() were not all called
**  	VBR_STAT_CANTVRFY -- DNS issue prevented resolution
**
**  Notes:
**  	- "pass" is the result if ANY certifier vouched for the message.
**  	- "res" is not modified if no result could be determined
**  	- "cert" and "domain" are not modified if a "pass" is not returned
**  	- there's no attempt to validate the values found
*/

extern VBR_STAT vbr_query __P((VBR *, char **, char **));

#ifdef USE_ARLIB
/*
**  VBR_SETTIMEOUT -- set the DNS timeout
**
**  Parameters:
**  	vbr -- VBR handle, created by vbr_init()
**  	timeout -- requested timeout (seconds)
**
**  Return value:
**  	None (yet).
*/

extern void vbr_settimeout __P((VBR *, u_int));

/*
**  VBR_SETCALLBACKINT -- set the DNS callback interval
**
**  Parameters:
**  	vbr -- VBR handle, created by vbr_init()
**  	cbint -- requested callback interval (seconds)
**
**  Return value:
**  	None (yet).
*/

extern void vbr_setcallbackint __P((VBR *, u_int));

/*
**  VBR_SETCALLBACKCTX -- set the DNS callback context
**
**  Parameters:
**  	vbr -- VBR handle, created by vbr_init()
**  	ctx -- context to pass to the DNS callback
**
**  Return value:
**  	None (yet).
*/

extern void vbr_setcallbackctx __P((VBR *, void *));

/*
**  VBR_SETDNSCALLBACK -- set the DNS wait callback
**
**  Parameters:
**  	vbr -- VBR handle, created by vbr_init()
**  	func -- function to call; should take an opaque context pointer
**
**  Return value:
**  	None.
*/

extern VBR_STAT vbr_setdnscallback __P((VBR *vbr,
                                        void (*func)(const void *context)));

#endif /* USE_ARLIB */

#endif /* _VBR_H_ */
