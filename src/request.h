#ifndef _RAF_REQUEST
#define _RAF_REQUEST

#include <http_parser.h>
#include <map.h>

typedef struct {
	map_str_t headers;
	char *url;
	int method;
	char *method_str;
} Request;

Request *init_request();
void free_request(Request*);
Request *parse_request(char*, size_t);

#endif
