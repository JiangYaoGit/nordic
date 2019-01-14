
/********************************* WULIAN DOOR LOOK CONFIGURATION*************************/

#ifndef WL_DOOR_LOCK_CONFIG_H
#define WL_DOOR_LOCK_CONFIG_H

#include "zboss_api.h"

// <<< Use Configuration Wizard in Context Menu >>>\n



// <h> Basic Attrubute
//==========================================================Basic Attrubute

// <q> BASIC_ATTR_ZCL_VERSION
#ifndef BASIC_ATTR_ZCL_VERSION
#define BASIC_ATTR_ZCL_VERSION 1
#endif

// <q> BASIC_ATTR_VERSION_APPLICCATION
#ifndef BASIC_ATTR_VERSION_APPLICCATION
#define BASIC_ATTR_VERSION_APPLICCATION 1
#endif

// <q> BASIC_ATTR_STACK_VERSION
#ifndef BASIC_ATTR_STACK_VERSION
#define BASIC_ATTR_STACK_VERSION 1
#endif

// <q> BASIC_ATTR_HW_VERSION
#ifndef BASIC_ATTR_HW_VERSION
#define BASIC_ATTR_HW_VERSION 1
#endif

// <q> BASIC_ATTR_MANUFACTURER_NAME
#ifndef BASIC_ATTR_MANUFACTURER_NAME
#define BASIC_ATTR_MANUFACTURER_NAME 1
#endif

// <q> BASIC_ATTR_MODEL_IDENTIFIER
#ifndef BASIC_ATTR_MODEL_IDENTIFIER
#define BASIC_ATTR_MODEL_IDENTIFIER 1
#endif

// <q> BASIC_ATTR_DATE_CODE
#ifndef BASIC_ATTR_DATE_CODE
#define BASIC_ATTR_DATE_CODE 1
#endif

// <q> BASIC_ATTR_POWER_SOURCE
#ifndef BASIC_ATTR_POWER_SOURCE
#define BASIC_ATTR_POWER_SOURCE 1
#endif

// <q> BASIC_ATTR_LOCATION_DESCRIPTION
#ifndef BASIC_ATTR_LOCATION_DESCRIPTION
#define BASIC_ATTR_LOCATION_DESCRIPTION 1
#endif

// <q> BASIC_ATTR_PHYSICAL_ENVIRONMENT
#ifndef BASIC_ATTR_PHYSICAL_ENVIRONMENT
#define BASIC_ATTR_PHYSICAL_ENVIRONMENT 1
#endif

// <q> BASIC_ATTR_DEVICE_ENABLED
#ifndef BASIC_ATTR_DEVICE_ENABLED
#define BASIC_ATTR_DEVICE_ENABLED 0
#endif

// <q> BASIC_ATTR_ALARM_MASK
#ifndef BASIC_ATTR_ALARM_MASK
#define BASIC_ATTR_ALARM_MASK 0
#endif

//==========================================================Basic Attrubute
// </h> 



// <h> Identify Attrubute
//==========================================================Identify Attrubute

// <q> IDENTIFY_ATTR_IDENTIFY_TIME
#ifndef IDENTIFY_ATTR_IDENTIFY_TIME
#define IDENTIFY_ATTR_IDENTIFY_TIME 1
#endif

// <q> IDENTIFY_ATTR_COMMISSION_STATE
#ifndef IDENTIFY_ATTR_COMMISSION_STATE
#define IDENTIFY_ATTR_COMMISSION_STATE 1
#endif

//==========================================================Identify Attrubute
// </h> 



// <h> Group Attrubute
//==========================================================Group Attrubute

// <q> GROUPS_ATTR_NAME_SUPPORT
#ifndef GROUPS_ATTR_NAME_SUPPORT
#define GROUPS_ATTR_NAME_SUPPORT 1
#endif

//==========================================================Group Attrubute
// </h> 


// <h> Scenes Attrubute
//==========================================================Scenes Attrubute

// <q> SCENE_ATTR_SCENE_COUNT
#ifndef SCENE_ATTR_SCENE_COUNT
#define SCENE_ATTR_SCENE_COUNT 1
#endif

// <q> SCENE_ATTR_CURRENT_SCENE
#ifndef SCENE_ATTR_CURRENT_SCENE
#define SCENE_ATTR_CURRENT_SCENE 1
#endif

// <q> SCENE_ATTR_CURRENT_GROUP
#ifndef SCENE_ATTR_CURRENT_GROUP
#define SCENE_ATTR_CURRENT_GROUP 1
#endif

// <q> SCENE_ATTR_SCENE_VALID
#ifndef SCENE_ATTR_SCENE_VALID
#define SCENE_ATTR_SCENE_VALID 1
#endif

// <q> SCENE_ATTR_NAME_SUPPORT
#ifndef SCENE_ATTR_NAME_SUPPORT
#define SCENE_ATTR_NAME_SUPPORT 1
#endif

//==========================================================Scenes Attrubute
// </h> 



// <h> Door Lock Attribute
//==========================================================Door Lock Attribute

// <e> Basic Information Attribute Set	0x0000-0x000F

// <q> DOOR_LOCK_ATTR_LOCK_STATE
#ifndef DOOR_LOCK_ATTR_LOCK_STATE
#define DOOR_LOCK_ATTR_LOCK_STATE 1
#endif

