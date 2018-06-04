#ifndef _RAF_REQUEST
#define _RAF_REQUEST

#include <http_parser.h>
#include <map.h>

typedef struct {
	unsigned int method;
	map_str_t headers;
	char *url;
	char *method_str;
} Request;

Request *init_request();
void free_request(Request*);
Request *parse_request(char*, size_t);

char **get_header_value(Request*, char*);

#endif
