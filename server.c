#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN 256

struct file_meta {
        char file_name[MAX_LEN];
        long int file_size;
};

int main(int argc, char **argv)
{
int sock_fd = 0;
int new_fd = 0, has_bytes = 0;
char buff[MAX_LEN];
struct sockaddr_in server_addr, client_addr;
int socket_len = 0, ret_val = 0, client_length=0;
struct file_meta file_metadata;
FILE *fp;

        if ( argc != 3 ) {
                printf("USAGE : ./serv <ip> <port>\n");
                exit(0);
        }

        /* Create a socket */
        sock_fd = socket(PF_INET, SOCK_STREAM, 0);
        if ( sock_fd == -1 ) {
                printf("Socket creation error\n"); exit(0);
        }
        /* Bind the socket */
        //Address family
        server_addr.sin_family = AF_INET;
        //Set port number, use htons to ensure proper byte order
        server_addr.sin_port = htons(atoi(argv[2]));
        //Set IP address of this server
        //server_addr.sin_addr.s_addr = INADDR_ANY; To listen on all interfaces in this server
        server_addr.sin_addr.s_addr = inet_addr(argv[1]);
        ret_val = bind(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
        if ( ret_val == -1 ) {
                printf("Socket bind failed\n"); exit(0);
        }
        /* Specify the listen queue length for this socket */
        ret_val = listen(sock_fd, 1024);
        if ( ret_val == -1 ) {
                printf("Listen failed\n"); exit(0);
        }

        /* Wait on accept */
        has_bytes = 0;
        for ( ; ; ) {
                printf("Waiting on accept file transfer connections : \n");
                client_length=sizeof(client_addr);
                new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_length);
                if ( ret_val == -1 ) {
                        printf("Accept error\n"); exit(0);
                } else {
                        printf("Received Connection From : %s\n", inet_ntoa(client_addr.sin_addr));
                }

                //Read the file metadata
                ret_val = 0;
                for ( ; ; ) {
                        ret_val += read(new_fd, &file_metadata, sizeof(file_metadata));
                        if ( ret_val == -1 ) {
                                printf("Read failed\n"); exit(0);
                        }
                        if ( ret_val == sizeof(file_metadata)) {
                                break;
                        } else {
                                continue;
                        }
                }
                printf("Initiation of file transfer : FileName : %s, FileSize : %u\n", file_metadata.file_name, file_metadata.file_size);
                fp = fopen(file_metadata.file_name, "a");
                if ( fp == NULL ) {
                        printf("Error creating file : %s\n", file_metadata.file_name);exit(0);
                }

                ret_val=0;
                for ( ; ; ) {
                        ret_val = read(new_fd, buff, sizeof(buff));
                        has_bytes += ret_val;

                        if ( ret_val == -1 ) {
                                printf("Read failed\n"); exit(0);
                        }

                        if ( has_bytes == file_metadata.file_size ) {
                                buff[ret_val]='\0';
                                fputs(buff, fp);
                                fclose(fp);
                                printf("\nCompleted transfer of file : FileName : %s, FileSize : %u\n", file_metadata.file_name, ret_val);
                                break;
                        } else {
                                buff[ret_val]='\0';
                                fputs(buff, fp);
                                printf("#");
                        }
                }
                close(new_fd);
        }
        close(sock_fd);
}/*End of main*/