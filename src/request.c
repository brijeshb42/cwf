#include <stdlib.h>
#include <string.h>
#include "request.h"

static char *previous_header = NULL;
static int prev_header_length = 0;

static int on_header_field(http_parser *parser, const char *at, size_t length) {
	previous_header = (char*)at;
	prev_header_length = length;
	return 0;
}

static int on_header_value(http_parser *parser, const char *at, size_t length) {
	if (previous_header != NULL && prev_header_length) {
		char *key = malloc(sizeof(char) * (prev_header_length + 1));
		char *value = malloc(sizeof(char) * (length + 1));
		strncpy(key, previous_header, prev_header_length);
		key[prev_header_length] = '\0';
		strncpy(value, at, length);
		value[length] = '\0';
		Request *req = (Request*)parser->data;
		map_set(&req->headers, key, value);
	}
	return 0;
}

static int on_url(http_parser *parser, const char *at, size_t length) {
	Request *req = (Request*)parser->data;
	req->url = malloc(sizeof(char) * (length + 1));
	strncpy(req->url, at, length);
	return 0;
}

static int on_headers_complete(http_parser *p) {
    previous_header = NULL;
    prev_header_length = 0;
    return 0;
}

Request *init_request() {
    Request *req = malloc(sizeof(Request));
	req->url = NULL;
	map_init(&req->headers);
	return req;
}

void free_request(Request *req) {
	map_deinit(&req->headers);
	free(req->url);
	free(req);
}

static http_parser_settings parser_settings = {
    .on_message_begin = 0,
    .on_header_field = *on_header_field,
    .on_header_value = *on_header_value,
    .on_url = *on_url,
    .on_status = 0,
    .on_body = 0,
    .on_headers_complete = *on_headers_complete,
    .on_message_complete = 0,
    .on_chunk_header = 0,
    .on_chunk_complete = 0,
};

Request *parse_request(char *text, size_t length) {
	http_parser *parser = (http_parser*)malloc(sizeof(http_parser));

	if (!parser) {
		return NULL;
	}

	Request *r = init_request();
	parser->data = r;
	http_parser_init(parser, HTTP_REQUEST);
	int nparsed = http_parser_execute(parser, &parser_settings, text, length);
    on_headers_complete(parser);
	r->method = parser->method;
	if (nparsed != length) {
		free_request(r);
		return NULL;
	}
	free(parser);
	return r;
}
