#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN 256

int main(int argc, char **argv)
{
int client_socket = 0;
char buff[MAX_LEN];
struct sockaddr_in serverAddr;
int ret_val= 0;

        if ( argc != 3 ) {
                printf("USAGE : ./serv <ip> <port>\n");
                exit(0);
        }

        /* Create a socket */
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if ( client_socket == -1 ) {
                printf("Client socket creation failed\n"); exit(0);
        }

        /*---- Configure settings of the server address struct ----*/
        /* Address family = Internet */
        serverAddr.sin_family = AF_INET;
        /* Set port number, using htons function to use proper byte order */
        serverAddr.sin_port = htons(atoi(argv[2]));
        /* Set IP address to localhost */
        serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

        /*---- Connect the socket to the server using the address struct ----*/
        ret_val = connect(client_socket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
        if ( ret_val == -1 ) {
                printf("Error in connect\n"); exit(0);
        }

        ret_val = write(client_socket, "Hello World", sizeof("Hello World"));
        if ( ret_val == -1 ) {
                printf("Write failed\n");exit(0);
        }

        close(client_socket);
}
