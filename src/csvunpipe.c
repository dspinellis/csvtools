#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "debug.h"


#define NULL_ENCODED '\x1a'

//#define BUFFER_SIZE 3

static char _buffer[BUFFER_SIZE];

static FILE* _source;

static void parse_config(int argc, char** argv);
static void do_unpipe(size_t chars_read);

int main(int argc, char** argv) {
	parse_config(argc, argv);

	size_t chars_read;
	while ((chars_read = fread(_buffer, sizeof(char), BUFFER_SIZE, _source)) > 0) {
		do_unpipe(chars_read);
	}
	return 0;
}

static void print_help() {
	fprintf(stderr, "usage: csvunpipe [OPTIONS] [FILE]");
	fprintf(stderr, "options:");
	fprintf(stderr, "-p header,row,to,print\n");
	fprintf(stderr, "  Header row to print first\n");
}

static void parse_config(int argc, char** argv) {
	_source = stdin;
	char c;
	while ((c = getopt (argc, argv, "p:")) != -1) {
		switch (c) {
			case 'p':
				fwrite(optarg, sizeof(char), strlen(optarg), stdout);
				fwrite("\n", sizeof(char), 1, stdout);
				break;
			case '?':
			case 'h':
			default:
				print_help();
				exit(1);
				break;
		}
	}
	if (optind < argc) {
		_source = fopen(argv[optind], "r");
		if (_source) {
			fprintf(stderr, "Could not open file %s for reading\n", argv[optind]);
			exit(1);
		}
	}
}

static void do_unpipe(size_t chars_read) {
	char* restrict current_char = _buffer;
	char const* restrict char_end = _buffer + chars_read;

	while (current_char < char_end) {
		if (*current_char == '\0') {
			*current_char = '\n';
		}
		else if (*current_char == NULL_ENCODED) {
			*current_char = '\0';
		}
		current_char++;
	}
	fwrite(_buffer, sizeof(char), chars_read, stdout);
}
