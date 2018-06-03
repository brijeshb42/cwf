#ifndef _RAF_SERVER_H
#define _RAF_SERVER_H

#include <uv.h>

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

int start_server(char *, int);

#endif
