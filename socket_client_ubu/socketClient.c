/*
    C ECHO client example using sockets
*/
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <curses.h>

int main(int argc , char *argv[])
{
    int sock, index;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];


    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    printf("Socket created %d", sock);

    server.sin_addr.s_addr = inet_addr("192.168.1.22");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    //keep communicating with server
    char counter = 65;
    while(1)
    {
//    	for(index = 0; index < strlen(message); index++){
//    		message[index] = '\0';
//    		server_reply[index] = '\0';
//    	}
    	memset(&message, 0, 1000 * sizeof(char));
    	memset(&server_reply, 0, 1000 * sizeof(char));
    	//send(sock, message, strlen(message)+1, 0);
    	printf("Before send: %s", message);

        printf("Enter message : ");
        scanf("%s" , message);
        message[1] = '\0';
        message[0] = counter++;
        printf("Sending %d as str %s\n", message[0], message);
        //Send some data
        if( send(sock , message , strlen(message)+1 , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }

        //Receive a reply from the server
        //oczekiwanie na dane dobrze dorzucić
        //sprawdzić w dokumentacji funkcję recv, ile znaków odbiera
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }

        printf("Remote Server reply: [%s]\n", server_reply);

    }

    close(sock);

    return 0;
}
