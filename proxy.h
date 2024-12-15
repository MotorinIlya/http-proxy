#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <assert.h>
#include <stdlib.h>

#include "server.h"

typedef struct {
    int client_socket;
    char *request;
} context;

void run_proxy();