// <q> DOOR_LOCK_ATTR_LOCK_TYPE
#ifndef DOOR_LOCK_ATTR_LOCK_TYPE
#define DOOR_LOCK_ATTR_LOCK_TYPE 1
#endif

// <q> DOOR_LOCK_ATTR_ACTUATOR_ENABLED
#ifndef DOOR_LOCK_ATTR_ACTUATOR_ENABLED
#define DOOR_LOCK_ATTR_ACTUATOR_ENABLED 1
#endif

// <q> DOOR_LOCK_ATTR_DOOR_STATE
#ifndef DOOR_LOCK_ATTR_DOOR_STATE
#define DOOR_LOCK_ATTR_DOOR_STATE 0
#endif

// <q> DOOR_LOCK_ATTR_DOOR_OPEN_EVENTS
#ifndef DOOR_LOCK_ATTR_DOOR_OPEN_EVENTS
#define DOOR_LOCK_ATTR_DOOR_OPEN_EVENTS 0
#endif

// <q> DOOR_LOCK_ATTR_DOOR_CLOSED_EVENTS
#ifndef DOOR_LOCK_ATTR_DOOR_CLOSED_EVENTS
#define DOOR_LOCK_ATTR_DOOR_CLOSED_EVENTS 0
#endif

// <q> DOOR_LOCK_ATTR_OPEN_PERIOD
#ifndef DOOR_LOCK_ATTR_OPEN_PERIOD
#define DOOR_LOCK_ATTR_OPEN_PERIOD 0
#endif

// </e>	0x0000-0x000F


// <e> User/PIN/Schedule Information Attribute	0x0010-0x001F

// <q> DOOR_LOCK_ATTR_NUMBER_OF_LOG_RECORDS_SUPPORTED
#ifndef DOOR_LOCK_ATTR_NUMBER_OF_LOG_RECORDS_SUPPORTED
#define DOOR_LOCK_ATTR_NUMBER_OF_LOG_RECORDS_SUPPORTED 0
#endif

// <q> DOOR_LOCK_ATTR_NUMBER_OF_TOTAL_USERS_SUPPORTED
#ifndef DOOR_LOCK_ATTR_NUMBER_OF_TOTAL_USERS_SUPPORTED
#define DOOR_LOCK_ATTR_NUMBER_OF_TOTAL_USERS_SUPPORTED 1
#endif

// <q> DOOR_LOCK_ATTR_NUMBER_OF_PIN_USERS_SUPPORTED
#ifndef DOOR_LOCK_ATTR_NUMBER_OF_PIN_USERS_SUPPORTED
#define DOOR_LOCK_ATTR_NUMBER_OF_PIN_USERS_SUPPORTED 1
#endif

// <q> DOOR_LOCK_ATTR_NUMBER_OF_RFID_USERS_SUPPORTED
#ifndef DOOR_LOCK_ATTR_NUMBER_OF_RFID_USERS_SUPPORTED
#define DOOR_LOCK_ATTR_NUMBER_OF_RFID_USERS_SUPPORTED 1
#endif

// <q> DOOR_LOCK_ATTR_NUMBER_OF_WEEKDAY_SCHEDULES_SUPPORTED_PER_USER
#ifndef DOOR_LOCK_ATTR_NUMBER_OF_WEEKDAY_SCHEDULES_SUPPORTED_PER_USER
#define DOOR_LOCK_ATTR_NUMBER_OF_WEEKDAY_SCHEDULES_SUPPORTED_PER_USER 1
#endif

// <q> DOOR_LOCK_ATTR_NUMBER_OF_YEARDAY_SCHEDULES_SUPPORTED_PER_USER
#ifndef DOOR_LOCK_ATTR_NUMBER_OF_YEARDAY_SCHEDULES_SUPPORTED_PER_USER
#define DOOR_LOCK_ATTR_NUMBER_OF_YEARDAY_SCHEDULES_SUPPORTED_PER_USER 1
#endif


// <q> DOOR_LOCK_ATTR_NUMBER_OF_HOLIDAY_SCHEDULES_SUPPORTED
#ifndef DOOR_LOCK_ATTR_NUMBER_OF_HOLIDAY_SCHEDULES_SUPPORTED
#define DOOR_LOCK_ATTR_NUMBER_OF_HOLIDAY_SCHEDULES_SUPPORTED 1
#endif

// <q> DOOR_LOCK_ATTR_MAX_PIN_CODE_LENGTH
#ifndef DOOR_LOCK_ATTR_MAX_PIN_CODE_LENGTH
#define DOOR_LOCK_ATTR_MAX_PIN_CODE_LENGTH 1
#endif

// <q> DOOR_LOCK_ATTR_MIN_PIN_CODE_LENGTH
#ifndef DOOR_LOCK_ATTR_MIN_PIN_CODE_LENGTH
#define DOOR_LOCK_ATTR_MIN_PIN_CODE_LENGTH 1
#endif

// <q> DOOR_LOCK_ATTR_MAX_RFID_CODE_LENGTH
#ifndef DOOR_LOCK_ATTR_MAX_RFID_CODE_LENGTH
#define DOOR_LOCK_ATTR_MAX_RFID_CODE_LENGTH 1
#endif

// <q> DOOR_LOCK_ATTR_MIN_RFID_CODE_LENGTH
#ifndef DOOR_LOCK_ATTR_MIN_RFID_CODE_LENGTH
#define DOOR_LOCK_ATTR_MIN_RFID_CODE_LENGTH 1
#endif

