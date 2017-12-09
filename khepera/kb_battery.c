/*! 
 * \file   kb_battery.c Khepera4 Battery Turret layer              
 *
 * \brief 
 *         This module is layer for communication with the khepera 4 Battery Extension. It offers simple interface 
 *         to the user to communicate with ADS1015 ADC converter of the Battery extension.
 *        
 * \author   Frederic Lambercy (K-Team SA)                               
 *
 * \note     Copyright (C) 2015 K-TEAM SA
 * \bug      none discovered.                                         
 * \todo     nothing.
 */

#include "khepera.h"
#include "kb_error.h"


/*! kbattery_init initializes the KoreBot library
 * This function needs to be called BEFORE any other functions.
 * 
 *
 * \return A value:
 *       - <0 on error 
 *       - 0 on success 
 *
 */
int kbattery_init( void )
{
 int rc;

 /* First of all this function initializes the khepera library */
 
 if((rc = kb_init( 0 , NULL )) < 0 )
 {
 	/* Unable to initialize the khepera library */
	printf("kb_kh4_init ",KB_ERROR_KH4KBINIT);
	return KB_ERROR_KH4KBINIT;
 }
}

/********************** READ FUNCTION ********************************/

/*--------------------------------------------------------------------*/
/*! 
 * This function return the Result value of the ADS1015
 *
 * \param dev    	K-Net Device Descriptor
 * \return Result 	ADS1015 Result value
 *

 */ 
unsigned short kbattery_Get_Result( knet_dev_t * dev )
{
  unsigned short Result;
  knet_read_word( dev , KBAT_RESULT , &Result );

  return Result;
  
}

/*--------------------------------------------------------------------*/
/*! 
 * This function return the Config register of the ADS1015
 *
 * \param dev    	K-Net Device Descriptor
 * \return Config 	ADS1015 Configuration value
 *

 */ 
unsigned short kbattery_Get_Config( knet_dev_t * dev )
{
  unsigned short Config;
  knet_read_word( dev , KBAT_CONFIG , &Config );

  return Config;
  
}

/*--------------------------------------------------------------------*/
/*! 
 * This function return the Low Threshold value of the ADS1015
 *
 * \param dev    	K-Net Device Descriptor
 * \return Threshold 	ADS1015 Low Threshold register value
 *

 */ 
unsigned short kbattery_Get_Low_Threshold( knet_dev_t * dev )
{
  unsigned short Threshold;
  knet_read_word( dev , KBAT_LOW_THRES , &Threshold );

  return Threshold;
  
}

/*--------------------------------------------------------------------*/
/*! 
 * This function return the High Threshold value of the ADS1015
 *
 * \param dev    	K-Net Device Descriptor
 * \return Threshold 	ADS1015 high Threshold register value
 *

 */ 
unsigned short kbattery_Get_High_Threshold( knet_dev_t * dev )
{
  unsigned short Threshold;
  knet_read_word( dev , KBAT_HIGH_THRES , &Threshold );

  return Threshold;
  
}



/********************** WRITE FUNCTION ********************************/
/*--------------------------------------------------------------------*/
/*! 
 * This function set a the configuration register of the ADS1015
 *
 * \param dev    	K-Net Device Descriptor
 * \param CONFIG 	Configuration value
 *
 */ 
void kbattery_Set_Config( knet_dev_t * dev, unsigned short Config )
{
  knet_set_order( dev ,  KBATTERY_ORDER_MASK );
  knet_write_word( dev , KBAT_CONFIG , Config );



}

/*--------------------------------------------------------------------*/
/*! 
 * This function set a the low threshold of the ADS1015
 *
 * \param dev    	K-Net Device Descriptor
 * \param THRESHOLD 	Low Threshold Value
 *
 */ 
void kbattery_Set_Low_Threshold(  knet_dev_t * dev, unsigned short Threshold )
{
  knet_set_order( dev ,  KBATTERY_ORDER_MASK );
  knet_write_word( dev , KBAT_LOW_THRES , Threshold );



}

/*--------------------------------------------------------------------*/
/*! 
 * This function set a the low threshold of the ADS1015
 *
 * \param dev    	K-Net Device Descriptor
 * \param THRESHOLD 	High Threshold Value
 *
 */ 
void kbattery_Set_High_Threshold(  knet_dev_t * dev, unsigned short Threshold)
{
  knet_set_order( dev ,  KBATTERY_ORDER_MASK );
  knet_write_word( dev , KBAT_HIGH_THRES , Threshold );



}



/********************** HIGH LEVEL FUNCTION ***************************/
/*--------------------------------------------------------------------*/
/*! 
 * This function configure the ADS1015 to read automatically the Battery voltage at 1600SPS
 *
 * \param dev    	K-Net Device Descriptor
 *
 */ 
