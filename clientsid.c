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
void inputS();


/*  main()  */

int main(int argc, char *argv[]) {

    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *szAddress;             /*  Holds remote IP address   */
    char     *szPort;                /*  Holds remote port         */
    char     *endptr;                /*  for strtol()              */

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
    int c;
    int d;
    do {
         c = getchar();             // Gets the first input
         d = getchar();             // Gets the <enter> input, which is ignored
    } while (((c !='s') && (c != 't') && (c != 'q')));

    printf("\n");
    if (c == 'q'){
        printf("Quitting...\n");
        return 0;
    } else if (c == 's') {
        inputS(buffer, conn_s);
    } else {
        printf("input t");
    }

    return EXIT_SUCCESS;

}

    /*  Get string to echo from user  */
void inputS(char buffer[MAX_LINE], int conn_s){

    char      preStr[MAX_LINE];                // Holds CAP\n
    char      postStr[2];
    printf("You have selected s!\n");
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

    *szAddress = argv[1];                              // address is the second argument
    *szPort = argv[2];                                 // port is the third argument
    return 0;
}
