/* $OpenBSD: fuse_opt.c,v 1.19 2017/11/16 12:56:58 helg Exp $ */
/*
 * Copyright (c) 2013 Sylvestre Gallon <ccna.syl@gmail.com>
 * Copyright (c) 2013 Stefan Sperling <stsp@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "fuse_opt.h"
#include "fuse_private.h"

#define IFUSE_OPT_DISCARD 0
#define IFUSE_OPT_KEEP 1
#define IFUSE_OPT_NEED_ANOTHER_ARG 2

static void
free_argv(char **argv, int argc)
{
	int i;

	for (i = 0; i < argc; i++)
		free(argv[i]);
	free(argv);
}

static int
alloc_argv(struct fuse_args *args)
{
	char **argv;
	int i;

	assert(!args->allocated);

	argv = calloc(args->argc, sizeof(*argv));
	if (argv == NULL)
		return (-1);

	if (args->argv) {
		for (i = 0; i < args->argc; i++) {
			argv[i] = strdup(args->argv[i]);
			if (argv[i] == NULL) {
				free_argv(argv, i + 1);
				return (-1);
			}
		}
	}

	args->allocated = 1;
	args->argv = argv;

	return (0);
}

static int
match_opt(const char *templ, const char *opt)
{
	const char *o, *t;
	char *arg;

	arg = strpbrk(templ, " =");

	/* verify template */
	t = templ;
	if (*t == '-') {
		t++;
		if (*t == '-')
			t++;
		if (*t == 'o' || *t == '\0')
			return (0);
	}

	/* skip leading -, -o, and -- in option name */
	o = opt;
	if (*o == '-') {
		o++;
		if (*o == 'o' || *o == '-')
			o++;
	}

	/* empty option name is invalid */
	if (*o == '\0')
		return (0);

	/* match option name */
	while (*t && *o) {
		if (*t++ != *o++)
			return (0);
		if (arg && t == arg) {
			if (*o != ' ' && *o != '=')
				return (0);
			o++; /* o now points at argument */
			if (*o == '\0')
				return (0);
			break;
		}
	}

	/* match argument */
	if (arg) {
		if (t != arg)
			return (0);
		t++;
		/* does template have an argument? */
		if (*t != '%' && *t != '\0')
			return (0);
		if (*t == '%' && t[1] == '\0')
			return (0);
		/* yes it does, consume argument in opt */
		while (*o && *o != ' ')
			o++;
	} else if (*t != '\0')
		return (0);

	/* we should have consumed entire opt string */
	if (*o != '\0')
		return (0);

	return (1);
}

static int
add_opt(char **opts, const char *opt)
{
	char *new_opts;

	if (*opts == NULL) {
		*opts = strdup(opt);
		if (*opts == NULL)
			return (-1);
		return (0);
	}

	if (asprintf(&new_opts, "%s,%s", *opts, opt) == -1)
		return (-1);

	free(*opts);
	*opts = new_opts;
	return (0);
}

int
fuse_opt_add_opt(char **opts, const char *opt)
{
	int ret;

	if (opt == NULL || opt[0] == '\0')
		return (-1);

	ret = add_opt(opts, opt);
	return (ret);
}

int
fuse_opt_add_opt_escaped(char **opts, const char *opt)
{
	size_t size = 0, escaped = 0;
	const char *s = opt;
	char *escaped_opt, *p;
	int ret;

	if (opt == NULL || opt[0] == '\0')
		return (-1);

	while (*s) {
		/* malloc(size + escaped) overflow check */
		if (size >= (SIZE_MAX / 2))
			return (-1);

		if (*s == ',' || *s == '\\')
			escaped++;
		s++;
		size++;
	}

	if (escaped > 0) {
		escaped_opt = malloc(size + escaped);
		if (escaped_opt == NULL)
			return (-1);
		s = opt;
		p = escaped_opt;
		while (*s) {
			switch (*s) {
			case ',':
			case '\\':
				*p++ = '\\';
				/* FALLTHROUGH */
			default:
				*p++ = *s++;
			}
		}
		*p = '\0';
	} else {
		escaped_opt = strdup(opt);
		if (escaped_opt == NULL)
			return (-1);
	}

	ret = add_opt(opts, escaped_opt);
	free(escaped_opt);
	return (ret);
}

int
fuse_opt_add_arg(struct fuse_args *args, const char *name)
{
	return (fuse_opt_insert_arg(args, args->argc, name));
}

