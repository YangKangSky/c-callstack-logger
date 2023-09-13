/*
  A hacky replacement for backtrace_symbols in glibc
  backtrace_symbols in glibc looks up symbols using dladdr which is limited in
  the symbols that it sees. libbacktracesymbols opens the executable and shared
  libraries using libbfd and will look up backtrace information using the symbol
  table and the dwarf line information.
  It may make more sense for this program to use libelf instead of libbfd.
  However, I have not investigated that yet.
  Derived from addr2line.c from GNU Binutils by Jeff Muizelaar
  Copyright 2007 Jeff Muizelaar
*/
/* addr2line.c -- convert addresses to line number and function name
   Copyright 1997, 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
   Contributed by Ulrich Lauther <Ulrich.Lauther@mchp.siemens.de>
   This file was part of GNU Binutils.
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA.  */

#define fatal(a, b) exit(1)
#define bfd_fatal(a) exit(1)
#define bfd_nonfatal(a) exit(1)
#define list_matching_formats(a) exit(1)
/* 2 characters for each byte, plus 1 each for 0, x, and NULL */
#define PTRSTR_LEN (sizeof(void *) * 2 + 3)
#define true 1
#define false 0
#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <bfd.h>
//#include <libiberty.h>
#include <dlfcn.h>
#include <link.h>

#include "backtrace-symbols.h"


#if 0
void (*dbfd_init)(void);
bfd_vma (*dbfd_scan_vma)(const char *string, const char **end, int base);
bfd* (*dbfd_openr)(const char *filename, const char *target);
bfd_boolean (*dbfd_check_format)(bfd *abfd, bfd_format format);
bfd_boolean (*dbfd_check_format_matches)(bfd *abfd, bfd_format format, char ***matching);
bfd_boolean (*dbfd_close)(bfd *abfd);
bfd_boolean (*dbfd_map_over_sections)(bfd *abfd, void (*func)(bfd *abfd, asection *sect, void *obj),
		void *obj);
#define bfd_init dbfd_init
static void load_funcs(void)
{
	void * handle = dlopen("libbfd.so", RTLD_NOW);
	dbfd_init = dlsym(handle, "bfd_init");
	dbfd_scan_vma = dlsym(handle, "bfd_scan_vma");
	dbfd_openr = dlsym(handle, "bfd_openr");
	dbfd_check_format = dlsym(handle, "bfd_check_format");
	dbfd_check_format_matches = dlsym(handle, "bfd_check_format_matches");
	dbfd_close = dlsym(handle, "bfd_close");
	dbfd_map_over_sections = dlsym(handle, "bfd_map_over_sections");
}
#endif
static asymbol **syms;		/* Symbol table.  */
/* 150 isn't special; it's just an arbitrary non-ASCII char value.  */
#define OPTION_DEMANGLER	(150)
static void slurp_symtab(bfd * abfd);
static void find_address_in_section(bfd *abfd, asection *section, void *data);
/* Read in the symbol table.  */
/* Read in the symbol table.  */

static void
slurp_symtab (bfd *abfd)
{
  long storage;
  long symcount;
  bfd_boolean dynamic = FALSE;

  if ((bfd_get_file_flags (abfd) & HAS_SYMS) == 0)
    return;

  storage = bfd_get_symtab_upper_bound (abfd);
  if (storage == 0)
    {
      storage = bfd_get_dynamic_symtab_upper_bound (abfd);
      dynamic = TRUE;
    }
  if (storage < 0)
    bfd_fatal (bfd_get_filename (abfd));

  syms = (asymbol **) xmalloc (storage);
  if (dynamic)
    symcount = bfd_canonicalize_dynamic_symtab (abfd, syms);
  else
    symcount = bfd_canonicalize_symtab (abfd, syms);
  if (symcount < 0)
    bfd_fatal (bfd_get_filename (abfd));

  /* If there are no symbols left after canonicalization and
     we have not tried the dynamic symbols then give them a go.  */
  if (symcount == 0
      && ! dynamic
      && (storage = bfd_get_dynamic_symtab_upper_bound (abfd)) > 0)
    {
      free (syms);
      syms = xmalloc (storage);
      symcount = bfd_canonicalize_dynamic_symtab (abfd, syms);
    }

  /* PR 17512: file: 2a1d3b5b.
     Do not pretend that we have some symbols when we don't.  */
  if (symcount <= 0)
    {
      free (syms);
      syms = NULL;
    }
}

/* These global variables are used to pass information between
   translate_addresses and find_address_in_section.  */

static bfd_vma pc;
static const char *filename;
static const char *functionname;
static unsigned int line;
static unsigned int discriminator;
static bfd_boolean found;

