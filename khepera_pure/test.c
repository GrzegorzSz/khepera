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

int main(){
	double fpos,dval;
	int lpos,rpos;
	char Buffer[100],bar[12][64],revision,version;
	int i,n,val,type_of_test=0,sl,sr,pl,pr,mean;
	short index, value,sensors[12],usvalues[5];
	char c;
	long motspeed;
	char line[80],l[9];
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

	//  ------  battery example --------------------------------------------------
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

	//------ motor example -----------------------------------------------------
	motspeed = (long)(42.0/KH4_SPEED_TO_MM_S);
	kh4_SetMode(kh4RegSpeed, dsPic);
	kh4_set_speed(motspeed, -motspeed, dsPic);
	printf("\nRotating 5s at %.1f mm/s (pulse speed %ld) with speed only\n", 42.0, motspeed);
	sleep(5);
	kh4_set_speed(0, 0, dsPic);

	printf("Moving backward and forward 2s\n");
	kh4_set_speed(-motspeed, -motspeed, dsPic);
	sleep(2);
	kh4_set_speed(motspeed, motspeed, dsPic);
	sleep(2);
	kh4_set_speed(0, 0, dsPic);
	kh4_SetMode(kh4RegIdle, dsPic);
	kh4_ResetEncoders(dsPic);

	//na koniec upewnijmy się, że silniki nie pracują
	//a diody nie świecą:
	kh4_set_speed(0, 0, dsPic);
	kh4_SetRGBLeds(0, 0, 0, 0, 0, 0, 0, 0, 0, dsPic);
	kh4_SetMode(kh4RegIdle, dsPic);		//w tym trybie silniki nie pobierają prądu


	return 0;
}
