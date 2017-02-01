#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#ifndef PG_SOCK_HELP
#define PG_SOCK_HELP
#include <unistd.h>             /*  for ssize_t data type  */
#define LISTENQ        (1024)   /*  Backlog for listen()   */


/*  Function declarations  */

ssize_t Readline(int fd, void *vptr, size_t maxlen);
ssize_t Writeline(int fc, const void *vptr, size_t maxlen);
#endif 

/*  Global constants  */

#define MAX_LINE           (1000)


/*  Function declarations  */

int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort);
void substring(char[], char[], int, int);
void inputS(char[], int);
void inputT(char [], int );


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
    printf("Enter s for CAPs, t for copying, or q for quitting.\n");
    do {
         c = getchar();             // Gets the first input
         d = getchar();             // Gets the <enter> input, which is ignored
    } while (((c !='s') && (c != 't') && (c != 'q')));

    printf("\n");
    if (c == 'q'){
        printf("Quitting...\n");    //Quits when user enters q
        return 0;
    } else if (c == 's') {          // If the user enters s, inputS function is called.
        inputS(buffer, conn_s);
    } else {
        inputT(buffer, conn_s);     //If the user enters s, inputT function is called.
    }

    return EXIT_SUCCESS;

}


void inputS(char buffer[MAX_LINE], int conn_s){ //A function to deal with capitalizing input.

    char      preStr[MAX_LINE];                // Holds CAP\n
    char      postStr[2];
    printf("You have selected s!\n");
    printf("Enter: \n");

    char c;
    int i = 0;

    while (( c = getchar()) != '\n')            //Getting user input char by char in order to avoid <enter>\n.
        buffer[i++] = c;                       // insert NULL at the end of string

    buffer[i] = '\0';

    strcpy(preStr, "CAP\\n");           // Stores CAP\\n to preStr variable
    strcpy(postStr, "\\n");             // Stores \\n to 
    strcat(preStr, buffer);             // Concatenates and stores the value in preStr    
    strcat(preStr, postStr);
    preStr[strlen(preStr)] = '\n';      //Adding newline character

    /*  Send string to echo server, and retrieve response  */

    Writeline(conn_s, preStr, strlen(preStr));
    Readline(conn_s, buffer, MAX_LINE-1);       //Get the input capitalized from the server
    printf("\nResponse: %s ", buffer);

    if ( close(conn_s) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error calling close()\n");
        exit(EXIT_FAILURE);
    }

}

void inputT(char buffer[MAX_LINE], int conn_s){     //A function to deal with creating files according to the input.
    char      preStr[MAX_LINE];                     // Holds CAP\n
    char      postStr[2];
    printf("You have selected t!\n");
    printf("Enter: \n");

    char c;
    int i = 0;

    while (( c = getchar()) != '\n')
        buffer[i++] = c;
    
    // insert NULL at the end of string

    buffer[i] = '\0';

    strcpy(preStr, "FILE\\n\0");           // Stores CAP\\n to preStr variable
    strcpy(postStr, "\\n\0");             // Stores \\n to 
    strcat(preStr, buffer);
    strcat(preStr, postStr);
    preStr[strlen(preStr)] = '\n';

    /*  Send string to echo server, and retrieve response  */

    Writeline(conn_s, preStr, strlen(preStr));      //Concatenating and sending the output to the server

    Readline(conn_s, buffer, MAX_LINE-1);
    int j = 0;
    char charCheck[10];
    while (buffer[j] != '\n'){                      //Searching for newline character to detect the file size.
        // if (buffer[j] != '\n') {
            charCheck[j] = buffer[j];
            j = j + 1;    
    }
    printf("char check %s\n", charCheck );


    /*  Output echoed string  */

    if ( close(conn_s) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error calling close()\n");
        exit(EXIT_FAILURE);
    }
}

void substring(char s[], char sub[], int p, int l) {            //This function helps obtain the substring of a string. The parameter s is for source, sub is for the resultant substring, p is for the starting position, and l for length.
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

ssize_t Readline(int sockd, void *vptr, size_t maxlen) {
    ssize_t n, rc;
    char    c, *buffer;

    buffer = vptr;

    for ( n = 1; n < maxlen; n++ ) {
    
    if ( (rc = read(sockd, &c, 1)) == 1 ) {
        *buffer++ = c;
        if ( c == '\n' )
        break;
    }
    else if ( rc == 0 ) {
        if ( n == 1 )
        return 0;
        else
        break;
    }
    else {
        if ( errno == EINTR )
        continue;
        return -1;
    }
    }

    *buffer = 0;
    return n;
}


/*  Write a line to a socket  */

ssize_t Writeline(int sockd, const void *vptr, size_t n) {
    size_t      nleft;
    ssize_t     nwritten;
    char *temp_buffer;
    const char *buffer;

    buffer = vptr;
    nleft  = n;

    while ( nleft > 0 ) {
    if ( (nwritten = write(sockd, buffer, nleft)) <= 0 ) {
        if ( errno == EINTR )
        nwritten = 0;
        else
        return -1;
    }
    nleft  -= nwritten;
    buffer += nwritten;
    }

    return n;
}
