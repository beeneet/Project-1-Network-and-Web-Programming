/*

  ECHOSERV.C
  ==========
  (c) Paul Griffiths, 1999
  Email: mail@paulgriffiths.net
  
  Simple TCP/IP echo server.

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "helper.h"           /*  our own helper functions  */
#include "helper.c"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


/*  Global constants  */

#define ECHO_PORT          (2002)
#define MAX_LINE           (1000)

void substring(char s[], char sub[], int p, int l) ;
void capitalize(char source[], char destination[]);
int main(int argc, char *argv[]) {
    int       list_s;                /*  listening socket          */
    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *endptr;                /*  for strtol()              */
    char      temp[MAX_LINE];
    char      lineBreak[2];
    char      lengthChar[2];        //For storing the string value of length


    /*  Get port number from the command line, and
        set to default port if no arguments were supplied  */

    if ( argc == 2 ) {
	port = strtol(argv[1], &endptr, 0);
	if ( *endptr ) {
	    fprintf(stderr, "ECHOSERV: Invalid port number.\n");
	    exit(EXIT_FAILURE);
	}
    }
    else if ( argc < 2 ) {
	port = ECHO_PORT;
    }
    else {
	fprintf(stderr, "ECHOSERV: Invalid arguments.\n");
	exit(EXIT_FAILURE);
    }

	
    /*  Create the listening socket  */

    if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
	exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);


    /*  Bind our socket addresss to the 
	listening socket, and call listen()  */

    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
	fprintf(stderr, "ECHOSERV: Error calling bind()\n");
	exit(EXIT_FAILURE);
    }

    if ( listen(list_s, LISTENQ) < 0 ) {
	fprintf(stderr, "ECHOSERV: Error calling listen()\n");
	exit(EXIT_FAILURE);
    }

    
    /*  Enter an infinite loop to respond
        to client requests and echo input  */

    while ( 1 ) {

	/*  Wait for a connection, then accept() it  */

	if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 ) {
	    fprintf(stderr, "ECHOSERV: Error calling accept()\n");
	    exit(EXIT_FAILURE);
	} else {
    printf("Connection accepted\n");
  }


	/*  Retrieve an input line from the connected socket
	    then simply write it back to the same socket.     */

	Readline(conn_s, buffer, MAX_LINE-1);
  printf("Incoming %s", buffer );                 //Prints the incoming message for testing purpose

  substring(buffer, buffer, 6, strlen(buffer)-8);
  printf("Client message: %s", buffer);           // Obtains actual user input after removing CAP\nxx\n
  printf("\n");

  capitalize(buffer, buffer);                     // Capitalizes and stores value in buffer
  printf("Capitalized: %s", buffer);              // Printing for testing purpose

  int lengthOfCaptitalize = strlen(buffer);       // Obtains length of the Capitalized string
  printf("\n");
  printf("Length is %d\n", lengthOfCaptitalize ); // Prints the length:: for testing purpose

  strcpy(lineBreak, "\\n");
  sprintf(lengthChar, "%d", lengthOfCaptitalize);   //converting int to char for concatenation

  sprintf(temp, "%s%s", lengthChar, lineBreak);     //temp = length of string + \n
  sprintf(temp, "%s%s", temp, buffer); //Obtaining <length>\n
  printf("temp is %s\n", temp );

	Writeline(conn_s, temp, strlen(temp));           // Sending back to client

	/*  Close the connected socket  */

	if ( close(conn_s) < 0 ) {
	    fprintf(stderr, "ECHOSERV: Error calling close()\n");
	    exit(EXIT_FAILURE);
	} else {
      printf("Connection Closed\n");
    }
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

void capitalize(char source[], char destination[]) {
  int l = strlen(source);
  int c = 0;
  while (c < l) {
    destination[c] = toupper(source[c]);
    c++;
  }
  destination[c] = '\0';
}

