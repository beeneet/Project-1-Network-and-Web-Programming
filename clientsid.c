/*

  ECHOCLNT.C
  ==========
  (c) Paul Griffiths, 1999
  Email: mail@paulgriffiths.net
  
  Simple TCP/IP echo client.

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "helper.h"           /*  Our own helper functions  */
#include "helper.c"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>

/*  Global constants  */

#define MAX_LINE           (1000)


/*  Function declarations  */

int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort);
void substring(char[], char[], int, int);


/*  main()  */

int main(int argc, char *argv[]) {

    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *szAddress;             /*  Holds remote IP address   */
    char     *szPort;                /*  Holds remote port         */
    char     *endptr;                /*  for strtol()              */
    char      preStr[MAX_LINE];                // Holds CAP\n
    char      postStr[2];

    /*  Get command line arguments  */

    ParseCmdLine(argc, argv, &szAddress, &szPort);


    /*  Set the remote port  */

    port = strtol(szPort, &endptr, 0);
    if ( *endptr ) {
	printf("ECHOCLNT: Invalid port supplied.\n");
	exit(EXIT_FAILURE);
    }
	

    /*  Create the listening socket  */

    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");
	exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);


    /*  Set the remote IP address  */

    if ( inet_aton(szAddress, &servaddr.sin_addr) <= 0 ) {
	printf("ECHOCLNT: Invalid remote IP address.\n");
	exit(EXIT_FAILURE);
    }

    
    /*  connect() to the remote echo server  */

    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
	printf("ECHOCLNT: Error calling connect()\n");
	exit(EXIT_FAILURE);
    }


    /*  Get string to echo from user  */

    printf("Input s: Enter: \n");

    char c;
    int i = 0;

    while (( c = getchar()) != '\n')
        buffer[i++] = c;
    // insert NULL at the end of string

    buffer[i] = '\0';

    strcpy(preStr, "CAP\\n");           // Stores CAP\\n to preStr variable
    strcpy(postStr, "\\n");             // Stores \\n to 
    strcat(preStr, buffer);
    strcat(preStr, postStr);
    preStr[strlen(preStr)] = '\n';

    printf("Sending this to server: %s", preStr);

    
    

    /*  Send string to echo server, and retrieve response  */

    Writeline(conn_s, preStr, strlen(preStr));
    printf("Sent to server\n");
    Readline(conn_s, buffer, MAX_LINE-1);

    /*  Output echoed string  */
        


    printf("Echo response: %s", buffer);
    printf("\n");

    if ( close(conn_s) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error calling close()\n");
        exit(EXIT_FAILURE);
    }


    return EXIT_SUCCESS;
}


void substring(char s[], char sub[], int p, int l) {
   int c = 0;
 
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}


int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort) {

    int n = 1;

    while ( n < argc ) {
    if ( !strncmp(argv[n], "-a", 2) || !strncmp(argv[n], "-A", 2) ) {
        *szAddress = argv[++n];
    }
    else if ( !strncmp(argv[n], "-p", 2) || !strncmp(argv[n], "-P", 2) ) {
        *szPort = argv[++n];
    }
    else if ( !strncmp(argv[n], "-h", 2) || !strncmp(argv[n], "-H", 2) ) {
        printf("Usage:\n\n");
        printf("    timeclnt -a (remote IP) -p (remote port)\n\n");
        exit(EXIT_SUCCESS);
    }
    ++n;
    }

    return 0;
}
