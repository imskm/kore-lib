#include <stdio.h>
#include <ctype.h>		/* For tolower() and toupper() function */

#include "str.h"

/**
 * Converts all upper alphabets to lower alphabets
 * usage:
 *    char s[100] = "THIS IS a test #1.";
 *    kore_strtolower(s);  OR printf("%s\n", kore_strtolower(s));
 *
 *    output:
 *      this is a test #1.
 *
 * bug:
 *    char *s = "this type of initialisation gives segfault";
 *    because this initialisation creates read only memory
 *
 *
 * @param char*  pointer to string
 * @return char*  original string base address (but string is in lower case)
 */
char *kore_strtolower(char *s)
{
	if (s == NULL) return s;

	for(char *p = s; *p; p++)
		*p = (unsigned char)tolower(*p);

	return s;
}

/**
 * Converts all lower alphabets to upper alphabets
 * usage:
 *    char s[100] = "THIS IS a test #1.";
 *    kore_strtolower(s);  OR printf("%s\n", kore_strtolower(s));
 *
 *    output:
 *      THIS IS A TEST #1.
 *
 * bug:
 *    char *s = "this type of initialisation gives segfault";
 *    because this initialisation creates read only memory
 *
 *
 * @param char*  pointer to string
 * @return char*  original string base address (but string is in upper case)
 */
char *kore_strtoupper(char *s)
{
	if (s == NULL) return s;

	for(char *p = s; *p; p++)
		*p = (unsigned char)toupper(*p);

	return s;
}