// </e>	0x0010-0x001F


// <e> Operational Settings Attribute Set	0x0020-0x002F

// <q> DOOR_LOCK_ATTR_ENABLE_LOGGING
#ifndef DOOR_LOCK_ATTR_ENABLE_LOGGING
#define DOOR_LOCK_ATTR_ENABLE_LOGGING 0
#endif

// <q> DOOR_LOCK_ATTR_LANGUAGE
#ifndef DOOR_LOCK_ATTR_LANGUAGE
#define DOOR_LOCK_ATTR_LANGUAGE 1
#endif

// <q> DOOR_LOCK_ATTR_LED_SETTINGS
#ifndef DOOR_LOCK_ATTR_LED_SETTINGS
#define DOOR_LOCK_ATTR_LED_SETTINGS 1
#endif

// <q> DOOR_LOCK_ATTR_AUTO_RELOCK_TIME
#ifndef DOOR_LOCK_ATTR_AUTO_RELOCK_TIME
#define DOOR_LOCK_ATTR_AUTO_RELOCK_TIME 1
#endif

// <q> DOOR_LOCK_ATTR_SOUND_VOLUME
#ifndef DOOR_LOCK_ATTR_SOUND_VOLUME
#define DOOR_LOCK_ATTR_SOUND_VOLUME 1
#endif

// <q> DOOR_LOCK_ATTR_OPERATING_MODE
#ifndef DOOR_LOCK_ATTR_OPERATING_MODE
#define DOOR_LOCK_ATTR_OPERATING_MODE 1
#endif

// <q> DOOR_LOCK_ATTR_SUPPORTED_OPERATING_MODES
#ifndef DOOR_LOCK_ATTR_SUPPORTED_OPERATING_MODES
#define DOOR_LOCK_ATTR_SUPPORTED_OPERATING_MODES 1
#endif

// <q> DOOR_LOCK_ATTR_DEFAULT_CONFIGURATION_REGISTER
#ifndef DOOR_LOCK_ATTR_DEFAULT_CONFIGURATION_REGISTER
#define DOOR_LOCK_ATTR_DEFAULT_CONFIGURATION_REGISTER 1
#endif

// <q> DOOR_LOCK_ATTR_ENABLE_LOCAL_PROGRAMMING
#ifndef DOOR_LOCK_ATTR_ENABLE_LOCAL_PROGRAMMING
#define DOOR_LOCK_ATTR_ENABLE_LOCAL_PROGRAMMING 1
#endif

// <q> DOOR_LOCK_ATTR_ENABLE_ONE_TOUCH_LOCKING
#ifndef DOOR_LOCK_ATTR_ENABLE_ONE_TOUCH_LOCKING
#define DOOR_LOCK_ATTR_ENABLE_ONE_TOUCH_LOCKING 1
#endif

// <q> DOOR_LOCK_ATTR_ENABLE_INSIDE_STATUS_LED
#ifndef DOOR_LOCK_ATTR_ENABLE_INSIDE_STATUS_LED
#define DOOR_LOCK_ATTR_ENABLE_INSIDE_STATUS_LED 1
#endif

// <q> DOOR_LOCK_ATTR_ENABLE_PRIVACY_MODE_BUTTON
#ifndef DOOR_LOCK_ATTR_ENABLE_PRIVACY_MODE_BUTTON
#define DOOR_LOCK_ATTR_ENABLE_PRIVACY_MODE_BUTTON 1
#endif

// </e>	0x0020-0x002F


// <e> Security Settings Attribute Set 0x0030-0x003F

// <q> DOOR_LOCK_ATTR_WRONG_CODE_ENTRY_LIMIT
#ifndef DOOR_LOCK_ATTR_WRONG_CODE_ENTRY_LIMIT
#define DOOR_LOCK_ATTR_WRONG_CODE_ENTRY_LIMIT 0
#endif

// <q> DOOR_LOCK_ATTR_USER_CODE_TEMPORARY_DISABLE_TIME
#ifndef DOOR_LOCK_ATTR_USER_CODE_TEMPORARY_DISABLE_TIME
#define DOOR_LOCK_ATTR_USER_CODE_TEMPORARY_DISABLE_TIME 1
#endif

// <q> DOOR_LOCK_ATTR_SEND_PIN_OVER_THE_AIR
#ifndef DOOR_LOCK_ATTR_SEND_PIN_OVER_THE_AIR
#define DOOR_LOCK_ATTR_SEND_PIN_OVER_THE_AIR 1
#endif

// <q> DOOR_LOCK_ATTR_REQUIRE_PIN_FOR_RF_OPERATION
#ifndef DOOR_LOCK_ATTR_REQUIRE_PIN_FOR_RF_OPERATION
#define DOOR_LOCK_ATTR_REQUIRE_PIN_FOR_RF_OPERATION 1
#endif

// <q> DOOR_LOCK_ATTR_ZIGBEE_SECURITY_LEVEL
#ifndef DOOR_LOCK_ATTR_ZIGBEE_SECURITY_LEVEL
#define DOOR_LOCK_ATTR_ZIGBEE_SECURITY_LEVEL 1
#endif

// </e>	0x0030-0x003F


// <e> Alarm and Event Masks Attribute	0x0040-0x004F

