/*
 * Copyright (c) 2017 Sergey Bronnikov
 * Copyright (c) 1982, 1986, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by the University of
 *  California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/lock.h>
#include <sys/malloc.h>
#include <sys/gcov.h>
#include <sys/kernel.h>

/*

FIXME: see http://bxr.su/DragonFly/sys/kern/kern_linker.c

static void
linker_file_register_profile(linker_file_t lf)
{
  ctor_t *start, *stop;

  if (linker_file_lookup_set(lf, "ctors_set", &start, &stop, NULL) != 0)
	return;

  gcov_register_ctors(lf, start, stop);
}

static void
linker_file_unregister_profile(linker_file_t lf)
{
  gcov_unregister_ctors(lf);
}
*/

struct mutex gcov_mutex = MUTEX_INITIALIZER(IPL_NONE);
struct gcov_context
{
  LIST_ENTRY(gcov_context) gcov_link;
  struct linker_file *lf;
  unsigned long count;
  struct bb** bb;
};

static LIST_HEAD(, gcov_context) gcov_list = LIST_HEAD_INITIALIZER(&gcov_list);
static struct gcov_context *current_context = NULL;

/* Structure emitted by --profile-arcs  */
struct bb
{
  long zero_word;
  const char *filename;
  void *counts;
  long ncounts;
  struct bb *next;
};

static struct bb *bb_head = NULL;

void
__bb_fork_func(void)
{
}

void
__bb_init_func(struct bb *blocks)
{
  if (blocks->zero_word)
	return;
  printf("bb: Adding %s\n", blocks->filename);
  if (current_context)
	current_context->bb[current_context->count++] = blocks;
  blocks->zero_word = 1;
  blocks->next = bb_head;
  bb_head = blocks;
}

void
gcov_register_ctors(struct linker_file *lf, ctor_t *start, ctor_t *stop)
{
  int bbcount;
  struct gcov_context *context;
  ctor_t* ctor;

  bbcount = stop - start;

/*
  FreeBSD:
  MALLOC(context, struct gcov_context *,
		 sizeof(struct gcov_context) + bbcount * sizeof(struct bb *),
		 M_GCOV, M_WAITOK);
*/
  malloc(struct gcov_context *, sizeof(struct gcov_context) + bbcount * sizeof(struct bb *), M_GCOV | M_WAITOK)
  mtx_enter(&gcov_mutex);
  KASSERT(current_context == NULL);
  current_context = context;
  current_context->lf = lf;
  current_context->count = 0;
  current_context->bb = (struct bb **)(current_context + 1);
  LIST_INSERT_HEAD(&gcov_list, current_context, gcov_link);

  for (ctor = start; ctor < stop; ctor++)
	if (*ctor != NULL)
	  (*ctor)();

  current_context = NULL;
  mtx_leave(&gcov_mutex);
}

void
gcov_unregister_ctors(struct linker_file *lf)
{
  struct gcov_context *context;
  mtx_enter(&gcov_mutex);
  LIST_FOREACH(context, &gcov_list, gcov_link) {
	if (context->lf == lf) {
	  struct bb *prev = NULL, *bb;
	  int i;

	  for (bb = bb_head; bb ; bb = bb->next) {
		for (i = 0; i < context->count; i++) {
		  if (context->bb[i] == bb) {
			printf("bb: Deleting %s\n", bb->filename);
			if (prev)
			  prev->next = bb->next;
			else
			  bb_head = bb->next;
			break;
		  }
		}
		if (i == context->count)
		  prev = bb;
	  }
	  LIST_REMOVE(context, gcov_link);
	  mtx_leave(&gcov_mutex);
	  return;
	}
  }

  mtx_leave(&gcov_mutex);
}
