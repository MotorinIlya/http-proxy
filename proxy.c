#include "proxy.h"

int server_is_on = 1;
sem_t thread_semaphore;

void* execute_client_request(void *arg) 
{
    context *ctx = (context *) arg;
    int client_socket = ctx->client_socket;
    char *request0 = ctx->request;
    char request[MAX_BUFFER_SIZE];
    strcpy(request, request0);

    unsigned char host[HOST_SIZE];
    const unsigned char *host_result = memccpy(host, strstr((char *) request, HOST) + 6, '\r', sizeof(host));
    host[host_result - host - 1] = END_STR;
    logg_char("Remote server host name: ", (char *) host, GREEN);

    int dest_socket = connect_to_remote((char *) host);
    if (dest_socket == SOCKET_ERROR) 
    {
        close(client_socket);
    }
    logg("Create new connection with remote server", GREEN);

    ssize_t bytes_sent = write(dest_socket, request, strlen(request));
    if (bytes_sent == WRITE_ERROR) 
    {
        perror("Error while sending request to remote server");
        close(client_socket);
        close(dest_socket);
        return NULL;
    }
    logg_int("Send request to remote server, len = ", bytes_sent, GREEN);

    char *buffer = calloc(MAX_BUFFER_SIZE, sizeof(char));
    ssize_t bytes_read, all_bytes_read = 0;
    while ((bytes_read = read(dest_socket, buffer, MAX_BUFFER_SIZE)) > 0) 
    {
        bytes_sent = write(client_socket, buffer, bytes_read);
        all_bytes_read += bytes_read;
    }

    close(client_socket);
    close(dest_socket);
    free(buffer);
    free(request0);

    sem_post(&thread_semaphore);
    return NULL;
}

void accept_new_client(int server_socket) 
{
    int client_socket;
    while (server_is_on) 
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_size);
        if (client_socket == SOCKET_ERROR) 
        {
            perror("Error to accept");
            close(server_socket);
            exit(EXIT_FAILURE);
        }
        char *buff = calloc(MAX_BUFFER_SIZE, sizeof(char));
        sprintf(buff, "Client connected from %s:%d", inet_ntoa(client_addr.sin_addr),
                                                        ntohs(client_addr.sin_port));
        logg(buff, BLUE);
        free(buff);

        char *request = calloc(MAX_BUFFER_SIZE, sizeof(char));
        assert(request != NULL);
        int err = read_request(client_socket, request);
        if (err == EXIT_FAILURE) 
        {
            logg("Failed to read request", RED);
            free(request);
            close(client_socket);
            continue;
        }

        sem_wait(&thread_semaphore);
        logg("Init new connection", PURPLE);
        context ctx = {client_socket, request};
        pthread_t handler_thread;
        err = pthread_create(&handler_thread, NULL, execute_client_request, &ctx);
        if (err == PTHREAD_ERROR) 
        {
            perror("Failed to create thread");
            close(client_socket);
            close(server_socket);
            exit(EXIT_FAILURE);
        }
    }
}

void run_proxy() 
{
    sem_init(&thread_semaphore, 0, MAX_USERS_COUNT);
    int server_socket = create_server_socket();
    if (server_socket == SOCKET_ERROR) 
    {
        perror("Error to create server socket");
        exit(EXIT_FAILURE);
    }

    logg_int("Server listening on port ", PORT, GREEN);

    accept_new_client(server_socket);
    close(server_socket);
    sem_destroy(&thread_semaphore);
}

