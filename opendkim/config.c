/*
**  Copyright (c) 2006-2009 Sendmail, Inc. and its suppliers.
**	All rights reserved.
**
**  Copyright (c) 2009, The OpenDKIM Project.  All rights reserved.
**
**  $Id: config.c,v 1.4 2009/09/18 02:43:10 cm-msk Exp $
*/

#ifndef lint
static char config_c_id[] = "@(#)$Id: config.c,v 1.4 2009/09/18 02:43:10 cm-msk Exp $";
#endif /* !lint */

/* system includes */
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* libopendkim includes */
#include <dkim-strl.h>

/* opendkim includes */
#include "config.h"

/* limits */
#define	MAXLEVEL	5		/* max. include recursion */

/* prototypes */
static void config_attach __P((struct config *, struct config *));

/* errors */
#define	CONF_UNKNOWN	(-1)		/* unknown status */
#define	CONF_SUCCESS	0		/* no error */
#define	CONF_MISSING	1		/* required value missing */
#define	CONF_UNRECOG	2		/* unrecognized parameter */
#define	CONF_ILLEGAL	3		/* illegal value */
#define	CONF_NESTING	4		/* "include" nesting too deep */
#define	CONF_READING	5		/* error reading (see errno) */
#define	CONF_NMEMORY	6		/* malloc() failure */

/* statics */
static int conf_error;			/* configuration error number */

/*
**  CONFIG_ATTACH -- attach one config to another
**
**  Parameters:
**  	c1 -- configuration to attach
**  	c2 -- configuration to which to attach
**
**  Return value:
**  	None.
*/

static void
config_attach(struct config *c1, struct config *c2)
{
	struct config *prev;
	struct config *cur;

	assert(c1 != NULL);
	assert(c2 != NULL);

	prev = NULL;

	for (cur = c1; cur != NULL; cur = cur->cfg_next)
		prev = cur;

	prev->cfg_next = c2;
}

/*
**  CONFIG_LOAD_LEVEL -- load configuration from a file (internal version)
**
**  Parameters:
**  	file -- path from which to load; NULL or "-" implies stdin
**  	cd -- array of (struct configdef) elements containing the
**  	      configuration syntax to assert
**  	line -- line number where an error occurred (updated)
**  	outpath -- configuration file in which error occurred (updated)
**  	outpathlen -- bytes available at "outpath"
**  	level -- nesting level
**
**  Return value:
**  	Pointer to a (struct config) which is the head of a list of
**  	loaded configuration items, or NULL on error; if NULL, "line" is
**  	updated to indicate which line number contained the error and,
**  	if the configuration file being parsed was not the one referenced
**  	by "in", then "path" will be updated to point to the filename
**  	that was being processed.
*/

