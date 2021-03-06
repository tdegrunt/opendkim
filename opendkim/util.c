/*
**  Copyright (c) 2005-2009 Sendmail, Inc. and its suppliers.
**	All rights reserved.
**
**  Copyright (c) 2009, The OpenDKIM Project.  All rights reserved.
**
**  $Id: util.c,v 1.13 2009/10/20 23:13:56 cm-msk Exp $
*/

#ifndef lint
static char util_c_id[] = "@(#)$Id: util.c,v 1.13 2009/10/20 23:13:56 cm-msk Exp $";
#endif /* !lint */

/* system includes */
#include <sys/param.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <assert.h>
#include <syslog.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#ifdef _FFR_REPLACE_RULES
# include <regex.h>
#endif /* _FFR_REPLACE_RULES */

#ifdef SOLARIS
# if SOLARIS <= 20600
#  define socklen_t size_t
# endif /* SOLARIS <= 20600 */
#endif /* SOLARIS */

/* libopendkim includes */
#include <dkim-strl.h>

/* opendkim includes */
#include "opendkim.h"
#include "util.h"
#if POPAUTH || _FFR_BODYLENGTH_DB
# include "opendkim-db.h"
#endif /* POPAUTH || _FFR_BODYLENGTH_DB */

/* missing definitions */
#ifndef INADDR_NONE
# define INADDR_NONE	((uint32_t) -1)
#endif /* ! INADDR_NONE */

/* globals */
#if POPAUTH
static pthread_mutex_t pop_lock;
#endif /* POPAUTH */

static char *optlist[] =
{
#if DEBUG
	"DEBUG",
#endif /* DEBUG */

#if POPAUTH
	"POPAUTH",
#endif /* POPAUTH */

#if QUERY_CACHE
	"QUERY_CACHE",
#endif /* QUERY_CACHE */

#if USE_ARLIB
	"USE_ARLIB",
#endif /* USE_ARLIB */

#if USE_UNBOUND
	"USE_UNBOUND",
#endif /* USE_UNBOUND */

#if VERIFY_DOMAINKEYS
	"VERIFY_DOMAINKEYS",
#endif /* VERIFY_DOMAINKEYS */

#ifdef _FFR_BODYLENGTH_DB
	"_FFR_BODYLENGTH_DB",
#endif /* _FFR_BODYLENGTH_DB */

#if _FFR_CAPTURE_UNKNOWN_ERRORS
	"_FFR_CAPTURE_UNKNOWN_ERRORS",
#endif /* _FFR_CAPTURE_UNKNOWN_ERRORS */

#if _FFR_DIFFHEADERS
	"_FFR_DIFFHEADERS",
#endif /* _FFR_DIFFHEADERS */

#if _FFR_DKIM_REPUTATION
	"_FFR_DKIM_REPUTATION",
#endif /* _FFR_DKIM_REPUTATION */

#if _FFR_DNS_UPGRADE
	"_FFR_DNS_UPGRADE",
#endif /* _FFR_DNS_UPGRADE */

#if _FFR_IDENTITY_HEADER
	"_FFR_IDENTITY_HEADER",
#endif /* _FFR_IDENTITY_HEADER */

#if _FFR_MULTIPLE_SIGNATURES
	"_FFR_MULTIPLE_SIGNATURES",
#endif /* _FFR_MULTIPLE_SIGNATURES */

#if _FFR_PARSE_TIME
	"_FFR_PARSE_TIME",
#endif /* _FFR_PARSE_TIME */

#if _FFR_REDIRECT
	"_FFR_REDIRECT",
#endif /* _FFR_REDIRECT */

#if _FFR_REPLACE_RULES
	"_FFR_REPLACE_RULES",
#endif /* _FFR_REPLACE_RULES */

#if _FFR_SELECT_CANONICALIZATION
	"_FFR_SELECT_CANONICALIZATION",
#endif /* _FFR_SELECT_CANONICALIZATION */

#if _FFR_SELECTOR_HEADER
	"_FFR_SELECTOR_HEADER",
#endif /* _FFR_SELECTOR_HEADER */

#if _FFR_SENDER_MACRO
	"_FFR_SENDER_MACRO",
#endif /* _FFR_SENDER_MACRO */

#if _FFR_STATS
	"_FFR_STATS",
#endif /* _FFR_STATS */

#if _FFR_VBR
	"_FFR_VBR",
#endif /* _FFR_VBR */

#if _FFR_ZTAGS
	"_FFR_ZTAGS",
#endif /* _FFR_ZTAGS */

	NULL
};

/* struct dkimf_dstring -- a dynamically-sized string */
struct dkimf_dstring
{
	int			ds_alloc;
	int			ds_max;
	int			ds_len;
	char *			ds_buf;
};

