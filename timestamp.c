
#include "timestamp.h"

char *timestamp(){
    time_t current_time;
    char* c_time_string;

    /* Obtain current time. */
    current_time = time(NULL);

    if (current_time == ((time_t)-1))
    {
        (void) fprintf(stderr, "Failure to obtain the current time.\n");
        exit(EXIT_FAILURE);
    }

    /* Convert to local time format. */
    c_time_string = ctime(&current_time);

    if (c_time_string == NULL)
    {
        (void) fprintf(stderr, "Failure to convert the current time.\n");
        exit(EXIT_FAILURE);
    }
    c_time_string[strlen(c_time_string)-1] = '\0';
    return c_time_string;
}

/*int main(void)
{
    char * ts = timestamp();
    printf("Current time is : |%s| %ld\n", ts,strlen(ts));
    exit(EXIT_SUCCESS);
}*/