// <q> DOOR_LOCK_ATTR_ALARM_MASK
#ifndef DOOR_LOCK_ATTR_ALARM_MASK
#define DOOR_LOCK_ATTR_ALARM_MASK 0
#endif

// <q> DOOR_LOCK_ATTR_KEYPAD_OPERATION_EVENT_MASK
#ifndef DOOR_LOCK_ATTR_KEYPAD_OPERATION_EVENT_MASK
#define DOOR_LOCK_ATTR_KEYPAD_OPERATION_EVENT_MASK 1
#endif

// <q> DOOR_LOCK_ATTR_RF_OPERATION_EVENT_MASK
#ifndef DOOR_LOCK_ATTR_RF_OPERATION_EVENT_MASK
#define DOOR_LOCK_ATTR_RF_OPERATION_EVENT_MASK 1
#endif

// <q> DOOR_LOCK_ATTR_MANUAL_OPERATION_EVENT_MASK
#ifndef DOOR_LOCK_ATTR_MANUAL_OPERATION_EVENT_MASK
#define DOOR_LOCK_ATTR_MANUAL_OPERATION_EVENT_MASK 1
#endif

// <q> DOOR_LOCK_ATTR_RFID_OPERATION_EVENT_MASK
#ifndef DOOR_LOCK_ATTR_RFID_OPERATION_EVENT_MASK
#define DOOR_LOCK_ATTR_RFID_OPERATION_EVENT_MASK 1
#endif
// <q> DOOR_LOCK_ATTR_KEYPAD_PROGRAMMING_EVENT_MASK
#ifndef DOOR_LOCK_ATTR_KEYPAD_PROGRAMMING_EVENT_MASK
#define DOOR_LOCK_ATTR_KEYPAD_PROGRAMMING_EVENT_MASK 1
#endif

// <q> DOOR_LOCK_ATTR_RF_PROGRAMMING_EVENT_MASK
#ifndef DOOR_LOCK_ATTR_RF_PROGRAMMING_EVENT_MASK
#define DOOR_LOCK_ATTR_RF_PROGRAMMING_EVENT_MASK 1
#endif

// <q> DOOR_LOCK_ATTR_RFID_PROGRAMMING_EVENT_MASK
#ifndef DOOR_LOCK_ATTR_RFID_PROGRAMMING_EVENT_MASK
#define DOOR_LOCK_ATTR_RFID_PROGRAMMING_EVENT_MASK 1
#endif

// </e>	0x0040-0x004F

//==========================================================Door Lock Attribute
// </h> 
 


// <h> Door Lock Command : ZIGBEE ALLIANCE
//==========================================================Door Lock Command : ZIGBEE ALLIANCE

// <o> DOOR_LOCK_COMMADN_LOCK_DOOR
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_LOCK_DOOR
#define DOOR_LOCK_COMMADN_LOCK_DOOR 0x00
#endif

// <o> DOOR_LOCK_COMMADN_UNLOCK_DOOR
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_UNLOCK_DOOR
#define DOOR_LOCK_COMMADN_UNLOCK_DOOR 0x01
#endif

// <o> DOOR_LOCK_COMMADN_TOGGLE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_TOGGLE
#define DOOR_LOCK_COMMADN_TOGGLE 0x02
#endif

// <o> DOOR_LOCK_COMMADN_UNLOCK_WITH_TIMEOUT
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_UNLOCK_WITH_TIMEOUT
#define DOOR_LOCK_COMMADN_UNLOCK_WITH_TIMEOUT 0x03
#endif

// <o> DOOR_LOCK_COMMADN_GET_LOG_RECORD
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_GET_LOG_RECORD
#define DOOR_LOCK_COMMADN_GET_LOG_RECORD 0x04
#endif

// <o> DOOR_LOCK_COMMADN_SET_PIN_CODE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_SET_PIN_CODE
#define DOOR_LOCK_COMMADN_SET_PIN_CODE 0x05
#endif

// <o> DOOR_LOCK_COMMADN_GET_PIN_CODE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_GET_PIN_CODE
#define DOOR_LOCK_COMMADN_GET_PIN_CODE 0x06
#endif

// <o> DOOR_LOCK_COMMADN_CLEAR_PIN_CODE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_CLEAR_PIN_CODE
#define DOOR_LOCK_COMMADN_CLEAR_PIN_CODE 0x07
#endif

// <o> DOOR_LOCK_COMMADN_CLEAR_ALL_PIN_STATUS
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_CLEAR_ALL_PIN_STATUS
#define DOOR_LOCK_COMMADN_CLEAR_ALL_PIN_STATUS 0x08
#endif

// <o> DOOR_LOCK_COMMADN_SET_USER_STATUS
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_SET_USER_STATUS
#define DOOR_LOCK_COMMADN_SET_USER_STATUS 0x09
#endif


// <o> DOOR_LOCK_COMMADN_GET_USER_STATUS
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_GET_USER_STATUS
#define DOOR_LOCK_COMMADN_GET_USER_STATUS 0x0A
#endif

// <o> DOOR_LOCK_COMMADN_SET_WEEKDAY_SCHEDULE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_SET_WEEKDAY_SCHEDULE
#define DOOR_LOCK_COMMADN_SET_WEEKDAY_SCHEDULE 0x0B
#endif

