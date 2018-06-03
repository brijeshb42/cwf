#include <stdlib.h>
#include <uv.h>
#include "server.h"
#include "request.h"

void free_write_req(uv_write_t *req) {
    write_req_t *wr = (write_req_t*)req;
    free(wr->buf.base);
    free(wr);
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = (char*) malloc(suggested_size);
    buf->len = suggested_size;
}

void echo_write(uv_write_t *req, int status) {
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    free_write_req(req);
}

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread > 0) {
        write_req_t *req = malloc(sizeof(write_req_t));
        req->buf = uv_buf_init(buf->base, nread);
        uv_write((uv_write_t*) req, client, &req->buf, 1 , echo_write);

        Request *request = parse_request(req->buf.base, req->buf.len);

        if (request == NULL) {
            fprintf(stderr, "Error while parsing request\n");
        } else {
            fprintf(stdout, "Method - %d\n", request->method);
            fprintf(stdout, "Url - %s\n", request->url);
            const char *key;
            map_iter_t iter = map_iter(&request->headers);
            while((key = map_next(&request->headers, &iter))){
                fprintf(stdout, "%s -> %s\n", key, *map_get(&request->headers, key));
            }
            free_request(request);
        }
    } else {
        if (nread != UV_EOF) {
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        }
    }

    uv_close((uv_handle_t*) client, NULL);
    
    if (nread <= 0) {
        free(buf->base);
    }
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New Connection error %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(server->loop, client);

    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, alloc_buffer, echo_read);
    } else {
        uv_close((uv_handle_t *) client, NULL);
    }
}

int start_server(char *host, int port) {
    struct sockaddr_in addr;
    uv_loop_t *loop = uv_default_loop();

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    uv_ip4_addr(host, port, &addr);

    uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*) &server, 128, on_new_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return r;
    }

    fprintf(stdout, "Started listening on : %s:%d\n", host, port);

    return uv_run(loop, UV_RUN_DEFAULT);
}
