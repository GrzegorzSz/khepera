/*--------------------------------------------------------------------
 * kb_battery.h - KoreBot Library - Khepera3 Gripper functions
 *--------------------------------------------------------------------
 * $Author: flambercy $
 * $Date: 2015/10/16
 * $Revision: 1.0 $
 *-------------------------------------------------------------------*/ 

#ifndef __kbattery__
#define __kbattery__

#include "knet.h"

/*--------------------------------------------------------------------
 *!  Kbattery Order Mask (See knet_set_order)
 */
#define KBATTERY_ORDER_MASK ( KNET_ORDER_LITTLE | \
                         KNET_ORDER_REP_ADR | KNET_ORDER_MSB_FIRST )


/*--------------------------------------------------------------------
 *! Kbattery I2C address
 *!   
 */
#define KBAT_I2C_ADDR		0x48

/*--------------------------------------------------------------------
 *! Kbattery Registers Definition 
 *!   
 */
#define	KBAT_RESULT		0x00
#define	KBAT_CONFIG		0x01
#define	KBAT_LOW_THRES		0x02
#define	KBAT_HIGH_THRES		0x03

/*--------------------------------------------------------------------
 *! Kbattery Configuration value
 *!   
 */
#define	KBAT_CONFIG_VOLTAGE_AUTO	0x0483
#define	KBAT_CONFIG_VOLTAGE_SINGLE	0x0583

#define	KBAT_CONFIG_CURRENT_AUTO	0x3483
#define	KBAT_CONFIG_CURRENT_SINGLE	0x3583

#define	KBAT_CONFIG_START_MEAS 		0x8000

/*--------------------------------------------------------------------
 *! Kbattery Conversion value
 *!   
 */
#define KBAT_CONVERT_VOLTAGE_MULT	32	
#define KBAT_CONVERT_VOLTAGE_DIV	7

#define KBAT_CONVERT_CURRENT_MULT	250
#define KBAT_CONVERT_CURRENT_DIV	97


/*--------------------------------------------------------------------
 *! Prototypes Declaration
 */
extern int kbattery_init( void );
extern unsigned short kbattery_Get_Result( knet_dev_t * dev );
extern unsigned short kbattery_Get_Config( knet_dev_t * dev );
extern unsigned short kbattery_Get_Low_Threshold( knet_dev_t * dev );
extern unsigned short kbattery_Get_High_Threshold( knet_dev_t * dev );

extern void kbattery_Set_Config( knet_dev_t * dev, unsigned short Config );
extern void kbattery_Set_Low_Threshold( knet_dev_t * dev, unsigned short Threshold );
extern void kbattery_Set_High_Threshold( knet_dev_t * dev, unsigned short Threshold );

extern void kbattery_Config_Voltage( knet_dev_t * dev);
extern void kbattery_Config_Current( knet_dev_t * dev);

extern unsigned short kbattery_Get_Battery_Voltage( knet_dev_t * dev );
extern unsigned short kbattery_Get_Battery_Current( knet_dev_t * dev );

extern unsigned short kbattery_Get_Battery_Voltage_Single_Shot( knet_dev_t * dev );
extern unsigned short kbattery_Get_Battery_Current_Single_Shot( knet_dev_t * dev );


#endif /* __kbattery__ */