static struct config *
config_load_level(char *file, struct configdef *def,
                  unsigned int *line, char *outpath, size_t outpathlen,
                  int level)
{
	int arg;
	int n = -1;
	int err = 0;
	unsigned int myline = 0;
	int value = -1;
	FILE *in;
	char *p;
	char *str = NULL;
	struct config *new = NULL;
	struct config *cur = NULL;
	char buf[BUFRSZ + 1];

	assert(def != NULL);

	if (level > MAXLEVEL)
	{
		conf_error = CONF_NESTING;
		return NULL;
	}

	memset(buf, '\0', sizeof buf);

	if (file == NULL || (file[0] == '-' && file[1] == '\0'))
	{
		in = stdin;
		file = "(stdin)";
	}
	else
	{
		in = fopen(file, "r");
		if (in == NULL)
		{
			conf_error = CONF_READING;
			if (line != NULL)
				*line = myline;
			if (outpath != NULL)
				strlcpy(outpath, file, outpathlen);
			return NULL;
		}
	}

	while (fgets(buf, sizeof buf - 1, in) != NULL)
	{
		myline++;
		str = NULL;

		/* read a line; truncate at newline or "#" */
		for (p = buf; *p != '\0'; p++)
		{
			if (*p == '#' || *p == '\n')
			{
				*p = '\0';
				break;
			}
		}

		arg = 0;

		/* break down the arguments */
		/* XXX -- need something better than strtok(), for quoting */
		for (p = strtok(buf, " \t");
		     err == 0 && p != NULL; 
		     p = strtok(NULL, " \t"))
		{
			/* recognize the first? */
			if (arg == 0)
			{
				for (n = 0; ; n++)
				{
					/* nope */
					if (def[n].cd_name == NULL)
					{
						conf_error = CONF_UNRECOG;
						err = 1;
						break;
					}

					if (strcasecmp(def[n].cd_name, p) == 0)
						break;
				}
			}
			else if (str == NULL)
			{
				char *q;

				switch (def[n].cd_type)
				{
				  case CONFIG_TYPE_STRING:
				  case CONFIG_TYPE_INCLUDE:
					str = p;
					break;

				  case CONFIG_TYPE_BOOLEAN:
					if (p[0] == 't' ||
					    p[0] == 'T' ||
					    p[0] == 'y' ||
					    p[0] == 'Y' ||
					    p[0] == '1')
					{
						value = 1;
					}
					else if (p[0] == 'f' ||
					         p[0] == 'F' ||
					         p[0] == 'n' ||
					         p[0] == 'N' ||
					         p[0] == '0')
					{
						value = 0;
					}
					else
					{
						conf_error = CONF_ILLEGAL;
						err = 1;
					}

					break;

				  case CONFIG_TYPE_INTEGER:
					value = (int) strtol(p, &q, 0);
					if (*q != '\0')
					{
						conf_error = CONF_ILLEGAL;
						err = 1;
					}

					str = p;

					break;

				  default:
					assert(0);
					/* NOTREACHED */
					return NULL;
				}
			}

			arg++;
		}

		/* no arguments */
		if (arg == 0)
			continue;

		/* a parse error, or only one argument, is no good */
		if (arg == 1 || err == 1)
		{
			config_free(cur);

			if (line != NULL)
				*line = myline;
			if (outpath != NULL)
				strlcpy(outpath, file, outpathlen);

			if (in != stdin)
				fclose(in);

			return NULL;
		}

		if (def[n].cd_type != CONFIG_TYPE_INCLUDE)
		{
			new = (struct config *) malloc(sizeof(struct config));
			if (new == NULL)
			{
				config_free(cur);

				conf_error = CONF_NMEMORY;

				if (line != NULL)
					*line = myline;
				if (outpath != NULL)
					strlcpy(outpath, file, outpathlen);

				if (in != stdin)
					fclose(in);

				return NULL;
			}

			new->cfg_next = cur;
			new->cfg_name = def[n].cd_name;
			new->cfg_type = def[n].cd_type;
		}

		switch (def[n].cd_type)
		{
		  case CONFIG_TYPE_INCLUDE:
		  {
			struct config *incl;

			incl = config_load_level(str, def, line, outpath,
			                         outpathlen, level + 1);
			if (incl == NULL)
			{
				if (in != stdin)
					fclose(in);

				return NULL;
			}

			config_attach(incl, cur);
			new = incl;

			break;
		  }

		  case CONFIG_TYPE_STRING:
			new->cfg_string = strdup(str);
			break;

		  case CONFIG_TYPE_BOOLEAN:
			new->cfg_bool = (bool) value;
			break;

		  case CONFIG_TYPE_INTEGER:
			new->cfg_int = value;
			break;

		  default:
			assert(0);
		}

		cur = new;
	}

	conf_error = CONF_SUCCESS;

	if (in != stdin)
		fclose(in);

	if (myline == 0)
	{
		cur = (struct config *) malloc(sizeof *cur);
		if (cur != NULL)
		{
			cur->cfg_bool = FALSE;
			cur->cfg_type = CONFIG_TYPE_STRING;
			cur->cfg_int = 0;
			cur->cfg_name = "";
			cur->cfg_string = "";
			cur->cfg_next = NULL;

			return cur;
		}
		else
		{
			conf_error = CONF_NMEMORY;

			if (line != NULL)
				*line = myline;
			if (outpath != NULL)
				strlcpy(outpath, file, outpathlen);

			return NULL;
		}
	}
	else
	{
		return cur;
	}
}

/*
**  CONFIG_ERROR -- return a string describing a configuration error
**
**  Parameters:
**  	None.
**
**  Return value:
**  	Pointer to a NULL-terminated string explaining the last error.
*/

char *
config_error(void)
{
	switch (conf_error)
	{
	  case CONF_SUCCESS:
		return "no error";

	  case CONF_MISSING:
		return "required value missing";

	  case CONF_UNRECOG:
		return "unrecognized parameter";

	  case CONF_ILLEGAL:
		return "illegal value";

	  case CONF_NESTING:
		return "nesting too deep";

	  case CONF_READING:
		return "error reading configuration file";

	  case CONF_NMEMORY:
		return "memory allocation failure";

	  case CONF_UNKNOWN:
	  default:
		return "unknown error";
	}

	/* NOTREACHED */
}

/*
**  CONFIG_FREE -- release memory associated with a config list
**
**  Parameters:
**  	head -- head of the config list
**
**  Return value:
**  	None.
*/

void
config_free(struct config *head)
{
	struct config *next;
	struct config *cur;

	cur = head;
	while (cur != NULL)
	{
		next = cur->cfg_next;
		if (cur->cfg_type == CONFIG_TYPE_STRING)
			free(cur->cfg_string);
		free(cur);
		cur = next;
	}
}

