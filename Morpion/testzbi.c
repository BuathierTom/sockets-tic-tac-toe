#include <stdio.h>
#include <stdbool.h>   
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */


int main(int argc, char *argv[]){

    char tabtab[100];
    char rep[100];


    int var;
    char zbi;

    sprintf(tabtab, "%d", 78);
    sprintf(tabtab, "%s", 'CONTINUEEEE');
    printf("le int: %s\n", tabtab[0]);
    printf("le char: %d\n", tabtab[1]);



    sscanf(rep, "%d", var);
    sscanf(rep, "%s", zbi);
    printf("le int: %s\n", rep[0]);
    printf("le char: %d\n", rep[1]);






    return 0;


}