// #define for driver mode
#define BIG_SPEED_FACTOR 25
#define SPEED_FACTOR 1
#define MAX_SPEED 1500
#define MIN_SPEED 15
#define DEFAULT_SPEED 400
#define ROTATE_HIGH_SPEED_FACT 0.5
#define ROTATE_LOW_SPEED_FACT 0.75
#define ROT_SPEED_HIGH_TRESH 300
#define STOP_TIME 100000 // us
#define DETECTION_THRESHOLD 40
#define PULSES_PER_MM 147.4
#define PULSES_PER_ONE_DEGREE 130

#define SIGN(x) ((x)>0?1:((x)<0?-1:0))  // sign or zero

#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <khepera/khepera.h>
#include <signal.h>
#include <limits.h>

void azimuth_convert_to_pulses(char* client_message);
void azimuth_drive(long length_pulses, long angle_pulses);
void turn_off();
void circle_path_drive(char* client_message);

static knet_dev_t* dsPic;	//robot PIC microcontroller access
static int quitReq = 0;

static void ctrlc_handler(int sig){
	quitReq = 1;
	turn_off();
	exit(0);					//exit the program
}


int drive_robot(int* read_size, int* client_sock, char* client_message, int socket_desc)
{
	printf("\t\tSocket descriptor = %d\n", socket_desc);
	sleep (1);
	int speed=DEFAULT_SPEED;
	int oldSpeedValue = speed;
	char c, errReply[] = "error";
	char* newSpeedValue;
	char travelCompletedMsg[] = "travel_completed";

	kh4_SetMode(kh4RegSpeed, dsPic);

	printf("inside drive_robot fun\n");

	while( (*read_size = recv(*client_sock , client_message , 2000 , 0)) > 0)
	{
		printf("Srv rcv: [%s]\n", client_message);
		//Send the message back to client
		send(*client_sock , client_message , strlen(client_message)+1 , 0);

		c = client_message[0];
		switch (c){
		case 65:		//Up arrow

			kh4_set_speed(speed, speed, dsPic);
			usleep(300000);
			kh4_set_speed(0,0,dsPic);
			break;

		case 66: // DOWN arrow

			kh4_set_speed(-speed, -speed, dsPic);
			usleep(300000);
			kh4_set_speed(0, 0, dsPic);
			break;

		case 68: // LEFT arrow
			oldSpeedValue = speed;
			speed = DEFAULT_SPEED - 150;
			kh4_set_speed(-speed, speed, dsPic);
//			if (speed > ROT_SPEED_HIGH_TRESH) // at high speed, rotate too fast
//				kh4_set_speed(-speed*ROTATE_HIGH_SPEED_FACT ,speed*ROTATE_HIGH_SPEED_FACT ,dsPic );
//			else
//				kh4_set_speed(-speed*ROTATE_LOW_SPEED_FACT ,speed*ROTATE_LOW_SPEED_FACT ,dsPic );
			usleep(300000);
			speed = oldSpeedValue;
			kh4_set_speed(0, 0, dsPic);
			break;

		case 67: // RIGHT arrow = right
			oldSpeedValue = speed;
			speed = DEFAULT_SPEED-150;
			kh4_set_speed(speed, -speed, dsPic);
//			if (speed > ROT_SPEED_HIGH_TRESH) // at high speed, rotate too fast
//				kh4_set_speed(speed*ROTATE_HIGH_SPEED_FACT ,-speed*ROTATE_HIGH_SPEED_FACT ,dsPic );
//			else
//				kh4_set_speed(speed*ROTATE_LOW_SPEED_FACT ,-speed*ROTATE_LOW_SPEED_FACT ,dsPic );
			speed = oldSpeedValue;
			usleep(300000);
			kh4_set_speed(0, 0, dsPic);
			break;
		case 's':
		case 'S':
			newSpeedValue = &client_message[1];
			if(atoi(newSpeedValue) >= MIN_SPEED && atoi(newSpeedValue) <= MAX_SPEED){
				printf("speedValue OK, [%s]\n", newSpeedValue);
				speed = atoi(newSpeedValue);
				send(*client_sock , newSpeedValue , strlen(newSpeedValue)+1 , 0);
				printf("new speed value sent.\n");
			}
			else{
				send(*client_sock, errReply, strlen(errReply)+1, 0);
			}
			break;
		case 'a':
			azimuth_convert_to_pulses(client_message);
			break;
		case 'r':
			printf("round path choosen\n");
			circle_path_drive(client_message);
			send(*client_sock, travelCompletedMsg, strlen(travelCompletedMsg)+1, 0);
			break;
		case 'p':
			printf("path drive choosen\n");
			azimuth_convert_to_pulses(client_message);
			send(*client_sock, travelCompletedMsg, strlen(travelCompletedMsg)+1, 0);
			break;
		default:
			printf(".[");	//debug
			kh4_set_speed(0,0,dsPic);
			break;
		}
		memset(client_message, 0, 2000 * sizeof(char));		//clear the client_message buffer
	}



	printf("\n");
	return 0;
}

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];
    const char cameraStartCommand[] = "mjpg_streamer -i \"input_uvc.so -yuv -f 15\" -o \"output_http.so -w /usr/local/mjpg-streamer/www\"&";


    char Buffer[100],revision,version;
    int i;
    int kp,ki,kd;
    int pmarg,maxsp,accinc,accdiv,minspacc, minspdec; // SetSpeedProfile


    //-----------------------library initialization
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

    //--------------------- turn off ultrasound sensors
    kh4_activate_us(0, dsPic);		//31 turns all on

    //---------------------initializing motors
    //parameters:
    pmarg = 20;					//error margin
    kh4_SetPositionMargin(pmarg, dsPic);	//sending settings to uC
    kp = 10;
    ki = 5;
    kd = 1;
    kh4_ConfigurePID(kp, ki, kd, dsPic);

    accinc = 3;		//acceleration increment (1-255)
    accdiv = 0;		//acceleration divider (0-255)
    minspacc = 20;	//minimum speed acceleration
    minspdec = 1;	//minimum speed dec
    maxsp = 400;	//(max 1200)

    kh4_SetSpeedProfile(accinc, accdiv, minspacc, minspdec, maxsp, dsPic);
    kh4_SetMode(kh4RegIdle, dsPic);			//turn idle mode (no control)

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

	//------RGB LED flash------------------------------------------------------
	printf("\nRGB LED example\n");

	for(i = 0; i<0x20; i++){		//0x20 => 50% brightness
		kh4_SetRGBLeds(0, i, 0, i, 0, 0, 0, 0, i, dsPic);
		usleep(20000);
	}

	//-------------------------Camera streaming start
	system(cameraStartCommand);
	kh4_SetRGBLeds(0, 0, 0, 0, 0, 0, 0, 0, 0, dsPic);		//stop LED

    //-------------------------Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
    }
    printf("Socket created\n");

    //--------------Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //-----------------Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }
    printf("bind done\n");

    //Listen on socket
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

    drive_robot(&read_size, &client_sock, client_message, socket_desc);

    if(read_size == 0)
    {
        printf("Client disconnected\n");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    turn_off();

    return 0;
}

