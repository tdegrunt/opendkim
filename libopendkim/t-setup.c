/*
**  Copyright (c) 2007, 2008 Sendmail, Inc. and its suppliers.
**    All rights reserved.
**
**  Copyright (c) 2009, The OpenDKIM Project.  All rights reserved.
*/

#ifndef lint
static char t_setup_c_id[] = "@(#)$Id: t-setup.c,v 1.3 2009/07/23 17:40:24 cm-msk Exp $";
#endif /* !lint */

/* system includes */
#include <sys/types.h>
#include <assert.h>
#include <stdio.h>


/* libopendkim includes */
#include "dkim.h"
#include "t-testdata.h"

/*
**  MAIN -- program mainline
**
**  Parameters:
**  	The usual.
**
**  Return value:
**  	Exit status.
*/

int
main(int argc, char **argv)
{
	char *p;
	FILE *f;

	printf("*** test setup\n");

	f = fopen(KEYFILE, "w");
	assert(f != NULL);

	fprintf(f, "%s.%s.%s ", SELECTOR, DKIM_DNSKEYNAME, DOMAIN);
	for (p = PUBLICKEY; *p != '\0'; p++)
	{
		if (*p != '\n')
			putc(*p, f);
	}
	fprintf(f, "\n");

	fprintf(f, "%s.%s.%s ", SELECTOR2, DKIM_DNSKEYNAME, DOMAIN);
	for (p = PUBLICKEY2; *p != '\0'; p++)
	{
		if (*p != '\n')
			putc(*p, f);
	}
	fprintf(f, "\n");

	fprintf(f, "%s.%s.%s ", SELECTOR, DKIM_DNSKEYNAME, DOMAIN2);
	for (p = PUBLICKEYNOS; *p != '\0'; p++)
	{
		if (*p != '\n')
			putc(*p, f);
	}
	fprintf(f, "\n");

	fprintf(f, "%s.%s.%s ", SELECTOR2, DKIM_DNSKEYNAME, DOMAIN2);
	for (p = PUBLICKEYG; *p != '\0'; p++)
	{
		if (*p != '\n')
			putc(*p, f);
	}
	fprintf(f, "\n");

	fprintf(f, "%s.%s.%s ", SELECTORBADV, DKIM_DNSKEYNAME, DOMAIN);
	for (p = PUBLICKEYBADV; *p != '\0'; p++)
	{
		if (*p != '\n')
			putc(*p, f);
	}
	fprintf(f, "\n");

	fprintf(f, "%s.%s.%s ", SELECTOR256, DKIM_DNSKEYNAME, DOMAIN);
	for (p = PUBLICKEY256; *p != '\0'; p++)
	{
		if (*p != '\n')
			putc(*p, f);
	}
	fprintf(f, "\n");

	fprintf(f, "%s.%s.%s ", SELECTORBADH, DKIM_DNSKEYNAME, DOMAIN);
	for (p = PUBLICKEYBADH; *p != '\0'; p++)
	{
		if (*p != '\n')
			putc(*p, f);
	}
	fprintf(f, "\n");

	fprintf(f, "%s.%s.%s ", SELECTORNOK, DKIM_DNSKEYNAME, DOMAIN);
	for (p = PUBLICKEYNOK; *p != '\0'; p++)
	{
		if (*p != '\n')
			putc(*p, f);
	}
	fprintf(f, "\n");

	fprintf(f, "%s.%s.%s ", SELECTORBADK, DKIM_DNSKEYNAME, DOMAIN);
	for (p = PUBLICKEYBADK; *p != '\0'; p++)
	{
		if (*p != '\n')
			putc(*p, f);
	}
	fprintf(f, "\n");

	fprintf(f, "%s.%s.%s ", SELECTOREMPTYP, DKIM_DNSKEYNAME, DOMAIN);
	for (p = PUBLICKEYEMPTYP; *p != '\0'; p++)
	{
		if (*p != '\n')
			putc(*p, f);
	}
	fprintf(f, "\n");

	fprintf(f, "%s.%s.%s ", SELECTORNOP, DKIM_DNSKEYNAME, DOMAIN);
	for (p = PUBLICKEYNOP; *p != '\0'; p++)
	{
		if (*p != '\n')
			putc(*p, f);
	}
	fprintf(f, "\n");

	fprintf(f, "%s.%s.%s ", SELECTORCORRUPTP, DKIM_DNSKEYNAME, DOMAIN);
	for (p = PUBLICKEYCORRUPTP; *p != '\0'; p++)
	{
		if (*p != '\n')
			putc(*p, f);
	}
	fprintf(f, "\n");

	fprintf(f, "%s.%s.%s ", DKIM_DNSPOLICYNAME, DKIM_DNSKEYNAME, DOMAIN2);
	fprintf(f, "dkim=all; t=s; r=%s\n", REPLYADDRESS);

	fprintf(f, "%s exists\n", DOMAIN2);

	fclose(f);

	return 0;
}
