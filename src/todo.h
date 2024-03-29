/**
 * This file is part of "todo".
 *
 * "todo" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 * "todo" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "todo".  If not, see <http:www.gnu.org/licenses/>.
 */


#ifndef TODO_H
#define TODO_H


/* Symbolic constants */

#define MAX_ITEMS (1 << 8)
#ifdef WIN32
# define NO_COLORS 1
# define USE_PLAIN_ASCII 1
#else
# define NO_COLORS 0
# define USE_PLAIN_ASCII 0
#endif
#define MAX_LINE (1 << 16)

#include <stdbool.h>


/* Structure definitions */

struct todo_list {
    const char *text;
    bool done;
    unsigned int number;
    struct todo_list *next;
};


/* Typedefs for structures; should have the same name to avoid confusion.
 * Note that this is perfectly legal because of different namespaces.
 * The C89 Standard says so.
 */
typedef struct todo_list todo_list;


/* Function prototypes */

void do_nothing(void);

todo_list *todo_list_add(const char *text);

void todo_list_print(void);
void todo_list_print_done(void);
void todo_list_print_undone(void);

void todo_list_print_one_item(const todo_list *item);

void todo_list_destroy(void);

void todo_list_destroy_v2(void);

void todo_list_done(todo_list *item);

int todo_list_dump_to_file(void);

int todo_list_read_dump_file(void);

void todo_list_toggle_done(long itemnum);

todo_list *todo_list_get_nth_item(long itemnum);

void todo_list_print_nth_item(long itemnum);

void todo_list_remove_nth_item(int itemnum);

void todo_list_print_item(const todo_list *item);

int todo_list_get_itemnum(const todo_list *item);


#endif /* TODO_H */
