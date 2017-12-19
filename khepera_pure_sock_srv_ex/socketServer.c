// #define for driver mode
#define BIG_SPEED_FACTOR 25
#define SPEED_FACTOR 1
#define MAX_SPEED 1500
#define MIN_SPEED 15
#define DEFAULT_SPEED 200
#define ROTATE_HIGH_SPEED_FACT 0.5
#define ROTATE_LOW_SPEED_FACT 0.75
#define ROT_SPEED_HIGH_TRESH 300
#define STOP_TIME 100000 // us

#define SIGN(x) ((x)>0?1:((x)<0?-1:0))  // sign or zero

#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <khepera/khepera.h>
#include <signal.h>

static knet_dev_t* dsPic;	//robot PIC microcontroller access
static int quitReq = 0;

static void ctrlc_handler(int sig){
	quitReq = 1;

	kh4_set_speed(0, 0, dsPic);		//zatrzymanie robota
	kh4_SetMode(kh4RegIdle, dsPic);		//wprowadzenie w tryb bezczynny

	kh4_SetRGBLeds(0, 0, 0, 0, 0, 0, 0, 0, 0, dsPic);	//wygaszenie diod
	kb_change_term_mode(0);		//revert to original terminal if called

	exit(0);					//zakończenie programu
}

long long timeval_diff(struct timeval *difference,
		struct timeval *end_time,
		struct timeval *start_time)
{
	struct timeval temp_diff;

	if(difference==NULL)
	{
		difference=&temp_diff;
	}

	difference->tv_sec =end_time->tv_sec -start_time->tv_sec ;
	difference->tv_usec=end_time->tv_usec-start_time->tv_usec;

	/* Using while instead of if below makes the code slightly more robust. */

	while(difference->tv_usec<0)
	{
		difference->tv_usec+=1000000;
		difference->tv_sec -=1;
	}

	return 1000000LL*difference->tv_sec+
			difference->tv_usec;

} /* timeval_diff() */

