/**
 * @author: Victor Caballero (vicaba)
 */
#include "regex.h"

void regex_getRegexString(char *buffero, char *bufferi, regmatch_t *pmatch, int numMatch);

void regex_getRegexString(char *buffero, char *bufferi, regmatch_t *pmatch, int numMatch)
{
	strncpy(buffero, bufferi + pmatch[numMatch].rm_so, pmatch[numMatch].rm_eo - pmatch[numMatch].rm_so);
	buffero[(pmatch[numMatch].rm_eo - pmatch[numMatch].rm_so)] = '\0';
}
