/* extString.c
 *
 * Copyright 2024 axtlos <axtlos@disroot.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *strlwr(char *s)
{
  unsigned char *p = (unsigned char *)s;

  while (*p) {
     *p = tolower((unsigned char)*p);
      p++;
  }

  return s;
}

char *trim(char *s)
{
  char *result = strdup(s);
  char *end;

  while(isspace((unsigned char)*result)) result++;

  if(*result == 0)
    return result;

  end = result + strlen(result) - 1;
  while(end > result && isspace((unsigned char)*end)) end--;

  end[1] = '\0';

  return result;
}

char *replaceStr(char *s, char *old, char *replace) {
  char* result; 
  int i, cnt = 0; 
  size_t newSize = strlen(replace); 
  size_t oldSize = strlen(old); 
 
  for (i = 0; s[i] != '\0'; i++) { 
    if (strstr(&s[i], old) == &s[i]) { 
      cnt++; 
      i += oldSize - 1; 
    } 
  } 

  result = (char*)malloc(i + cnt * (newSize - oldSize) + 1); 
 
  i = 0; 
  while (*s) { 
    if (strstr(s, old) == s) { 
      strcpy(&result[i], replace); 
      i += newSize; 
      s += oldSize; 
    } 
    else
      result[i++] = *s++; 
  } 
 
  result[i] = '\0'; 
  return result; 
};
