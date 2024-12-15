#include "logger.h"

void logg(char *msg, char *color) 
{
    pthread_t thread_id = pthread_self();
    if (strcmp(color, RED) == 0) 
    {
        char buf[LOG_BUFFER_SIZE];
        sprintf(buf, "%s[Thread %ld] %s%s", color, thread_id, msg, RESET);
        perror(buf);
    } 
    else 
    {
        printf("%s[Thread %ld] %s%s\n", color, thread_id, msg, RESET);
    }
    fflush(stdout);
}

void logg_char(char *msg, char *info, char *color) 
{
    char buf[LOG_BUFFER_SIZE + 100];
    sprintf(buf, "%s %s", msg, info);
    logg(buf, color);
}

void logg_int(char *msg, long info, char *color) 
{
    char buf[LOG_BUFFER_SIZE + 100];
    sprintf(buf, "%s%ld", msg, info);
    logg(buf, color);
}