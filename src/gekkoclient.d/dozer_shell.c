/**
 * @author: Victor Caballero (vicaba)
 */
#include "dozer_shell.h"

int dshell_parseShellCommand(char *command, int *argc, char ***argv);

int dshell_parseShellCommand(char *command, int *argc, char ***argv)
{

	regex_t regex;
	int reti;
	size_t nmatch;
	regmatch_t *pmatch;

	char auxbuff[512];

	// Command: show stock
	reti = regcomp(&regex, "^show stock$", REG_EXTENDED | REG_ICASE);

	if (reti) {
		return -2;
	}

	reti = regexec(&regex, command, 0, NULL, 0);

	if (!reti) {
		regfree(&regex);
		*argc = 0;
		return DSHELL_SHOW_STOCK;
	} else {
		regfree(&regex);
	}

	// Command: show me the money
	reti = regcomp(&regex, "^show me the money$", REG_EXTENDED | REG_ICASE);

	if (reti) {
		return -2;
	}

	reti = regexec(&regex, command, 0, NULL, 0);

	if (!reti) {
		regfree(&regex);
		*argc = 0;
		return DSHELL_SHOW_ME_THE_MONEY;
	} else {
		regfree(&regex);
	}

	// Command: show ibex
	reti = regcomp(&regex, "^show ibex$", REG_EXTENDED | REG_ICASE);

	if (reti) {
		return -2;
	}

	reti = regexec(&regex, command, 0, NULL, 0);

	if (!reti) {
		regfree(&regex);
		*argc = 0;
		return DSHELL_SHOW_IBEX;
	} else {
		regfree(&regex);
	}

	// Command: buy <ticker> <num>
	reti = regcomp(&regex, "^buy (.*) ([0-9]+)$", REG_EXTENDED | REG_ICASE);

	if (reti) {
		return -2;
	}

	nmatch = 3;
	pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * nmatch);

	reti = regexec(&regex, command, nmatch, pmatch, 0);

	if (!reti) {
		regfree(&regex);
		*argc = 2;
		*argv = (char **)malloc(sizeof(char*) * (*argc));

		strncpy(auxbuff, command + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
		auxbuff[(pmatch[1].rm_eo - pmatch[1].rm_so)] = '\0';
		(*argv)[0] = strdup(auxbuff);

		strncpy(auxbuff, command + pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
		auxbuff[(pmatch[2].rm_eo - pmatch[2].rm_so)] = '\0';
		(*argv)[1] = strdup(auxbuff);

		free(pmatch);
		return DSHELL_BUY_TICKER;
	} else {
		free(pmatch);
		regfree(&regex);
	}

	// Command: sell <ticker> <num>
	reti = regcomp(&regex, "^sell (.*) ([0-9]+)$", REG_EXTENDED | REG_ICASE);

	if (reti) {
		return -2;
	}

	nmatch = 3;
	pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * nmatch);

	reti = regexec(&regex, command, nmatch, pmatch, 0);

	if (!reti) {
		regfree(&regex);
		*argc = 2;
		*argv = (char **)malloc(sizeof(char*) * (*argc));

		strncpy(auxbuff, command + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
		auxbuff[(pmatch[1].rm_eo - pmatch[1].rm_so)] = '\0';
		(*argv)[0] = strdup(auxbuff);

		strncpy(auxbuff, command + pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
		auxbuff[(pmatch[2].rm_eo - pmatch[2].rm_so)] = '\0';
		(*argv)[1] = strdup(auxbuff);

		free(pmatch);
		return DSHELL_SELL_TICKER;
	} else {
		free(pmatch);
		regfree(&regex);
	}

	// Command: exit
	reti = regcomp(&regex, "^exit$", REG_EXTENDED | REG_ICASE);

	if (reti) {
		return -2;
	}

	reti = regexec(&regex, command, 0, NULL, 0);

	if (!reti) {
		regfree(&regex);
		*argc = 0;
		return DSHELL_EXIT;
	} else {
		regfree(&regex);
	}

	return -1;
}
