/*
**  Copyright (c) 2007, 2008 Sendmail, Inc. and its suppliers.
**    All rights reserved.
**
**  Copyright (c) 2009, The OpenDKIM Project.  All rights reserved.
*/

#ifndef _DKIM_TEST_H_
#define _DKIM_TEST_H_

#ifndef lint
static char dkim_test_h_id[] = "@(#)$Id: dkim-test.h,v 1.2 2009/07/23 17:40:24 cm-msk Exp $";
#endif /* !lint */

/* libopendkim includes */
#include "dkim.h"

/* prototypes */
extern int dkim_test_adsp __P((DKIM_LIB *, const char *, dkim_policy_t *,
                               int *, char *, size_t));
extern int dkim_test_key __P((DKIM_LIB *, char *, char *, char *, size_t,
                              char *, size_t));

extern size_t dkim_test_dns_get __P((DKIM *, u_char *, size_t));
extern int dkim_test_dns_put __P((DKIM *, int, int, int, u_char *, u_char *));

#endif /* ! _DKIM_TEST_H_ */
