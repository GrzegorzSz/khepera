/*
    C ECHO client example using sockets
*/
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <unistd.h>
#include <termios.h>

int mygetch( )
{
   struct termios stary, nowy;
   int dozwrocenia;
   tcgetattr( STDIN_FILENO, &stary );
   nowy = stary;
   nowy.c_lflag &= ~( ICANON | ECHO );
   tcsetattr( STDIN_FILENO, TCSANOW, &nowy );
   dozwrocenia = getchar();
   tcsetattr( STDIN_FILENO, TCSANOW, &stary );
   return dozwrocenia;
}

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

    server.sin_addr.s_addr = inet_addr("192.168.1.28");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    printf("Connected\n");

    //keep communicating with server
    char c = 0;
    char newSpeed[5];
    char azimuth[10];
    char *chr1, *chr2;
    while(1)
    {
//    	for(index = 0; index < strlen(message); index++){
//    		message[index] = '\0';
//    		server_reply[index] = '\0';
//    	}
    	memset(&message, 0, 1000 * sizeof(char));
    	memset(&server_reply, 0, 1000 * sizeof(char));
    	//send(sock, message, strlen(message)+1, 0);
//    	printf("Before send: %s", message);
//
//        printf("Enter message : ");
//        scanf("%s" , message);
        message[1] = '\0';
        message[0] = getchar();
        c = message[0];
        printf("wpisane: %c\n", c);
        if(c == 's' || c == 'S')
        {
        	memset(&newSpeed, 0, 5 * sizeof(char));
        	printf("podaj nową prędkość: ");
        	scanf("%s", newSpeed);
        	newSpeed[5] = 0;
        	message[0] = 's';
        	chr1 = &message[1];
        	chr2 = &newSpeed[0];
        	while (*chr1++ = *chr2++){
        		;
        	}
        }

        if(c == 'a'){
        	memset(&azimuth, 0, 10 * sizeof(char));
        	printf("odległość ; kąt: ");
        	scanf("%s", newSpeed);
        	message[0] = 'a';
        	chr1 = &message[1];
        	chr2 = &newSpeed[0];
        	while (*chr1++ = *chr2++){
        		;
        	}
        }

        if((c == 'p') || (c == 'r')){
                	memset(&azimuth, 0, 10 * sizeof(char));
                	printf("odległość ; kąt: ");
                	scanf("%s", newSpeed);
                	message[0] = c;
                	chr1 = &message[1];
                	chr2 = &newSpeed[0];
                	while (*chr1++ = *chr2++){
                		;
                	}
                }

        //Send some data
        if( send(sock , message , strlen(message)+1 , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        if((message[0] == 'p') || (message[0] == 'r')){
        	while(recv(sock , server_reply , 2000 , 0) == -1){
        		;
        	}
        	printf("Remote Server reply: [%s]\n", server_reply);
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
