/*
**  Copyright (c) 2005-2008 Sendmail, Inc. and its suppliers.
**    All rights reserved.
**
**  Copyright (c) 2009, The OpenDKIM Project.  All rights reserved.
*/

#ifndef lint
static char t_test73_c_id[] = "@(#)$Id: t-test73.c,v 1.3.2.1 2009/11/02 01:12:35 cm-msk Exp $";
#endif /* !lint */

/* system includes */
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

/* libopendkim includes */
#include "dkim.h"
#include "t-testdata.h"

#define	BUFRSZ		1024
#define	MAXADDRESS	256
#define	MAXHEADER	4096

#define SIG2 "v=1;  a=rsa-sha256; c=simple/simple; d=example.com; s=test;\r\n\tt=1172620939; bh=yHBAX+3IwxTZIynBuB/5tlsBInJq9n8qz5fgAycHi80=;\r\n\th=Received:Received:Received:From:To:Date:Subject:Message-ID; b=Y3y\r\n\tVeA3WZdCZl1sGuOZNC3BBRhtGCOExkZdw5xQoGPvSX/q6AC1SAJvOUWOri95AZAUGs0\r\n\t/bIDzzt23ei9jc+rptlavrl/5ijMrl6ShmvkACk6It62KPkJcDpoGfi5AZkrfX1Ou/z\r\n\tqGg5xJX86Kqd7FgNolMg7PbfyWliK2Yb84="

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
	printf("*** simple/simple rsa-sha256 verifying with extra signature spaces and reportinfo (failure) SKIPPED\n");

#else /* ! DKIM_SIGN_RSASHA256 */

	int hfd;
	int bfd;
	dkim_policy_t pcode = DKIM_POLICY_NONE;
	u_int flags;
	DKIM_STAT status;
	DKIM *dkim;
	DKIM_LIB *lib;
	DKIM_SIGINFO *sig;
	dkim_query_t qtype = DKIM_QUERY_FILE;
	unsigned char hdr[MAXHEADER + 1];
	unsigned char addr[MAXADDRESS + 1];
	unsigned char fmt[BUFRSZ];
	unsigned char opts[BUFRSZ];
	unsigned char smtp[BUFRSZ];

	printf("*** simple/simple rsa-sha256 verifying with extra signature spaces and reportinfo (failure)\n");

	/* instantiate the library */
	lib = dkim_init(NULL, NULL);
	assert(lib != NULL);

	/* set flags */
	flags = DKIM_LIBFLAGS_TMPFILES;
# ifdef TEST_KEEP_FILES
	flags |= DKIM_LIBFLAGS_KEEPFILES;
# endif /* TEST_KEEP_FILES */
	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_FLAGS, &flags,
	                    sizeof flags);

	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_QUERYMETHOD,
	                    &qtype, sizeof qtype);
	(void) dkim_options(lib, DKIM_OP_SETOPT, DKIM_OPTS_QUERYINFO,
	                    KEYFILE, strlen(KEYFILE));

	dkim = dkim_verify(lib, JOBID, NULL, &status);
	assert(dkim != NULL);

	snprintf(hdr, sizeof hdr, "%s: %s", DKIM_SIGNHEADER, SIG2);
	status = dkim_header(dkim, hdr, strlen(hdr));
	assert(status == DKIM_STAT_OK);

	status = dkim_header(dkim, HEADER01, strlen(HEADER01));
	assert(status == DKIM_STAT_OK);

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

	status = dkim_body(dkim, BODY00, strlen(BODY00));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY01, strlen(BODY01));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY01A, strlen(BODY01A));
	assert(status == DKIM_STAT_OK);
	status = dkim_body(dkim, BODY01B, strlen(BODY01B));
	assert(status == DKIM_STAT_OK);
	status = dkim_body(dkim, BODY01C, strlen(BODY01C));
	assert(status == DKIM_STAT_OK);
	status = dkim_body(dkim, BODY01D, strlen(BODY01D));
	assert(status == DKIM_STAT_OK);
	status = dkim_body(dkim, BODY01E, strlen(BODY01E));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY02, strlen(BODY02));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY04, strlen(BODY04));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY05, strlen(BODY05));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_body(dkim, BODY03, strlen(BODY03));
	assert(status == DKIM_STAT_OK);

	status = dkim_eom(dkim, NULL);
	assert(status == DKIM_STAT_BADSIG);

	/* set up for the rest */
	hfd = -1;
	bfd = -1;
	sig = dkim_getsignature(dkim);
	assert(sig != NULL);

	/* policy query, verify correct result */
	status = dkim_policy(dkim, &pcode, NULL);
	assert(status == DKIM_STAT_OK);
	assert(pcode == DKIM_POLICY_ALL);

	/* request report info, verify valid descriptors and address */
	memset(addr, '\0', sizeof addr);
	memset(fmt, '\0', sizeof fmt);
	memset(opts, '\0', sizeof opts);
	memset(smtp, '\0', sizeof smtp);
	status = dkim_sig_getreportinfo(dkim, sig, &hfd, &bfd,
	                                addr, sizeof addr,
	                                fmt, sizeof fmt,
	                                opts, sizeof opts,
	                                smtp, sizeof smtp, NULL);
	assert(status == DKIM_STAT_OK);
	assert(hfd > 2);
	assert(bfd > 2);
	assert(strcmp(addr, REPLYADDRESS) == 0);
	assert(strcmp(smtp, SMTPTOKEN) == 0);

	/* test descriptors */
	status = lseek(hfd, 0, SEEK_CUR);
	assert(status >= 0);
	status = lseek(bfd, 0, SEEK_CUR);
	assert(status >= 0);

	status = dkim_free(dkim);
	assert(status == DKIM_STAT_OK);

	dkim_close(lib);
#endif /* ! DKIM_SIGN_RSASHA256 */

	return 0;
}
