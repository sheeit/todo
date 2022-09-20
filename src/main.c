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


/* For strdup() */
#define _POSIX_C_SOURCE (200809L)

#include "../config.h"
#include "todo.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void check_for_v(int argc, const char * const *argv);

int main(int argc, char **argv)
{
    int i;
    int item_to_process;
    bool ignore = false;
    bool print_all = true;
    todo_list *last_item = NULL;
    int errors = 0;

    check_for_v(argc - 1, (const char * const *) argv + 1);

    if (todo_list_read_dump_file() != 0) {
        /* TODO:
         * if dump_file doesn't exist; then create it.
         */
        fprintf(stderr, "main(): Reading dumpfile failed\n");
        abort();
    }

    /* TODO: replace all this shit with a parser function that does it better
     * and in an elegant way. This right here is shit.
     */
    for (i = 1; i < argc; ++i) {

        if (ignore) {
            ignore = false;
            continue;
        }

        if (argv[i][0] != '-') {
            last_item = todo_list_add(strdup(argv[i]));
            print_all = false;
        } else if (i > 1) {
            /* argv[i][0] == '-' */
            if (argv[i][1] == 'y') {
                if (!last_item) {
                    fputs("-y can only be used after adding an item.\n",
                            stderr);
                    ++errors;
                } else {
                    todo_list_done(last_item);
                }
            } else if (argv[i][1] == 'n') {
                /* Do nothing; it's already marked as undone. */
                ;
            } else {
                ++errors;
                fprintf(stderr, "%s: no such option: -%c\n",
                    argv[0], argv[i][1]);
            }
        } else {
            /* argv[i][0] == '-' && i == 1 */
            print_all = false;

            if (isdigit(argv[i][1])
                    || (argv[i][1] == '-' && isdigit(argv[i][2]))) {
                item_to_process = atoi(&argv[i][1]);
                todo_list_print_nth_item(item_to_process);

            } else {

                switch (argv[i][1]) {

                case 'r': /* r == remove */
                    if (i + 1 < argc) {
                        char *endptr = NULL;
                        ignore = true;
                        item_to_process
                            = (int) strtol(argv[i + 1], &endptr, 10);
                        if (endptr == argv[i + 1] || *endptr != '\0') {
                            ++errors;
                            fprintf(stderr, "Error: %s doesn't appear to be "
                                "a valid integer.\n",
                                argv[i + 1]);
                        } else {
                            todo_list_print_nth_item(item_to_process);
                            todo_list_remove_nth_item(item_to_process);
                            fprintf(stderr, "Item removed.\n");
                        }
                    } else {
                        ++errors;
                        fprintf(stderr, "Usage: %s -r itemnum\n"
                            "Remove the item number itemnum.\n",
                            argv[0]);
                    }
                    break;

                case 't': /* t == toggle_done */
                    if (i + 1 < argc) {
                        char *endptr = NULL;
                        ignore = true;
                        item_to_process =
                            (int) strtol(argv[i + 1], &endptr, 10);
                        if (endptr == argv[i + 1] || *endptr != '\0') {
                            ++errors;
                            fprintf(stderr, "Error: %s doesn't appear to be "
                                "a valid integer.\n",
                                argv[i + 1]);
                        } else {
                            todo_list_toggle_done(item_to_process);
                            todo_list_print_nth_item(item_to_process);
                        }
                    } else {
                        ++errors;
                        fprintf(stderr, "Usage: %s -t itemnum\n"
                            "Mark the item number itemnum as done "
                            "or undone (toggle its 'done' state).\n",
                            argv[0]);
                    }
                    break;

                case 'd': /* t == list_done */
                    todo_list_print_done();
                    break;

                case 'u': /* u == list_undone */
                    todo_list_print_undone();
                    break;

                default:
                    ++errors;
                    fprintf(stderr, "%s: no such option: -%c\n",
                        argv[0], argv[i][1]);
                    break;

                }

            }

        }

    }

    if (errors)
        goto quit;

    if (print_all)
        todo_list_print();
    else if (last_item)
        todo_list_print_item(last_item);

    if (todo_list_dump_to_file() == 2) {
        ++errors;
        fprintf(stderr, "todo_list_dump_to_file() returned 2.\n"
            "Writing the UTF-8 BOM failed.\n");
    }

quit:
    todo_list_destroy_v2();

    exit(!errors ? EXIT_SUCCESS : EXIT_FAILURE);
}

void check_for_v(int argc, const char * const *argv)
{
    void print_copyright_info(void);
    const char *s;

    while (argc--) {
        s = *argv++;
        if (s[0] == '-'
                && (s[1] == 'v' || !strncmp(s + 1, "-version", (size_t) 8u))) {
            print_copyright_info();
            exit(EXIT_SUCCESS);
        }
    }

    return;
}

void print_copyright_info(void)
{
    fputs("todo v" VERSION " Copyright \xC2\xA9 2017 strupo <strupo AT autisti"
          "ci DOT org>\nThis program comes with ABSOLUTELY NO WARRANTY; for de"
          "tails see the COPYING\nfile that was distributed along with this pr"
          "ogram; or see\n<http://www.gnu.org/licenses>\n", stdout);

    return;
}
