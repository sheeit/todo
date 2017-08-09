/**
 * This file is part of "todo".
 *
 * "todo" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "todo" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "todo".  If not, see <http://www.gnu.org/licenses/>.
 */


#include "get_dumpfile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Dumpfile_location "/.todo_dumpfile"

static char *Dumpfile = NULL;

char *get_dumpfile_path(void)
{
    const char *const home = getenv("HOME");
    const size_t additional_chars = sizeof(Dumpfile_location);
    size_t homelen;
    extern char *Dumpfile;

    if (Dumpfile)
        return Dumpfile;

    if (!home) {
        fputs("get_dumpfile_path: home is NULL\n", stdout);
        return NULL;
    }

    homelen = strlen(home);
    if (!homelen) {
        fputs("get_dumpfile_path: homelen is 0\n", stdout);
        return NULL;
    }

    Dumpfile = calloc(homelen + additional_chars, 1);
    if (!Dumpfile) {
        fputs("get_dumpfile_path: Dumpfile is NULL\n", stdout);
        return NULL;
    }

    strncpy(Dumpfile, home, homelen + 1);
    strncat(Dumpfile, Dumpfile_location, additional_chars);

#if defined(DEBUG) && DEBUG
    printf("Your dumpfile is %s\n", Dumpfile);
#endif /* defined(DEBUG) && DEBUG */

    return Dumpfile;
}

#undef Dumpfile_location