/* Look for an address in a section.  This is called via
   bfd_map_over_sections.  */

static void
find_address_in_section (bfd *abfd, asection *section,
			 void *data ATTRIBUTE_UNUSED)
{
  bfd_vma vma;
  bfd_size_type size;

  if (found)
    return;

  if ((bfd_section_flags (section) & SEC_ALLOC) == 0)
    return;

  vma = bfd_section_vma (section);
  if (pc < vma)
    return;

  size = bfd_section_size (section);
  if (pc >= vma + size)
    return;

  found = bfd_find_nearest_line_discriminator (abfd, section, syms, pc - vma,
                                               &filename, &functionname,
                                               &line, &discriminator);
}

/* Look for an offset in a section.  This is directly called.  */

static void
find_offset_in_section (bfd *abfd, asection *section)
{
  bfd_size_type size;

  if (found)
    return;

  if ((bfd_section_flags (section) & SEC_ALLOC) == 0)
    return;

  size = bfd_section_size (section);
  if (pc >= size)
    return;

  found = bfd_find_nearest_line_discriminator (abfd, section, syms, pc,
                                               &filename, &functionname,
                                               &line, &discriminator);
}

/* Read hexadecimal addresses from stdin, translate into
   file_name:line_number and optionally function name.  */

static void
translate_addresses (bfd *abfd, asection *section)
{
  int read_stdin = (naddr == 0);

  for (;;)
    {
      if (read_stdin)
	{
	  char addr_hex[100];

	  if (fgets (addr_hex, sizeof addr_hex, stdin) == NULL)
	    break;
	  pc = bfd_scan_vma (addr_hex, NULL, 16);
	}
      else
	{
	  if (naddr <= 0)
	    break;
	  --naddr;
	  pc = bfd_scan_vma (*addr++, NULL, 16);
	}

      if (bfd_get_flavour (abfd) == bfd_target_elf_flavour)
	{
	  const struct elf_backend_data *bed = get_elf_backend_data (abfd);
	  bfd_vma sign = (bfd_vma) 1 << (bed->s->arch_size - 1);

	  pc &= (sign << 1) - 1;
	  if (bed->sign_extend_vma)
	    pc = (pc ^ sign) - sign;
	}

      if (with_addresses)
        {
          printf ("0x");
          bfd_printf_vma (abfd, pc);

          if (pretty_print)
            printf (": ");
          else
            printf ("\n");
        }

      found = FALSE;
      if (section)
	find_offset_in_section (abfd, section);
      else
	bfd_map_over_sections (abfd, find_address_in_section, NULL);

      if (! found)
	{
	  if (with_functions)
	    {
	      if (pretty_print)
		printf ("?? ");
	      else
		printf ("??\n");
	    }
	  printf ("??:0\n");
	}
      else
	{
	  while (1)
            {
              if (with_functions)
                {
                  const char *name;
                  char *alloc = NULL;

                  name = functionname;
                  if (name == NULL || *name == '\0')
                    name = "??";
                  else if (do_demangle)
                    {
                      alloc = bfd_demangle (abfd, name, demangle_flags);
                      if (alloc != NULL)
                        name = alloc;
                    }

                  printf ("%s", name);
                  if (pretty_print)
		    /* Note for translators:  This printf is used to join the
		       function name just printed above to the line number/
		       file name pair that is about to be printed below.  Eg:

		         foo at 123:bar.c  */
                    printf (_(" at "));
                  else
                    printf ("\n");

                  if (alloc != NULL)
                    free (alloc);
                }

              if (base_names && filename != NULL)
                {
                  char *h;

                  h = strrchr (filename, '/');
                  if (h != NULL)
                    filename = h + 1;
                }

              printf ("%s:", filename ? filename : "??");
	      if (line != 0)
                {
                  if (discriminator != 0)
                    printf ("%u (discriminator %u)\n", line, discriminator);
                  else
                    printf ("%u\n", line);
                }
	      else
		printf ("?\n");
              if (!unwind_inlines)
                found = FALSE;
              else
                found = bfd_find_inliner_info (abfd, &filename, &functionname,
					       &line);
              if (! found)
                break;
              if (pretty_print)
		/* Note for translators: This printf is used to join the
		   line number/file name pair that has just been printed with
		   the line number/file name pair that is going to be printed
		   by the next iteration of the while loop.  Eg:

		     123:bar.c (inlined by) 456:main.c  */
                printf (_(" (inlined by) "));
            }
	}

      /* fflush() is essential for using this command as a server
         child process that reads addresses from a pipe and responds
         with line number information, processing one address at a
         time.  */
      fflush (stdout);
    }
}

