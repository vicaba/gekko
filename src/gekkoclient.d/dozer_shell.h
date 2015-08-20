/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __DOZER_SHELL_H__
#define __DOZER_SHELL_H__

#include <regex.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define DSHELL_SHOW_STOCK	1
#define DSHELL_SHOW_ME_THE_MONEY	2
#define DSHELL_SHOW_IBEX	3
#define DSHELL_BUY_TICKER	4
#define DSHELL_SELL_TICKER	5
#define DSHELL_EXIT	6

#define DSHELL_CANOT_START_SHELL	-2

int dshell_parseShellCommand(char *command, int *argc, char ***argv);

#endif