// <o> DOOR_LOCK_COMMADN_GET_WEEKDAY_SCHEDULE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_GET_WEEKDAY_SCHEDULE
#define DOOR_LOCK_COMMADN_GET_WEEKDAY_SCHEDULE 0x0C
#endif

// <o> DOOR_LOCK_COMMADN_CLEAR_WEEKDAY_SCHEDULE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_CLEAR_WEEKDAY_SCHEDULE
#define DOOR_LOCK_COMMADN_CLEAR_WEEKDAY_SCHEDULE 0x0D
#endif

// <o> DOOR_LOCK_COMMADN_SET_YEARDAY_SCHEDULE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_SET_YEARDAY_SCHEDULE
#define DOOR_LOCK_COMMADN_SET_YEARDAY_SCHEDULE 0x0E
#endif

// <o> DOOR_LOCK_COMMADN_GET_YEARDAY_SCHEDULE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_GET_YEARDAY_SCHEDULE
#define DOOR_LOCK_COMMADN_GET_YEARDAY_SCHEDULE 0x0F
#endif

// <o> DOOR_LOCK_COMMADN_CLEAR_YEARDAY_SCHEDULE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_CLEAR_YEARDAY_SCHEDULE
#define DOOR_LOCK_COMMADN_CLEAR_YEARDAY_SCHEDULE 0x10
#endif

// <o> DOOR_LOCK_COMMADN_SET_HOLIDAY_SCHEDULE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_SET_HOLIDAY_SCHEDULE
#define DOOR_LOCK_COMMADN_SET_HOLIDAY_SCHEDULE 0x11
#endif

// <o> DOOR_LOCK_COMMADN_GET_HOLIDAY_SCHEDULE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_GET_HOLIDAY_SCHEDULE
#define DOOR_LOCK_COMMADN_GET_HOLIDAY_SCHEDULE 0x12
#endif

// <o> DOOR_LOCK_COMMADN_CLEAR_HOLIDAY_SCHEDULE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_CLEAR_HOLIDAY_SCHEDULE
#define DOOR_LOCK_COMMADN_CLEAR_HOLIDAY_SCHEDULE 0x13
#endif

// <o> DOOR_LOCK_COMMADN_SET_USER_TYPE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_SET_USER_TYPE
#define DOOR_LOCK_COMMADN_SET_USER_TYPE 0x14
#endif

// <o> DOOR_LOCK_COMMADN_GET_USER_TYPE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_GET_USER_TYPE
#define DOOR_LOCK_COMMADN_GET_USER_TYPE 0x15
#endif

// <o> DOOR_LOCK_COMMADN_SET_RFID_CODE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_SET_RFID_CODE
#define DOOR_LOCK_COMMADN_SET_RFID_CODE 0x16
#endif

// <o> DOOR_LOCK_COMMADN_GET_RFID_CODE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_GET_RFID_CODE
#define DOOR_LOCK_COMMADN_GET_RFID_CODE 0x17
#endif

// <o> DOOR_LOCK_COMMADN_CLEAR_RFID_CODE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_CLEAR_RFID_CODE
#define DOOR_LOCK_COMMADN_CLEAR_RFID_CODE 0x18
#endif

// <o> DOOR_LOCK_COMMADN_CLEAR_ALL_RFID_CODE
// <0x00-0x20> 
#ifndef DOOR_LOCK_COMMADN_CLEAR_ALL_RFID_CODE
#define DOOR_LOCK_COMMADN_CLEAR_ALL_RFID_CODE 0x19
#endif
//==========================================================Door Lock Command : ZIGBEE ALLIANCE
// </h> 



// <h> Door Lock Command : WULIAN CUSTOM
//==========================================================Door Lock Command : WULIAN CUSTOM
// <e> Door Lock Command : WULIAN CUSTOM

#ifndef DOOR_LOCK_COMMADN_MANUF_ADD_USER
#define DOOR_LOCK_COMMADN_MANUF_ADD_USER 0x80
#endif

#ifndef DOOR_LOCK_COMMADN_MANUF_DELETE_USER
#define DOOR_LOCK_COMMADN_MANUF_DELETE_USER 0x81
#endif

#ifndef DOOR_LOCK_COMMADN_MANUF_DELETE_ALL_USER
#define DOOR_LOCK_COMMADN_MANUF_DELETE_ALL_USER 0x82
#endif

#ifndef DOOR_LOCK_COMMADN_MANUF_CHANGE_CODE
#define DOOR_LOCK_COMMADN_MANUF_CHANGE_CODE 0x83
#endif

#ifndef DOOR_LOCK_COMMADN_MANUF_TIME_SYNC
#define DOOR_LOCK_COMMADN_MANUF_TIME_SYNC 0x90
#endif

#ifndef DOOR_LOCK_COMMADN_MANUF_GET_USER_INFO
#define DOOR_LOCK_COMMADN_MANUF_GET_USER_INFO 0x10
#endif

#ifndef DOOR_LOCK_COMMADN_MANUF_GET_DEV_INFO
#define DOOR_LOCK_COMMADN_MANUF_GET_DEV_INFO 0x11
#endif

#ifndef DOOR_LOCK_COMMADN_MANUF_STATE_REPORT
#define DOOR_LOCK_COMMADN_MANUF_STATE_REPORT 0xF0
#endif

// </e> 
//==========================================================Door Lock Command : WULIAN CUSTOM
// </h> Door Lock Command : WULIAN CUSTOM


