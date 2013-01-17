/*
 * Copyright (c) 2011, 2012, 2013 LEVAI Daniel
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


#include "common.h"
#include "commands.h"


extern xmlDocPtr	db;
extern xmlNodePtr	keychain;
extern char		dirty;


void
cmd_import(const char *e_line, command *commands)
{
	xmlDocPtr		db_new = NULL;
	xmlNodePtr		db_root = NULL, db_root_new = NULL, db_node_new = NULL, keychain_new = NULL;
	xmlParserInputBufferPtr	buf = NULL;
	xmlDtdPtr		dtd = NULL;
	xmlValidCtxt		valid_ctx;

	char			*cmd = NULL, append = 0;
	char			*line = NULL, *import_filename = NULL;


	line = strdup(e_line);

	cmd = strtok(line, " ");		/* get the command name */
	if (strcmp(cmd, "append") == 0)
		append = 1;

	import_filename = strtok(NULL, " ");	/* assign the command's parameter */
	if (!import_filename) {
		puts(commands->usage);

		free(line); line = NULL;
		return;
	}

	buf = xmlParserInputBufferCreateStatic(KC_DTD, sizeof(KC_DTD), XML_CHAR_ENCODING_NONE);
	if (!buf) {
		xmlGenericError(xmlGenericErrorContext, "Could not allocate buffer for DTD.\n");

		free(line); line = NULL;
		return;
	}

	dtd = xmlIOParseDTD(NULL, buf, XML_CHAR_ENCODING_NONE);
	if (!dtd) {
		xmlGenericError(xmlGenericErrorContext, "Could not parse kc DTD.\n");

		free(line); line = NULL;
		return;
	}

	if (getenv("KC_DEBUG"))
		db_new = xmlReadFile(import_filename, "UTF-8", XML_PARSE_NONET | XML_PARSE_RECOVER);
	else
		db_new = xmlReadFile(import_filename, "UTF-8", XML_PARSE_NONET | XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_RECOVER);

	if (!db_new) {
		xmlGenericError(xmlGenericErrorContext, "Failed to import from '%s'.\n", import_filename);

		free(line); line = NULL;
		return;
	}

	if (!xmlValidateDtd(&valid_ctx, db_new, dtd)) {
		printf("Not a valid kc XML file: '%s'.\n", import_filename);

		xmlFreeDoc(db_new);
		free(line); line = NULL;
		return;
	}


	if (append) {
		db_root = xmlDocGetRootElement(db);	/* the existing db root */
		db_root_new = xmlDocGetRootElement(db_new);

		if (db_root_new->children->next == NULL) {
			puts("Won't append from an empty database!");

			xmlFreeDoc(db_new);
			free(line); line = NULL;
			return;
		}

		/* extract the keychain from the document being appended */
		db_node_new = db_root_new->children->next;

		/* We would like to append every keychain that is in the source file,
		 * hence the loop. */
		while (db_node_new) {
			if (db_node_new->type == XML_ELEMENT_NODE) {	/* we only care about ELEMENT nodes */
				keychain_new = xmlCopyNode(db_node_new, 1);

				xmlAddChild(db_root, xmlNewText(BAD_CAST "\t"));
				xmlAddChild(db_root, keychain_new);
				xmlAddChild(db_root, xmlNewText(BAD_CAST "\n"));
			}

			db_node_new = db_node_new->next;
		}

		xmlFreeDoc(db_new);
	} else {
		db_root_new = xmlDocGetRootElement(db_new);
		if (db_root_new->children->next == NULL) {
			puts("Won't import an empty database!");

			xmlFreeDoc(db_new);
			free(line); line = NULL;
			return;
		}

		keychain = db_root_new->children->next;

		xmlFreeDoc(db);
		db = db_new;
	}

	dirty = 1;

	if (append)
		puts("Append OK!");
	else
		puts("Import OK!");

	free(line); line = NULL;
} /* cmd_import() */
