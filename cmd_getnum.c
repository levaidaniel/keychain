/*
 * Copyright (c) 2011 LEVAI Daniel
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *	* Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 *	* Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in the
 *	documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL LEVAI Daniel BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <stdarg.h>
#include <wctype.h>

#include "common.h"
#include "commands.h"


char get_line(xmlChar *, int, int *, wint_t *, int);


extern xmlNodePtr	keychain;
extern char		batchmode;

#ifndef _READLINE
extern EditLine		*e;
extern History		*eh;
extern HistEvent	eh_ev;
#endif


void
cmd_getnum(int idx, int space)
{
	xmlNodePtr	db_node = NULL;
	xmlChar		*key = NULL, *value = NULL, *value_part = NULL;

	int		newlines = 0, i = 0, pos = 0, erase_len = 0, line_len = 0, value_len = 0;
	int		*utfclen = NULL;
	int		c = -1;
	char		*wc_tmp = NULL;
	char		rc = 0;
	char		*rand_str = NULL;


	db_node = find_key(idx);
	if (db_node) {
		key = xmlGetProp(db_node, "name");

		printf("[%s]\n", key);	// print the key

#ifndef _READLINE
		// clear the prompt temporarily
		if (el_set(e, EL_PROMPT, el_prompt_null) != 0) {
			perror("el_set(EL_PROMPT)");
		}
		if (el_set(e, EL_UNBUFFERED, 1) != 0) {
			perror("el_set(EL_UNBUFFERED)");
			return;
		}
#else
		rl_prep_terminal(1);
#endif

		idx = 1;
		value = xmlGetProp(db_node, BAD_CAST "value");

		value_len = xmlStrlen(value);

		// count how many lines are in the string
		for (i=0; i < value_len; i++)
			if (value[i] == '\n')
				newlines++;


		utfclen = malloc(sizeof(int)); malloc_check(utfclen);

		while (c != '\0') {		// handle multiline values

			if (newlines)
				printf("[%d/%d] ", idx, newlines + 1);	// if multiline, prefix the line with a line number

			/* count the actual line's length */
			do {
				c = value[line_len++];	// this is the actual line length
			} while (c != '\n'  &&  c != '\0');
			--line_len;
			if (debug)
				printf("line_len = %d\n", line_len);

			value_part = realloc(value_part, line_len + 1); malloc_check(value_part);	// this 'part' is exactly one line from the 'value'
			value_part[0] = '\0';
			if (debug)
				printf("value_part(init):'%s'\n", value_part);

			if (space) {
				/* add the random characters' numbers to the line_len
				 * because it will be that much longer at the end */
				line_len += line_len + line_len * space + space;

				// print the first random character(s)
				rand_str = get_random_str(space, 0);
				if (!rand_str)
					return;

				strlcat(value_part, rand_str, line_len);

				free(rand_str); rand_str = NULL;
			}

			wc_tmp = xmlUTF8Strsub(value, pos, 1);
			while ((strcmp(wc_tmp, "\n") != 0)  &&  (strcmp(wc_tmp, "\0") != 0)) {
				xmlFree(wc_tmp); wc_tmp = NULL;
				wc_tmp = xmlUTF8Strsub(value, pos, 1);
				pos += strlen(wc_tmp);
				if ((strcmp(wc_tmp, "\n") == 0)  ||  (strcmp(wc_tmp, "\0") == 0))	// don't print the newlines and the NUL
					break;
				else {
					if (debug)
						printf("wc_tmp:'%s'\n", wc_tmp);
					strlcat(value_part, wc_tmp, line_len + 1);
					if (debug)
						printf("value_part:'%s'\n", value_part);

					if (space) {
						// print random character(s)
						rand_str = get_random_str(space, 0);
						if (!rand_str)
							return;

						strlcat(value_part, rand_str, line_len + 1);

						free(rand_str); rand_str = NULL;
					}
				}
			}
			printf("%s", value_part);
#ifdef _READLINE
			rl_redisplay();
#endif

			if (!batchmode) {
				// after printing a line, wait for user input
				rc = 0;
				while (	rc != 'q' &&
					rc != 10  &&  rc != 'f'  &&  rc != 'n'  &&  rc != 'j'  &&  rc != ' ' &&	// newline or 'f' ... display next line
					rc != 8   &&  rc != 'b'  &&  rc != 'p'  &&  rc != 'k'  &&		// backspace or 'b' ... display previous line
					(rc < '1'  ||  rc > '9')
					) {
#ifndef _READLINE
					el_getc(e, &rc);
#else
					rc = rl_read_key();
#endif
				}

				// erase (overwrite) the written value with spaces
				erase_len = line_len + (newlines ? digit_length(idx) + digit_length(newlines + 1) + 4 : 0);	// add the line number prefix too
				printf("\r");
				for (i=0; i < erase_len; i++)
					putchar(' ');

				printf("\r");

				switch (rc) {
					// forward
					case 10:
					case 'f':
					case 'n':
					case 'j':
					case ' ':
						idx++;
					break;
					// backward
					case 8:
					case 'b':
					case 'p':
					case 'k':
						if (idx - 1 > 0) {	// don't go back, if we are already on the first line
							idx--;		// 'idx' is the line number we want!

							pos = get_line(value, value_len, &pos, &c, idx);
						} else
							pos -= line_len + 1;	// rewind back to the current line's start, to display it again
					break;
					// quit
					case 'q':
						c = '\0';	// this is our exit condition
					break;
					// we got a number (this will be a line number)
					default:
						rc -= 48;		// 'idx' is the line number we want and 'rc' is the ascii version of the line number we got
						if ( rc <= newlines + 1 ) {
							idx = rc;
							pos = get_line(value, value_len, &pos, &c, idx);
						} else {
							pos -= line_len + 1;	// rewind back to the current line's start, to display it again
							c = value[pos];
						}
					break;
				}
			} else {
				// if we are in batch mode
				idx++;
				puts("");
			}

			line_len = 0;	// this is the actual line length
		}

		xmlFree(value); value = NULL;
		free(value_part); value_part = NULL;

#ifndef _READLINE
		// re-enable the default prompt
		if (el_set(e, EL_PROMPT, prompt_str) != 0) {
			perror("el_set(EL_PROMPT)");
		}
		el_set(e, EL_UNBUFFERED, 0);
#else
		rl_deprep_terminal();
#endif
	} else
		puts("invalid index!");
} /* cmd_getnum() */


char
get_line(xmlChar *value, int value_len, int *pos, wint_t *c, int idx)
{
	int	i = 1;	// this counts how many '\n' we've found so far

	// if we want the first line (which can't be identified like the rest
	// of the lines: by presuming a '\n' character before the line)
	// just set everything to the beginning, and return() from this function().
	if (idx == 1) {
		*pos = 0;
		*c = value[*pos];
		return(*pos);
	}

	for (*pos=0; *pos < value_len; (*pos)++) {	// search the entire string
		if (i < idx) {			// while newline count ('i') is smaller than the line number requested
			if (value[*pos] == '\n')	// we found a '\n'
				i++;
		} else
			break;
	}

	*c = value[*pos];	// set the current character ('c') to the position's character in 'value'

	return(*pos);
} /* get_line() */
