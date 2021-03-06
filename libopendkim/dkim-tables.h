/*
**  Copyright (c) 2005-2008 Sendmail, Inc. and its suppliers.
**    All rights reserved.
**
**  Copyright (c) 2009, The OpenDKIM Project.  All rights reserved.
*/

#ifndef _DKIM_TABLES_H_
#define _DKIM_TABLES_H_

#ifndef lint
static char dkim_tables_h_id[] = "@(#)$Id: dkim-tables.h,v 1.2 2009/08/03 18:44:40 cm-msk Exp $";
#endif /* !lint */

#ifdef __STDC__
# ifndef __P
#  define __P(x)  x
# endif /* ! __P */
#else /* __STDC__ */
# ifndef __P
#  define __P(x)  ()
# endif /* ! __P */
#endif /* __STDC__ */

/* structures */
struct nametable
{
	const char *	tbl_name;	/* name */
	const int	tbl_code;	/* code */
};

/* tables */
struct nametable *algorithms;
struct nametable *canonicalizations;
struct nametable *hashes;
struct nametable *keyflags;
struct nametable *keyparams;
struct nametable *keytypes;
struct nametable *policies;
struct nametable *policyflags;
struct nametable *policyparams;
struct nametable *policyresults;
struct nametable *querytypes;
struct nametable *results;
struct nametable *settypes;
struct nametable *sigerrors;
struct nametable *sigparams;

/* prototypes */
extern const char *dkim_code_to_name __P((struct nametable *tbl,
                                          const int code));
extern const int dkim_name_to_code __P((struct nametable *tbl,
                                        const char *name));

#endif /* _DKIM_TABLES_H_ */
