#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

char* read_file(char* filepath);
char** read_lines(char* filepath);
size_t read_file_to_buffer(char* filepath, char** buffer, size_t capacity);

#endif // !FILE_READER_H

#ifdef FILE_READER_IMPL

char* read_file(char* filepath) {
	size_t size = 0;
	size_t cap = 1024;
	char* contents = (char*)calloc(cap, sizeof(char));
	FILE* f = fopen(filepath, "r");

	assert(f != NULL && "Could not open file\n");

	while (!feof(f)) {
		if (size >= cap) {
			cap *= 1.5;
			contents = (char*)realloc(contents, cap * sizeof(char));
		}
		contents[size++] = fgetc(f);
	}

	return contents;
}

char** read_lines(char* filepath) {
	size_t line_size = 0;
	size_t cur_line_cap = 1024;
	size_t line_count = 1;
	size_t line_cap = 1024;
	char** lines = (char**)calloc(line_cap, sizeof(char*));
	lines[line_count-1] = (char*)calloc(cur_line_cap, sizeof(char));
	FILE* f = fopen(filepath, "r");

	assert(f != NULL && "Could not open file\n");

	while (!feof(f)) {
		int c = fgetc(f);
		if (c == '\n') {
			if (line_count >= line_cap) {
				line_cap *= 1.5;
				lines = (char**) calloc(line_cap, sizeof(char*));
			}

			lines[line_count++] = (char*)calloc(cur_line_cap, sizeof(char));
			continue;
		}

		lines[line_count-1][line_size] = c;
	}

	return lines;
}

size_t read_file_to_buffer(char* filepath, char** buffer, size_t capacity) {
	size_t size = 0;
	char* contents = *buffer;
	FILE* f = fopen(filepath, "r");

	assert(f != NULL && "Could not open file\n");

	while (!feof(f)) {
		if (size >= capacity) {
			return size;
		}
		contents[size++] = fgetc(f);
	}

	return size;
}

#endif // FILE_READER_IMPL