// <<< end of configuration section >>>


/* Basic cluster attributes. 0x0000 */
typedef struct
{
	#if	BASIC_ATTR_ZCL_VERSION
		zb_uint8_t zcl_version;			//0x0000
	#endif	
	#if	BASIC_ATTR_VERSION_APPLICCATION
		zb_uint8_t app_version;			//0x0001
	#endif	
	#if	BASIC_ATTR_STACK_VERSION
		zb_uint8_t stack_version;		//0x0002
	#endif	
	#if BASIC_ATTR_HW_VERSION
		zb_uint8_t hw_version;			//0x0003
	#endif
	#if BASIC_ATTR_MANUFACTURER_NAME
		zb_char_t  mf_name[32];			//0x0004   	string
	#endif
	#if BASIC_ATTR_MODEL_IDENTIFIER
		zb_char_t  model_id[32];		//0x0005
	#endif
	#if BASIC_ATTR_DATE_CODE
		zb_char_t  date_code[16];		//0x0006
	#endif
	#if BASIC_ATTR_POWER_SOURCE
		zb_uint8_t power_source;		//0x0007   	enum8
	#endif
	#if BASIC_ATTR_LOCATION_DESCRIPTION
		zb_char_t  location_desc[17];	//0x0010
	#endif
	#if BASIC_ATTR_PHYSICAL_ENVIRONMENT
		zb_char_t  ph_env;				//0x0011	bool
	#endif
	#if BASIC_ATTR_DEVICE_ENABLED
		 zb_bool_t device_enable;		//0x0012
	#endif
	#if BASIC_ATTR_ALARM_MASK
		zb_uint8_t alarm_mask;			//0x0013	map8
	#endif
} zb_device_basic_attr_t;			

/* Identify cluster attributes. 0x0003 */
typedef struct
{
	#if IDENTIFY_ATTR_IDENTIFY_TIME
		zb_uint16_t identify_time;		//0x0000
	#endif
	#if IDENTIFY_ATTR_COMMISSION_STATE
		zb_uint8_t  commission_state;	//0x0001
	#endif
} zb_device_identify_attr_t;

/* Groups cluster attributes. 0x0004 */
typedef struct
{
	#if GROUPS_ATTR_NAME_SUPPORT
		zb_uint8_t name_support;		//0x0004
	#endif
} zb_device_groups_attr_t;

/* Scenes cluster attributes. 0x0005 */
typedef struct
{
	#if SCENE_ATTR_SCENE_COUNT
		zb_uint8_t  scene_count;		//0x000
	#endif
	#if SCENE_ATTR_CURRENT_SCENE
		zb_uint8_t  current_scene;		//0x001
	#endif
	#if SCENE_ATTR_CURRENT_GROUP
		zb_uint16_t current_group;		//0x002
	#endif
	#if SCENE_ATTR_SCENE_VALID
		zb_bool_t   scene_valid;		//0x003
	#endif
	#if SCENE_ATTR_NAME_SUPPORT
		zb_uint8_t  name_support;   	//0x004
	#endif
} zb_device_scenes_attr_t;

