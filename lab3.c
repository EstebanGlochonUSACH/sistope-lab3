#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <regex.h>
#include <pthread.h>
#include <string.h>
#include "funciones.h"

regex_t re;
list_t *results;
params_t params;
pthread_t *tid;
pthread_mutex_t read_lock;
pthread_mutex_t write_lock;
int processed_lines;

void *main_thread(void *arg)
{
	char line_buffer[4096];
	int chunk_counter = 0;
	FILE *fp_input = (FILE*)arg;
	list_t *tmp_lines = list_create();
	int match_status;

	while(!feof(fp_input)){
		// TODO: read N lines
		pthread_mutex_lock(&read_lock);
		chunk_counter = 0;
		while (fgets(line_buffer, sizeof(line_buffer), fp_input) != NULL) {
			processed_lines += 1;
			if(str_endswith(line_buffer, "\n")) strtrim(line_buffer);

			match_status = regexec(&re, line_buffer, (size_t)0, NULL, 0);
			if (match_status == 0) strcat(line_buffer, " si");
			else strcat(line_buffer, " no");
			list_add(tmp_lines, line_buffer);

			chunk_counter += 1;
			if(chunk_counter == params.chunk_size) break;
		}
		pthread_mutex_unlock(&read_lock);

		// TODO: write results to struct
		pthread_mutex_lock(&write_lock);
		node_t *tmp_node;
		list_iter_start(tmp_lines);
		while((tmp_node = list_iter_next(tmp_lines)) != NULL){
			list_add(results, tmp_node->value);
		}
		list_empty(tmp_lines);
		pthread_mutex_unlock(&write_lock);
	}

	list_destroy(tmp_lines);
	return 0;
};

int main(int argc, char *argv[])
{
	static const char *pattern = "^(A|C|T)*G+T+C(A|C|G|T)*$";
	results = list_create();
	parse_params(&params, argc, argv);
	processed_lines = 0;

	if(!params.has_file_in){
		fprintf(stderr, "Error: Se requiere el parametro 'i'\n");
		exit(-1);
	}

	if(!params.has_file_out){
		fprintf(stderr, "Error: Se requiere el parametro 'o'\n");
		exit(-1);
	}

	if(params.chunk_size == INT_MIN){
		fprintf(stderr, "Error: Se requiere el parametro 'c'\n");
		exit(-1);
	}

	if(params.chunk_size < 1){
		fprintf(stderr, "Error: El tama%co del chunk no puede ser menor a 1!\n", 241);
		exit(-1);
	}

	if(params.total_workers == INT_MIN){
		fprintf(stderr, "Error: Se requiere el parametro 'n'\n");
		exit(-1);
	}

	if(params.total_workers < 1){
		fprintf(stderr, "Error: El total de workers no puede ser menor a 1!\n");
		exit(-1);
	}

	if(!file_exists(params.file_in)){
		fprintf(stderr, "Error: El archivo de entrada no existe!\n");
		exit(-1);
	}

	if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
		fprintf(stderr, "Error: El patron regex no pudo ser compilado!\n");
		exit(-1);
	}

	FILE *fp_input, *fp_output;
	if((fp_input = fopen(params.file_in, "r")) == NULL){
		fprintf(stderr, "Error: No se pudo acceder al archivo de entrada!\n");
		exit(-1);
	}

	if (pthread_mutex_init(&read_lock, NULL) != 0) {
		fprintf(stderr, "Error: No se pudo inicializar el mutex \"read\"!\n");
		fclose(fp_input);
		exit(-1);
	}

	if (pthread_mutex_init(&write_lock, NULL) != 0) {
		fprintf(stderr, "Error: No se pudo inicializar el mutex \"write\"!\n");
		fclose(fp_input);
		exit(-1);
	}

	int i, error;
	tid = (pthread_t*)malloc(sizeof(pthread_t) * params.total_workers);
	for(i = 0; i < params.total_workers; ++i){
        error = pthread_create(&(tid[i]), NULL, &main_thread, (void*)fp_input);
        if (error != 0){
			fprintf(stderr, "Error: No se pudo inicializar la hebra!\n");
			fclose(fp_input);
			exit(-1);
		}
	}

	for(i = 0; i < params.total_workers; ++i){
		pthread_join(tid[i], NULL);
	}

	fclose(fp_input);
    pthread_mutex_destroy(&read_lock);
    pthread_mutex_destroy(&write_lock);
	free(tid);

	// TODO
	if((fp_output = fopen(params.file_out, "w")) == NULL){
		fprintf(stderr, "Error: No se pudo acceder al archivo de salida!\n");
		exit(-1);
	}

	int counter_match = 0, counter_no_match = 0, counter_total = 0;
	node_t *tmp_node;
	list_iter_start(results);
	while((tmp_node = list_iter_next(results)) != NULL){
		dup_printf(params.flag_verbose, fp_output, "%s\n", tmp_node->value);
		counter_total += 1;
		if(str_endswith(tmp_node->value, " si")) counter_match += 1;
		else if(str_endswith(tmp_node->value, " no")) counter_no_match += 1;
	}

	dup_printf(params.flag_verbose, fp_output, "\n");
	dup_printf(params.flag_verbose, fp_output, "Total de expresiones que Si son regulares:%d\n", counter_match);
	dup_printf(params.flag_verbose, fp_output, "Total de expresiones que No son regulares:%d\n", counter_no_match);
	dup_printf(params.flag_verbose, fp_output, "Total de lineas leídas:%d\n", counter_total);
	dup_printf(params.flag_verbose, fp_output, "Total de lineas procesadas:%d\n", processed_lines);

	// TODO: print read/write by thread

	list_destroy(results);
	return 0;
};