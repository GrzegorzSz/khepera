#include <khepera/khepera.h>
#include "kb_battery.h"

static knet_dev_t *Battery; 		//handle to the Battery device (knet socket, i2c mode)

int initBattery(void){
	kbattery_init();
	Battery = knet_open("Kbattery:ADC", KNET_BUS_I2C, 0, NULL);
	if(Battery != 0)
	   return 1;
	  else
	   return 0;
}

int getcur( int argc, char * argv[], void * data)
{

  unsigned short Current, i;

  // ensure that the ADS1015 is correctly configured
  kbattery_Config_Current(Battery);

  while(!kb_kbhit())
  {
  kb_clrscr();
  Current = kbattery_Get_Battery_Current(  Battery);
  printf("Battery Current:   %5umA\n",Current);
  usleep(80000);
  }
  return 1;
}

int main(int argc, char *argv[]) {
  int rc;

  /* Set the libkhepera debug level - Highly recommended for development. */
  kb_set_debug_level(2);

  printf("LibKhepera Template Program\r\n");

    /* Init the khepera library */
  if((rc = kb_init( argc , argv )) < 0 )
    return 1;

	/* ADD YOUR CODE HERE */
  printf("This is my code.\n :/\n");


 return 0;
}
