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
    char      temp[MAX_LINE];
    char      preStr[MAX_LINE];                // Holds CAP\n
    char      postStr[MAX_LINE];
    //char      usrInput;
    char      inputUsr[MAX_LINE];


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

    // printf("Enter s,t or f: ");                                 //printf("Enter the string to echo: ");
    // scanf("%c", &usrInput);
    // printf("Entered %c", usrInput);
    // return 0;
    // while ( usrInput != 's' && usrInput !='t' && usrInput != 'f' ){
    //     if ( usrInput == 's') {
    //         printf("Enter a string: ");
    //     } else if ( usrInput == 't') {
    //         printf("Enter a string: ");
    //     } else
    //         printf("Exiting...");
    //         exit(0);
    // }
    printf("Input s: Enter: \n");

    // int i = 0;
    // char choice;
    // while((choice=fgetc(stdin)) != '\n')
    //     buffer[i++] = choice;
    //fgets(buffer, MAX_LINE, stdin);
    // char *newline = strchr( buffer, '\n');
    // if ( newline )
    //     *newline = 0;
    //strncpy(buffer, buffer, strlen(buffer));
    //Concatenation
    //printf("buffer: %s", buffer);
    //substring(buffer, buffer, 1, strlen(buffer)-1);       //Removing the newline character
    //buffer[strlen(buffer)-1] = '\0';

    //printf("temp is: %s", temp );
    // copy_string(buffer, temp);
    // printf("Newbuffer: %s", buffer);
    //strcpy(buffer, temp);
    // char *found;
    // found = strtok(buffer,"\n");
    // found[strlen(found)] = '\0';
    //printf("found: %s", found );
    //char* p = strtok(buffer, "\n")
    //strcpy(temp, buffer);
    //buffer[strlen(buffer)-1] = 0;
    //buffer[strlen(buffer)] = '\0';

    char c;
    int i = 0;

    while (( c = getchar()) != '\n')
        buffer[i++] = c;
    // insert NULL at the end of string

    buffer[i] = '\0';
    // printf("to: %s", buffer);
    // char *ptr;
    // if ((ptr= strchr(buffer, '\n')) == NULL)
    //     *ptr = '\0'; 
    //removeStringTrailingNewline(buffer);
    //printf("new: %s \n", buffer);
    //strcpy(buffer, inputUsr);
    // size_t len = strlen(buffer)-1;
    // if ( buffer[len] == '\n')
    //     buffer[len] = '\0';
    strcpy(preStr, "CAP\\n");
    strcpy(postStr, "\\n");
    strcat(preStr, buffer);
    strcat(preStr, postStr);
    //strcpy(temp, preStr);

    printf("Sending this to server: %s \n", preStr);

    
    

    /*  Send string to echo server, and retrieve response  */

    Writeline(conn_s, preStr, strlen(preStr));
    printf("Sent to server\n");
    Readline(conn_s, buffer, MAX_LINE-1);

    //printf("Client Side %s\n", buffer );


    /*  Output echoed string  */

    printf("Echo response: %s\n", buffer);

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

void copy_string(char d[], char s[]) {
   int c = 0;
 
   while (s[c] != '\0') {
      d[c] = s[c];
      c++;
   }
   d[c] = '\0';
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