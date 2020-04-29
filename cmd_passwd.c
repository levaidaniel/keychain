/*
 * Copyright (c) 2011-2020 LEVAI Daniel
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
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "common.h"
#include "commands.h"
#include "ssha.h"
#ifdef _HAVE_YUBIKEY
#include "ykchalresp.h"
#endif


extern db_parameters	db_params;
extern BIO		*bio_chain;


void
cmd_passwd(const char *e_line, command *commands)
{
	int	c = 0, largc = 0;
	char	**largv = NULL;
	char	*line = NULL;
	char	*ssha_type = NULL, *ssha_comment = NULL;

	unsigned long int	ykchalresp = 0;
	char			*inv = NULL;

	db_parameters	db_params_tmp;


	/* initial db_params for the temporary database */
	db_params_tmp.ssha_type[0] = '\0';
	db_params_tmp.ssha_comment[0] = '\0';
	db_params_tmp.ykdev = 0;
	db_params_tmp.ykslot = 0;
	db_params_tmp.pass = NULL;
	db_params_tmp.pass_len = 0;
	db_params_tmp.pass_filename = NULL;
	db_params_tmp.kdf = NULL;
	db_params_tmp.cipher = NULL;
	db_params_tmp.cipher_mode = NULL;
	db_params_tmp.kdf = strdup(db_params.kdf);
	if (!db_params_tmp.kdf) {
		perror("ERROR: Could not duplicate the KDF");
		goto exiting;
	}
	db_params_tmp.cipher = strdup(db_params.cipher);
	if (!db_params_tmp.cipher) {
		perror("ERROR: Could not duplicate the cipher");
		goto exiting;
	}
	db_params_tmp.cipher_mode = strdup(db_params.cipher_mode);
	if (!db_params_tmp.cipher_mode) {
		perror("ERROR: Could not duplicate the cipher mode");
		goto exiting;
	}


	/* Parse the arguments */
	line = strdup(e_line);
	if (!line) {
		perror("ERROR: Could not duplicate the command line");
		goto exiting;
	}
	larg(line, &largv, &largc);
	free(line); line = NULL;

	optind = 0;
	while ((c = getopt(largc, largv, "A:P:e:m:y:")) != -1)
		switch (c) {
			case 'A':
				/* in case this parameter is being parsed multiple times */
				free(ssha_type); ssha_type = NULL;
				free(ssha_comment); ssha_comment = NULL;

				ssha_type = strndup(strsep(&optarg, ","), 11);
				if (ssha_type == NULL  ||  !strlen(ssha_type)) {
					dprintf(STDERR_FILENO, "ERROR: SSH key type is empty!\n");
					goto exiting;
				}
				if (	strncmp(ssha_type, "ssh-rsa", 7) != 0  &&
					strncmp(ssha_type, "ssh-ed25519", 11) != 0
				) {
					dprintf(STDERR_FILENO, "ERROR: SSH key type is unsupported: '%s'\n", ssha_type);
					goto exiting;
				}

				ssha_comment = strndup(optarg, 512);
				if (ssha_comment == NULL  ||  !strlen(ssha_comment)) {
					dprintf(STDERR_FILENO, "ERROR: SSH key comment is empty!\n");
					goto exiting;
				}

				if (strlcpy(db_params_tmp.ssha_type, ssha_type, sizeof(db_params_tmp.ssha_type)) >= sizeof(db_params_tmp.ssha_type)) {
					dprintf(STDERR_FILENO, "ERROR: Error while getting SSH key type.\n");
					goto exiting;
				}

				if (strlcpy(db_params_tmp.ssha_comment, ssha_comment, sizeof(db_params_tmp.ssha_comment)) >= sizeof(db_params_tmp.ssha_comment)) {
					dprintf(STDERR_FILENO, "ERROR: Error while getting SSH key type.\n");
					goto exiting;
				}

				printf("Using (%s) %s identity for encryption\n", db_params_tmp.ssha_type, db_params_tmp.ssha_comment);
			break;
			case 'P':
				free(db_params_tmp.kdf); db_params_tmp.kdf = NULL;
				db_params_tmp.kdf = strdup(optarg);
				if (!db_params_tmp.kdf) {
					perror("ERROR: Could not duplicate the KDF");
					goto exiting;
				}
			break;
			case 'e':
				free(db_params_tmp.cipher); db_params_tmp.cipher = NULL;
				db_params_tmp.cipher = strdup(optarg);
				if (!db_params_tmp.cipher) {
					perror("ERROR: Could not duplicate the cipher");
					goto exiting;
				}
			break;
			case 'm':
				free(db_params_tmp.cipher_mode); db_params_tmp.cipher_mode = NULL;
				db_params_tmp.cipher_mode = strdup(optarg);
				if (!db_params_tmp.cipher_mode) {
					perror("ERROR: Could not duplicate the cipher mode");
					goto exiting;
				}
			break;
			case 'y':
				ykchalresp = strtoul(optarg, &inv, 10);
				if (inv[0] == '\0') {
					if (ykchalresp <= 0  ||  optarg[0] == '-') {
						dprintf(STDERR_FILENO, "ERROR: YubiKey slot/device parameter is zero or negative.\n");
						quit(EXIT_FAILURE);
					} else if (ykchalresp > 29) {
						dprintf(STDERR_FILENO, "ERROR: YubiKey slot/device parameter is too high.\n");
						quit(EXIT_FAILURE);
					} else if (ykchalresp < 10) {
						db_params_tmp.ykslot = ykchalresp;
						db_params_tmp.ykdev = 0;
					} else {
						db_params_tmp.ykslot = ykchalresp / 10 ;
						db_params_tmp.ykdev = ykchalresp - (ykchalresp / 10 * 10);
					}
				} else {
					dprintf(STDERR_FILENO, "ERROR: Unable to convert the YubiKey slot/device parameter.\n");
					quit(EXIT_FAILURE);
				}

				if (db_params_tmp.ykslot > 2  ||  db_params_tmp.ykslot < 1  ) {
					dprintf(STDERR_FILENO, "ERROR: YubiKey slot number is not 1 or 2.\n");
					quit(EXIT_FAILURE);
				}

				printf("Using YubiKey slot #%d on device #%d\n", db_params_tmp.ykslot, db_params_tmp.ykdev);
			break;
			default:
				puts(commands->usage);
				goto exiting;
			break;
		}


	if (kc_crypt_iv_salt(&db_params_tmp) != 1) {
		dprintf(STDERR_FILENO, "ERROR: Could not generate IV and/or salt!\n");
		goto exiting;
	}

	if (strlen(db_params_tmp.ssha_type)) {
		/* use SSH agent to generate the password */
		if (!kc_ssha_get_password(&db_params_tmp))
			goto exiting;
	} else {
		/* ask for the new password */
		if (kc_password_read(&db_params_tmp, 1) != 1)
			goto exiting;
	}

	/* Setup cipher mode and turn on encrypting */
	if (	kc_crypt_key(&db_params_tmp) != 1  ||
		kc_crypt_setup(bio_chain, 1, &db_params_tmp) != 1
	) {
		dprintf(STDERR_FILENO, "ERROR: Could not setup encrypting!\n");
		goto exiting;
	}


	/* store the new IV/salt/key in our working copy of 'db_params' */
	if (strlcpy((char *)db_params.iv, (const char*)db_params_tmp.iv, sizeof(db_params.iv)) >= sizeof(db_params.iv)) {
		dprintf(STDERR_FILENO, "ERROR: Could not copy IV!\n");
		goto exiting;
	}
	if (strlcpy((char *)db_params.salt, (const char*)db_params_tmp.salt, sizeof(db_params.salt)) >= sizeof(db_params.salt)) {
		dprintf(STDERR_FILENO, "ERROR: Could not copy salt!\n");
		goto exiting;
	}
	memcpy(db_params.key, db_params_tmp.key, KEY_LEN);
	if (memcmp(db_params.key, db_params_tmp.key, KEY_LEN) != 0) {
		dprintf(STDERR_FILENO, "ERROR: Could not copy encryption key!");

		goto exiting;
	}

	free(db_params.kdf); db_params.kdf = NULL;
	db_params.kdf = strdup(db_params_tmp.kdf);
	if (!db_params.kdf) {
		perror("ERROR: Could not save the KDF");
		goto exiting;
	}
	free(db_params.cipher); db_params.cipher = NULL;
	db_params.cipher = strdup(db_params_tmp.cipher);
	if (!db_params.cipher) {
		perror("ERROR: Could not save the cipher");
		goto exiting;
	}
	free(db_params.cipher_mode); db_params.cipher_mode = NULL;
	db_params.cipher_mode = strdup(db_params_tmp.cipher_mode);
	if (!db_params.cipher_mode) {
		perror("ERROR: Could not save the cipher mode");
		goto exiting;
	}
	if (strlcpy((char *)db_params.ssha_type, (const char*)db_params_tmp.ssha_type, sizeof(db_params.ssha_type)) >= sizeof(db_params.ssha_type)) {
		dprintf(STDERR_FILENO, "ERROR: Could not save SSH key type!\n");
		goto exiting;
	}
	if (strlcpy((char *)db_params.ssha_comment, (const char*)db_params_tmp.ssha_comment, sizeof(db_params.ssha_comment)) >= sizeof(db_params.ssha_comment)) {
		dprintf(STDERR_FILENO, "ERROR: Could not save SSH key comment!\n");
		goto exiting;
	}


	cmd_write(NULL, NULL);
	puts("Password change OK");

exiting:
	for (c = 0; c < largc; c++) {
		free(largv[c]); largv[c] = NULL;
	}
	free(largv); largv = NULL;

	free(ssha_type); ssha_type = NULL;
	free(ssha_comment); ssha_comment = NULL;

	memset(db_params_tmp.key, '\0', KEY_LEN);
	if (db_params_tmp.pass) {
		memset(db_params_tmp.pass, '\0', db_params_tmp.pass_len);
		free(db_params_tmp.pass); db_params_tmp.pass = NULL;
	}
	free(db_params_tmp.kdf); db_params_tmp.kdf = NULL;
	free(db_params_tmp.cipher); db_params_tmp.cipher = NULL;
	free(db_params_tmp.cipher_mode); db_params_tmp.cipher_mode = NULL;
} /* cmd_passwd() */
