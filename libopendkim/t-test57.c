/*
**  Copyright (c) 2005-2008 Sendmail, Inc. and its suppliers.
**    All rights reserved.
**
**  Copyright (c) 2009, The OpenDKIM Project.  All rights reserved.
*/

#ifndef lint
static char t_test57_c_id[] = "@(#)$Id: t-test57.c,v 1.4 2009/07/23 17:54:40 cm-msk Exp $";
#endif /* !lint */

/* system includes */
#include <sys/types.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* libopendkim includes */
#include "dkim.h"
#include "t-testdata.h"
#include "dkim-strl.h"

#define	MAXHEADER	4096

#define SIG2 "v=1; a=rsa-sha256; c=relaxed/simple; d=example.com; s=test;\r\n\tt=1172620939; bh=TRy6u0KYixKENCPL95G1Y1uwclQ9rWthX6rFgE7gWb8=;\r\n\th=Received:Received:Received:From:To:Date:Subject:Message-ID;\r\n\tb=aMDMwcz08Na0zYLrEZnUMBUipMh3blE+pWiBSvX+t2ynaDUGTqd27xuR5JddL9y1T\r\n\t 6h0LkUMIcijsPBWa1WWfKL23QceksV75Y1/Tt6gmVi3SdVAoJbbZImrOaAQ3YQAn64\r\n\t dp1vI0Z8gSashYlwy+7gtmVSLAj/fpcAF6yNDPuE="

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
#ifndef DKIM_SIGN_RSASHA256
	printf("*** relaxed/simple rsa-sha256 signing random chunk sizes SKIPPED\n");

#else /* ! DKIM_SIGN_RSASHA256 */

	u_int wlen = LARGEBODYSIZE;
	u_int remain;
# ifdef TEST_KEEP_FILES
	u_int flags;
# endif /* TEST_KEEP_FILES */
	DKIM_STAT status;
	time_t fixed_time;
	u_char *start;
	u_char *end;
	DKIM *dkim;
	DKIM_LIB *lib;
	dkim_sigkey_t key;
	struct timeval t;
	unsigned char hdr[MAXHEADER + 1];
	unsigned char body[LARGEBODYSIZE];

	printf("*** relaxed/simple rsa-sha256 signing random chunk sizes\n");

	/* instantiate the library */
	lib = dkim_init(NULL, NULL);
	assert(lib != NULL);

# ifdef TEST_KEEP_FILES
	/* set flags */
	flags = (DKIM_LIBFLAGS_TMPFILES|DKIM_LIBFLAGS_KEEPFILES);
	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_FLAGS, &flags,
	                    sizeof flags);
# endif /* TEST_KEEP_FILES */

	key = KEY;

	dkim = dkim_sign(lib, JOBID, NULL, key, SELECTOR, DOMAIN,
	                 DKIM_CANON_RELAXED, DKIM_CANON_SIMPLE,
	                 DKIM_SIGN_RSASHA256, -1L, &status);
	assert(dkim != NULL);

	/* fix signing time */
	fixed_time = 1172620939;
	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_FIXEDTIME,
	                    &fixed_time, sizeof fixed_time);

	status = dkim_header(dkim, HEADER02, strlen(HEADER02));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER03, strlen(HEADER03));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER04, strlen(HEADER04));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER05, strlen(HEADER05));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER06, strlen(HEADER06));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER07, strlen(HEADER07));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER08, strlen(HEADER08));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER09, strlen(HEADER09));
	assert(status == DKIM_STAT_OK);

	status = dkim_eoh(dkim);
	assert(status == DKIM_STAT_OK);

	memset(body, '\0', sizeof body);

	while (wlen > strlen(GIBBERISH))
	{
		strlcat(body, GIBBERISH, sizeof body);
		wlen -= strlen(GIBBERISH);
	}

	/* random-length chunks */
	(void) gettimeofday(&t, NULL);
	srandom(t.tv_usec);
	start = body;
	remain = strlen(body);
	end = body + remain;
	while (start < end)
	{
		wlen = random() % sizeof body + 1;
		if (wlen > remain)
			wlen = remain;
		printf("%d... ", wlen);
		status = dkim_body(dkim, start, wlen);
		assert(status == DKIM_STAT_OK);
		start += wlen;
		remain -= wlen;
	}

	printf("done!\n");

	status = dkim_body(dkim, CRLF, strlen(CRLF));
	assert(status == DKIM_STAT_OK);

	status = dkim_eom(dkim, NULL);
	assert(status == DKIM_STAT_OK);

	memset(hdr, '\0', sizeof hdr);
	status = dkim_getsighdr(dkim, hdr, sizeof hdr,
	                        strlen(DKIM_SIGNHEADER) + 2);
	assert(status == DKIM_STAT_OK);
	assert(strcmp(SIG2, hdr) == 0);

	status = dkim_free(dkim);
	assert(status == DKIM_STAT_OK);

	dkim_close(lib);
#endif /* ! DKIM_SIGN_RSASHA256 */

	return 0;
}
