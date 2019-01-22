
/********************************* WULIAN DOOR LOOK UART COMMAND*************************/

#define	UART_PROTOCOL_HEADER_FIELD 				0xA1
#define	UART_PROTOCOL_TAIL_FIELD				0xFF
#define	UART_PROTOCOL_RESERVE_FIELD				0x00		//保留字段
#define	UART_PROTOCOL_HEADER_FIELD_POSITION 	0
#define	UART_PROTOCOL_LOAD_FIELD_POSITION 		4

#define Zb_Mcu_Fill_Reserve(cmd, len)			UART_PROTOCOL_HEADER_FIELD, cmd, UART_PROTOCOL_RESERVE_FIELD, (len & 0xFF00) >> 8, len & 0x00FF
#define Zb_Mcu_Fill_No_Reserve(cmd)				UART_PROTOCOL_HEADER_FIELD, cmd, (len & 0xFF00) >> 8, len & 0x00FF

#define Mcu_Zb_Fill_Standard(cmd, attri)		ZIGBEE_STANDART_BIT, cmd, (attri & 0xFF00) >> 8, attri & 0x00FF
#define Mcu_Zb_Fill_Manuf(cmd, attri)			MANUFAC_SPECIFIC_BIT, cmd, (attri & 0xFF00) >> 8, attri & 0x00FF

#define HIGH_FOUR_BIT_0							0x00
#define HIGH_FOUR_BIT_1							0x01
#define HIGH_FOUR_BIT_2							0x02
#define HIGH_FOUR_BIT_3							0x03
#define HIGH_FOUR_BIT_4							0x04
#define HIGH_FOUR_BIT_5							0x05
#define HIGH_FOUR_BIT_6							0x06
#define HIGH_FOUR_BIT_7							0x07
#define HIGH_FOUR_BIT_8							0x08
#define HIGH_FOUR_BIT_9							0x09
#define HIGH_FOUR_BIT_A							0x0A
#define HIGH_FOUR_BIT_B							0x0B
#define HIGH_FOUR_BIT_C							0x0C
#define HIGH_FOUR_BIT_D							0x0D
#define HIGH_FOUR_BIT_E							0x0E
#define HIGH_FOUR_BIT_F							0x0F
#define LOW_FOUR_BIT_0							0x00
#define LOW_FOUR_BIT_1							0x01
#define LOW_FOUR_BIT_2							0x02
#define LOW_FOUR_BIT_3							0x03
#define LOW_FOUR_BIT_4							0x04
#define LOW_FOUR_BIT_5							0x05
#define LOW_FOUR_BIT_6							0x06
#define LOW_FOUR_BIT_7							0x07
#define LOW_FOUR_BIT_8							0x08
#define LOW_FOUR_BIT_9							0x09
#define LOW_FOUR_BIT_A							0x0A
#define LOW_FOUR_BIT_B							0x0B
#define LOW_FOUR_BIT_C							0x0C
#define LOW_FOUR_BIT_D							0x0D
#define LOW_FOUR_BIT_E							0x0E
#define LOW_FOUR_BIT_F							0x0F

#define Param_Num_4 							"%x%x%x%x"
#define Param_Num_5 							"%x%x%x%x%x"
#define Param_Num_6 							"%x%x%x%x%x%x"
#define Param_Num_7 							"%x%x%x%x%x%x%x"
#define Param_Num_8 							"%x%x%x%x%x%x%x%x"
#define Param_Num_9								"%x%x%x%x%x%x%x%x%x"
#define Param_Num_A 							"%x%x%x%x%x%x%x%x%x%x"

/********************************* Zigbee to Mcu Uart Load Length*************************/
#define ZB_MCU_LOAD_LEN_0x11							0x08
#define ZB_MCU_LOAD_LEN_0x12							0x07
#define ZB_MCU_LOAD_LEN_0x13							0x14
#define ZB_MCU_LOAD_LEN_0x14							0x03
#define ZB_MCU_LOAD_LEN_0x15							0x08
#define ZB_MCU_LOAD_LEN_0x1A							0x03		//0x55, 0xAA

#define ZB_MCU_LOAD_LEN_0x29							0x02

#define ZB_MCU_LOAD_LEN_0x30							0x01
#define ZB_MCU_LOAD_LEN_0x31							0x02
#define ZB_MCU_LOAD_LEN_0x32							0x02
#define ZB_MCU_LOAD_LEN_0x33							0x02
#define ZB_MCU_LOAD_LEN_0x34							0x01
#define ZB_MCU_LOAD_LEN_0x37							0x01
#define ZB_MCU_LOAD_LEN_0x39							0x0D

#define ZB_MCU_LOAD_LEN_0x42							0x06
#define ZB_MCU_LOAD_LEN_0x43							0x08
#define ZB_MCU_LOAD_LEN_0x44							0x00
#define ZB_MCU_LOAD_LEN_0x45							0x00
#define ZB_MCU_LOAD_LEN_0x4F							0x00

#define ZB_MCU_LOAD_LEN_0xB0							0x02

#define ZB_MCU_LOAD_LEN_0xB1							0x02
#define ZB_MCU_LOAD_LEN_0xB2							0x03
#define ZB_MCU_LOAD_LEN_0xB3							0x01
#define ZB_MCU_LOAD_LEN_0xB4							0x01
#define ZB_MCU_LOAD_LEN_0xB5							0x01
#define ZB_MCU_LOAD_LEN_0xB6							0x02

#define ZB_MCU_LOAD_LEN_0xFD							0x01
/********************************* Zigbee to Mcu Uart Load Length*************************/


