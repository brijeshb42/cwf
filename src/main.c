#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <http_parser.h>
#include <map.h>

char *test = "GET /brijeshb42/e241b1dc89d1801270f2adb08b2109ea HTTP/1.1\n\
Host: gist.github.com\n\
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.13; rv:61.0) Gecko/20100101 Firefox/61.0\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\n\
Accept-Language: en-US,en;q=0.5\n\
Accept-Encoding: gzip, deflate, br\n\
Referer: https://gist.github.com/\n\
Cookie: _ga=1; _octo=1; logged_in=yes; dotcom_user=user; gist_user_session=sessioid;\n\
DNT: 1\n\
Connection: keep-alive\n\
Upgrade-Insecure-Requests: 1\n\
Cache-Control: max-age=0";

typedef struct {
	map_str_t headers;
	char *url;
	int method;
	char *method_str;
} Request;

char *previous_header = NULL;
int prev_header_length = 0;

int on_header_field(http_parser *parser, const char *at, size_t length) {
	previous_header = (char*)at;
	prev_header_length = length;
	return 0;
}

int on_header_value(http_parser *parser, const char *at, size_t length) {
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

int on_url(http_parser *parser, const char *at, size_t length) {
	Request *req = (Request*)parser->data;
	req->url = malloc(sizeof(char) * (length + 1));
	strncpy(req->url, at, length);
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

Request *parse_request(char *text, size_t length) {
	http_parser *parser = (http_parser*)malloc(sizeof(http_parser));

	if (!parser) {
		return NULL;
	}

	http_parser_settings settings_null = {
		.on_message_begin = 0,
		.on_header_field = *on_header_field,
		.on_header_value = *on_header_value,
		.on_url = *on_url,
		.on_status = 0,
		.on_body = 0,
		.on_headers_complete = 0,
		.on_message_complete = 0,
		.on_chunk_header = 0,
		.on_chunk_complete = 0,
	};
	Request *r = init_request();
	parser->data = r;
	http_parser_init(parser, HTTP_REQUEST);
	int nparsed = http_parser_execute(parser, &settings_null, text, length);
	if (nparsed != length) {
		return NULL;
	}
	r->method = parser->method;
	free(parser);
	return r;
}

int main() {
	Request *r = parse_request(test, 516);

	if (r == NULL) {
		exit(1);
	}

	printf("URL - %s\n", r->url);
	printf("Method - %d - %d\n", r->method, HTTP_GET);
	const char *key;
	map_iter_t iter = map_iter(&r->headers);
	
	while((key = map_next(&r->headers, &iter))){
		printf("%s -> %s\n", key, *map_get(&r->headers, key));
	}

	free_request(r);
	return 0;
}