/*
**  CONFIG_LOAD -- load configuration from a file
**
**  Parameters:
**  	file -- path from which to load; NULL or "-" implies stdin
**  	cd -- array of (struct configdef) elements containing the
**  	      configuration syntax to assert
**  	line -- line number where an error occurred (updated)
**  	path -- configuration file in which error occurred (updated)
**  	pathlen -- number of bytes available at "path"
**
**  Return value:
**  	Pointer to a (struct config) which is the head of a list of
**  	loaded configuration items, or NULL on error; if NULL, "line" is
**  	updated to indicate which line number contained the error and,
**  	if the configuration file being parsed was not the one referenced
**  	by "in", then "path" will be updated to point to the filename
**  	that was being processed.
*/

struct config *
config_load(char *file, struct configdef *def, unsigned int *line,
            char *path, size_t pathlen)
{
	conf_error = CONF_UNKNOWN;

	return config_load_level(file, def, line, path, pathlen, 0);
}

/*
**  CONFIG_CHECK -- verify that stuff marked "required" is present
**
**  Parameters:
**  	head -- head of config list
**  	def -- definitions
**
**  Return value:
**  	Name of the first parameter in "def" that was marked "required"
**  	yet absent from the configuration parsed, or NULL if nothing
**  	required was missing.
*/

char *
config_check(struct config *head, struct configdef *def)
{
	int n;
	struct config *cur;

	assert(head != NULL);
	assert(def != NULL);

	conf_error = CONF_UNKNOWN;

	for (n = 0; ; n++)
	{
		if (def[n].cd_name == NULL)
		{
			conf_error = CONF_SUCCESS;
			return NULL;
		}
		if (!def[n].cd_req)
			continue;

		for (cur = head; cur != NULL; cur = cur->cfg_next)
		{
			if (cur->cfg_name == def[n].cd_name)
				break;
		}

		if (cur == NULL)
		{
			conf_error = CONF_MISSING;

			return def[n].cd_name;
		}
	}

	/* NOTREACHED */
}

/*
**  CONFIG_GET -- retrieve a parameter's value
**
**  Parameter:
**  	head -- head of config list
**  	name -- name of the parameter of interest
**  	value -- where to write the result (returned)
**  	size -- bytes available at "value"
**
**  Return value:
**  	1 if the data was found, 0 otherwise, -1 if the request was illegal
**
**  Notes:
**  	"value" is a (void *).  It can be used directly, such as:
**
**  		int x;
**
**  		(void) config_get(conflist, "MyInteger", (void *) &x);
*/

int
config_get(struct config *head, const char *name, void *value, size_t size)
{
	struct config *cur;

	assert(head != NULL);
	assert(name != NULL);
	assert(value != NULL);
	assert(size > 0);

	conf_error = CONF_UNKNOWN;

	for (cur = head; cur != NULL; cur = cur->cfg_next)
	{
		if (strcasecmp(cur->cfg_name, name) == 0)
		{
			switch (cur->cfg_type)
			{
			  case CONFIG_TYPE_BOOLEAN:
				if (size != sizeof(bool))
				{
					conf_error = CONF_ILLEGAL;
					return -1;
				}
				memcpy(value, &cur->cfg_bool, size);
				break;

			  case CONFIG_TYPE_INTEGER:
				if (size != sizeof(int))
				{
					conf_error = CONF_ILLEGAL;
					return -1;
				}
				memcpy(value, &cur->cfg_int, size);
				break;

			  case CONFIG_TYPE_INCLUDE:
				conf_error = CONF_ILLEGAL;
				return -1;

			  default:
				if (size != sizeof(char *))
				{
					conf_error = CONF_ILLEGAL;
					return -1;
				}
				memcpy(value, &cur->cfg_string, size);
				break;
			}

			return 1;
		}
	}

	conf_error = CONF_SUCCESS;

	return 0;
}

#ifdef DEBUG
/*
**  CONFIG_DUMP -- dump configuration contents
**
**  Parameters:
**  	cfg -- head of assembled configuration values
**  	out -- stream to which to write
**
**  Return value:
**  	None.
*/

void
config_dump(struct config *cfg, FILE *out)
{
	struct config *cur;

	assert(cfg != NULL);
	assert(out != NULL);

	for (cur = cfg; cur != NULL; cur = cur->cfg_next)
	{
		fprintf(out, "%p: \"%s\" ", cur, cur->cfg_name);

		switch (cur->cfg_type)
		{
		  case CONFIG_TYPE_STRING:
			fprintf(out, "\"%s\"\n", cur->cfg_string);
			break;

		  case CONFIG_TYPE_INTEGER:
			fprintf(out, "%d\n", cur->cfg_int);
			break;

		  case CONFIG_TYPE_BOOLEAN:
			fprintf(out, "%s\n", cur->cfg_bool ? "True" : "False");
			break;

		  default:
			assert(0);
		}
	}
}
#endif /* DEBUG */