/* base64 alphabet */
static unsigned char alphabet[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#ifdef _FFR_REPLACE_RULES
/*
**  DKIMF_ISBLANK -- return TRUE iff a string contains only whitespace
**  
**  Parameters:
**  	str -- string to check
**
**  Return value:
**  	TRUE if "str" is either zero-length or contains only whitespace
*/

bool
dkimf_isblank(char *str)
{
	char *p;

	for (p = str; *p != '\0'; p++)
	{
		if (isascii(*p) && isspace(*p))
			continue;

		return FALSE;
	}

	return TRUE;
}
#endif /* _FFR_REPLACE_RULES */

/*
**  DKIMF_OPTLIST -- print active FFRs
**
**  Parameters:
**  	where -- where to write the list
**
**  Return value:
**   	None.
*/

void
dkimf_optlist(FILE *where)
{
	bool first = TRUE;
	int c;

	assert(where != NULL);

	for (c = 0; optlist[c] != NULL; c++)
	{
		if (first)
		{
			fprintf(where, "\tActive code options:\n");
			first = FALSE;
		}

		fprintf(where, "\t\t%s\n", optlist[c]);
	}
}

/*
**  DKIMF_SETMAXFD -- increase the file descriptor limit as much as possible
**
**  Parameters:
**  	None.
**
**  Return value:
**  	None.
*/

void
dkimf_setmaxfd(void)
{
	struct rlimit rlp;

	if (getrlimit(RLIMIT_NOFILE, &rlp) != 0)
	{
		syslog(LOG_WARNING, "getrlimit(): %s", strerror(errno));
	}
	else
	{
		rlp.rlim_cur = rlp.rlim_max;
		if (setrlimit(RLIMIT_NOFILE, &rlp) != 0)
		{
			syslog(LOG_WARNING, "setrlimit(): %s",
			       strerror(errno));
		}
	}
}

/*
**  DKIMF_STRIPBRACKETS -- remove angle brackets from the sender address
**
**  Parameters:
** 	addr -- address to be processed
**
**  Return value:
**  	None.
*/

void
dkimf_stripbrackets(char *addr)
{
	char *p, *q;

	assert(addr != NULL);

	p = addr;
	q = addr + strlen(addr) - 1;

	while (*p == '<' && *q == '>')
	{
		p++;
		*q-- = '\0';
	}

	if (p != addr)
	{
		for (q = addr; *p != '\0'; p++, q++)
			*q = *p;
		*q = '\0';
	}
}

/*
**  DKIMF_LOWERCASE -- lowercase-ize a string
**
**  Parameters:
**  	str -- string to convert
**
**  Return value:
**  	None.
*/

void
dkimf_lowercase(u_char *str)
{
	u_char *p;

	assert(str != NULL);

	for (p = str; *p != '\0'; p++)
	{
		if (isascii(*p) && isupper(*p))
			*p = tolower(*p);
	}
}

#ifdef NETINET6
/*
**  DKIMF_LIST_LOOKUP -- look up a name in a peerlist
**
**  Parameters:
**  	list -- list of records to check
** 	data -- record to find
**
**  Return value:
**   	TRUE if found, FALSE otherwise
*/

static bool
dkimf_list_lookup(Peer list, char *data)
{
	Peer current;

	assert(data != NULL);

	for (current = list; current != NULL; current = current->peer_next)
	{
		if (strcasecmp(data, current->peer_info) == 0)
			return TRUE;
	}

	return FALSE;
}
#endif /* NETINET6 */

/*
**  DKIMF_CHECKHOST -- check the peerlist for a host and its wildcards
**
**  Parameters:
**  	list -- list of records to check
**  	host -- hostname to find
**
**  Return value:
**  	TRUE if there's a match, FALSE otherwise.
*/

bool
dkimf_checkhost(Peer list, char *host)
{
	bool out = FALSE;
	char *p;
	Peer node;

	assert(host != NULL);

	/* short circuit */
	if (list == NULL)
		return FALSE;

	/* walk the list */
	for (node = list; node != NULL; node = node->peer_next)
	{
		if (strcasecmp(host, node->peer_info) == 0)
		{
			out = !node->peer_neg;
			continue;
		}

		for (p = strchr(host, '.');
		     p != NULL;
		     p = strchr(p + 1, '.'))
		{
			if (strcasecmp(p, node->peer_info) == 0)
			{
				out = !node->peer_neg;
				break;
			}
		}
	}

	return out;
}

/*
**  DKIMF_CHECKIP -- check a peerlist table for an IP address or its matching
**                 wildcards
**
**  Parameters:
**  	list -- list to check
**  	ip -- IP address to find
**
**  Return value:
**  	TRUE if there's a match, FALSE otherwise.
*/

bool
dkimf_checkip(Peer list, struct sockaddr *ip)
{
	bool out = FALSE;
	char ipbuf[DKIM_MAXHOSTNAMELEN + 1];

	assert(ip != NULL);

	/* short circuit */
	if (list == NULL)
		return FALSE;

#if NETINET6
	if (ip->sa_family == AF_INET6)
	{
		struct sockaddr_in6 sin6;
		struct in6_addr addr;

		memcpy(&sin6, ip, sizeof sin6);

		memcpy(&addr, &sin6.sin6_addr, sizeof addr);

		if (IN6_IS_ADDR_V4MAPPED(&addr))
		{
			inet_ntop(AF_INET,
			          &addr.s6_addr[INET6_ADDRSTRLEN - INET_ADDRSTRLEN],
			          ipbuf, sizeof ipbuf);
		}
		else
		{
			char *dst;
			size_t sz;
			size_t dst_len;

			dst = ipbuf;
			dst_len = sizeof ipbuf;

			memset(ipbuf, '\0', sizeof ipbuf);

			sz = strlcpy(ipbuf, "IPv6:", sizeof ipbuf);
			if (sz >= sizeof ipbuf)
				return FALSE;

			dst += sz;
			dst_len -= sz;
			inet_ntop(AF_INET6, &addr, dst, dst_len);
		}

		return (dkimf_list_lookup(list, ipbuf));
	}
#endif /* NETINET6 */

	if (ip->sa_family == AF_INET)
	{
		int bits;
		char *p;
		char *q;
		struct Peer *node;
		struct in_addr addr;
		struct in_addr mask;
		struct in_addr compare;
		struct sockaddr_in sin;

		memcpy(&sin, ip, sizeof sin);

		memcpy(&addr.s_addr, &sin.sin_addr, sizeof addr.s_addr);

		/* walk the list */
		for (node = list; node != NULL; node = node->peer_next)
		{
			/* try the IP direct match */
			(void) dkimf_inet_ntoa(addr, ipbuf, sizeof ipbuf);
			if (strcmp(ipbuf, node->peer_info) == 0)
			{
				out = !node->peer_neg;
				continue;
			}

			/* try the IP/CIDR and IP/mask possibilities */
			p = strchr(node->peer_info, '/');
			if (p == NULL)
				continue;

			*p = '\0';
			compare.s_addr = inet_addr(node->peer_info);
			if (compare.s_addr == INADDR_NONE)
			{
				*p = '/';
				continue;
			}

			bits = strtoul(p + 1, &q, 10);

			if (*q == '.')
			{
				mask.s_addr = inet_addr(p + 1);
				if (mask.s_addr == INADDR_NONE)
				{
					*p = '/';
					continue;
				}
			}
			else if (*q != '\0')
			{
				*p = '/';
				continue;
			}
			else
			{
				int c;

				mask.s_addr = 0;
				for (c = 0; c < bits; c++)
					mask.s_addr |= htonl(1 << (31 - c));
			}

			if ((addr.s_addr & mask.s_addr) == (compare.s_addr & mask.s_addr))
				out = !node->peer_neg;

			*p = '/';
		}
	}

	return out;
}

#if POPAUTH
/*
**  DKIMF_INITPOPAUTH -- initialize POPAUTH stuff
**
**  Parameters:
**  	None.
**
**  Return value:
**  	0 on success, an error code on failure.  See pthread_mutex_init().
*/

int
dkimf_initpopauth(void)
{
	return pthread_mutex_init(&pop_lock, NULL);
}

/*
**  DKIMF_CHECKPOPAUTH -- check a POP before SMTP database for client
**                        authentication
**
**  Parameters:
**  	db -- DB handle to use for searching
**  	ip -- IP address to find
**
**  Return value:
**  	TRUE iff the database could be opened and the client was verified.
**
**  Notes:
**  	- does the key contain anything meaningful, like an expiry time?
*/

bool
dkimf_checkpopauth(DB *db, struct sockaddr *ip)
{
	bool exists;
	int status;
	struct sockaddr_in *sin;
	struct in_addr addr;
	char ipbuf[DKIM_MAXHOSTNAMELEN + 1];

	assert(ip != NULL);

	if (db == NULL)
		return FALSE;

	/* skip anything not IPv4 (for now) */
	if (ip->sa_family != AF_INET)
		return FALSE;
	else
		sin = (struct sockaddr_in *) ip;


	memcpy(&addr.s_addr, &sin->sin_addr, sizeof addr.s_addr);

	dkimf_inet_ntoa(addr, ipbuf, sizeof ipbuf);
	exists = FALSE;
	status = dkimf_db_get(db, ipbuf, NULL, NULL, &exists, &pop_lock);
	return (status == 0 && exists);
}
#endif /* POPAUTH */

/*
**  DKIMF_ENQUEUE -- enqueue an entry onto a peerlist
**
**  Parameters:
**  	head -- queue head (updated)
**  	tail -- queue tail (updated)
**  	str -- entry
**
**  Return value:
**  	TRUE if successful, FALSE otherwise (memory allocation error)
*/

static bool
dkimf_enqueue(struct Peer **head, struct Peer **tail, char *str)
{
	struct Peer *newpeer;

	assert(head != NULL);
	assert(tail != NULL);
	assert(str != NULL);

	newpeer = (struct Peer *) malloc(sizeof(struct Peer));
	if (newpeer == NULL)
		return FALSE;

	newpeer->peer_next = NULL;

	if (*head == NULL)
	{
		*head = newpeer;
		*tail = newpeer;
	}
	else
	{
		(*tail)->peer_next = newpeer;
		*tail = newpeer;
	}

	if (str[0] == '!')
	{
		newpeer->peer_info = strdup(&str[1]);
		newpeer->peer_neg = TRUE;
	}
	else
	{
		newpeer->peer_info = strdup(str);
		newpeer->peer_neg = FALSE;
	}

	if (newpeer->peer_info == NULL)
		return FALSE;

	return TRUE;
}

/*
**  DKIMF_LOAD_LIST -- load a list of hosts/CIDR blocks into memory
**
**  Parameters:
**  	in -- input stream (or NULL if none)
**  	deflist -- array of entries to include (or NULL if none)
**  	list -- resultant list (updated)
**
**  Return value:
**  	TRUE if successful, FALSE otherwise
*/

bool
dkimf_load_list(FILE *in, char **deflist, struct Peer **list)
{
	struct Peer *head = NULL;
	struct Peer *tail = NULL;
	char peer[BUFRSZ + 1];

	assert(list != NULL);

	memset(peer, '\0', sizeof peer);

	if (deflist != NULL)
	{
		int c;

		for (c = 0; deflist[c] != NULL; c++)
		{
			if (!dkimf_enqueue(&head, &tail, deflist[c]))
				return FALSE;
		}
	}

	if (in != NULL)
	{
		char *p;

		while (fgets(peer, sizeof(peer) - 1, in) != NULL)
		{
			for (p = peer; *p != '\0'; p++)
			{
				if (*p == '\n')
				{
					*p = '\0';
					break;
				}
			}

			if (!dkimf_enqueue(&head, &tail, peer))
				return FALSE;
		}
	}

	*list = head;

	return TRUE;
}

/*
**  DKIMF_FREE_LIST -- destroy a list of peer information
**
**  Parameters:
**  	list -- list to destroy
**
**  Return value:
**  	None.
*/

void
dkimf_free_list(struct Peer *list)
{
	struct Peer *cur;
	struct Peer *next;

	assert(list != NULL);

	for (cur = list; cur != NULL; cur = next)
	{
		next = cur->peer_next;
		free(cur->peer_info);
		free(cur);
	}
}

#ifdef _FFR_REPLACE_RULES
/*
**  DKIMF_LOAD_REPLIST -- load a list of replace patterns
**
**  Parameters:
**  	in -- input stream (must already be open)
**  	list -- list to be updated
**
**  Return value:
**  	TRUE if successful, FALSE otherwise
**
**  Side effects:
**  	Prints an error message when appropriate.
*/

bool
dkimf_load_replist(FILE *in, struct replace **list)
{
	int line;
	int status;
	char *p;
	struct replace *newrep;
	char rule[BUFRSZ + 1];

	assert(in != NULL);
	assert(list != NULL);

	memset(rule, '\0', sizeof rule);

	while (fgets(rule, sizeof(rule) - 1, in) != NULL)
	{
		line++;

		for (p = rule; *p != '\0'; p++)
		{
			if (*p == '\n' || *p == '#')
			{
				*p = '\0';
				break;
			}
		}

		if (dkimf_isblank(rule))
			continue;

		newrep = (struct replace *) malloc(sizeof(struct replace));
		if (newrep == NULL)
		{
			fprintf(stderr, "%s: malloc(): %s\n", progname,
			        strerror(errno));
			return FALSE;
		}

		p = strrchr(rule, '\t');
		if (p == NULL)
			return FALSE;

		*p = '\0';

		status = regcomp(&newrep->repl_re, rule, 0);
		if (status != 0)
		{
			fprintf(stderr, "%s: regcomp() failed\n", progname);
			return FALSE;
		}

		newrep->repl_txt = strdup(p + 1);
		if (newrep->repl_txt == NULL)
		{
			fprintf(stderr, "%s: strdup(): %s\n", progname,
			        strerror(errno));
			return FALSE;
		}

		newrep->repl_next = *list;

		*list = newrep;
	}

	return TRUE;
}

/*
**  DKIMF_FREE_REPLIST -- destroy a list of replacement information
**
**  Parameters:
**  	list -- list to destroy
**
**  Return value:
**  	None.
*/

void
dkimf_free_replist(struct replace *list)
{
	struct replace *cur;
	struct replace *next;

	assert(list != NULL);

	for (cur = list; cur != NULL; cur = next)
	{
		next = cur->repl_next;
		regfree(&cur->repl_re);
		free(cur->repl_txt);
		free(cur);
	}
}

#endif /* _FFR_REPLACE_RULES */

/*
**  DKIMF_INET_NTOA -- thread-safe inet_ntoa()
**
**  Parameters:
**  	a -- (struct in_addr) to be converted
**  	buf -- destination buffer
**  	buflen -- number of bytes at buf
**
**  Return value:
**  	Size of the resultant string.  If the result is greater than buflen,
**  	then buf does not contain the complete result.
*/

size_t
dkimf_inet_ntoa(struct in_addr a, char *buf, size_t buflen)
{
	in_addr_t addr;

	assert(buf != NULL);

	addr = ntohl(a.s_addr);

	return snprintf(buf, buflen, "%d.%d.%d.%d",
	                (addr >> 24), (addr >> 16) & 0xff,
	                (addr >> 8) & 0xff, addr & 0xff);
}

/*
**  DKIMF_TRIMSPACES -- trim trailing whitespace
**
**  Parameters:
**  	str -- string to modify
**
**  Return value:
**  	None.
*/

void
dkimf_trimspaces(u_char *str)
{
	u_char *p;
	u_char *last;

	assert(str != NULL);

	last = NULL;

	for (p = str; *p != '\0'; p++)
	{
		if (isascii(*p) && isspace(*p) && last == NULL)
		{
			last = p;
			continue;
		}

		if (!isascii(*p) || !isspace(*p))
			last = NULL;
	}

	if (last != NULL)
		*last = '\0';
}

/*
**  DKIMF_STRIPCR -- remove CRs
**
**  Parameters:
**  	str -- string to modify
**
**  Return value:
**  	None.
*/

void
dkimf_stripcr(char *str)
{
	char *p;
	char *q;

	assert(str != NULL);

	for (p = str, q = str; *p != '\0'; p++)
	{
		if (*p == '\r')
			continue;

		if (q != p)
			*q = *p;
		q++;
	}

	if (q != p)
		*q = *p;
}

/*
**  DKIMF_MKPATH -- generate a path
**
**  Parameters:
**  	path -- output buffer
**  	pathlen -- bytes available at "path"
**  	root -- root to infer; if empty, use getcwd()
**  	file -- filename to use
**
**  Return value:
**  	None.
*/

void
dkimf_mkpath(char *path, size_t pathlen, char *root, char *file)
{
	assert(path != NULL);
	assert(root != NULL);
	assert(file != NULL);

	if (file[0] == '/')				/* explicit path */
	{
		strlcpy(path, file, pathlen);
	}
	else if (root[0] == '\0')			/* no root, use cwd */
	{
		(void) getcwd(path, pathlen);
		strlcat(path, "/", pathlen);
		strlcat(path, file, pathlen);
	}
	else						/* use root */
	{
		strlcpy(path, root, pathlen);
		if (root[strlen(root) - 1] != '/')
			strlcat(path, "/", pathlen);
		strlcat(path, file, pathlen);
	}
}

/*
**  DKIMF_HOSTLIST -- see if a hostname is in a pattern of hosts/domains
**
**  Parameters:
**  	host -- hostname to compare
**   	list -- NULL-terminated char * array to search
**
**  Return value:
**  	TRUE iff either "host" was in the list or it match a domain pattern
**  	found in the list.
*/

bool
dkimf_hostlist(char *host, char **list)
{
	int c;
	char *p;

	assert(host != NULL);
	assert(list != NULL);

	/* walk the entire list */
	for (c = 0; list[c] != NULL; c++)
	{
		/* first try a full hostname match */
		if (strcasecmp(host, list[c]) == 0)
			return TRUE;

		/* try each domain */
		for (p = strchr(host, '.'); p != NULL; p = strchr(p + 1, '.'))
		{
			if (strcasecmp(p, list[c]) == 0)
				return TRUE;
		}
	}

	/* not found */
	return FALSE;
}

/*
**  DKIMF_DSTRING_RESIZE -- resize a dynamic string (dstring)
**
**  Parameters:
**  	dstr -- DKIMF_DSTRING handle
**  	len -- number of bytes desired
**
**  Return value:
**  	TRUE iff the resize worked (or wasn't needed)
**
**  Notes:
**  	This will actually ensure that there are "len" bytes available.
**  	The caller must account for the NULL byte when requesting a
**  	specific size.
*/

static bool
dkimf_dstring_resize(struct dkimf_dstring *dstr, int len)
{
	int newsz;
	char *new;

	assert(dstr != NULL);
	assert(len > 0);

	if (dstr->ds_alloc >= len)
		return TRUE;

	/* must resize */
	for (newsz = dstr->ds_alloc * 2;
	     newsz < len;
	     newsz *= 2)
	{
		/* impose ds_max limit, if specified */
		if (dstr->ds_max > 0 && newsz > dstr->ds_max)
		{
			if (len <= dstr->ds_max)
			{
				newsz = len;
				break;
			}

			return FALSE;
		}

		/* check for overflow */
		if (newsz > INT_MAX / 2)
		{
			/* next iteration will overflow "newsz" */
			return FALSE;
		}
	}

	new = malloc(newsz);
	if (new == NULL)
		return FALSE;

	memcpy(new, dstr->ds_buf, dstr->ds_alloc);

	free(dstr->ds_buf);

	dstr->ds_alloc = newsz;
	dstr->ds_buf = new;

	return TRUE;
}

/*
**  DKIMF_DSTRING_NEW -- make a new dstring
**
**  Parameters:
**  	dkim -- DKIM handle
**  	len -- initial number of bytes
**  	maxlen -- maximum allowed length (0 == unbounded)
**
**  Return value:
**  	A DKIMF_DSTRING handle, or NULL on failure.
*/

struct dkimf_dstring *
dkimf_dstring_new(int len, int maxlen)
{
	struct dkimf_dstring *new;

	/* fail on invalid parameters */
	if ((maxlen > 0 && len > maxlen) || len == 0)
		return NULL;

	if (len < BUFRSZ)
		len = BUFRSZ;

	new = malloc(sizeof(struct dkimf_dstring));
	if (new == NULL)
		return NULL;

	new->ds_buf = malloc(len);
	if (new->ds_buf == NULL)
	{
		free(new);
		return NULL;
	}

	memset(new->ds_buf, '\0', len);
	new->ds_alloc = len;
	new->ds_len = 0;
	new->ds_max = maxlen;

	return new;
}

/*
**  DKIMF_DSTRING_FREE -- destroy an existing dstring
**
**  Parameters:
**  	dstr -- DKIMF_DSTRING handle to be destroyed
**
**  Return value:
**  	None.
*/

void
dkimf_dstring_free(struct dkimf_dstring *dstr)
{
	assert(dstr != NULL);

	free(dstr->ds_buf);
	free(dstr);
}

/*
**  DKIMF_DSTRING_COPY -- copy data into a dstring
**
**  Parameters:
**  	dstr -- DKIMF_DSTRING handle to update
**  	str -- input string
**
**  Return value:
**  	TRUE iff the copy succeeded.
**
**  Side effects:
**  	The dstring may be resized.
*/

bool
dkimf_dstring_copy(struct dkimf_dstring *dstr, char *str)
{
	int len;

	assert(dstr != NULL);
	assert(str != NULL);

	len = strlen(str);

	/* too big? */
	if (dstr->ds_max > 0 && len >= dstr->ds_max)
		return FALSE;

	/* fits now? */
	if (dstr->ds_alloc <= len)
	{
		/* nope; try to resize */
		if (!dkimf_dstring_resize(dstr, len + 1))
			return FALSE;
	}

	/* copy */
	dstr->ds_len = strlcpy(dstr->ds_buf, str, dstr->ds_alloc);

	return TRUE;
}

/*
**  DKIMF_DSTRING_CAT -- append data onto a dstring
**
**  Parameters:
**  	dstr -- DKIMF_DSTRING handle to update
**  	str -- input string
**
**  Return value:
**  	TRUE iff the update succeeded.
**
**  Side effects:
**  	The dstring may be resized.
*/

bool
dkimf_dstring_cat(struct dkimf_dstring *dstr, char *str)
{
	int len;

	assert(dstr != NULL);
	assert(str != NULL);

	len = strlen(str) + dstr->ds_len;

	/* too big? */
	if (dstr->ds_max > 0 && len >= dstr->ds_max)
		return FALSE;

	/* fits now? */
	if (dstr->ds_alloc <= len)
	{
		/* nope; try to resize */
		if (!dkimf_dstring_resize(dstr, len + 1))
			return FALSE;
	}

	/* append */
	dstr->ds_len = strlcat(dstr->ds_buf, str, dstr->ds_alloc);

	return TRUE;
}

/*
**  DKIMF_DSTRING_CAT1 -- append one byte onto a dstring
**
**  Parameters:
**  	dstr -- DKIMF_DSTRING handle to update
**  	c -- input character
**
**  Return value:
**  	TRUE iff the update succeeded.
**
**  Side effects:
**  	The dstring may be resized.
*/

bool
dkimf_dstring_cat1(struct dkimf_dstring *dstr, int c)
{
	int len;

	assert(dstr != NULL);

	len = dstr->ds_len + 1;

	/* too big? */
	if (dstr->ds_max > 0 && len >= dstr->ds_max)
		return FALSE;

	/* fits now? */
	if (dstr->ds_alloc <= len)
	{
		/* nope; try to resize */
		if (!dkimf_dstring_resize(dstr, len + 1))
			return FALSE;
	}

	/* append */
	dstr->ds_buf[dstr->ds_len++] = c;
	dstr->ds_buf[dstr->ds_len] = '\0';

	return TRUE;
}

/*
**  DKIMF_DSTRING_GET -- retrieve data in a dstring
**
**  Parameters:
**  	dstr -- DKIMF_DSTRING handle whose string should be retrieved
**
**  Return value:
**  	Pointer to the NULL-terminated contents of "dstr".
*/

char *
dkimf_dstring_get(struct dkimf_dstring *dstr)
{
	assert(dstr != NULL);

	return dstr->ds_buf;
}

/*
**  DKIMF_DSTRING_LEN -- retrieve length of data in a dstring
**
**  Parameters:
**  	dstr -- DKIMF_DSTRING handle whose string should be retrieved
**
**  Return value:
**  	Number of bytes in a dstring.
*/

int
dkimf_dstring_len(struct dkimf_dstring *dstr)
{
	assert(dstr != NULL);

	return dstr->ds_len;
}

/*
**  DKIMF_DSTRING_BLANK -- clear out the contents of a dstring
**
**  Parameters:
**  	dstr -- DKIMF_DSTRING handle whose string should be cleared
**
**  Return value:
**  	None.
*/

void
dkimf_dstring_blank(struct dkimf_dstring *dstr)
{
	assert(dstr != NULL);

	dstr->ds_len = 0;
	dstr->ds_buf[0] = '\0';
}

/*
**  DKIMF_DSTRING_CHOP -- truncate contents of a dstring
**
**  Parameters:
**  	dstr -- DKIMF_DSTRING handle whose string should be cleared
**  	len -- length after which to clobber
**
**  Return value:
**  	None.
*/

void
dkimf_dstring_chop(struct dkimf_dstring *dstr, int len)
{
	assert(dstr != NULL);

	if (len < dstr->ds_len)
	{
		dstr->ds_len = len;
		dstr->ds_buf[len] = '\0';
	}
}

/*
**  DKIMF_SOCKET_CLEANUP -- try to clean up the socket
**
**  Parameters:
**  	sockspec -- socket specification
**
**  Return value:
**  	0 -- nothing to cleanup or cleanup successful
**  	other -- an error code (a la errno)
*/

int
dkimf_socket_cleanup(char *sockspec)
{
	int s;
	char *colon;
	struct sockaddr_un sock;

	assert(sockspec != NULL);

	/* we only care about "local" or "unix" sockets */
	colon = strchr(sockspec, ':');
	if (colon != NULL)
	{
		if (strncasecmp(sockspec, "local:", 6) != 0 &&
		    strncasecmp(sockspec, "unix:", 5) != 0)
			return 0;
	}

	/* find the filename */
	if (colon == NULL)
	{
		colon = sockspec;
	}
	else
	{
		if (*(colon + 1) == '\0')
			return EINVAL;
	}

	/* get a socket */
	s = socket(PF_UNIX, SOCK_STREAM, 0);
	if (s == -1)
		return errno;

	/* set up a connection */
	memset(&sock, '\0', sizeof sock);
#ifdef BSD
	sock.sun_len = sizeof sock;
#endif /* BSD */
	sock.sun_family = PF_UNIX;
	strlcpy(sock.sun_path, colon + 1, sizeof sock.sun_path);

	/* try to connect */
	if (connect(s, (struct sockaddr *) &sock, (socklen_t) sizeof sock) != 0)
	{
		/* if ECONNREFUSED, try to unlink */
		if (errno == ECONNREFUSED)
		{
			close(s);

			if (unlink(sock.sun_path) == 0)
				return 0;
			else
				return errno;
		}

		/* if ENOENT, the socket's not there */
		else if (errno == ENOENT)
		{
			close(s);

			return 0;
		}

		/* something else happened */
		else
		{
			int saveerr;

			saveerr = errno;

			close(s);

			return saveerr;
		}
	}

	/* connection apparently succeeded */
	close(s);
	return EADDRINUSE;
}

/*
**  DKIMF_MKREGEXP -- make a regexp string from a glob string
**
**  Parameters:
**  	src -- source string
**  	dst -- destination string
**  	dstlen -- space available at "dest"
**
**  Return value:
**  	TRUE iff "dest" was big enough (based on destlen)
*/

bool
dkimf_mkregexp(char *src, char *dst, size_t dstlen)
{
	char *p;
	char *q;
	char *end;

	assert(src != NULL);
	assert(dst != NULL);

	if (dstlen == 0)
		return FALSE;

	dst[0] = '^';

	end = dst + dstlen;

	for (p = src, q = dst + 1; *p != '\0' && q < end; p++)
	{
		switch (*p)
		{
		  case '*':
			*q = '.';
			q++;
			*q = '*';
			q++;
			break;

		  case '.':
			*q = '\\';
			q++;
			*q = '.';
			q++;
			break;

		  default:
			*q = *p;
			q++;
			break;
		}
	}

	*q++ = '$';

	if (q >= end)
		return FALSE;
	else
		return TRUE;
}

/*
**  DKIMF_BASE64_ENCODE_FILE -- base64-encode a file
**
**  Parameters:
**  	infd -- input file descriptor
**  	out -- output stream
**  	lm -- left margin
** 	rm -- right margin
**  	initial -- space consumed on the initial line
**
**  Return value:
**  	None (yet).
*/

void
dkimf_base64_encode_file(infd, out, lm, rm, initial)
	int infd;
	FILE *out;
	int lm;
	int rm;
	int initial;
{
	int len;
	int bits;
	int c;
	int d;
	int char_count;
	ssize_t rlen;
	char buf[MAXBUFRSZ];

	assert(infd >= 0);
	assert(out != NULL);
	assert(lm >= 0);
	assert(rm >= 0);
	assert(initial >= 0);

	bits = 0;
	char_count = 0;
	len = initial;

	(void) lseek(infd, 0, SEEK_SET);

	for (;;)
	{
		rlen = read(infd, buf, sizeof buf);
		if (rlen == -1)
			break;

		for (c = 0; c < rlen; c++)
		{
			bits += buf[c];
			char_count++;
			if (char_count == 3)
			{
				fputc(alphabet[bits >> 18], out);
				fputc(alphabet[(bits >> 12) & 0x3f], out);
				fputc(alphabet[(bits >> 6) & 0x3f], out);
				fputc(alphabet[bits & 0x3f], out);
				len += 4;
				if (rm > 0 && lm > 0 && len >= rm - 4)
				{
					fputc('\n', out);
					for (d = 0; d < lm; d++)
						fputc(' ', out);
					len = lm;
				}
				bits = 0;
				char_count = 0;
			}
			else
			{
				bits <<= 8;
			}
		}

		if (rlen < (ssize_t) sizeof buf)
			break;
	}

	if (char_count != 0)
	{
		if (rm > 0 && lm > 0 && len >= rm - 4)
		{
			fputc('\n', out);
			for (d = 0; d < lm; d++)
				fputc(' ', out);
		}
		bits <<= 16 - (8 * char_count);
		fputc(alphabet[bits >> 18], out);
		fputc(alphabet[(bits >> 12) & 0x3f], out);
		if (char_count == 1)
			fputc('=', out);
		else
			fputc(alphabet[(bits >> 6) & 0x3f], out);
		fputc('=', out);
	}
}

/*
**  DKIMF_SUBDOMAIN -- determine whether or not one domain is a subdomain
**                     of the other
**
**  Parameters:
**  	d1 -- candidate domain
**  	d2 -- possible superdomain
**
**  Return value:
**  	TRUE iff d1 is a subdomain of d2.
*/

bool
dkimf_subdomain(char *d1, char *d2)
{
	char *p;

	assert(d1 != NULL);
	assert(d2 != NULL);

#if 0
	if (strcasecmp(d1, d2) == 0)
		return TRUE;
#endif /* 0 */

	for (p = strchr(d1, '.'); p != NULL; p = strchr(p + 1, '.'))
	{
		if (strcasecmp(d2, p + 1) == 0)
			return TRUE;
	}

	return FALSE;
}
