/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __REGEX_H__
#define __REGEX_H__

#include <string.h>
#include <regex.h>

/**
 * Helper utility to extract a regex matching group
 * @param buffero the buffer where the matching group will be copied to
 * @param bufferi the buffer where the matching group will be copied from. It should contain the whole
 * string the was used by the regex to match the regex expresion.
 * @param pmatch the matching structure used by regex.h system library
 * @param numMatch the matching group number to extract. Range: the length of pmatch
 */
void regex_getRegexString(char *buffero, char *bufferi, regmatch_t *pmatch, int numMatch);

#endif
