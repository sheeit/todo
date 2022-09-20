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
#include "todo.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PRINT_UNDONE    (1 << 0)
#define PRINT_DONE      (1 << 1)

static todo_list *First = NULL;
static todo_list *Last = NULL;
static void todo_list_print_internal(todo_list *list_item, char done);
static void todo_list_dump_file_internal(todo_list *item, FILE *dumpfile);
static todo_list *todo_list_get_nth_item_internal(todo_list *item,
        const unsigned itemnum);
static size_t chomp(char *str);
static void todo_list_destroy_v2_internal(todo_list *item);


todo_list *todo_list_add(const char *text)
{
    todo_list *item = (todo_list *) malloc(sizeof(todo_list));

    if (item == NULL) {
        fprintf(stderr, "todo_list_add(): malloc() failed\n");
        return NULL;
    }

    item->done = false;
    item->text = text;
    item->next = NULL;
    if (Last) {
        Last->next = item;
        item->number = Last->number + 1;
    } else {
        item->number = 0;
    }
    Last = item;

    if (First == NULL)
        /* Make this one the first */
        First = item;

    return item;
}

void todo_list_print_one_item(const todo_list *item)
{
    const char printf_string[] = "%s%2d%s [%s%s%s] %s\n";
#if defined(NO_COLORS) && NO_COLORS
    const char *color_1 = "";
    const char *color_2 = "";
    const char *color_3 = "";
    const char *color_end = "";
#else
    const char *color_1 = "\033[0;34m";
    const char *color_2 = "\033[0;31m";
    const char *color_3 = "\033[0;32m";
    const char *color_end = "\033[0m";
#endif /* NO_COLORS */
#if defined(USE_PLAIN_ASCII) && USE_PLAIN_ASCII
    const char *done = "+";
    const char *undone = "-";
#else
    const char *done = "\xE2\x9C\x93";
    const char *undone = "\xE2\x9C\x97";
#endif /* USE_PLAIN_ASCII */

    if (item)
        printf(printf_string,
                color_1, item->number, color_end,
                item->done ? color_3 : color_2,
                    item->done ? done : undone,
                color_end,
                item->text);

    return;
}

void todo_list_print(void)
{
    if (First)
        todo_list_print_internal(First, PRINT_DONE | PRINT_UNDONE);

    return;
}

void todo_list_print_done(void)
{
    if (First)
        todo_list_print_internal(First, PRINT_DONE);

    return;
}
void todo_list_print_undone(void)
{
    if (First)
        todo_list_print_internal(First, PRINT_UNDONE);

    return;
}

static void todo_list_print_internal(todo_list *list_item, char done)
{
    if ((list_item->done == true && (done & PRINT_DONE))
        || (list_item->done == false && (done & PRINT_UNDONE)))
        todo_list_print_one_item(list_item);

    if (list_item != Last)
        todo_list_print_internal(list_item->next, done);

    return;
}

void todo_list_destroy_v2(void)
{
    todo_list *item = First;

    if (item)
        todo_list_destroy_v2_internal(item);

    free(get_dumpfile_path());

    return;
}

static void todo_list_destroy_v2_internal(todo_list *item)
{
    if (item->next)
        todo_list_destroy_v2_internal(item->next);

    free((void *) (item->text));
    free(item);

    return;
}

int todo_list_dump_to_file(void)
{
    const char *const Dump_filename = get_dumpfile_path();
    FILE *dumpfile;
    unsigned char utf8_bom[0x1B] = { 0 };
    const size_t utf8_bom_size = sizeof utf8_bom / sizeof utf8_bom[0];
    int ret = 0;

    dumpfile = fopen(Dump_filename, "w");
    if (dumpfile == NULL) {
        fprintf(stderr, "todo_list_dump_to_file(): Unable to open dumpfile %s "
                "for writing.\n", Dump_filename);
        return 1;
    }


    utf8_bom[0x00] = (unsigned char) 0xEF;
    utf8_bom[0x01] = (unsigned char) 0xBB;
    utf8_bom[0x02] = (unsigned char) 0xBF;
    utf8_bom[0x03] = (unsigned char) 'D';
    utf8_bom[0x04] = (unsigned char) 'o';
    utf8_bom[0x05] = utf8_bom[0x17]
                   = (unsigned char) 'n';
    utf8_bom[0x06] = (unsigned char) '\'';
    utf8_bom[0x07] = utf8_bom[0x0D] = utf8_bom[0x10]
                   = (unsigned char) 't';
    utf8_bom[0x08] = utf8_bom[0x0F] = utf8_bom[0x14]
                   = (unsigned char) ' ';
    utf8_bom[0x09] = (unsigned char) 'd';
    utf8_bom[0x0A] = utf8_bom[0x0C] = utf8_bom[0x0E] = utf8_bom[0x18]
                   = (unsigned char) 'e';
    utf8_bom[0x0B] = utf8_bom[0x15]
                   = (unsigned char) 'l';
    utf8_bom[0x11] = (unsigned char) 'h';
    utf8_bom[0x12] = utf8_bom[0x16]
                   = (unsigned char) 'i';
    utf8_bom[0x13] = (unsigned char) 's';
    utf8_bom[0x19] = (unsigned char) '.';
    utf8_bom[0x1A] = (unsigned char) '\n';

    ret = !(fwrite(utf8_bom, 1, utf8_bom_size, dumpfile) == utf8_bom_size) * 2;

    if (First != NULL)
        todo_list_dump_file_internal(First, dumpfile);

    fclose(dumpfile);

    return ret;
}

