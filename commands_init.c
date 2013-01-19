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


extern unsigned char	readonly;


/* create a linked list for the commands.
 * the list contains the command's name and the function which handles it. */
void
commands_init(command **commands)
{
	command		*first = NULL;


	*commands = (command *)malloc(sizeof(command)); malloc_check(*commands);
	first = *commands;

	if (!readonly) {
		(*commands)->name = "append";
		(*commands)->usage = "append <filename>";
		(*commands)->help = "Append keychain(s) to the database from the XML file named 'filename'. It must be a properly formatted kc XML document.\nSee command 'xport' and 'import'.";
		(*commands)->fn = cmd_import;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;

		(*commands)->name = "cdel";
		(*commands)->usage = "cdel <keychain>";
		(*commands)->help = "Delete a keychain. 'keychain' can be the keychain's index number or name. Index number takes priority when addressing a keychain.";
		(*commands)->fn = cmd_cdel;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;

		(*commands)->name = "cnew";
		(*commands)->usage = "cnew [name]";
		(*commands)->help = "Create a new keychain. If 'name' is not given then prompt for one.";
		(*commands)->fn = cmd_cnew;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;

		(*commands)->name = "copy";
		(*commands)->usage = "copy <index> <keychain>";
		(*commands)->help = "Copy a key in the current keychain to another keychain. 'index' is the key's index to copy and 'keychain' is the destination keychain's index number or name. Index number takes priority when addressing a keychain.";
		(*commands)->fn = cmd_copy;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;
		(*commands)->name = "cp";
		(*commands)->usage = "cp <index> <keychain>";
		(*commands)->help = "Alias of 'copy'.";
		(*commands)->fn = cmd_copy;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;

		(*commands)->name = "cren";
		(*commands)->usage = "cren <keychain>";
		(*commands)->help = "Rename a keychain. 'keychain' can be the keychain's index number or name. Index number takes priority when addressing a keychain.";
		(*commands)->fn = cmd_cren;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;

		(*commands)->name = "del";
		(*commands)->usage = "del <index>";
		(*commands)->help = "Delete a key from the current keychain. 'index' is the key's index number in the current keychain.";
		(*commands)->fn = cmd_del;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;
		(*commands)->name = "rm";
		(*commands)->usage = "rm <index>";
		(*commands)->help = "Alias of 'del'.";
		(*commands)->fn = cmd_del;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;

		(*commands)->name = "edit";
		(*commands)->usage = "edit <index>";
		(*commands)->help = "Edit a key in the current keychain. 'index' is the key's index number in the current keychain.\n\nCharacter sequence rules in values apply to this command also.\nSee command 'new' for more information about this.";
		(*commands)->fn = cmd_edit;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;

		(*commands)->name = "import";
		(*commands)->usage = "import <filename>";
		(*commands)->help = "Import a database from the XML file named 'filename'. It must be a properly formatted kc XML document.\nSee command 'xport' and 'append'.\n\nNOTE: The current database will be overwritten if saved.";
		(*commands)->fn = cmd_import;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;

		(*commands)->name = "move";
		(*commands)->usage = "move <index> <keychain>";
		(*commands)->help = "Move a key in the current keychain to another keychain. 'index' is the key's index to move and 'keychain' is the destination keychain's index number or name. Index number takes priority when addressing a keychain.";
		(*commands)->fn = cmd_copy;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;
		(*commands)->name = "mv";
		(*commands)->usage = "mv <index> <keychain>";
		(*commands)->help = "Alias of 'move'.";
		(*commands)->fn = cmd_copy;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;

		(*commands)->name = "new";
		(*commands)->usage = "new [name]";
		(*commands)->help = "Create a new key with a value in the current keychain. Both key and value will be prompted for, except when 'name' is specified; then it will be used as the key's name.\n\nCharacter sequences can be used in values:\n\"\\n\" - create a new line, and make the result a multi-line value.\n\"\\r\", \"\\R\" - these will be replaced with 2 and 4 (respectively) random printable characters.\n\"\\a\", \"\\A\" - these will be replaced with 2 and 4 (respectively) random alpha-numeric characters.";
		(*commands)->fn = cmd_new;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;

		(*commands)->name = "passwd";
		(*commands)->usage = "passwd";
		(*commands)->help = "Change the current database's password. All changes will be written immediately.";
		(*commands)->fn = cmd_passwd;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;

		(*commands)->name = "write";
		(*commands)->usage = "write";
		(*commands)->help = "Save the current database.";
		(*commands)->fn = cmd_write;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;
		(*commands)->name = "save";
		(*commands)->usage = "save";
		(*commands)->help = "Alias of 'write'.";
		(*commands)->fn = cmd_write;
		(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
		(*commands) = (*commands)->next;
	}

	(*commands)->name = "c";
	(*commands)->usage = "c <keychain>";
	(*commands)->help = "Change the current keychain. 'keychain' can be the keychain's index number or name. Index number takes priority when addressing a keychain.";
	(*commands)->fn = cmd_c;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "c/";
	(*commands)->usage = "c/<pattern>";
	(*commands)->help = "Search for 'pattern' regular expression in keychain names in the current database.";
	(*commands)->fn = cmd_searchre;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "clear";
	(*commands)->usage = "clear [count]";
	(*commands)->help = "Emulate a screen clearing. Scrolls 50 lines by default, which can be multiplied by 'count' times if specified.";
	(*commands)->fn = cmd_clear;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "clist";
	(*commands)->usage = "clist";
	(*commands)->help = "List keychains in the current database. Every keychain gets prefixed by its index number.";
	(*commands)->fn = cmd_clist;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;
	(*commands)->name = "cls";
	(*commands)->usage = "cls";
	(*commands)->help = "Alias of 'clist'.";
	(*commands)->fn = cmd_clist;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "csearch";
	(*commands)->usage = "csearch <string>";
	(*commands)->help = "Search for 'string' in keychain names in the current database.";
	(*commands)->fn = cmd_search;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "dump";
	(*commands)->usage = "dump <filename> [keychain]";
	(*commands)->help = "Dump the current database to the XML file named 'filename'. When specifying a keychain, dump only that keychain to the XML file. 'keychain' can be the keychain's index number or name. Index number takes priority when addressing a keychain.\nSee command 'xport'\n\nNOTE: the created XML file will be plain text.";
	(*commands)->fn = cmd_export;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "help";
	(*commands)->usage = "help [command]";
	(*commands)->help = "Print application help or describe a 'command'.";
	(*commands)->fn = cmd_help;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "info";
	(*commands)->usage = "info <index>";
	(*commands)->help = "Print information about a key in the current keychain. 'index' is the key's index number in the current keychain.";
	(*commands)->fn = cmd_info;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "list";
	(*commands)->usage = "list [keychain]";
	(*commands)->help = "List the keys in the current keychain or if specified, in the keychain named 'keychain'. Every key gets prefixed by its index number. 'keychain' can be the keychain's index number or name. Index number takes priority when addressing a keychain.";
	(*commands)->fn = cmd_list;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;
	(*commands)->name = "ls";
	(*commands)->usage = "ls [keychain]";
	(*commands)->help = "Alias of 'list'.";
	(*commands)->fn = cmd_list;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "quit";
	(*commands)->usage = "quit";
	(*commands)->help = "Quit the program. If the database has been modified, then ask if it should be saved.";
	(*commands)->fn = cmd_quit;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;
	(*commands)->name = "exit";
	(*commands)->usage = "exit";
	(*commands)->help = "Alias of 'quit'.";
	(*commands)->fn = cmd_quit;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "random";
	(*commands)->usage = "random [length]";
	(*commands)->help = "Print a random string with 'length' length. The default 'length' is 8.";
	(*commands)->fn = cmd_random;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "search";
	(*commands)->usage = "search <string>";
	(*commands)->help = "Search for 'string' in key names in the current keychain. If the search command is prefixed by a '*' (eg.: *search) then search in every keychain in the current database.";
	(*commands)->fn = cmd_search;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;
	(*commands)->name = "s";
	(*commands)->usage = "s <string>";
	(*commands)->help = "Alias of 'search'.";
	(*commands)->fn = cmd_search;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "*search";
	(*commands)->usage = "*search <string>";
	(*commands)->help = "Search for 'string' in key names in every keychain.";
	(*commands)->fn = cmd_search;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;
	(*commands)->name = "*s";
	(*commands)->usage = "*s <string>";
	(*commands)->help = "Alias of '*search'.";
	(*commands)->fn = cmd_search;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "status";
	(*commands)->usage = "status";
	(*commands)->help = "Display information about the current database.";
	(*commands)->fn = cmd_status;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "version";
	(*commands)->usage = "version";
	(*commands)->help = "Display the program version.";
	(*commands)->fn = cmd_version;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "xport";
	(*commands)->usage = "xport <filename> [keychain]";
	(*commands)->help = "Export the current database to the encrypted file named 'filename'. When specifying a keychain, export only that keychain. 'keychain' can be the keychain's index number or name. Index number takes priority when addressing a keychain.\nSee command 'dump', 'import' and 'append'";
	(*commands)->fn = cmd_export;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "/";
	(*commands)->usage = "/<pattern>";
	(*commands)->help = "Search for 'pattern' regular expression in key names in the current keychain. If the / command is prefixed by a '*' (eg.: */) then search in every keychain in the current database.";
	(*commands)->fn = cmd_searchre;
	(*commands)->next = (command *)malloc(sizeof(command)); malloc_check((*commands)->next);
	(*commands) = (*commands)->next;

	(*commands)->name = "*/";
	(*commands)->usage = "*/<pattern>";
	(*commands)->help = "Search for 'pattern' regular expression in key names in every keychain.";
	(*commands)->fn = cmd_searchre;

	(*commands)->next = NULL;


	*commands = first;
} /* commands_init() */