void circle_path_drive(char* client_msg){
	char* p_msg = &client_msg[1];
	double radius = atof(p_msg) * 1.0;
	double wheel1_rad = radius + 52.35;
	double wheel2_rad = radius - 52.35;
	double path_length_mm = 3.1416 * 2.0 * wheel1_rad;
	int path_length_pulses = (int)(path_length_mm * PULSES_PER_MM);
	int l_engine_spd = DEFAULT_SPEED;
	int r_engine_spd = DEFAULT_SPEED;
	int l_engine_pos = 0;
	int r_engine_pos = 0;

	FILE *file;
	file = fopen("position.txt", "w");
	if(file == NULL){
		printf("problem z plikiem\n");
	}

	//-----calculating speed difference
	double ratio = ((l_engine_spd * wheel2_rad) + (l_engine_spd * 104.7)) / (l_engine_spd * wheel2_rad);
	int new_r_engine_spd = (int)(ratio * l_engine_spd);
	kh4_ResetEncoders(dsPic);
	kh4_set_speed(l_engine_spd, new_r_engine_spd, dsPic);

	do{
		kh4_get_position(&l_engine_pos, &r_engine_pos, dsPic);
		fprintf(file, "%d;%d\n", l_engine_pos, r_engine_pos);
		usleep(500);
	}while(r_engine_pos < path_length_pulses);

	kh4_set_speed(0, 0, dsPic);
	fclose(file);
}