static void todo_list_dump_file_internal(todo_list *item, FILE *dumpfile)
{
    fprintf(dumpfile, "[%c]\t%s\n\f\n", item->done ? 'x' : ' ', item->text);

    if (item->next != NULL)
        todo_list_dump_file_internal(item->next, dumpfile);

    return;
}

void todo_list_done(todo_list *item)
{
    item->done = !item->done;

    return;
}

int todo_list_read_dump_file(void)
{
    char *const Dump_filename = get_dumpfile_path();
    FILE *dumpfile;
    char line[MAX_LINE] = {'\0'};
    size_t len;
    char *text;
    bool done;
    todo_list *this_item = NULL;

    dumpfile = fopen(Dump_filename, "r");

    if (dumpfile == NULL) {
        fprintf(stderr, "todo_list_dump_to_file(): Unable to open dumpfile %s "
                "for reading.\nMaybe it doesn't exist\n", Dump_filename);
        return 1;
    }

    while ((fgets(line, sizeof(line), dumpfile)) != NULL ) {
        if (line[0] == '\f'
            || (line[0] == '\xEF' && line[1] == '\xBB' && line[2] == '\xBF')) {
            /* Skip the line if it contains the UTF-8 Byte Order Mark or a form
             * feed, which will be eventually used to separate items.
             */
            continue;
        } else {
            if (line[1] == 'x')
                done = true;
            else
                done = false;

            (void) chomp(line);

            len = strlen(&line[4]) + 1;

            text = malloc(len);
            if (text == NULL) {
                return 1;
            }

            strncpy(text, &line[4], len);

            this_item = todo_list_add(text);
            if (done)
                todo_list_done(this_item);
        }
    }

    fclose(dumpfile);

    return 0;
}

static size_t chomp(char *str)
{
    size_t len = strlen(str);
    int i = (int) len;

    while (i > 0 && str[i - 1] == '\n') {
        --i;
        str[i] = '\0';
    }

    return len - i;
}

todo_list *todo_list_get_nth_item(long itemnum)
{
    todo_list *item = NULL;

    if (First) {
        const long last = Last->number + 1;
        const unsigned number = ((itemnum % last) + last) % last;
        item = todo_list_get_nth_item_internal(First, number);
    }

    if (!item)
        fprintf(stderr, "todo_list_get_nth_item(): Can't find the %ld%s item."
                "\n", itemnum,
                (itemnum == 11
                || itemnum == 12
                || itemnum == 13
                ) ? "th" :
                (itemnum % 10 == 1) ? "st" :
                (itemnum % 10 == 2) ? "nd" :
                (itemnum % 10 == 3) ? "rd" : "th");

    return item;
}

static todo_list *todo_list_get_nth_item_internal(todo_list *item,
        const unsigned itemnum)
{
    if (!item)
        return NULL;

    if (item->number == itemnum)
        return item;

    return todo_list_get_nth_item_internal(item->next, itemnum);
}

void todo_list_toggle_done(long itemnum)
{
    todo_list *item = todo_list_get_nth_item(itemnum);

    if (item) {
        todo_list_done(item);
    }

    return;
}

void todo_list_print_nth_item(long itemnum)
{
    todo_list *item = todo_list_get_nth_item(itemnum);

    if (item)
        todo_list_print_one_item(item);

    return;
}

/* TODO: Check if it still works, and rewrite it to use item->number */
void todo_list_remove_nth_item(int itemnum)
{
    todo_list *prev;
    todo_list *item;

    if (itemnum == 0) {
        item = First;
        if (item) {
            First = item->next;
            free((void *) item->text);
            free((void *) item);
            if (First == Last)
                Last = NULL;
        } else {
            fputs("No such item.\n", stderr);
        }
    } else {
        prev = todo_list_get_nth_item(itemnum - 1);
        item = prev ? prev->next : NULL;

        if (item) {
            prev->next = item->next;
            free((void *) item->text);
            free((void *) item);
            if (item == Last) {
                Last = prev;
            }
        } else {
            fputs("No such item.\n", stderr);
        }
    }

    return;
}

void todo_list_print_item(const todo_list *item)
{
    todo_list_print_one_item(item);

    return;
}
