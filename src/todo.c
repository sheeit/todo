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


#define _POSIX_C_SOURCE (201704L)


#include "get_dumpfile.h"
#include "todo.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static todo_list *First = NULL;
static todo_list *Last = NULL;
static todo_list *Malloced_array[MAX_ITEMS] = {NULL};
static int Current_item_in_malloced_array = 0;
static char *Malloced_text[MAX_ITEMS] = {NULL};
static int Current_item_in_malloced_text = 0;
static void todo_list_print_internal(todo_list *list_item, int n);
static void todo_list_dump_file_internal(todo_list *item, FILE *dumpfile);
static todo_list *todo_list_get_nth_item_internal(todo_list *item,
        int itemnum);
static size_t chomp(char *str);
static void todo_list_destroy_v2_internal(todo_list *item);


todo_list *todo_list_add(const char *text)
{
    todo_list *item;
    if (Current_item_in_malloced_array >= MAX_ITEMS - 1)
        return NULL;

    item = (todo_list *) malloc(sizeof(todo_list));
    if (item == NULL) {
        fprintf(stderr, "todo_list_add(): malloc() failed\n");
        return NULL;
    }
    Malloced_array[Current_item_in_malloced_array] = item;
    ++Current_item_in_malloced_array;

    item->done = false;
    item->text = strndup(text, (1 << 10));
    item->next = NULL;
    if (Last) {
        Last->next = item;
    }
    Last = item;

    if (First == NULL) {
        /* Make this one the first */
        First = item;
    }

    return item;
}

void todo_list_print_one_item(todo_list *item, int n)
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
    const char *done = "x";
    const char *undone = "x";
#else
    const char *done = "\xE2\x9C\x93";
    const char *undone = "\xE2\x9C\x97";
#endif /* USE_PLAIN_ASCII */

    if (item)
        printf(printf_string,
                color_1, n, color_end,
                item->done ? color_3 : color_2,
                    item->done ? done : undone,
                color_end,
                item->text);

    return;
}

void todo_list_print(void)
{
    if (First)
        todo_list_print_internal(First, 0);

    return;
}

static void todo_list_print_internal(todo_list *list_item, int n)
{
    todo_list_print_one_item(list_item, n);

    if (list_item != Last)
        todo_list_print_internal(list_item->next, n + 1);

    return;
}

void todo_list_destroy(void)
{
    int i;

    for (i = 0; i < Current_item_in_malloced_array; ++i)
        free((void *) (Malloced_array[i]));

    for (i = 0; i < Current_item_in_malloced_text; ++i)
        free((void *) (Malloced_text[i]));

    return;
}

void todo_list_destroy_v2(void)
{
    todo_list *item = First;

    if (item)
        todo_list_destroy_v2_internal(item);

    return;
}

static void todo_list_destroy_v2_internal(todo_list *item)
{
    if (item->next)
        todo_list_destroy_v2_internal(item->next);

    /* Change this because not all text is malloc'd. Sme of it is literal text.
     * This issue can be solved by adding another flag, is_mallocd, to mean
     * that this text is malloc'd and should be freed.
     * The todo_list_read_dump_file() function produces malloc'd texts, but not
     * the todo_list_add() function, which just uses the pointer to the string
     * constant passed to it.
     * Now that I think of it, a simpler solution would be to just use malloc
     * for all texts, even those in todo_list_add. Hmmm. I'm gonna need a
     * function to copy a string literal into a malloc'd string, and do all the
     * calculating and all of that. This is actually pretty simple, and I'm
     * like (100 - LDBL_EPSILON)% sure that the standard library provides such
     * a function, maybe called strdup. But what the hell; I'm doing this to
     * learn, and not to rely on ready-made functions written by people far
     * more experienced than me, so being the big boy that I am, I'll do it
     * myself. What should I call it... I have no idea. I 'm not really good at
     * naming things, which I'm sure you would have noticed, if you are reading
     * this. Anyway, comments are for documenting code, not for stories.
     */
    free((void *) (item->text));
    free(item);

    return;
}

void do_nothing(void)
{
    /* nothing */
    return;
}

int todo_list_dump_to_file(void)
{
    const char *const Dump_filename = get_dumpfile_path();
    FILE *dumpfile;
    unsigned char * const utf8_bom = (unsigned char *) malloc((size_t) 0x1ALU);
    int ret = 0;

    dumpfile = fopen(Dump_filename, "w");
    if (dumpfile == NULL) {
        fprintf(stderr, "todo_list_dump_to_file(): Unable to open dumpfile %s "
                "for writing.\n", Dump_filename);
        free(utf8_bom);
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

    ret = (fwrite(utf8_bom, 1, (size_t) 0x1BLU, dumpfile) == (size_t) 0x1BLU)
            ? 0 : 2;

    free((void *) utf8_bom);

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
    const char *const Dump_filename = get_dumpfile_path();
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

            Malloced_text[Current_item_in_malloced_text] = text;
            ++Current_item_in_malloced_text;

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
    size_t len;

    for (len = 0; str[len] != '\n'; ++len)
        continue;

    str[len] = '\0';

    return len;
}

todo_list *todo_list_get_nth_item(int itemnum)
{
    todo_list *item = NULL;
    if (First)
        item = todo_list_get_nth_item_internal(First, itemnum);

    if (!item)
        fprintf(stderr, "todo_list_get_nth_item(): Can't find the %d%s item."
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

static todo_list *todo_list_get_nth_item_internal(todo_list *item, int itemnum)
{
    todo_list *nth_item = NULL;

    if (!item)
        nth_item = NULL;
    else if (itemnum == 0)
        nth_item = item;
    else
        nth_item = todo_list_get_nth_item_internal(item->next, itemnum - 1);

    return nth_item;
}

void todo_list_toggle_done(int itemnum)
{
    todo_list *item = todo_list_get_nth_item(itemnum);

    if (item) {
        todo_list_done(item);
    }

    return;
}

void todo_list_print_nth_item(int itemnum)
{
    todo_list *item = todo_list_get_nth_item(itemnum);

    if (item)
        todo_list_print_one_item(item, itemnum);

    return;
}

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
        }
    } else {
        prev = todo_list_get_nth_item(itemnum - 1);
        item = prev->next;

        if (item) {
            prev->next = item->next;
            free((void *) item->text);
            free((void *) item);
            if (item == Last) {
                Last = prev;
            }
        }
    }

    return;
}
