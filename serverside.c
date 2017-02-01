#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "helper.h"
#include <unistd.h>
#include <errno.h>
#ifndef PG_SOCK_HELP
#define PG_SOCK_HELP
#include <unistd.h>             /*  for ssize_t data type  */
#define LISTENQ        (1024)   /*  Backlog for listen()   */
/*  Function declarations  */
ssize_t Readline(int fd, void *vptr, size_t maxlen);
ssize_t Writeline(int fc, const void *vptr, size_t maxlen);
#endif 




/*  Global constants  */

#define ECHO_PORT          (2002)
#define MAX_LINE           (1000)

void substring(char s[], char sub[], int p, int l) ;
void capitalize(char source[], char destination[]);
void CAPfunction(char temp[], char buffer[], int);
unsigned long getSize(char * );
void FILEfunction(char [], char [], int);
int main(int argc, char *argv[]) {
    int       list_s;                /*  listening socket          */
    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *endptr;                /*  for strtol()              */
    char      temp[MAX_LINE];
    

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
  int indicator = 2;
  if (buffer[0] == 'C'){
    indicator = 0;                  // Callng the CAP function. If CAP\nxxx\n is received
  } else if (buffer[0] == 'F'){
    indicator = 1;
  }

  if (indicator == 0){
    substring(buffer, buffer, 6, strlen(buffer)-8);
    printf("\n");
    CAPfunction(temp, buffer, conn_s);                    // Callng the CAP function. If CAP\nxxx\n is received
  } else {
    substring(buffer, buffer, 7, strlen(buffer)-9);
    printf("Client message: %s", buffer);           // Obtains actual user input after removing CAP\nxx\n
    printf("\n");
    FILEfunction(temp, buffer, conn_s);
  }
  
 
	/*  Close the connected socket  */

	if ( close(conn_s) < 0 ) {
	    fprintf(stderr, "ECHOSERV: Error calling close()\n");
	    exit(EXIT_FAILURE);
	} else {
      printf("Connection Closed\n");
    }
  }
}

void CAPfunction(char temp[], char buffer[], int conn_s){

  char      lineBreak[2];
  char      lengthChar[2];        //For storing the string value of length

  capitalize(buffer, buffer);                     // Capitalizes and stores value in buffer

  int lengthOfCaptitalize = strlen(buffer);       // Obtains length of the Capitalized string

  strcpy(lineBreak, "\\n");
  sprintf(lengthChar, "%d", lengthOfCaptitalize);   //converting int to char for concatenation

  sprintf(temp, "%s%s", lengthChar, lineBreak);     //temp = length of string + \n
  sprintf(temp, "%s%s", temp, buffer); //Obtaining <length>\n

  temp[strlen(temp)] = '\n';                        // Adding \n. Used for detecting end by Readline()

   Writeline(conn_s, temp, strlen(temp));  

}

void FILEfunction(char temp[], char buffer[], int conn_s){    //This function gets the contents of the file, concatenates it and sends it back to the client for creating a file.
   char bytesChar[MAX_LINE];                                  //For storing number of bytes as char
   char lineBreak[2];
   int i;
   FILE *input;
   char newInt[MAX_LINE];
   unsigned long abc;
   abc = getSize(buffer);
   char st[MAX_LINE];
   input = fopen(buffer, "rb");
   fread(st, sizeof(st), abc, input);
   sprintf(bytesChar, "%ld", abc);
   strcpy(lineBreak, "\\n");
   strcat(bytesChar, lineBreak);
   strcat(bytesChar, st);
   printf("BytesCHAR %s\n", bytesChar );
   bytesChar[MAX_LINE] = '\0';
   fclose(input);

   Writeline(conn_s, bytesChar, strlen(bytesChar));  

}

void substring(char s[], char sub[], int p, int l) {          // This function generates the substring of a string.
   int c = 0;
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}

void capitalize(char source[], char destination[]) {        //Function for capitalizing strings. It capitalises string char by char.
  int l = strlen(source);
  int c = 0;
  while (c < l) {
    destination[c] = toupper(source[c]);
    c++;
  }
  destination[c] = '\0';
}

unsigned long getSize(char * filename)                      // This function generates the size of the file.
{
    FILE *fp;
    filename[strlen(filename)] = '\0';
    fp = fopen(filename, "rb");
    if (!fp){
      return -1;
    }
    fseek(fp, 0, SEEK_END);

    unsigned long sz = (unsigned long)ftell(fp);
    fclose(fp);
    printf("%ld\n", sz );
    return sz;
}

ssize_t Readline(int sockd, void *vptr, size_t maxlen) {    //Helper functions for reading line
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