void azimuth_convert_to_pulses(char* client_message){
	long length = 0;
	float angle = 0;
	int index;
	char message[7];				//tmp variable. Used for decode command
	char* p_message = message;
	char* p_client_message = &client_message[1];

	while(*p_client_message != ';'){
		*p_message = *p_client_message;
		p_message++;
		p_client_message++;
	}
	*p_message = 0;
	printf("\nOtrzymane %s", client_message);
	printf("\ndroga: %s", message);
	length = atof(message);			//length in pixels
	if(client_message[0] == 'a'){

		if (length > 105){
			length = PULSES_PER_MM * 150;			//max length in pulses
		} else {
			length /= 7;				//length in cm
			length *= (PULSES_PER_MM * 10);	//length in pulses
		}
	} else {
		//expected length of path in mm delivered by client_message
		printf("inside of other than a[] message\n");
		length *= PULSES_PER_MM;
		printf("length calculated.\n");
	}

	for(index = 0; index < sizeof(message); index++){
		message[index] = '\0';			//clear table
	}

	p_client_message++;
	p_message = &message;

	while(*p_client_message){
		*p_message = *p_client_message;
		p_message++;
		p_client_message++;
	}
	angle = atof(message);					//angle in degrees
	printf("Azimuth: %ld\t\t%.2f st\n", length, angle);

	angle *= PULSES_PER_ONE_DEGREE;			//angle in pulses
	azimuth_drive(length, (long)angle);
}

void azimuth_drive(long length_pulses, long angle_pulses){
	int position = 0;
	int l_engine_spd = 0;
	int r_engine_spd = 0;
	kh4_get_speed(&l_engine_spd, &r_engine_spd, dsPic);
	printf("Azimuth drive len: %d angl: %d\n", length_pulses, angle_pulses);
	kh4_ResetEncoders(dsPic);
	kh4_SetMode(kh4RegPosition, dsPic);
	kh4_set_position(angle_pulses, (angle_pulses * -1), dsPic);
	usleep(200000);
	do
	{
		kh4_get_position(&position, &position, dsPic);
		printf("Position: %d\n", position);
		usleep(10000);
	}while((abs(angle_pulses) - abs(position)) > 5);

	kh4_ResetEncoders(dsPic);
	kh4_set_position(length_pulses, length_pulses, dsPic);

	do
		{
			kh4_get_position(&position, &position, dsPic);
			printf("Position: %d\n", position);
			usleep(100000);
		}while((abs(length_pulses) - abs(position)) > 5);

	//kh4_SetSpeedProfile(3, 0, 20, 1, 400, dsPic);
	kh4_SetMode(kh4RegSpeedProfile, dsPic);
	kh4_set_speed(l_engine_spd, r_engine_spd, dsPic);
}

void turn_off(){
	kh4_set_speed(0, 0, dsPic);		//stop the robot
	kh4_SetMode(kh4RegIdle, dsPic);		//turn on idle mode
	kh4_SetRGBLeds(0, 0, 0, 0, 0, 0, 0, 0, 0, dsPic);	//all LED off
	kb_change_term_mode(0);		//revert to original terminal if called
	system("kill $(pidof mjpg_streamer)");		//turn camera off
	printf("\nAll turned off.\nEngines in idle mode.\n");
}