void kbattery_Config_Voltage( knet_dev_t * dev )
{
  kbattery_Set_Config( dev, KBAT_CONFIG_VOLTAGE_AUTO);


}

/*--------------------------------------------------------------------*/
/*! 
 * This function configure the ADS1015 to read automatically the Battery Current at 1600SPS
 *
 * \param dev    	K-Net Device Descriptor
 *
 */ 
void kbattery_Config_Current( knet_dev_t * dev )
{
  kbattery_Set_Config( dev, KBAT_CONFIG_CURRENT_AUTO);


}

/*--------------------------------------------------------------------*/
/*! 
 * This function read the Result register and convert it into mV (the ADS1015 must configure to read the battery Voltage first)
 *
 * \param dev    	K-Net Device Descriptor
 *
 */ 
unsigned short kbattery_Get_Battery_Voltage( knet_dev_t * dev )
{
  unsigned short Voltage;
  unsigned int   Calc_Temp;

  // Read the result register
  Calc_Temp = kbattery_Get_Result( dev);
  // Clear the 4 unused LSB bits
  Calc_Temp = Calc_Temp >> 4;
  // Convert the ADC value into mV value
  Calc_Temp *= KBAT_CONVERT_VOLTAGE_MULT;
  Calc_Temp /= KBAT_CONVERT_VOLTAGE_DIV;
  Voltage = Calc_Temp;

  return Voltage;


}

/*--------------------------------------------------------------------*/
/*! 
 * This function Configure the ADS1015 in single shot mode to read the battery Voltage.
 *
 * \param dev    	K-Net Device Descriptor
 *
 */ 
unsigned short kbattery_Get_Battery_Voltage_Single_Shot( knet_dev_t * dev )
{
  unsigned short Voltage;
  unsigned int   Calc_Temp;

  // Configure the ADS1015 in single shot mode, to read the battery voltage first. But do not start the conversion
  kbattery_Set_Config(dev,KBAT_CONFIG_VOLTAGE_SINGLE);

  // Wait that conversion is finished
  while(!(kbattery_Get_Config(dev) & KBAT_CONFIG_START_MEAS));

  // Start the conversion in single shot mode to read the voltage
  kbattery_Set_Config(dev,KBAT_CONFIG_START_MEAS | KBAT_CONFIG_VOLTAGE_SINGLE);

  // Wait that conversion is finished
  while(!(kbattery_Get_Config(dev) & KBAT_CONFIG_START_MEAS));
  // Read the battery voltage
  Voltage = kbattery_Get_Battery_Voltage(dev); 

  return Voltage;


}

/*--------------------------------------------------------------------*/
/*! 
 * This function read the Result register and convert it into mA (the ADS1015 must configure to read the battery Current first)
 *
 * \param dev    	K-Net Device Descriptor
 *
 */ 
unsigned short kbattery_Get_Battery_Current( knet_dev_t * dev )
{
  unsigned short Current;
  unsigned int   Calc_Temp;

  // Read the result register
  Calc_Temp = kbattery_Get_Result( dev);
  // Clear the 4 unused LSB bits
  Calc_Temp = Calc_Temp >> 4;
  // Convert the ADC value into mA value
  Calc_Temp *= KBAT_CONVERT_CURRENT_MULT;
  Calc_Temp /= KBAT_CONVERT_CURRENT_DIV;
  Current = Calc_Temp;

  return Current;


}

/*--------------------------------------------------------------------*/
/*! 
 * This function Configure the ADS1015 in single shot mode to read the battery Current.
 *
 * \param dev    	K-Net Device Descriptor
 *
 */ 
unsigned short kbattery_Get_Battery_Current_Single_Shot( knet_dev_t * dev )
{
  unsigned short Current;
  unsigned int   Calc_Temp;

  // Configure the ADS1015 in single shot mode, to read the battery voltage first. But do not start the conversion
  kbattery_Set_Config(dev,KBAT_CONFIG_CURRENT_SINGLE);

  // Wait that conversion is finished
  while(!(kbattery_Get_Config(dev) & KBAT_CONFIG_START_MEAS));

  // Start the conversion in single shot mode to read the voltage
  kbattery_Set_Config(dev,KBAT_CONFIG_START_MEAS | KBAT_CONFIG_CURRENT_SINGLE);

  // Wait that conversion is finished
  while(!(kbattery_Get_Config(dev) & KBAT_CONFIG_START_MEAS));
  // Read the battery voltage
  Current = kbattery_Get_Battery_Current(dev); 

  return Current;


}