// TODO: adjust function drive_robot to sockets
int drive_robot()
{
	int out=0,speed=DEFAULT_SPEED*4,vsl,vsr,anymove=0;
	char c;
	struct timeval startt,endt;


	//kb_clrscr(); // erase screen

	printf("Drive the robot with the keyboard:\n  's' for stop\n  arrows (UP, DOWN, LEFT , RIGHT) for direction\n  PAGE UP/DOWN for changing speed  by small increments\n  Home/End for changing speed by big increments\n  'q' for going back to main menu\n");

	printf("\ndefault parameters:\n  robot speed %d  (%5.1f mm/s)  (min %d, max %d)\n\n",DEFAULT_SPEED,DEFAULT_SPEED*KH4_SPEED_TO_MM_S,MIN_SPEED,MAX_SPEED);
	kb_change_term_mode(1); // change terminal mode for kbhit and getchar to return immediately
	kh4_SetMode(kh4RegSpeed, dsPic);
	gettimeofday(&startt,0x0);

	// loop until 'q' is pushed
	while(!out)
	{
		if(kb_kbhit())
		{
			c = getchar();


			// get special keys
			if (c == 27)
			{

				if (c = getchar() == 91) // escape with [
				{
					c = getchar();

					switch(c)
					{
					case 65: // UP arrow = forward
						kh4_set_speed(speed ,speed,dsPic );
						anymove=1;
						break;
					case 66: // DOWN arrow = backward
						kh4_set_speed(-speed ,-speed,dsPic  );
						anymove=1;
						break;

					case 68: // LEFT arrow = left
						if (speed > ROT_SPEED_HIGH_TRESH) // at high speed, rotate too fast
							kh4_set_speed(-speed*ROTATE_HIGH_SPEED_FACT ,speed*ROTATE_HIGH_SPEED_FACT ,dsPic );
						else
							kh4_set_speed(-speed*ROTATE_LOW_SPEED_FACT ,speed*ROTATE_LOW_SPEED_FACT ,dsPic );
						anymove=1;
						break;

					case 67: // RIGHT arrow = right
						if (speed > ROT_SPEED_HIGH_TRESH) // at high speed, rotate too fast
							kh4_set_speed(speed*ROTATE_HIGH_SPEED_FACT ,-speed*ROTATE_HIGH_SPEED_FACT ,dsPic );
						else
							kh4_set_speed(speed*ROTATE_LOW_SPEED_FACT ,-speed*ROTATE_LOW_SPEED_FACT ,dsPic );
						anymove=1;
						break;

					case 53: // PAGE UP  = speed up
						speed+=SPEED_FACTOR;
						if (speed>MAX_SPEED)
						{
							speed=MAX_SPEED;
						};
						c = getchar(); // get last character

						kh4_get_speed(&vsl,&vsr,dsPic );
						kh4_set_speed(SIGN(vsl)*speed ,SIGN(vsr)*speed ,dsPic ); // set new speed, keeping direction with sign
						printf("\033[1`\033[Krobot speed: %d (%5.1f mm/s)",speed,speed*KH4_SPEED_TO_MM_S); // move cursor to first column, erase line and print info
						fflush(stdout); // make the display refresh
						anymove=1;
						break;

					case 54: // PAGE DOWN = speed down
						speed-=SPEED_FACTOR;
						if (speed<MIN_SPEED)
						{
							speed=MIN_SPEED;
						};
						c = getchar(); // get last character

						kh4_get_speed(&vsl,&vsr,dsPic );
						kh4_set_speed(SIGN(vsl)*speed ,SIGN(vsr)*speed,dsPic  ); // set new speed, keeping direction with sign
						printf("\033[1`\033[Krobot speed: %d (%5.1f mm/s)",speed,speed*KH4_SPEED_TO_MM_S); // move cursor to first column, erase line and print info
						fflush(stdout); // make the display refresh
						anymove=1;
						break;

					case 36: // Home  = speed up
						speed+=BIG_SPEED_FACTOR;
						if (speed>MAX_SPEED)
						{
							speed=MAX_SPEED;
						};
						//c = getchar(); // get last character

						kh4_get_speed(&vsl,&vsr,dsPic );
						kh4_set_speed(SIGN(vsl)*speed ,SIGN(vsr)*speed ,dsPic ); // set new speed, keeping direction with sign
						printf("\033[1`\033[Krobot speed: %d (%5.1f mm/s)",speed,speed*KH4_SPEED_TO_MM_S); // move cursor to first column, erase line and print info
						fflush(stdout); // make the display refresh
						anymove=1;
						break;

					case 35: // End = speed down
						speed-=BIG_SPEED_FACTOR;
						if (speed<MIN_SPEED)
						{
							speed=MIN_SPEED;
						};
						//c = getchar(); // get last character

						kh4_get_speed(&vsl,&vsr,dsPic );
						kh4_set_speed(SIGN(vsl)*speed ,SIGN(vsr)*speed,dsPic  ); // set new speed, keeping direction with sign
						printf("\033[1`\033[Krobot speed: %d (%5.1f mm/s)",speed,speed*KH4_SPEED_TO_MM_S); // move cursor to first column, erase line and print info
						fflush(stdout); // make the display refresh
						anymove=1;
						break;


					default:
						break;
					} // switch(c)
				} // escape with [
				else
				{ // other special key code

					c = getchar();

					switch(c){

					case 72: // Home  = speed up
						speed+=BIG_SPEED_FACTOR;
						if (speed>MAX_SPEED)
						{
							speed=MAX_SPEED;
						};
						//c = getchar(); // get last character

						kh4_get_speed(&vsl,&vsr,dsPic );
						kh4_set_speed(SIGN(vsl)*speed ,SIGN(vsr)*speed ,dsPic ); // set new speed, keeping direction with sign
						printf("\033[1`\033[Krobot speed: %d (%5.1f mm/s)",speed,speed*KH4_SPEED_TO_MM_S); // move cursor to first column, erase line and print info
						fflush(stdout); // make the display refresh
						anymove=1;
						break;

					case 70: // End = speed down
						speed-=BIG_SPEED_FACTOR;
						if (speed<MIN_SPEED)
						{
							speed=MIN_SPEED;
						};
						//c = getchar(); // get last character

						kh4_get_speed(&vsl,&vsr,dsPic );
						kh4_set_speed(SIGN(vsl)*speed ,SIGN(vsr)*speed,dsPic  ); // set new speed, keeping direction with sign
						printf("\033[1`\033[Krobot speed: %d (%5.1f mm/s)",speed,speed*KH4_SPEED_TO_MM_S); // move cursor to first column, erase line and print info
						fflush(stdout); // make the display refresh
						anymove=1;
						break;

					default:
						break	;

					}

				} // ether special key code


			} // if (c== '\027')
			else
			{
				switch(c)
				{
				case 'q': // quit to main menu
					out=1;
					break;
				case 's': // stop motor
					kh4_set_speed(0,0,dsPic);
					break;

				default:
					break;
				}
			}

			gettimeofday(&startt,0x0);
		} else
		{
			gettimeofday(&endt,0x0);;
			// stop when no key is pushed after some time

			if (anymove &&  (timeval_diff(NULL,&endt,&startt)>STOP_TIME))
			{
				kh4_set_speed(0 ,0,dsPic );
				anymove=0;
			}
		}
		usleep(10000); // wait some ms
	} // while

	kb_change_term_mode(0); // switch to normal key input mode
	kh4_set_speed(0,0,dsPic );	 // stop robot
	kh4_SetMode(kh4RegIdle,dsPic );
	return 0;
}

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];


    char Buffer[100],revision,version;
    int i;
    int kp,ki,kd;
    int pmarg,maxsp,accinc,accdiv,minspacc, minspdec; // SetSpeedProfile

    printf("Próba sterowania Khepera IV.");

    //inicjalizacja biblioteki
    if(kh4_init(0, NULL) != 0){
    	printf("\nERROR: could not initiate the libkhepera!\n\n");
    	return -1;
    }

    //open microcontr. socket and store the handler to its pointer
    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
    if(dsPic == NULL){
    	printf("ERROR: Nie można zainicjować komunikacji z KH4 dsPic\n\n");
    	return -2;
    }

    //--------------------- wyłączenie czujników ultradźwiękowych
    kh4_activate_us(0, dsPic);		//31 włącza wszystkie

    //---------------------inicjalizacja silników--------------------------
    //parametry:
    pmarg = 20;					//margines błędu
    kh4_SetPositionMargin(pmarg, dsPic);	//wysłanie ustawienia do uC
    kp = 10;
    ki = 5;
    kd = 1;
    kh4_ConfigurePID(kp, ki, kd, dsPic);

    accinc = 3;		//acceleration increment (1-255)
    accdiv = 0;		//acceleration divider (0-255)
    minspacc = 20;	//minimum speed acceleration (minimalna prędkość używana przez uC)
    minspdec = 1;	//minimum speed dec
    maxsp = 400;	//(max 1200)

    kh4_SetSpeedProfile(accinc, accdiv, minspacc, minspdec, maxsp, dsPic);
    kh4_SetMode(kh4RegIdle, dsPic);			//ustawienie trybu bezczynności (no control)

    // get revision
    if(kh4_revision(Buffer, dsPic)==0){
    	version=(Buffer[0]>>4) +'A';
    	revision=Buffer[0] & 0x0F;
    	printf("\r\nVersion = %c, Revision = %u\r\n",version,revision);
    }

    signal(SIGINT, ctrlc_handler);		//set signal for catching ctrl-c sigvalue 2

	//  ------  battery status --------------------------------------------------
	kh4_battery_status(Buffer,dsPic);
	printf("\nBattery:\n  status (DS2781)   :  0x%x\n",Buffer[0]);
	printf("  remaining capacity:  %4.0f mAh\n",(Buffer[1] | Buffer[2]<<8)*1.6);
	printf("  remaining capacity:   %3d %%\n",Buffer[3]);
	printf("  current           : %5.0f mA\n",(short)(Buffer[4] | Buffer[5]<<8)*0.07813);
	printf("  average current   : %5.0f mA\n",(short)(Buffer[6] | Buffer[7]<<8)*0.07813);
	printf("  temperature       :  %3.1f C \n",(short)(Buffer[8] | Buffer[9]<<8)*0.003906);
	printf("  voltage           :  %4.0f mV \n",(Buffer[10] | Buffer[11]<<8)*9.76);
	printf("  charger           :  %s\n",kh4_battery_charge(dsPic)?"plugged":"unplugged");

	//------RGB LED example------------------------------------------------------
	printf("\nRGB LED example\n");

	for(i = 0; i<0x20; i++){		//0x20 => 50% brightness
		kh4_SetRGBLeds(0, i, 0, i, 0, 0, 0, 0, i, dsPic);
		usleep(20000);
	}
	kh4_SetRGBLeds(0, 0, 0, 0, 0, 0, 0, 0, 0, dsPic);		//stop LED

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
    }
    printf("Socket created\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    printf("bind done\n");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    printf("Connection accepted\n");

    //drive_robot();

    //Receive a message from client
    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    {
    	printf("Srv rcv: [%s]\n", client_message);
        //Send the message back to client
    	send(client_sock , client_message , strlen(client_message)+1 , 0);
        //write(client_sock , client_message , strlen(client_message));
//        for(i = 0; i < strlen(client_message); i++){
//        	client_message[i] = '\0';
//        }
        //write(client_sock , client_message , strlen(client_message));
    }

    if(read_size == 0)
    {
        printf("Client disconnected\n");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //na koniec upewnijmy się, że silniki nie pracują
    //a diody nie świecą:
    kh4_set_speed(0, 0, dsPic);
    kh4_SetRGBLeds(0, 0, 0, 0, 0, 0, 0, 0, 0, dsPic);
    kh4_SetMode(kh4RegIdle, dsPic);		//w tym trybie silniki nie pobierają prądu
    printf("Motors stoped and turn on idle mode, all LEDs switched off\n\n");

    return 0;
}
