#ifndef _H_FUNCIONES
#define _H_FUNCIONES

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

bool file_exists(char *filename);

typedef struct {
	char file_in[100];
	int has_file_in;
	char file_out[100];
	int has_file_out;
	int flag_verbose;
	int total_workers;
	int chunk_size;
} params_t;

void parse_params(params_t *params, int argc, char *argv[]);

void dup_printf(bool flag_verbose, FILE *f, char const *fmt, ...);

bool str_endswith(const char *str, const char *suffix);

char *strtrim(char *s);

typedef struct __node {
	char *value;
	struct __node *next;
} node_t;

typedef struct {
	node_t *head;
	node_t *tail;
	node_t *curr;
} list_t;

list_t *list_create();
node_t *list_iter_start(list_t *list);
node_t *list_iter_next(list_t *list);
node_t *list_add(list_t *list, char *value);
void list_empty(list_t *list);
void list_destroy(list_t *list);

#endif