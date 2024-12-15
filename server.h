#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#include "logger.h"

int create_server_socket();

int connect_to_remote(char *host);

int read_request(int client_socket, char *request);