/* Process a file.  Returns an exit value for main().  */

static int
process_file (const char *file_name, const char *section_name,
	      const char *target)
{
  bfd *abfd;
  asection *section;
  char **matching;

  if (get_file_size (file_name) < 1)
    return 1;

  abfd = bfd_openr (file_name, target);
  if (abfd == NULL)
    bfd_fatal (file_name);

  /* Decompress sections.  */
  abfd->flags |= BFD_DECOMPRESS;

  if (bfd_check_format (abfd, bfd_archive))
    fatal (_("%s: cannot get addresses from archive"), file_name);

  if (! bfd_check_format_matches (abfd, bfd_object, &matching))
    {
      bfd_nonfatal (bfd_get_filename (abfd));
      if (bfd_get_error () == bfd_error_file_ambiguously_recognized)
	{
	  list_matching_formats (matching);
	  free (matching);
	}
      xexit (1);
    }

  if (section_name != NULL)
    {
      section = bfd_get_section_by_name (abfd, section_name);
      if (section == NULL)
	fatal (_("%s: cannot find section %s"), file_name, section_name);
    }
  else
    section = NULL;

  slurp_symtab (abfd);

  translate_addresses (abfd, section);

  if (syms != NULL)
    {
      free (syms);
      syms = NULL;
    }

  bfd_close (abfd);

  return 0;
}
char **backtrace_symbols_in(void *const *buffer, int size)
{
	int stack_depth = size - 1;
	int x,y;
	/* discard calling function */
	int total = 0;
	char ***locations;
	char **final;
	char *f_strings;

	locations = malloc(sizeof(char**) * (stack_depth+1));
	bfd_init();
	for(x=stack_depth, y=0; x>=0; x--, y++){
		struct file_match match = { .address = buffer[x] };
		char **ret_buf;
		bfd_vma addr;
		dl_iterate_phdr(find_matching_file, &match);
		addr = buffer[x] - match.base;
		if (match.file && strlen(match.file))
			ret_buf = process_file(match.file, &addr, 1);
		else
			ret_buf = process_file("/proc/self/exe", &addr, 1);
		locations[x] = ret_buf;
		total += strlen(ret_buf[0]) + 1;
	}
	/* allocate the array of char* we are going to return and extra space for
	 * all of the strings */
	final = malloc(total + (stack_depth + 1) * sizeof(char*));
	/* get a pointer to the extra space */
	f_strings = (char*)(final + stack_depth + 1);
	/* fill in all of strings and pointers */
	for(x=stack_depth; x>=0; x--){
		strcpy(f_strings, locations[x][0]);
		free(locations[x]);
		final[x] = f_strings;
		f_strings += strlen(f_strings) + 1;
	}
	free(locations);
	return final;
}
void 
backtrace_symbols_fd_in(void *const *buffer, int size, int fd)
{
        
        char **strings;
        strings = backtrace_symbols_in(buffer, size);
        if (strings == NULL) {
			perror("backtrace_symbols_in");
			exit(EXIT_FAILURE);
        }
		
		int j;
        for (j = 0; j < size; j++)
			printf("---%s\n", strings[j]);
        free(strings);

		return strings;
}





/*  main.c:12	func4() */
void parseBacktraceSymbol(const char *symbol, BacktraceEntry *entry)
{
	BacktraceEntry *info = entry; //(BacktraceInfo*)malloc(sizeof(BacktraceInfo));
	if (info == NULL)
	{
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	// 使用 sscanf 解析字符串
	// 注意: 这个解析能力取决于你的backtrace_symbols的输出格式是否总是遵循 "filename:line function" 这个格式。
	// 如果格式有所不同，你可能需要调整这个解析格式字符串
	int parsed = sscanf(symbol, "%[^:]:%d %s]", info->filename, &(info->line_number), info->function_name);

	if (parsed != 3)
	{
		fprintf(stderr, "Parsing failed\n");
		free(info);
	}

	char *openParenPos = strchr(info->function_name, '(');
	if (openParenPos != NULL)
	{
		*openParenPos = '\0';
	}

	// printf("File: %s, Line: %d, Function: %s\n", entry->filename, entry->line_number, entry->function_name);
}

void SymbolReslove(const char* symbol, char* filename, int* line_number, char* function_name) {
    BacktraceEntry entry;
    parseBacktraceSymbol(symbol, &entry);

    strncpy(filename, entry.filename, sizeof(entry.filename) - 1);
    filename[sizeof(entry.filename) - 1] = '\0';

    *line_number = entry.line_number;

    strncpy(function_name, entry.function_name, sizeof(entry.function_name) - 1);
    function_name[sizeof(entry.function_name) - 1] = '\0';
}