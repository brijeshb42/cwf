#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<http_parser.h>
#include<map.h>

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
} request;

char *previous_header = NULL;
int prev_header_length = 0;

int on_header_field(http_parser *parser, const char *at, size_t length) {
	previous_header = (char*)at;
	prev_header_length = length;
	// char *test = malloc(sizeof(char)*(length + 1));
	// strncat(test, at, length);
	// printf("Header field received: %s\n", test);
	// printf("Header field received: %.*s, %zu\n", length, (at-length-1), length);
	return 0;
}

int on_header_value(http_parser *parser, const char *at, size_t length) {
	if (previous_header != NULL && prev_header_length) {
		char *key = malloc(sizeof(char) * (prev_header_length));
		char *value = malloc(sizeof(char) * (length));
		strncat(key, previous_header, prev_header_length);
		strncat(value, at, length);
		// printf("%s(%d): %s(%zu)\n", key, prev_header_length, value, length);
		request *req = (request*)parser->data;
		map_set(&req->headers, key, value);
	}
	// previous_header = at;
	// prev_header_length = length;
	// char *test = malloc(sizeof(char)*(length + 1));
	// strncat(test, at, length);
	// printf("Header value received: %s\n", test);
	// printf("Header field received: %.*s, %zu\n", length, (at-length-1), length);
	return 0;
}

request *init_request() {
	request *req = malloc(sizeof(request));
	map_init(&req->headers);
	return req;
}

void free_request(request *r) {
	map_deinit(&r->headers);
	free(r);
}

int main() {
	printf("http-parser version - %d.%d.%d\n", HTTP_PARSER_VERSION_MAJOR, HTTP_PARSER_VERSION_MINOR, HTTP_PARSER_VERSION_PATCH);
	http_parser *parser = (http_parser*)malloc(sizeof(http_parser));;
	http_parser_settings settings_null = {
		.on_message_begin = 0,
		.on_header_field = *on_header_field,
		.on_header_value = *on_header_value,
		.on_url = 0,
		.on_status = 0,
		.on_body = 0,
		.on_headers_complete = 0,
		.on_message_complete = 0,
		.on_chunk_header = 0,
		.on_chunk_complete = 0,
	};
	request *r = init_request();
	parser->data = r;
	http_parser_init(parser, HTTP_REQUEST);
	int nparsed = http_parser_execute(parser, &settings_null, test, 971);
	printf("%d\n", nparsed);
	printf("Method - %d - %d\n", parser->method, HTTP_GET);
	printf("http version: %d.%d\n", parser->http_major, parser->http_minor);
	const char *key;
	map_iter_t iter = map_iter(&r->headers);
	
	while((key = map_next(&r->headers, &iter))){
		printf("%s -> %s\n", key, *map_get(&r->headers, key));
	}

	free(r);
	free(parser);
	return 0;
}
