#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

// Entradas:    Recibe la ruta del archivo.
// Salidas:     Retorna verdadero o falso dependiendo de si el archivo existe.
// Descripción: Comprueba si un archivo existe.
bool file_exists(char *filename){
	struct stat buffer;   
	return(stat(filename, &buffer) == 0);
}

// Descripción: Estructura que contiene los parameros del programa.
typedef struct {
	char file_in[100];
	int has_file_in;
	char file_out[100];
	int has_file_out;
	int flag_verbose;
	int total_workers;
	int chunk_size;
} params_t;

// Entradas:    Recibe una estructura donde guardar los paramatros de entrada junto
//              con los mismos parameros que recibe la funcion main.
// Salidas:     No retorna. Guarda información en la estructura "params".
// Descripción: Procesa los parameros que recibe el main, en forma de valores string
//              o flags booleanos para guardarlos en una estructura especial que va
//              a contener toda esa información.
void parse_params(params_t *params, int argc, char *argv[])
{
	int option;
	char tmp[48];
	params->flag_verbose = 0;
	params->has_file_in = 0;
	params->has_file_out = 0;
	params->chunk_size = INT_MIN;
	params->total_workers = INT_MIN;
	while((option = getopt(argc, argv, "i:o:n:c:b")) != -1){
		switch(option) { 
			case 'i':
				strcpy(params->file_in, optarg);
				params->has_file_in = 1;
				break;
			case 'o':
				strcpy(params->file_out, optarg);
				params->has_file_out = 1;
				break;
			case 'b':
				params->flag_verbose = 1;
				break;
			case 'n':
				strcpy(tmp, optarg);
				params->total_workers = atoi(tmp);
				break;
			case 'c':
				strcpy(tmp, optarg);
				params->chunk_size = atoi(tmp);
				break;
		}
	}
};

// Entradas:    Recibe los mismos parametros que "fprintf", junto con un "flag".
// Salidas:     No retorna. Se espera que la funcion escriba en un archivo y opcionalmente
//              imprima texto en la terminal.
// Descripción: Recibe los mismos parametros que "fprintf" para poder escribir texto
//              en un archivo. El flag "flag_verbose" sirve ademas, para habilitar
//              la escritura de la misma informacion en el STDOUT del proceso.
void dup_printf(bool flag_verbose, FILE *f, char const *fmt, ...) { 
    va_list ap;
    if(flag_verbose){
        va_start(ap, fmt);
        vprintf(fmt, ap);
        va_end(ap);
    }
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
};

// Entradas:    El string a comprobar y el string que representa el "sufijo".
// Salidas:     Retorna verdadero o falso dependiendo del caso.
// Descripción: Comprueba si un string termina con otro string dado.
bool str_endswith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return false;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return false;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

// Entradas:    Un character.
// Salidas:     Retorna verdadero o falso dependiendo del caso.
// Descripción: Comprueba si un caracter dado es de tipo "espacio".
bool is_space(char c){
	return((c >= 0 && c <=32) || c == 127);
};

// Entradas:    Recibe "string" (arreglo de carácteres).
// Salidas:     Retorna el mismo puntero del arreglo de caracteres.
// Descripción: Remueve todos los carácteres "blancos" del inicio y final del string.
char *strtrim(char *s) {
	int start_index = 0;
	int end_index = strlen(s);
	char buffer[1000];
	// left trim
    while(is_space(s[start_index])) ++start_index;
	// right tirm
    while(is_space(s[end_index])) --end_index;
    s[end_index + 1] = '\0';

	// usar buffer para mover caracteres
	strcpy(buffer, &s[start_index]);
	strcpy(s, buffer);
    return s;
};

// Descripción: Estructura nodo para lista enlazada.
typedef struct __node {
	char *value;
	struct __node *next;
} node_t;

typedef struct {
	node_t *head;
	node_t *tail;
	node_t *curr;
} list_t;

list_t *list_create()
{
	list_t *list = (list_t*)malloc(sizeof(list_t));
	if(list == NULL) return NULL;
	list->head = NULL;
	list->tail = NULL;
	list->curr = NULL;
	return list;
};

node_t *list_iter_start(list_t *list)
{
	list->curr = list->head;
	return list->curr;
};

node_t *list_iter_next(list_t *list)
{
	node_t *curr = list->curr;
	if(list->curr != NULL){
		list->curr = list->curr->next;
	}
	return curr;
};

node_t *list_add(list_t *list, char *value)
{
	node_t *node = (node_t*)malloc(sizeof(node_t));
	if(node == NULL) return NULL;
	node->value = strdup(value);

	if(list->head == NULL){
		list->head = node;
		list->tail = node;
	}
	else{
		list->tail->next = node;
		list->tail = node;
	}

	return node;
};

void list_empty(list_t *list)
{
	if(list->head != NULL) {
		node_t *tmp_node = list->head;
		node_t *tmp_node2;
		while(tmp_node != NULL){
			tmp_node2 = tmp_node->next;
			free(tmp_node->value);
			free(tmp_node);
			tmp_node = tmp_node2;
		}
	}
	list->head = NULL;
	list->tail = NULL;
	list->curr = NULL;
};

void list_destroy(list_t *list)
{
	list_empty(list);
	free(list);
};