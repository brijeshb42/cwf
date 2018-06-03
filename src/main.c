#include <stdio.h>
#include <stdlib.h>
#include "server.h"

int main() {
	int status = start_server("0.0.0.0", 8080);

	if (status != 0) {
		printf("Error while starting server\n");
	}

	return 0;
}
