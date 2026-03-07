#include <stdio.h>
#include <string.h>
#include "lint.h"
#include "wiz_list.h"
#include "interpret.h"

/*
 * Maintain the wizards high score list about most popular castle.
 */

extern char *string_copy PROT((char *)), *xalloc PROT((size_t));

struct wiz_list *all_wiz;

/*
 * Sort the wiz list in ascending order.
 */

static struct wiz_list *insert(struct wiz_list *w, struct wiz_list *wl)
{
    if (wl == 0) {
	w->next = 0;
	return w;
    }
    if (w->score > wl->score) {
	wl->next = insert(w, wl->next);
	return wl;
    }
    w->next = wl;
    return w;
}

static void rebuild_list() {
    struct wiz_list *wl, *w, *new_list = 0;

    for (w = all_wiz; w; w = wl) {
	wl = w->next;
	new_list = insert(w, new_list);
    }
    all_wiz = new_list;
}

/*
 * Find the data, if it exists.
 */
static struct wiz_list *find_wiz(char *name)
{
    int length;
    struct wiz_list *wl;

    length = strlen(name);
    for (wl = all_wiz; wl; wl = wl->next)
        if (wl->length == length && strcmp(wl->name, name) == 0)
	    return wl;
    return 0;
}

/*
 * Check that a name exists. Add it, if it doesn't.
 */
struct wiz_list *add_name(char *str)
{
    struct wiz_list *wl;

    wl = find_wiz(str);
    if (wl)
        return wl;
    wl = (struct wiz_list *)xalloc(sizeof (struct wiz_list));
    str = string_copy(str);
    wl->name = str;
    wl->length = strlen(str);
    wl->score = 0;
    wl->cost = 0;
    wl->heart_beats = 0;
    wl->total_worth = 0;
    wl->next = all_wiz;
    wl->size_array = 0;
    wl->file_name = 0;
    wl->error_message = 0;
    all_wiz = wl;
    return wl;
}

/*
 * Add score to an existing name.
 */
void add_score(char *name, int score)
{
    struct wiz_list *wl;

    wl = find_wiz(name);
    if (!wl) {
        fatal("Add_score: could not find wizard %s\n", name);
        return;
    }
    wl->score += score;
}

/*
 * This one is called at every complete walkaround of reset.
 */
void wiz_decay() {
    struct wiz_list *wl;

    for (wl = all_wiz; wl; wl = wl->next) {
        wl->score = wl->score * 99 / 100;
	wl->total_worth = wl->total_worth * 99 / 100;
	wl->cost = wl->cost * 9 / 10;
	wl->heart_beats = wl->heart_beats * 9 / 10;
    }
}

/*
 * Load the wizlist file.
 */
void load_wiz_file()
{
    char buff[1000];		/* I hate not knowing how much I need. */
    FILE *f;

    f = fopen("WIZLIST", "r");
    if (f == NULL)
        return;
    while(fgets(buff, sizeof buff, f) != NULL) {
        char *p;
	int score;

	p = strchr(buff, ' ');
	if (p == 0) {
	    fprintf(stderr, "Bad WIZLIST file.\n");
	    break;
	}
	*p = '\0';
	p++;
	if (*p == '\0') {
	    fprintf(stderr, "Bad WIZLIST file.\n");
	    break;
	}
	score = atoi(p);
	if (score > 0) {
	    (void)add_name(buff);
	    add_score(buff, atoi(p));
	}
    }
    fclose(f);
}

/*
 * Save the wizlist file.
 */
void save_wiz_file()
{
    struct wiz_list *wl;
    FILE *f;

    f = fopen("WIZLIST", "w");
    if (f == NULL) {
        fprintf(stderr, "Could not open WIZLIST for write\n");
        return;
    }
    for (wl = all_wiz; wl; wl = wl->next)
        fprintf(f, "%s %d %d\n", wl->name, wl->score, wl->total_worth);
    fclose(f);
}

void wizlist(struct value *v)
{
    struct wiz_list *wl, *this_wiz;
    int total = 0, num = 0, this_pos = -1, total_wizards;
    extern struct object *command_giver;
    int all = 0;

    if (!command_giver)
	return;
    if (v == 0)
	v = apply("query_real_name", command_giver, 0);
    if (!v || v->type != T_STRING)
	return;
    if (strcmp(v->u.string, "ALL") == 0)
	all = 1;
    this_wiz = find_wiz(v->u.string);
    rebuild_list();
    for (num = 0, wl = all_wiz; wl; wl = wl->next) {
        total += wl->score;
	num++;
	if (wl == this_wiz)
	    this_pos = num;
    }
    total_wizards = num;
    add_message("\nWizard top score list\n\n");
    if (this_pos >= 0)
	this_pos = num - this_pos + 1;
    else
	all = 1;
    if (total == 0)
	total = 1;
    for (wl = all_wiz; wl; wl = wl->next) {
	if (!all && num > 15 && (num < this_pos - 2 || num > this_pos + 2))
		;
	else
	    add_message("%-15s %5d %2d%% (%d)\t[%4dk,%5d] %6d %d\n", wl->name,
			wl->score, wl->score * 100 / total, num,
			wl->cost / 1000,
			wl->heart_beats, wl->total_worth, wl->size_array);
	num--;
    }
    add_message("\nTotal         %7d     (%d)\n\n", total, total_wizards);
}

void remove_wiz_list() {
    struct wiz_list *wl, *w;

    for (w = all_wiz; w; w = wl) {
	free(w->name);
	wl = w->next;
	free((char *)w);
    }
}

void save_error(char *msg, char *file, int line)
{
    struct wiz_list *wl;
    char name[100];
    char *p;
    int len;

    if (sscanf(file, "players/%99[^/]/", name) != 1)
	return;
    
    p = strchr(name, '/');
    if (p)
	*p = '\0';
    wl = add_name(name);
    if (wl->file_name)
	free(wl->file_name);
    len = strlen(file);
    wl->file_name = xalloc(len + 3); /* May add .c plus the null byte */
    strcpy(wl->file_name, file);
    /*
     * If it is a cloned object, we have to find out what the file
     * name is, and add '.c'.
     */
    p = strrchr(wl->file_name, '#');
    if (p) {
	p[0] = '.';
	p[1] = 'c';
	p[2] = '\0';
	len = p - wl->file_name + 2;
    }
    if (wl->file_name[len-1] != 'c' || wl->file_name[len-2] != '.')
	strcat(wl->file_name, ".c");
    if (wl->error_message)
	free(wl->error_message);
    wl->error_message = string_copy(msg);
    wl->line_number = line;
}

char *get_error_file(char *name)
{
    extern struct object *command_giver;
    struct wiz_list *wl;

    wl = add_name(name);
    /*
     * The error_message is used as a flag if there has been any error.
     */
    if (wl->error_message == 0) {
	add_message("No error.\n");
	return 0;
    }
    add_message("%s line %d: %s\n", wl->file_name, wl->line_number,
		wl->error_message);
    free(wl->error_message);
    wl->error_message = 0;
    return wl->file_name;
}