/* door lock cluster attributes. 0x0101 */
typedef struct
{	
	/* 0x0000 - 0x000F Basic Information Attribute Set */
	#if DOOR_LOCK_ATTR_LOCK_STATE
		zb_uint8_t  	lock_state;							//0x0000
	#endif
	#if DOOR_LOCK_ATTR_LOCK_TYPE
		zb_uint8_t 		lock_type;							//0x0001
	#endif 	
	#if DOOR_LOCK_ATTR_ACTUATOR_ENABLED
		zb_bool_t 		actuator_enabled;					//0x0002
	#endif
	#if DOOR_LOCK_ATTR_DOOR_STATE
		zb_uint8_t 		door_state;							//0x0003
	#endif
	#if DOOR_LOCK_ATTR_DOOR_OPEN_EVENTS
		zb_uint32_t 	door_open_events;					//0x0004
	#endif
	#if DOOR_LOCK_ATTR_DOOR_CLOSED_EVENTS
		zb_uint32_t 	door_closed_events;					//0x0005
	#endif
	#if DOOR_LOCK_ATTR_OPEN_PERIOD
		zb_uint16_t 	open_period;						//0x0006
	#endif 	
		
	/* 0x0010 – 0x001F User, PIN, Schedule Information Attribute Set */
	#if DOOR_LOCK_ATTR_NUMBER_OF_LOG_RECORDS_SUPPORTED
		zb_uint16_t  	number_of_log_records_supported; 	//0x0010
	#endif
	#if DOOR_LOCK_ATTR_NUMBER_OF_TOTAL_USERS_SUPPORTED
		zb_uint16_t 	number_of_total_users_supported;	//0x0011
	#endif	
	#if DOOR_LOCK_ATTR_NUMBER_OF_PIN_USERS_SUPPORTED
		zb_uint16_t 	number_of_pin_users_supported;		//0x0012
	#endif
	#if DOOR_LOCK_ATTR_NUMBER_OF_RFID_USERS_SUPPORTED
		zb_uint16_t 	number_of_rfid_users_supported;		//0x0013
	#endif
	#if DOOR_LOCK_ATTR_NUMBER_OF_WEEKDAY_SCHEDULES_SUPPORTED_PER_USER	
		zb_uint8_t 		number_of_weekday_schedules_supported_per_user; 	//0x0014
	#endif
	#if	DOOR_LOCK_ATTR_NUMBER_OF_YEARDAY_SCHEDULES_SUPPORTED_PER_USER
		zb_uint8_t 		number_of_yearday_schedules_supported_per_user;		//0x0015
	#endif
	#if DOOR_LOCK_ATTR_NUMBER_OF_HOLIDAY_SCHEDULES_SUPPORTED
		zb_uint8_t 		number_of_holiday_schedules_supported;				//0x0016
	#endif
	#if DOOR_LOCK_ATTR_MAX_PIN_CODE_LENGTH	
		zb_uint8_t 		max_pin_code_length;				//0x0017
	#endif
	#if DOOR_LOCK_ATTR_MIN_PIN_CODE_LENGTH
		zb_uint8_t 		min_pin_code_length;				//0x0018
	#endif
	#if DOOR_LOCK_ATTR_MAX_RFID_CODE_LENGTH
		zb_uint8_t 		max_rfid_code_length;				//0x0019
	#endif 
	#if DOOR_LOCK_ATTR_MIN_RFID_CODE_LENGTH
		zb_uint8_t 		min_rfid_code_length;				//0x001A
	#endif	

	/* 0x0020 – 0x002F Operational Settings Attribute Set */
	#if DOOR_LOCK_ATTR_ENABLE_LOGGING
		zb_bool_t		enable_logging;						//0x0020
	#endif
	#if DOOR_LOCK_ATTR_LANGUAGE
		zb_char_t		language[3];						//0x0021
	#endif
	#if DOOR_LOCK_ATTR_LED_SETTINGS
		zb_uint8_t		led_settings;						//0x0022
	#endif
	#if DOOR_LOCK_ATTR_AUTO_RELOCK_TIME	
		zb_uint32_t  	atuo_relock_time;					//0x0023
	#endif	
	#if DOOR_LOCK_ATTR_SOUND_VOLUME
		zb_uint8_t		sound_volume;						//0x0024
	#endif
	#if DOOR_LOCK_ATTR_OPERATING_MODE
		zb_uint8_t      operating_mode; 					//0x0025
	#endif
	#if DOOR_LOCK_ATTR_SUPPORTED_OPERATING_MODES
		zb_uint16_t     supported_operating_modes; 			//0x0026
	#endif
	#if DOOR_LOCK_ATTR_DEFAULT_CONFIGURATION_REGISTER
		zb_uint16_t     default_configuration_register; 	//0x0027
	#endif
	#if DOOR_LOCK_ATTR_ENABLE_LOCAL_PROGRAMMING
		zb_bool_t		enable_local_programming;			//0x0028
	#endif
	#if DOOR_LOCK_ATTR_ENABLE_ONE_TOUCH_LOCKING
		zb_bool_t		enable_one_touch_locking;			//0x0029
	#endif	
	#if DOOR_LOCK_ATTR_ENABLE_INSIDE_STATUS_LED
		zb_bool_t		enable_inside_status_led;			//0x002A
	#endif
	#if DOOR_LOCK_ATTR_ENABLE_PRIVACY_MODE_BUTTON
		zb_bool_t		enable_privacy_mode_button;			//0x002B
	#endif	
		
	/* 0x0030 – 0x003F Security Settings Attribute Set */
	#if DOOR_LOCK_ATTR_WRONG_CODE_ENTRY_LIMIT
		zb_uint8_t      wrong_code_entry_limit;				//0x0030
	#endif
	#if DOOR_LOCK_ATTR_USER_CODE_TEMPORARY_DISABLE_TIME	
		zb_uint8_t		user_code_temporary_disable_time;	//0x0031
	#endif
	#if DOOR_LOCK_ATTR_SEND_PIN_OVER_THE_AIR
		zb_bool_t 		send_pin_over_the_air;				//0x0032
	#endif
	#if DOOR_LOCK_ATTR_REQUIRE_PIN_FOR_RF_OPERATION
		zb_bool_t 		require_pin_for_rf_operation;		//0x0033
	#endif	
	#if DOOR_LOCK_ATTR_ZIGBEE_SECURITY_LEVEL
		zb_uint8_t		zigbee_security_level;				//0x0034
	#endif
		
	/* 0x0040 – 0x004F Alarm and Event Masks Attribute Set */
	#if DOOR_LOCK_ATTR_ALARM_MASK
		zb_uint16_t		alarm_mask;							//0x0040
	#endif
	#if DOOR_LOCK_ATTR_KEYPAD_OPERATION_EVENT_MASK	
		zb_uint16_t 	keypad_operation_event_mask;		//0x0041
	#endif	
	#if DOOR_LOCK_ATTR_RF_OPERATION_EVENT_MASK
		zb_uint16_t		rf_operation_event_mask;			//0x0042
	#endif
	#if DOOR_LOCK_ATTR_MANUAL_OPERATION_EVENT_MASK	
		zb_uint16_t		manual_operation_event_mask;		//0x0043
	#endif 
	#if DOOR_LOCK_ATTR_RFID_OPERATION_EVENT_MASK	
		zb_uint16_t		rfid_operation_event_mask;			//0x0044
	#endif
	#if DOOR_LOCK_ATTR_KEYPAD_PROGRAMMING_EVENT_MASK	
		zb_uint16_t		keypad_programming_event_mask;		//0x0045
	#endif
	#if DOOR_LOCK_ATTR_RF_PROGRAMMING_EVENT_MASK	
		zb_uint16_t 	rf_programming_event_mask;			//0x0046
	#endif
	#if DOOR_LOCK_ATTR_RFID_PROGRAMMING_EVENT_MASK	
		zb_uint16_t		rfid_programming_event_mask;		//0x0047
	#endif		
	
	/* 0x8000 – 0xEFFF wulian manufacter specific attribute define */
	zb_uint8_t		verify_code_event;				//0x8000
	zb_uint8_t		number_of_figner_user_support;	//0x8001
	zb_uint8_t		add_user_event;					//0x8002
	zb_uint8_t		delete_user_event;				//0x8003
	zb_uint8_t		change_code_event;				//0x8004
	zb_uint8_t		time_state;						//0x9004
	zb_uint8_t		prying_resistant;				//0xf000
	zb_uint8_t		hijacking_prevention;			//0xf001
	zb_uint8_t		system_lock;					//0xf002
	zb_uint8_t		voltage_level;					//0xf003
	zb_uint8_t		back_lock;						//0xf004
	
} zb_device_door_lock_attr_t;

