/*
**  Copyright (c) 2005-2008 Sendmail, Inc. and its suppliers.
**    All rights reserved.
**
**  Copyright (c) 2009, The OpenDKIM Project.  All rights reserved.
*/

#ifndef lint
static char t_test51_c_id[] = "@(#)$Id: t-test51.c,v 1.3 2009/07/23 17:40:24 cm-msk Exp $";
#endif /* !lint */

/* system includes */
#include <sys/types.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>


/* libopendkim includes */
#include "dkim.h"
#include "t-testdata.h"

#define	MAXHEADER	4096

#define SIG2	"v=1; a=rsa-sha256; s=brisbane; d=example.com;\r\n" \
	"      c=simple/simple; q=dns/txt; i=joe@football.example.com;\r\n" \
	"      h=Received : From : To : Subject : Date : Message-ID;\r\n" \
	"      bh=2jUSOH9NhtVGCQWNr9BrIAPreKQjO6Sn7XIkfJVOzv8=;\r\n" \
	"      b=AuUoFEfDxTDkHlLXSZEpZj79LICEps6eda7W3deTVFOk4yAUoqOB\r\n" \
	"        4nujc7YopdG5dWLSdNg6xNAZpOPr+kHxt1IrE+NahM6L/LbvaHut\r\n" \
	"        KVdkLLkpVaVVQPzeRDI009SO2Il5Lu7rDNH6mZckBdrIx0orEtZV\r\n" \
	"        4bmp/YzhwvcubU4=;"

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
	printf("*** simple/simple rsa-sha256 verifying published example SKIPPED\n");

#else /* ! DKIM_SIGN_RSASHA256 */

# ifdef TEST_KEEP_FILES
	u_int flags;
# endif /* TEST_KEEP_FILES */
	DKIM_STAT status;
	DKIM *dkim;
	DKIM_LIB *lib;
	dkim_query_t qtype = DKIM_QUERY_FILE;
	unsigned char hdr[MAXHEADER + 1];

	printf("*** simple/simple rsa-sha256 verifying published example\n");

	/* instantiate the library */
	lib = dkim_init(NULL, NULL);
	assert(lib != NULL);

# ifdef TEST_KEEP_FILES
	/* set flags */
	flags = (DKIM_LIBFLAGS_TMPFILES|DKIM_LIBFLAGS_KEEPFILES);
	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_FLAGS, &flags,
	                    sizeof flags);
# endif /* TEST_KEEP_FILES */

	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_QUERYMETHOD,
	                    &qtype, sizeof qtype);
	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_QUERYINFO,
	                    KEYFILE, strlen(KEYFILE));

	dkim = dkim_verify(lib, JOBID, NULL, &status);
	assert(dkim != NULL);

	snprintf(hdr, sizeof hdr, "%s: %s", DKIM_SIGNHEADER, SIG2);
	status = dkim_header(dkim, hdr, strlen(hdr));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, THEADER00, strlen(THEADER00));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, THEADER01, strlen(THEADER01));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, THEADER02, strlen(THEADER02));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, THEADER03, strlen(THEADER03));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, THEADER04, strlen(THEADER04));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, THEADER05, strlen(THEADER05));
	assert(status == DKIM_STAT_OK);

	status = dkim_eoh(dkim);
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, TBODY, strlen(TBODY));
	assert(status == DKIM_STAT_OK);

	status = dkim_eom(dkim, NULL);
	assert(status == DKIM_STAT_OK);

	status = dkim_free(dkim);
	assert(status == DKIM_STAT_OK);

	dkim_close(lib);
#endif /* ! DKIM_SIGN_RSASHA256 */

	return 0;
}