static int
parse_opt(const struct fuse_opt *o, const char *val, void *data,
    fuse_opt_proc_t f, struct fuse_args *arg)
{
	int keyval;
	size_t idx;

	if (o == NULL)
		return IFUSE_OPT_KEEP;

	keyval = 0;

	for(; o->templ; o++) {
		/* check key=value or -p n */
		idx = strcspn(o->templ, "= ");

		if (strncmp(val, o->templ, idx) == 0) {
			if (o->templ[idx] == '=') {
				keyval = 1;
				val = &val[idx + 1];
			} else if (o->templ[idx] == ' ') {
				keyval = 1;
				if (idx == strlen(val)) {
					/* ask for next arg to be included */
					return IFUSE_OPT_NEED_ANOTHER_ARG;
				} else if (strchr(o->templ, '%') != NULL) {
					val = &val[idx];
				}
			}

			if (o->val == FUSE_OPT_KEY_DISCARD)
				return (IFUSE_OPT_DISCARD);

			if (o->val == FUSE_OPT_KEY_KEEP)
				return (IFUSE_OPT_KEEP);

			if (FUSE_OPT_IS_OPT_KEY(o)) {
				if (f == NULL)
					return IFUSE_OPT_KEEP;

				return f(data, val, o->val, arg);
			} else if (data == NULL) {
				return (-1);
			} else if (strchr(o->templ, '%') == NULL) {
				*((int *)(data + o->off)) = o->val;
			} else if (strstr(o->templ, "%u") != NULL) {
				*((unsigned int*)(data + o->off)) = atoi(val);
			} else if (strstr(o->templ, "%lu") != NULL) {
				*((unsigned long*)(data + o->off)) =
				    strtoul(val, NULL, 0);
			} else if (strstr(o->templ, "%s") != NULL) {
				*((char **)(data + o->off)) = strdup(val);
			} else {
				/* TODO other parameterised templates */
			}

			return (IFUSE_OPT_DISCARD);
		}
	}

	if (f != NULL)
		return f(data, val, FUSE_OPT_KEY_OPT, arg);

	return (IFUSE_OPT_KEEP);
}

/*
 * this code is not very sexy but we are forced to follow
 * the fuse api.
 *
 * when f() returns 1 we need to keep the arg
 * when f() returns 0 we need to discard the arg
 */
int
fuse_opt_parse(struct fuse_args *args, void *data,
    const struct fuse_opt *opt, fuse_opt_proc_t f)
{
	struct fuse_args outargs;
	const char *arg, *ap;
	char *optlist, *tofree;
	int ret;
	int i;

	if (!args || !args->argc || !args->argv)
		return (0);

	bzero(&outargs, sizeof(outargs));
	fuse_opt_add_arg(&outargs, args->argv[0]);

	for (i = 1; i < args->argc; i++) {
		arg = args->argv[i];
		ret = 0;

		/* not - and not -- */
		if (arg[0] != '-') {
			if (f == NULL)
				ret = IFUSE_OPT_KEEP;
			else
				ret = f(data, arg, FUSE_OPT_KEY_NONOPT, &outargs);

			if (ret == IFUSE_OPT_KEEP)
				fuse_opt_add_arg(&outargs, arg);
			if (ret == -1)
				goto err;
		} else if (arg[1] == 'o') {
			if (arg[2])
				arg += 2;	/* -ofoo,bar */
			else {
				if (++i >= args->argc)
					goto err;

				arg = args->argv[i];
			}

			tofree = optlist = strdup(arg);
			if (optlist == NULL)
				goto err;

			while ((ap = strsep(&optlist, ",")) != NULL &&
			    ret != -1) {
				ret = parse_opt(opt, ap, data, f, &outargs);
				if (ret == IFUSE_OPT_KEEP) {
					fuse_opt_add_arg(&outargs, "-o");
					fuse_opt_add_arg(&outargs, ap);
				}
			}

			free(tofree);

			if (ret == -1)
				goto err;
		} else {
			ret = parse_opt(opt, arg, data, f, &outargs);

			if (ret == IFUSE_OPT_KEEP)
				fuse_opt_add_arg(&outargs, arg);
			else if (ret == IFUSE_OPT_NEED_ANOTHER_ARG) {
				/* arg needs a value */
				if (++i >= args->argc) {
					fprintf(stderr, "fuse: missing argument after %s\n", arg);
					goto err;
				}

				if (asprintf(&tofree, "%s%s", arg,
				    args->argv[i]) == -1)
					goto err;

				ret = parse_opt(opt, tofree, data, f, &outargs);
				if (ret == IFUSE_OPT_KEEP)
					fuse_opt_add_arg(&outargs, tofree);
				free(tofree);
			}

			if (ret == -1)
				goto err;
		}
	}
	ret = 0;

err:
	/* Update args */
	fuse_opt_free_args(args);
	args->allocated = outargs.allocated;
	args->argc = outargs.argc;
	args->argv = outargs.argv;
	if (ret != 0)
		ret = -1;

	return (ret);
}

int
fuse_opt_insert_arg(struct fuse_args *args, int p, const char *name)
{
	char **av;
	char *this_arg, *next_arg;
	int i;

	if (name == NULL)
		return (-1);

	if (!args->allocated && alloc_argv(args))
		return (-1);

	if (p < 0 || p > args->argc)
		return (-1);

	av = reallocarray(args->argv, args->argc + 2, sizeof(*av));
	if (av == NULL)
		return (-1);

	this_arg = strdup(name);
	if (this_arg == NULL) {
		free(av);
		return (-1);
	}

	args->argc++;
	args->argv = av;
	args->argv[args->argc] = NULL;
	for (i = p; i < args->argc; i++) {
		next_arg = args->argv[i];
		args->argv[i] = this_arg;
		this_arg = next_arg;
	}
	return (0);
}

void
fuse_opt_free_args(struct fuse_args *args)
{
	if (!args->allocated)
		return;

	free_argv(args->argv, args->argc);
	args->argv = 0;
	args->argc = 0;
	args->allocated = 0;
}

int
fuse_opt_match(const struct fuse_opt *opts, const char *opt)
{
	const struct fuse_opt *this_opt = opts;

	if (opt == NULL || opt[0] == '\0')
		return (0);

	while (this_opt->templ) {
		if (match_opt(this_opt->templ, opt))
			return (1);
		this_opt++;
	}

	return (0);
}