#endif //WL_DOOR_LOCK_H



typedef struct
{
	zb_uint8_t lock_door_len;
	zb_uint8_t unlock_door_len;
	zb_uint8_t toggle_len;
	zb_uint8_t unlock_with_timeout_len;
	zb_uint8_t get_log_record_len;
	zb_uint8_t set_pin_code_len;
	zb_uint8_t get_pin_code_len;
	zb_uint8_t clear_pin_code_len;
	zb_uint8_t clear_all_pin_code_len;
	zb_uint8_t set_user_status_len;
	zb_uint8_t get_user_status_len;
	zb_uint8_t set_weekday_schedule_len;
	zb_uint8_t get_weekday_schedule_len;
	zb_uint8_t clear_weekday_schedule_len;
	zb_uint8_t set_yearday_schedule_len;
	zb_uint8_t get_yearday_schedule_len;
	zb_uint8_t clear_yearday_schedule_len;
	zb_uint8_t set_holiday_schedule_len;
	zb_uint8_t get_holiday_schedule_len;
	zb_uint8_t clear_holiday_schedule_len;
	zb_uint8_t set_user_type_len;
	zb_uint8_t get_user_type_len;
	zb_uint8_t set_rfid_code_len;
	zb_uint8_t get_rfid_code_len;
	zb_uint8_t clear_rfid_code_len;
	zb_uint8_t clear_all_rfid_code_len;
}zb_device_door_lock_cmd_len_t;

typedef struct
{
	zb_uint8_t * lock_door;
	zb_uint8_t * unlock_door;
	zb_uint8_t * toggle;
	zb_uint8_t * unlock_with_timeout;
	zb_uint8_t * get_log_record;
	zb_uint8_t * set_pin_code;
	zb_uint8_t * get_pin_code;
	zb_uint8_t * clear_pin_code;
	zb_uint8_t * clear_all_pin_code;
	zb_uint8_t * set_user_status;
	zb_uint8_t * get_user_status;
	zb_uint8_t * set_weekday_schedule;
	zb_uint8_t * get_weekday_schedule;
	zb_uint8_t * clear_weekday_schedule;
	zb_uint8_t * set_yearday_schedule;
	zb_uint8_t * get_yearday_schedule;
	zb_uint8_t * clear_yearday_schedule;
	zb_uint8_t * set_holiday_schedule;
	zb_uint8_t * get_holiday_schedule;
	zb_uint8_t * clear_holiday_schedule;
	zb_uint8_t * set_user_type;
	zb_uint8_t * get_user_type;
	zb_uint8_t * set_rfid_code;
	zb_uint8_t * get_rfid_code;
	zb_uint8_t * clear_rfid_code;
	zb_uint8_t * clear_all_rfid_code;
	zb_device_door_lock_cmd_len_t len;
}zb_device_door_lock_cmd_t;


#define ZB_CMD_PTR_DEF(name)  zb_device_door_lock_cmd_t name##_cmd = 			\
							{													\
								.lock_door = NULL,								\
								.unlock_door = NULL,							\
								.toggle = NULL,									\
								.unlock_with_timeout = NULL,					\
								.get_log_record = NULL,							\
								.set_pin_code = NULL,							\
								.get_pin_code = NULL,							\
								.clear_pin_code = NULL,							\
								.clear_all_pin_code = NULL,						\
								.set_user_status = NULL,						\
								.get_user_status = NULL,						\
								.set_weekday_schedule = NULL,					\
								.get_weekday_schedule = NULL,					\
								.clear_weekday_schedule = NULL,					\
								.set_yearday_schedule = NULL,					\
								.get_yearday_schedule = NULL,					\
								.clear_yearday_schedule = NULL,					\
								.set_holiday_schedule = NULL,					\
								.get_holiday_schedule = NULL,					\
								.clear_holiday_schedule = NULL,					\
								.set_user_type = NULL,							\
								.get_user_type = NULL,							\
								.set_rfid_code = NULL,							\
								.get_rfid_code = NULL,							\
								.clear_rfid_code = NULL,						\
								.clear_all_rfid_code = NULL,					\
								{												\
									0											\
								}												\
							}

						




