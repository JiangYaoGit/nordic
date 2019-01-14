/**
 * Copyright (c) 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup zigbee_examples_multiprotocol_nus_switch main.c
 * @{
 * @ingroup  zigbee_examples
 * @brief    UART over BLE application with Zigbee HA light switch profile.
 *
 * This file contains the source code for a sample application that uses the Nordic UART service
 * and a light switch operating a Zigbee network.
 * This application uses the @ref srvlib_conn_params module.
 */
#include "zboss_api.h"
#include "zb_mem_config_min.h"
#include "zb_error_handler.h"
#include "wl_door_lock_config.h"

#include "nrf_ble_gatt.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_hci.h"
#include "ble_nus.h"

#include "app_timer.h"
#include "boards.h"
#include "bsp_btn_ble.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
/****************************************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "bsp_btn_ble.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_state.h"
#include "ble_dfu.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "fds.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_clock.h"
#include "nrf_power.h"
#include "nrf_bootloader_info.h"
#include "fds.h"

#include "app_uart.h"
#include "nrf_uart.h"
/****************************************************************************************************************/
/** @brief General purpose button type. **/
typedef enum zb_button_e
{
	ZB_BUTTON_TIME_SHORT = 0, 		 /**<  >50ms */
	ZB_BUTTON_TIME_MIDDLE = 1, 		 /**<  >1s   */
	ZB_BUTTON_TIME_LONG = 3,   		 /**<  >3s   */
	ZB_BUTTON_TIME_THRESHOLD = 4     /**<  >10s  */
}zb_button_t; 

#define NETWORK_BUTTON_SET 						BSP_BOARD_BUTTON_3
#define NETWORK_BUTTON_SHORT_TIME  				ZB_TIME_ONE_SECOND * 2 
#define NETWORK_BUTTON_THRESHOLD_TIME  			ZB_TIME_ONE_SECOND * 3
#define NETWORK_BUTTON_LONG_TIME  				ZB_TIME_ONE_SECOND * 6 
#define NETWORK_BUTTON_SHORT_POLL_TMO  			ZB_MILLISECONDS_TO_BEACON_INTERVAL(30) 
#define NETWORK_BUTTON_LONG_POLL_TMO 			ZB_MILLISECONDS_TO_BEACON_INTERVAL(100)
/****************************************************************************************************************/
#define NETWORK_LED_INDICATE          			BSP_BOARD_LED_2 	
#define LED_FILL_IN_DUTY_TIMER(param_ms)           	ZB_MILLISECONDS_TO_BEACON_INTERVAL(param_ms)
#define LED_FILL_IN_CYCLE_TIMER(param_ms)           	ZB_MILLISECONDS_TO_BEACON_INTERVAL(param_ms)
static void zb_led_indicate(uint32_t led_idx, uint8_t led_cnt, uint8_t duty_timer, uint8_t cycle_timer);  //建议duty_timer不要超过(cycle_timer-20)
/****************************************************************************************************************/
/* bit7 bit6 bit5 bit4 bit3 bit2     bit1     bit0
*                reverser            leave    join */
#define NETWORK_STATUS_JOIN_REJOIN 				0x01
#define NETWORK_STATUS_LEAVE 				 	0x02	
zb_uint8_t network_status = NETWORK_STATUS_LEAVE;												//入网、退网状态设置，并写入flash

#define NETWORK_SEEK_START						0x01
#define NETWORK_SEEK_STOP						0x02
zb_uint8_t seek_network_status = NETWORK_SEEK_STOP;												//搜网状态设置
zb_uint32_t seek_network_damon_cnt = 0;															//守护进程计数值

//0 30 60 120 240 min	
#define TIME_30			30		
#define TIME_60			60
#define TIME_120		120
#define DEMON_PROCESS_TIME_BASE					10
#define ONE_MINUTE_EQUAL_60_SECOND				60
#define NETWORK_SEEK_FREQUENCY					APP_TIMER_TICKS(24 * 1000) 						//搜网周期24s，一个信道1s
#define NETWORK_SEEK_STOP_TIMER					ZB_TIME_ONE_SECOND * 120						//每个搜网持续时间2min
#define NETWORK_DEOMON_PROCESS					ZB_TIME_ONE_SECOND * DEMON_PROCESS_TIME_BASE    //搜网守护进程5s检测一次
#define NETWORK_SEEK_START_0					1
#define NETWORK_SEEK_START_30					TIME_30 * ONE_MINUTE_EQUAL_60_SECOND / DEMON_PROCESS_TIME_BASE
#define NETWORK_SEEK_START_60					TIME_60 * ONE_MINUTE_EQUAL_60_SECOND / DEMON_PROCESS_TIME_BASE
#define NETWORK_SEEK_START_120					TIME_120 * ONE_MINUTE_EQUAL_60_SECOND / DEMON_PROCESS_TIME_BASE					
/****************************************************************************************************************/
/****************************************************************************************************************/

#define FDS_TEST_FILE							0xF100
#define FDS_TEST_KEY							0xE000
/* 写入flash数据格式定义，此处可自行添加结构体成员 */
typedef struct 
{
	uint32_t cnt;
	zb_uint8_t status_flag;
}fds_type_t;

static fds_type_t fds_test_cfg =
{
    .cnt  = 0x0,
    .status_flag  = NETWORK_STATUS_LEAVE,
};

/* 初始化fds结构体，并将fds_type_t赋值给fds结构体 */
static fds_record_t const fds_test_record =
{
    .file_id           = FDS_TEST_FILE,
    .key               = FDS_TEST_KEY,
    .data.p_data       = &fds_test_cfg,
    /* The length of a record is always expressed in 4-byte units (words). */
    .data.length_words = (sizeof(fds_test_cfg) + 3) / sizeof(uint32_t),
};
bool   fds_init_flag = 0;           //fds初始化标志位
static void fds_evt_handler(fds_evt_t const * p_evt);
static zb_void_t fds_read_handler(zb_uint8_t * param);
static zb_void_t fds_write_handler(zb_uint8_t param);
/****************************************************************************************************************/
static zb_ieee_addr_t test_address =  {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
static zb_uint8_t zb_ic[18] = {0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x11, 0x22, 		\
						0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0xD4, 0x90};
/****************************************************************************************************************/
#define APP_BLE_CONN_CFG_TAG                1                                       /**< A tag identifying the SoftDevice BLE configuration. */

#define DEVICE_NAME                         "Zigbee_UART"                           /**< Name of device. Will be included in the advertising data. */
#define NUS_SERVICE_UUID_TYPE               BLE_UUID_TYPE_VENDOR_BEGIN              /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_BLE_OBSERVER_PRIO               1                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define APP_ADV_INTERVAL                    320                                     /**< The advertising interval (in units of 0.625 ms. This value corresponds to 200 ms). */
#define APP_ADV_DURATION                    18000                                   /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define MIN_CONN_INTERVAL                   MSEC_TO_UNITS(200, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (200 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL                   MSEC_TO_UNITS(300, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (300 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                       0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                    MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY      APP_TIMER_TICKS(5000)                   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT        3                                       /**< Number of attempts before giving up the connection parameter negotiation. */
#define NRF_BLE_GATT_ATT_MTU_DEFAULT        NRF_SDH_BLE_GATT_MAX_MTU_SIZE           /**< Requested ATT_MTU size. This value most not be greater than NRF_SDH_BLE_GATT_MAX_MTU_SIZE. */

#define CAFE_CAFE                           0xCAFECAFE                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define UART_TX_BUF_SIZE                    256                                     /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                    256                                     /**< UART RX buffer size. */

//#define IEEE_CHANNEL_MASK ((1l << 11)|(1l << 12)|(1l << 13)|(1l << 14)|				\
//			(1l << 15)|(1l << 16)|(1l << 17)|(1l << 18)|(1l << 19)|(1l << 20)|		\
//			(1l << 21)|(1l << 22)|(1l << 23)|(1l << 24)|(1l << 25)|(1l << 26))
			
#define IEEE_CHANNEL_MASK                   ZIGBEE_CHANNEL                  		/**< Scan only one, predefined channel to find the coordinator. */
#define LIGHT_SWITCH_ENDPOINT               1                                       /**< Source endpoint used to control light bulb. */
#define MATCH_DESC_REQ_START_DELAY          (2 * ZB_TIME_ONE_SECOND)                /**< Delay between the light switch startup and light bulb finding procedure. */
#define MATCH_DESC_REQ_TIMEOUT              (5 * ZB_TIME_ONE_SECOND)                /**< Timeout for finding procedure. */
#define ERASE_PERSISTENT_CONFIG             ZB_FALSE                                /**< Do not erase NVRAM to save the network parameters after device reboot or power-off. NOTE: If this option is set to ZB_TRUE then do full device erase for all network devices before running other samples. */
#define ZIGBEE_NETWORK_STATE_LED            BSP_BOARD_LED_2                         /**< LED indicating that light switch successfully joind ZigBee network. */
#define BULB_FOUND_LED                      BSP_BOARD_LED_3                         /**< LED indicating that light witch found a light bulb to control. */
#define LIGHT_SWITCH_BUTTON_OFF             BSP_BOARD_BUTTON_1                      /**< Button ID used to switch off the light bulb. */
#define LIGHT_SWITCH_BUTTON_ON              BSP_BOARD_BUTTON_0                      /**< Button ID used to switch on the light bulb. */
#define SLEEPY_ON_BUTTON                    BSP_BOARD_BUTTON_2                      /**< Button ID used to determine if we need the sleepy device behaviour (pressed means yes). */

#define LIGHT_SWITCH_DIMM_STEP              15                                      /**< DIm step size - increases/decreses current level (range 0x000 - 0xfe). */
#define LIGHT_SWITCH_DIMM_TRANSACTION_TIME  2                                       /**< Trasnsition time for a single step operation in 0.1 sec units. 0xFFFF - immediate change. */

#define LIGHT_SWITCH_BUTTON_THRESHOLD       ZB_TIME_ONE_SECOND                      /**< Number of beacon intervals the button should be pressed to dimm the light bulb. */
#define LIGHT_SWITCH_BUTTON_SHORT_POLL_TMO  ZB_MILLISECONDS_TO_BEACON_INTERVAL(50)  /**< Delay between button state checks used in order to detect button long press. */
#define LIGHT_SWITCH_BUTTON_LONG_POLL_TMO   ZB_MILLISECONDS_TO_BEACON_INTERVAL(300) /**< Time after which the button state is checked again to detect button hold - the dimm command is sent again. */

/* NOTE: Any numeric value within range 0 - 999 received over BLE UART will start a delayed toggle operation. */
#define COMMAND_ON                          "n"                                     /**< UART command that will turn on found light bulb(s). */
#define COMMAND_OFF                         "f"                                     /**< UART command that will turn off found light bulb(s). */
#define COMMAND_TOGGLE                      "t"                                     /**< UART command that will turn toggle found light bulb(s). */
#define COMMAND_INCREASE                    "i"                                     /**< UART command that will increase brightness of found light bulb(s). */
#define COMMAND_DECRESE                     "d"                                     /**< UART command that will decrease brightness of found light bulb(s). */
#define DELAYED_COMMAND_RETRY_MS            100                                     /**< If sending toggle command was impossible due tothe lack of Zigbee buffers, retry sending it after DELAYED_COMMAND_RETRY_MS ms. */

#if !defined ZB_ED_ROLE
#error Define ZB_ED_ROLE to compile light switch (End Device) source code.
#endif

/* Variables used to remember found light bulb(s). */
typedef struct light_switch_bulb_params_s
{
    zb_uint8_t  endpoint;
    zb_uint16_t short_addr;
} light_switch_bulb_params_t;

/* Variables used to recognize the type of button press. */
typedef struct light_switch_button_s
{
    zb_bool_t in_progress;
    zb_time_t timestamp;
} light_switch_button_t;

/* Main application customizable context. Stores all settings and static values. */
typedef struct light_switch_ctx_s
{
    light_switch_bulb_params_t bulb_params;
    light_switch_button_t      button;
} light_switch_ctx_t;


static zb_void_t find_light_bulb_timeout(zb_uint8_t param);

//NRF_BLE_QWR_DEF(m_qwr); 
APP_TIMER_DEF(seek_network_start_timer); 
//APP_TIMER_DEF(seek_network_stop_timer); 
//BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);                                   /**< BLE NUS service instance. */
NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
BLE_ADVERTISING_DEF(m_advertising);                                                 /**< Advertising module instance. */

static uint16_t   m_conn_handle          = BLE_CONN_HANDLE_INVALID;                 /**< Handle of the current connection. */
static uint16_t   m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;            /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
static ble_uuid_t m_adv_uuids[]          =                                          /**< Universally unique service identifier. */
{
    {BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}
};

static light_switch_ctx_t m_device_ctx;
/**********************************************************************************************/
#define HA_DOOR_LOCK_ENDPOINT        20    
/* Basic cluster attributes initial values. */
#define BULB_INIT_BASIC_APP_VERSION       01                                    /**< Version of the application software (1 byte). */
#define BULB_INIT_BASIC_STACK_VERSION     10                                    /**< Version of the implementation of the ZigBee stack (1 byte). */
#define BULB_INIT_BASIC_HW_VERSION        11                                    /**< Version of the hardware of the device (1 byte). */
#define BULB_INIT_BASIC_MANUF_NAME        "Nordic"                              /**< Manufacturer name (32 bytes). */
#define BULB_INIT_BASIC_MODEL_ID          "Dimable_Light_v0.1"                  /**< Model number assigned by manufacturer (32-bytes long string). */
#define BULB_INIT_BASIC_DATE_CODE         "20180416"                            /**< First 8 bytes specify the date of manufacturer of the device in ISO 8601 format (YYYYMMDD). Th rest (8 bytes) are manufacturer specific. */
#define BULB_INIT_BASIC_POWER_SOURCE      ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE   /**< Type of power sources available for the device. For possible values see section 3.2.2.2.8 of ZCL specification. */
#define BULB_INIT_BASIC_LOCATION_DESC     "Office desk"                         /**< Describes the physical location of the device (16 bytes). May be modified during commisioning process. */
#define BULB_INIT_BASIC_PH_ENV            ZB_ZCL_BASIC_ENV_UNSPECIFIED  
/**********************************************************************************************/
/************************************************************************************************************/
/*****************************************state reporting****************************************************/
//zcl头控制字段
#define GOLBAL_COMMAND                    1 
#define SPECIFIC_CLUSTER_COMMAND          0
#define MAMUFAC_SPECIFIC_BIT              ZB_ZCL_MANUFACTURER_SPECIFIC
#define ZIGBEE_STANDART_BIT               ZB_ZCL_NOT_MANUFACTURER_SPECIFIC 
#define MANUFAC_CODE  					  0xEE00        //厂商编号，zigbee联盟分配
zb_uint16_t dst_short_addr = 0;
//ZB_GET_OUT_BUF_DELAYED(report_handler);

#define ZB_ZCL_GENERAL_REPORT_HANDLER(buffer, load_buf, load_len, addr, cmd_id_manuf, cb)   \
{  																											\
	zb_uint8_t* ptr =NULL;																					\
	ptr = ZB_ZCL_START_PACKET(buffer)                                               				 		\
	ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(                                 					\
		ptr, ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC, ZB_ZCL_DISABLE_DEFAULT_RESPONSE); \
	ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(), 1, MANUFAC_CODE, cmd_id_manuf); 		    \
	for(char i = 0; i < load_len; i++)    					\
	{ 													    \
		ZB_ZCL_PACKET_PUT_DATA8(ptr, load_buf[i]);			\
	}   													\
	ZB_ZCL_FINISH_PACKET(buffer, ptr)    					\
	ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, 1, HA_DOOR_LOCK_ENDPOINT,  	\
	ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_DOOR_LOCK, cb);   											    \
}


static zb_void_t report_handler(zb_uint8_t param)  
{
	//zb_uint8_t 		   * ptr = NULL;
	zb_buf_t           * p_buf = ZB_BUF_FROM_REF(param);
	char buf[10] = {0};
	memcpy(buf, "hello", strlen("hello"));
	ZB_ZCL_GENERAL_REPORT_HANDLER(p_buf, buf, strlen("123"), dst_short_addr, DOOR_LOCK_COMMADN_MANUF_STATE_REPORT,NULL);

//	ZB_ZCL_GENERAL_INIT_CONFIGURE_REPORTING_CLI_REQ(p_buf, ptr, ZB_ZCL_DISABLE_DEFAULT_RESPONSE);
//	ZB_ZCL_GENERAL_SEND_CONFIGURE_REPORTING_REQ(p_buf, ptr, dst_short_addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, 1,
//		       HA_DOOR_LOCK_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_DOOR_LOCK, NULL);
	return;
}
/*****************************************state reporting****************************************************/
/************************************************************************************************************/

/* Main application customizable context. Stores all settings and static values. */
typedef struct
{
    zb_device_basic_attr_t         basic_attr;
    zb_device_identify_attr_t      identify_attr;
    zb_device_scenes_attr_t        scenes_attr;
    zb_device_groups_attr_t        groups_attr;
    zb_device_door_lock_attr_t     door_lock_attr;
} zb_device_ctx_t;

static zb_device_ctx_t dev_ctx;
ZB_CMD_PTR_DEF(rev);

/* declare cluster attributes*/
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST_HA(identify_attr_list,
                                       &dev_ctx.identify_attr.identify_time,
                                       &dev_ctx.identify_attr.commission_state);


ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attr_list, &dev_ctx.groups_attr.name_support);

ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(scenes_attr_list,
                                  &dev_ctx.scenes_attr.scene_count,
                                  &dev_ctx.scenes_attr.current_scene,
                                  &dev_ctx.scenes_attr.current_group,
                                  &dev_ctx.scenes_attr.scene_valid,
                                  &dev_ctx.scenes_attr.name_support);

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_HA_ADDS_FULL(basic_attr_list,
                                              &dev_ctx.basic_attr.zcl_version,
                                              &dev_ctx.basic_attr.app_version,
                                              &dev_ctx.basic_attr.stack_version,
                                              &dev_ctx.basic_attr.hw_version,
                                              dev_ctx.basic_attr.mf_name,
                                              dev_ctx.basic_attr.model_id,
                                              dev_ctx.basic_attr.date_code,
                                              &dev_ctx.basic_attr.power_source,
                                              dev_ctx.basic_attr.location_desc,
                                              &dev_ctx.basic_attr.ph_env);

/* Door lock cluster attributes - custom adds */
//#define ZB_ZCL_DECLARE_DOOR_LOCK_CLUSTER_ATTRIB_LIST_ADD(attr_list,      \
//                                                     lock_state,         \
//                                                     lock_type,          \
//                                                     actuator_enabled,   \
//												number_of_log_records_supported,\
//												number_of_total_users_supported,\
//												number_of_pin_users_supported,\
//												number_of_rfid_users_supported,\
//												number_of_weekday_schedules_supported_per_user,\
//												number_of_yearday_schedules_supported_per_user,\
//												number_of_holiday_schedules_supported,\
//												max_pin_code_length,\
//												min_pin_code_length,\
//												max_rfid_code_length,\
//												min_rfid_code_length)     \
//  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                         \
//  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_ID, (lock_state))             \
//  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_ID, (lock_type))               \
//  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_ID, (actuator_enabled)) \
//ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_NUMBER_OF_LOG_RECORDS_SUPPORTED_ID, (number_of_log_records_supported)) \
//ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_NUM_TOTAL_USERS_ID, (number_of_total_users_supported)) \
//ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_NUM_PIN_USERS_ID, (number_of_pin_users_supported)) \
//ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_NUMBER_OF_RFID_USERS_SUPPORTED_ID, (number_of_rfid_users_supported)) \
//ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_NUM_WEEK_DAY_SCHEDULE_PER_USER_ID, (number_of_weekday_schedules_supported_per_user)) \
//ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_NUM_YEAR_DAY_SCHEDULE_PER_USER_ID, (number_of_yearday_schedules_supported_per_user)) \
//ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_NUM_HOLIDAY_SCHEDULE_ID, (number_of_holiday_schedules_supported)) \
//ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_MAX_PIN_LEN_ID, (max_pin_code_length)) \
//ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_MIN_PIN_LEN_ID, (min_pin_code_length)) \
//ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_MAX_RFID_CODE_LENGTH_ID, (max_rfid_code_length)) \
//ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_MIN_RFID_CODE_LENGTH_ID, (min_rfid_code_length)) \
//  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST  

//ZB_ZCL_DECLARE_DOOR_LOCK_CLUSTER_ATTRIB_LIST_ADD(door_lock_attr_list,
//											&dev_ctx.door_lock_attr.lock_state,
//											&dev_ctx.door_lock_attr.lock_type,
//											&dev_ctx.door_lock_attr.actuator_enabled,
//											&dev_ctx.door_lock_attr.number_of_log_records_supported,
//											&dev_ctx.door_lock_attr.number_of_total_users_supported,
//											&dev_ctx.door_lock_attr.number_of_pin_users_supported,
//											&dev_ctx.door_lock_attr.number_of_rfid_users_supported,
//											&dev_ctx.door_lock_attr.number_of_weekday_schedules_supported_per_user,
//											&dev_ctx.door_lock_attr.number_of_yearday_schedules_supported_per_user,
//											&dev_ctx.door_lock_attr.number_of_holiday_schedules_supported,
//											&dev_ctx.door_lock_attr.max_pin_code_length,
//											&dev_ctx.door_lock_attr.min_pin_code_length,
//											&dev_ctx.door_lock_attr.max_rfid_code_length,
//											&dev_ctx.door_lock_attr.min_rfid_code_length);


#define ZB_ZCL_DECLARE_DOOR_LOCK_CLUSTER_ATTRIB_LIST_C(attr_list,          						\
														lock_state,        					 	\
														lock_type,        					  	\
														actuator_enabled,  						\
														number_of_total_users_supported,		\
														number_of_pin_users_supported,			\
														max_pin_code_length,					\
														min_pin_code_length,					\
														atuo_relock_time)   					\
	ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                         		\
	ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_ID, (lock_state))             		\
	ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_ID, (lock_type))               		\
	ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_ID, (actuator_enabled)) 		\
	ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_NUM_TOTAL_USERS_ID, (number_of_total_users_supported)) 	\
	ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_NUM_PIN_USERS_ID, (number_of_pin_users_supported)) 		\
	ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_MAX_PIN_LEN_ID, (max_pin_code_length)) 			\
	ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_MIN_PIN_LEN_ID, (min_pin_code_length)) 			\
	ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_AUTO_RELOCK_TIME_ID, (atuo_relock_time)) 		\
	ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST
  
ZB_ZCL_DECLARE_DOOR_LOCK_CLUSTER_ATTRIB_LIST_C(door_lock_attr_list,
											&dev_ctx.door_lock_attr.lock_state,
											&dev_ctx.door_lock_attr.lock_type,
											&dev_ctx.door_lock_attr.actuator_enabled,
											&dev_ctx.door_lock_attr.number_of_total_users_supported,
											&dev_ctx.door_lock_attr.number_of_pin_users_supported,
											&dev_ctx.door_lock_attr.max_pin_code_length,
											&dev_ctx.door_lock_attr.min_pin_code_length,
											&dev_ctx.door_lock_attr.atuo_relock_time);						
											
ZB_HA_DECLARE_DOOR_LOCK_CLUSTER_LIST(door_lock_clusters,
									door_lock_attr_list,
									basic_attr_list,
									identify_attr_list,
									groups_attr_list,
									scenes_attr_list);

ZB_HA_DECLARE_DOOR_LOCK_EP(door_lock_ep,
						HA_DOOR_LOCK_ENDPOINT,
						door_lock_clusters);

ZB_HA_DECLARE_DOOR_LOCK_CTX(door_lock_ctx,
						door_lock_ep);

/**@brief Function for initializing all clusters attributes.
 */
static void bulb_clusters_attr_init(void)
{
    /* Basic cluster attributes data */
    dev_ctx.basic_attr.zcl_version   = ZB_ZCL_VERSION;
    dev_ctx.basic_attr.app_version   = BULB_INIT_BASIC_APP_VERSION;
    dev_ctx.basic_attr.stack_version = BULB_INIT_BASIC_STACK_VERSION;
    dev_ctx.basic_attr.hw_version    = BULB_INIT_BASIC_HW_VERSION;

    /* Use ZB_ZCL_SET_STRING_VAL to set strings, because the first byte should
     * contain string length without trailing zero.
     *
     * For example "test" string wil be encoded as:
     *   [(0x4), 't', 'e', 's', 't']
     */
    ZB_ZCL_SET_STRING_VAL(dev_ctx.basic_attr.mf_name,
                          BULB_INIT_BASIC_MANUF_NAME,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MANUF_NAME));

    ZB_ZCL_SET_STRING_VAL(dev_ctx.basic_attr.model_id,
                          BULB_INIT_BASIC_MODEL_ID,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MODEL_ID));

    ZB_ZCL_SET_STRING_VAL(dev_ctx.basic_attr.date_code,
                          BULB_INIT_BASIC_DATE_CODE,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_DATE_CODE));

    dev_ctx.basic_attr.power_source = BULB_INIT_BASIC_POWER_SOURCE;

    ZB_ZCL_SET_STRING_VAL(dev_ctx.basic_attr.location_desc,
                          BULB_INIT_BASIC_LOCATION_DESC,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_LOCATION_DESC));


    dev_ctx.basic_attr.ph_env = BULB_INIT_BASIC_PH_ENV;

    /* Identify cluster attributes data */
    dev_ctx.identify_attr.identify_time    = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;
    dev_ctx.identify_attr.commission_state = ZB_ZCL_ATTR_IDENTIFY_COMMISSION_STATE_HA_ID_DEF_VALUE;
	
	dev_ctx.door_lock_attr.lock_state = ZB_ZCL_ATTR_DOOR_LOCK_DOOR_STATE_CLOSED;
	dev_ctx.door_lock_attr.lock_type = ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_MAGNETIC;
}

/*****************************ack******************************************************/
/**********************************************************************************************/




/**********************************************************************************************/

/*****************************uart packet******************************************************/
/**********************************************************************************************/
#define MCU_ZIGBEE_UART_MAX_LEN     50

void uart_send_event_handler(uint8_t * param, zb_uint8_t len)
{
	for(char i =0; i< len; i++)
	{
		app_uart_put(param[i]);
	}
}

void uart_receive_event_handler(uint8_t * param, zb_uint8_t len)
{
	for(char i =0; i< len; i++)
	{
		app_uart_put(param[i]);
	}
}

void uart_receive_event_callback(app_uart_evt_t * p_event)
{
    static uint8_t data_array[MCU_ZIGBEE_UART_MAX_LEN];
    static uint8_t index = 0;
    //uint32_t       err_code;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;

            if ((data_array[index - 1] == '\n') ||  (data_array[index - 1] == '\r') || (index >= MCU_ZIGBEE_UART_MAX_LEN))
            {
                if (index > 1)
                {
					uart_send_event_handler(data_array, index);		
					//ZB_GET_OUT_BUF_DELAYED(report_handler);	
                }

                index = 0;
            }
            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}

/**@snippet [Handling the data received over UART] */

/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
static void uart_init(void)
{
    uint32_t                     err_code;
    app_uart_comm_params_t const comm_params =
    {
        .rx_pin_no    = RX_PIN_NUMBER,
        .tx_pin_no    = TX_PIN_NUMBER,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
#if defined (UART_PRESENT)
        .baud_rate    = NRF_UART_BAUDRATE_115200
#else
        .baud_rate    = NRF_UARTE_BAUDRATE_115200
#endif
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_receive_event_callback,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}

/**********************************************************************************************************/
/**@brief Callback function for handling ZCL commands.
 *
 * @param[in]   param   Reference to ZigBee stack buffer used to pass received data.
 */
static zb_void_t zcl_device_cb(zb_uint8_t param)
{
    zb_uint16_t                    cluster_id;
    zb_uint16_t                    attr_id;
    zb_buf_t                       * p_buffer = ZB_BUF_FROM_REF(param);
    zb_zcl_device_callback_param_t * p_device_cb_param =
                     ZB_GET_BUF_PARAM(p_buffer, zb_zcl_device_callback_param_t);
	
    NRF_LOG_INFO("zcl_device_cb id %hd", p_device_cb_param->device_cb_id);

    /* Set default response value. */
    p_device_cb_param->status = RET_OK;

    switch (p_device_cb_param->device_cb_id)
    {		
        case ZB_ZCL_DOOR_LOCK_LOCK_DOOR_CB_ID:
		{
			NRF_LOG_INFO("light on");
			bsp_board_led_on(BULB_FOUND_LED);
            break;
		}
		
		case ZB_ZCL_DOOR_LOCK_UNLOCK_DOOR_CB_ID:
		{
			NRF_LOG_INFO("light off");		
			bsp_board_led_off(BULB_FOUND_LED);
            break;
		}

        case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
		{
			cluster_id = p_device_cb_param->cb_param.set_attr_value_param.cluster_id;
			attr_id    = p_device_cb_param->cb_param.set_attr_value_param.attr_id;
			NRF_LOG_INFO("cluster_id %x**attr_id %x", cluster_id, attr_id);

            if (cluster_id == ZB_ZCL_CLUSTER_ID_DOOR_LOCK)			//
            {
				NRF_LOG_INFO("cluster_id");
				switch (attr_id)
				{
					case ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_ID:
						NRF_LOG_INFO("0");
						break;

					case ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_ID:
						NRF_LOG_INFO("1");
						break;

					case ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_ID:
						NRF_LOG_INFO("2");
						break;
					
					default:
						NRF_LOG_INFO("Unused attribute");
						break;
				}
            }
            else
            {
                /* Other clusters can be processed here */
                NRF_LOG_INFO("Unhandled cluster attribute id: %d", cluster_id);
            }
            break;
		}

        default:
			NRF_LOG_INFO("Unhandled device callback id: %d", p_device_cb_param->device_cb_id);
            p_device_cb_param->status = RET_ERROR;
            break;
    }

    NRF_LOG_INFO("zcl_device_cb status: %hd", p_device_cb_param->status);
}
/*****************************airborne packet**************************************************/
/**********************************************************************************************/

static zb_void_t air_recevie_event_handler(zb_uint8_t param)
{
	switch(param)
	{
		case DOOR_LOCK_COMMADN_LOCK_DOOR:
		{
			for(char i = 0; i < rev_cmd.len.lock_door_len; i++)
			{
				NRF_LOG_INFO("%d", rev_cmd.lock_door[i]);
			}
			NRF_LOG_INFO("%d", rev_cmd.len.lock_door_len);
			
			dev_ctx.door_lock_attr.lock_state = ZB_ZCL_ATTR_DOOR_LOCK_DOOR_STATE_OPEN;
			free(rev_cmd.lock_door);
			rev_cmd.lock_door = NULL;
		}break;
		
		case DOOR_LOCK_COMMADN_UNLOCK_DOOR:
		{
			for(char i = 0; i < rev_cmd.len.unlock_door_len; i++)
			{
				NRF_LOG_INFO("%d", rev_cmd.unlock_door[i]);
			}
			NRF_LOG_INFO("%d", rev_cmd.len.unlock_door_len);
			
			dev_ctx.door_lock_attr.lock_state = ZB_ZCL_ATTR_DOOR_LOCK_DOOR_STATE_CLOSED;
			free(rev_cmd.unlock_door);
			rev_cmd.unlock_door = NULL;
		}break;
		
		case DOOR_LOCK_COMMADN_TOGGLE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_TOGGLE 0x02\n");
		}break;
		
		case DOOR_LOCK_COMMADN_UNLOCK_WITH_TIMEOUT:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_UNLOCK_WITH_TIMEOUT 0x03\n");
			free(rev_cmd.unlock_door);
			rev_cmd.unlock_door = NULL;
		}break;
		
		case DOOR_LOCK_COMMADN_GET_LOG_RECORD:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_LOG_RECORD 0x04\n");
		}break;
		
		case DOOR_LOCK_COMMADN_SET_PIN_CODE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_PIN_CODE 0x05\n");
		}break;
		
		case DOOR_LOCK_COMMADN_GET_PIN_CODE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_PIN_CODE 0x06\n");
		}break;
		
		case DOOR_LOCK_COMMADN_CLEAR_PIN_CODE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_PIN_CODE 0x07\n");
		}break;
		
		case DOOR_LOCK_COMMADN_CLEAR_ALL_PIN_STATUS:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_ALL_PIN_STATUS 0x08\n");
		}break;
		
		case DOOR_LOCK_COMMADN_SET_USER_STATUS:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_USER_STATUS 0x09\n");
		}break;
		
		case DOOR_LOCK_COMMADN_GET_USER_STATUS:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_USER_STATUS 0x0A\n");
		}break;
		
		case DOOR_LOCK_COMMADN_SET_WEEKDAY_SCHEDULE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_WEEKDAY_SCHEDULE 0x0B\n");
		}break;
		
		case DOOR_LOCK_COMMADN_GET_WEEKDAY_SCHEDULE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_WEEKDAY_SCHEDULE 0x0C\n");
		}break;
		
		case DOOR_LOCK_COMMADN_CLEAR_WEEKDAY_SCHEDULE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_WEEKDAY_SCHEDULE 0x0D\n");
		}break;
		
		case DOOR_LOCK_COMMADN_SET_YEARDAY_SCHEDULE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_YEARDAY_SCHEDULE 0x0E\n");
		}break;
		
		case DOOR_LOCK_COMMADN_GET_YEARDAY_SCHEDULE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_YEARDAY_SCHEDULE 0x0F\n");
		}break;
		
		case DOOR_LOCK_COMMADN_CLEAR_YEARDAY_SCHEDULE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_YEARDAY_SCHEDULE 0x10\n");
		}break;
		
		case DOOR_LOCK_COMMADN_SET_HOLIDAY_SCHEDULE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_HOLIDAY_SCHEDULE 0x11\n");
		}break;
		
		case DOOR_LOCK_COMMADN_GET_HOLIDAY_SCHEDULE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_HOLIDAY_SCHEDULE 0x12\n");
		}break;
		
		case DOOR_LOCK_COMMADN_CLEAR_HOLIDAY_SCHEDULE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_HOLIDAY_SCHEDULE 0x13\n");
		}break;
		
		case DOOR_LOCK_COMMADN_SET_USER_TYPE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_USER_TYPE 0x14\n");
		}break;
		
		case DOOR_LOCK_COMMADN_GET_USER_TYPE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_USER_TYPE 0x15\n");
		}break;
		
		case DOOR_LOCK_COMMADN_SET_RFID_CODE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_RFID_CODE 0x16\n");
		}break;
		
		case DOOR_LOCK_COMMADN_GET_RFID_CODE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_RFID_CODE 0x17\n");
		}break;
		
		case DOOR_LOCK_COMMADN_CLEAR_RFID_CODE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_RFID_CODE 0x18\n");
		}break;
		
		case DOOR_LOCK_COMMADN_CLEAR_ALL_RFID_CODE:
		{
			NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_ALL_RFID_CODE 0x19\n");
		}break;
		
		default:
			break;
	}		
}

zb_uint8_t air_recevie_event_callback(zb_uint8_t param)
{
	zb_ret_t                        zb_err_code;
	zb_uint16_t						profile_id;
	zb_uint16_t                     cluster_id;
    zb_uint16_t                     cmd_id;
	zb_uint8_t  					manuf_specific;
	zb_uint8_t  					common_command;
    zb_buf_t                       * p_buffer = ZB_BUF_FROM_REF(param);
    zb_zcl_parsed_hdr_t   * p_device_cb_param =
                       ZB_GET_BUF_PARAM(p_buffer, zb_zcl_parsed_hdr_t);
	
	profile_id = p_device_cb_param->profile_id;
	cluster_id = p_device_cb_param->cluster_id;
	cmd_id = p_device_cb_param->cmd_id;
	manuf_specific = p_device_cb_param->is_manuf_specific;
	common_command = p_device_cb_param->is_common_command;
	
	NRF_LOG_INFO("\n**profile_id %x**cluster_id %x**cmd_id %x**manuf_specific %x**common_command %x", profile_id, cluster_id, cmd_id, manuf_specific, common_command);
	
	zb_uint8_t buf_len = ZB_BUF_LEN_BY_REF(param);
	zb_uint8_t * buf_data = ZB_BUF_BEGIN_FROM_REF(param);
//	zb_uint8_t buf_len = ZB_BUF_LEN_BY_REF(param);
//	NRF_LOG_INFO("len = %d", buf_len);
//	zb_uint8_t * buf_data = ZB_BUF_BEGIN_FROM_REF(param);
//	for(zb_uint8_t i = 0; i < buf_len; i++)
//	{
//		NRF_LOG_INFO("data%d:%d", i, buf_data[i]);
//	}
	
	if(common_command == GOLBAL_COMMAND)
	{
		/* attribute operation */
		NRF_LOG_INFO("global command\n");
	}
	else if(common_command == SPECIFIC_CLUSTER_COMMAND)
	{
		if(profile_id == ZB_AF_HA_PROFILE_ID && cluster_id == ZB_ZCL_CLUSTER_ID_DOOR_LOCK && manuf_specific == ZIGBEE_STANDART_BIT)
		{
			switch(cmd_id)
			{
				case DOOR_LOCK_COMMADN_LOCK_DOOR:
				{
					if(rev_cmd.lock_door == NULL)
					{
						rev_cmd.lock_door = (zb_uint8_t *)calloc(buf_len, sizeof(zb_uint8_t));
						memcpy(rev_cmd.lock_door, buf_data, buf_len);
						rev_cmd.len.lock_door_len = buf_len;
						zb_err_code = ZB_SCHEDULE_ALARM(air_recevie_event_handler, DOOR_LOCK_COMMADN_LOCK_DOOR, ZB_TIME_ONE_SECOND);
						ZB_ERROR_CHECK(zb_err_code);
					}
				}break;
				
				case DOOR_LOCK_COMMADN_UNLOCK_DOOR:
				{
					if(rev_cmd.unlock_door == NULL)
					{
						rev_cmd.unlock_door = (zb_uint8_t *)calloc(buf_len, sizeof(zb_uint8_t));
						memcpy(rev_cmd.unlock_door, buf_data, buf_len);
						rev_cmd.len.unlock_door_len = buf_len;
						zb_err_code = ZB_SCHEDULE_ALARM(air_recevie_event_handler, DOOR_LOCK_COMMADN_UNLOCK_DOOR, ZB_TIME_ONE_SECOND);
						ZB_ERROR_CHECK(zb_err_code);
					}
				}break;
				
				case DOOR_LOCK_COMMADN_TOGGLE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_TOGGLE 0x02\n");
				}break;
				
				case DOOR_LOCK_COMMADN_UNLOCK_WITH_TIMEOUT:
				{
					//NRF_LOG_INFO("DOOR_LOCK_COMMADN_UNLOCK_WITH_TIMEOUT 0x03\n");
						rev_cmd.unlock_with_timeout = (zb_uint8_t *)calloc(buf_len, sizeof(zb_uint8_t));
						memcpy(rev_cmd.unlock_with_timeout, buf_data, buf_len);
						rev_cmd.len.unlock_with_timeout_len = buf_len;
						zb_err_code = ZB_SCHEDULE_ALARM(air_recevie_event_handler, DOOR_LOCK_COMMADN_UNLOCK_WITH_TIMEOUT, ZB_TIME_ONE_SECOND * 3);
						ZB_ERROR_CHECK(zb_err_code);
				}break;
				
				case DOOR_LOCK_COMMADN_GET_LOG_RECORD:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_LOG_RECORD 0x04\n");
				}break;
				
				case DOOR_LOCK_COMMADN_SET_PIN_CODE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_PIN_CODE 0x05\n");
				}break;
				
				case DOOR_LOCK_COMMADN_GET_PIN_CODE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_PIN_CODE 0x06\n");
				}break;
				
				case DOOR_LOCK_COMMADN_CLEAR_PIN_CODE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_PIN_CODE 0x07\n");
				}break;
				
				case DOOR_LOCK_COMMADN_CLEAR_ALL_PIN_STATUS:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_ALL_PIN_STATUS 0x08\n");
				}break;
				
				case DOOR_LOCK_COMMADN_SET_USER_STATUS:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_USER_STATUS 0x09\n");
				}break;
				
				case DOOR_LOCK_COMMADN_GET_USER_STATUS:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_USER_STATUS 0x0A\n");
				}break;
				
				case DOOR_LOCK_COMMADN_SET_WEEKDAY_SCHEDULE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_WEEKDAY_SCHEDULE 0x0B\n");
				}break;
				
				case DOOR_LOCK_COMMADN_GET_WEEKDAY_SCHEDULE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_WEEKDAY_SCHEDULE 0x0C\n");
				}break;
				
				case DOOR_LOCK_COMMADN_CLEAR_WEEKDAY_SCHEDULE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_WEEKDAY_SCHEDULE 0x0D\n");
				}break;
				
				case DOOR_LOCK_COMMADN_SET_YEARDAY_SCHEDULE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_YEARDAY_SCHEDULE 0x0E\n");
				}break;
				
				case DOOR_LOCK_COMMADN_GET_YEARDAY_SCHEDULE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_YEARDAY_SCHEDULE 0x0F\n");
				}break;
				
				case DOOR_LOCK_COMMADN_CLEAR_YEARDAY_SCHEDULE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_YEARDAY_SCHEDULE 0x10\n");
				}break;
				
				case DOOR_LOCK_COMMADN_SET_HOLIDAY_SCHEDULE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_HOLIDAY_SCHEDULE 0x11\n");
				}break;
				
				case DOOR_LOCK_COMMADN_GET_HOLIDAY_SCHEDULE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_HOLIDAY_SCHEDULE 0x12\n");
				}break;
				
				case DOOR_LOCK_COMMADN_CLEAR_HOLIDAY_SCHEDULE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_HOLIDAY_SCHEDULE 0x13\n");
				}break;
				
				case DOOR_LOCK_COMMADN_SET_USER_TYPE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_USER_TYPE 0x14\n");
				}break;
				
				case DOOR_LOCK_COMMADN_GET_USER_TYPE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_USER_TYPE 0x15\n");
				}break;
				
				case DOOR_LOCK_COMMADN_SET_RFID_CODE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_SET_RFID_CODE 0x16\n");
				}break;
				
				case DOOR_LOCK_COMMADN_GET_RFID_CODE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_GET_RFID_CODE 0x17\n");
				}break;
				
				case DOOR_LOCK_COMMADN_CLEAR_RFID_CODE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_RFID_CODE 0x18\n");
				}break;
				
				case DOOR_LOCK_COMMADN_CLEAR_ALL_RFID_CODE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_CLEAR_ALL_RFID_CODE 0x19\n");
				}break;
				
				default:
					NRF_LOG_INFO("do not support\n");
					break;
			}
		}
		else if(profile_id == ZB_AF_HA_PROFILE_ID && cluster_id == ZB_ZCL_CLUSTER_ID_DOOR_LOCK && manuf_specific == MAMUFAC_SPECIFIC_BIT)
		{
			switch(cmd_id)
			{
				case DOOR_LOCK_COMMADN_MANUF_ADD_USER:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_MANUF_ADD_USER");
				}break;
				
				case DOOR_LOCK_COMMADN_MANUF_DELETE_USER:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_MANUF_DELETE_USER");
				}break;
				
				case DOOR_LOCK_COMMADN_MANUF_DELETE_ALL_USER:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_MANUF_DELETE_ALL_USER");
				}break;
				
				case DOOR_LOCK_COMMADN_MANUF_CHANGE_CODE:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_MANUF_CHANGE_CODE");
				}break;
				
				case DOOR_LOCK_COMMADN_MANUF_TIME_SYNC:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_MANUF_TIME_SYNC");
				}break;
				
				case DOOR_LOCK_COMMADN_MANUF_GET_USER_INFO:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_MANUF_GET_USER_INFO");
				}break;
				
				case DOOR_LOCK_COMMADN_MANUF_GET_DEV_INFO:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_MANUF_GET_DEV_INFO");
				}break;
				
				case DOOR_LOCK_COMMADN_MANUF_STATE_REPORT:
				{
					NRF_LOG_INFO("DOOR_LOCK_COMMADN_MANUF_STATE_REPORT");
				}break;
							
				default:
					NRF_LOG_INFO("do not support\n");
					break;
			}
		}
	}
		
	return 0;
}
/**********************************************************************************************/
/**********************************************************************************************/


/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(CAFE_CAFE, line_num, p_file_name);
}


/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            //err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            //APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_IDLE:
            err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t                              err_code;

    UNUSED_PARAMETER(p_context);

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected");
            bsp_board_led_off(BSP_BOARD_LED_0);
            bsp_board_led_off(BSP_BOARD_LED_1);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling events from the GATT library.
 *
 * @param[in]  p_gatt  Reference to the GATT instance structure that contains status information for the GATT module.
 * @param[in]  p_evt   Reference to the GATT event structure.
 */
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        NRF_LOG_INFO("Data len is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
    }
    NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
                  p_gatt->att_mtu_desired_central,
                  p_gatt->att_mtu_desired_periph);
}


/***************************************************************************************************
 * @section Initialization
 **************************************************************************************************/

/**@brief Function for initializing the GATT library. */
void gatt_init(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling dfu events from the Buttonless Secure DFU service
 *
 * @param[in]   event   Event from the Buttonless Secure DFU service.
 */
static void disconnect(uint16_t conn_handle, void * p_context)
{
    UNUSED_PARAMETER(p_context);

    ret_code_t err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_WARNING("Failed to disconnect connection. Connection handle: %d Error: %d", conn_handle, err_code);
    }
    else
    {
        NRF_LOG_DEBUG("Disconnected connection handle %d", conn_handle);
    }
}
static void advertising_config_get(ble_adv_modes_config_t * p_config)
{
    memset(p_config, 0, sizeof(ble_adv_modes_config_t));

    p_config->ble_adv_fast_enabled  = true;
    p_config->ble_adv_fast_interval = APP_ADV_INTERVAL;
    p_config->ble_adv_fast_timeout  = APP_ADV_DURATION;
}
static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
        {
            NRF_LOG_INFO("Device is preparing to enter bootloader mode.");

            // Prevent device from advertising on disconnect.
            ble_adv_modes_config_t config;
            advertising_config_get(&config);
            config.ble_adv_on_disconnect_disabled = true;
            ble_advertising_modes_config_set(&m_advertising, &config);

            // Disconnect all other bonded devices that currently are connected.
            // This is required to receive a service changed indication
            // on bootup after a successful (or aborted) Device Firmware Update.
            uint32_t conn_count = ble_conn_state_for_each_connected(disconnect, NULL);
            NRF_LOG_INFO("Disconnected %d links.", conn_count);
            break;
        }

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
            //           by delaying reset by reporting false in app_shutdown_handler
            NRF_LOG_INFO("Device will enter bootloader mode.");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            NRF_LOG_ERROR("Request to enter bootloader mode failed asynchroneously.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            NRF_LOG_ERROR("Request to send a response to client failed.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            APP_ERROR_CHECK(false);
            break;

        default:
            NRF_LOG_ERROR("Unknown event from ble_dfu_buttonless.");
            break;
    }
}
/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    uint32_t       err_code;	
	/*dfu buttonless*/
    ble_dfu_buttonless_init_t dfus_init = {0};

    dfus_init.evt_handler = ble_dfu_evt_handler;

    err_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t               err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.srdata.uuids_complete.p_uuids  = m_adv_uuids;
	
    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing the application timer.
 */
static void seek_network_start_loop(void * param);
static void timer_init(void)
{
    uint32_t error_code = NRF_SUCCESS;
    error_code          = app_timer_init();
    APP_ERROR_CHECK(error_code);
	
	error_code = app_timer_create(&seek_network_start_timer, APP_TIMER_MODE_SINGLE_SHOT, seek_network_start_loop);
    APP_ERROR_CHECK(error_code);
}


/***************************************************************************************************
 * @section Zigbee stack related functions.
 **************************************************************************************************/


/**@brief Function for sending ON/OFF requests to the light bulb.
 *
 * @param[in]   param    Non-zero reference to ZigBee stack buffer that will be used to construct on/off request.
 * @param[in]   on_off   Requested state of the light bulb.
 */
static zb_void_t light_switch_send_on_off(zb_uint8_t param, zb_uint16_t on_off)
{
    zb_uint8_t           cmd_id;
    zb_buf_t           * p_buf = ZB_BUF_FROM_REF(param);

    if (on_off)
    {
        cmd_id = ZB_ZCL_CMD_ON_OFF_ON_ID;
    }
    else
    {
        cmd_id = ZB_ZCL_CMD_ON_OFF_OFF_ID;
    }

    NRF_LOG_INFO("Send ON/OFF command: %d", on_off);

    ZB_ZCL_ON_OFF_SEND_REQ(p_buf,
                           m_device_ctx.bulb_params.short_addr,
                           ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
                           m_device_ctx.bulb_params.endpoint,
                           LIGHT_SWITCH_ENDPOINT,
                           ZB_AF_HA_PROFILE_ID,
                           ZB_ZCL_DISABLE_DEFAULT_RESPONSE,
                           cmd_id,
                           NULL);
}


/**@brief Function for sending step requests to the light bulb.
  *
  * @param[in]   param        Non-zero reference to ZigBee stack buffer that will be used to construct step request.
  * @param[in]   is_step_up   Boolean parameter selecting direction of step change.
  */
static zb_void_t light_switch_send_step(zb_uint8_t param, zb_uint16_t is_step_up)
{
    zb_uint8_t           step_dir;
    zb_buf_t           * p_buf = ZB_BUF_FROM_REF(param);

    if (is_step_up)
    {
        step_dir = ZB_ZCL_LEVEL_CONTROL_STEP_MODE_UP;
    }
    else
    {
        step_dir = ZB_ZCL_LEVEL_CONTROL_STEP_MODE_DOWN;
    }

    NRF_LOG_INFO("Send step level command: %d", is_step_up);

    ZB_ZCL_LEVEL_CONTROL_SEND_STEP_REQ(p_buf,
                                       m_device_ctx.bulb_params.short_addr,
                                       ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
                                       m_device_ctx.bulb_params.endpoint,
                                       LIGHT_SWITCH_ENDPOINT,
                                       ZB_AF_HA_PROFILE_ID,
                                       ZB_ZCL_DISABLE_DEFAULT_RESPONSE,
                                       NULL,
                                       step_dir,
                                       LIGHT_SWITCH_DIMM_STEP,
                                       LIGHT_SWITCH_DIMM_TRANSACTION_TIME);
}


/**@brief Perform local operation - leave network.
 *
 * @param[in]   param   Reference to ZigBee stack buffer that will be used to construct leave request.
 */
static void light_switch_leave_nwk(zb_uint8_t param)
{
    zb_ret_t zb_err_code;

    /* We are going to leave */
    if (param)
    {
        zb_buf_t                  * p_buf = ZB_BUF_FROM_REF(param);
        zb_zdo_mgmt_leave_param_t * p_req_param;

        p_req_param = ZB_GET_BUF_PARAM(p_buf, zb_zdo_mgmt_leave_param_t);
        UNUSED_RETURN_VALUE(ZB_BZERO(p_req_param, sizeof(zb_zdo_mgmt_leave_param_t)));

        /* Set dst_addr == local address for local leave */
        p_req_param->dst_addr = ZB_PIBCACHE_NETWORK_ADDRESS();
        p_req_param->rejoin   = ZB_FALSE;
        UNUSED_RETURN_VALUE(zdo_mgmt_leave_req(param, NULL));
    }
    else
    {
        zb_err_code = ZB_GET_OUT_BUF_DELAYED(light_switch_leave_nwk);
        ZB_ERROR_CHECK(zb_err_code);
    }
}


/**@brief Function for starting join/rejoin procedure.
 *
 * param[in]   leave_type   Type of leave request (with or without rejoin).
 */
static zb_void_t seek_network_handler(zb_uint8_t param)  
{
    zb_bool_t comm_status;
	if(network_status == NETWORK_STATUS_JOIN_REJOIN)
	{
		zb_led_indicate(BSP_BOARD_LED_2, 5, LED_FILL_IN_DUTY_TIMER(250), LED_FILL_IN_CYCLE_TIMER(2000));
		
		NRF_LOG_INFO("*******************************seek network start");
		comm_status = bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        ZB_COMM_STATUS_CHECK(comm_status);
	}
}

static void seek_network_start_loop(void * param)
{
	ret_code_t  err_code;
	
	if(network_status == NETWORK_STATUS_JOIN_REJOIN)
	{
		seek_network_handler(0);	

		err_code = app_timer_start(seek_network_start_timer, NETWORK_SEEK_FREQUENCY, NULL);
		APP_ERROR_CHECK(err_code);			
	}
}
static void seek_network_stop(zb_uint8_t param)
{
	ret_code_t  err_code;
	
	err_code = app_timer_stop(seek_network_start_timer);
    APP_ERROR_CHECK(err_code);
	NRF_LOG_INFO("seek network stop*******************************");	
}
static void seek_network_damon_process(zb_uint8_t param)
{
	zb_ret_t zb_err_code;
	ret_code_t  err_code;

	if(!ZB_JOINED())
	{
		++seek_network_damon_cnt;
		//NRF_LOG_INFO("damon process %d",seek_network_damon_cnt);		
		switch(seek_network_damon_cnt)
		{			
			case NETWORK_SEEK_START_0:
			case NETWORK_SEEK_START_30:
			case NETWORK_SEEK_START_60:	
			case NETWORK_SEEK_START_120:
				if(seek_network_damon_cnt >= NETWORK_SEEK_START_120)
				{
					seek_network_damon_cnt = NETWORK_SEEK_START_60;
				}
				err_code = app_timer_start(seek_network_start_timer, APP_TIMER_TICKS(2 * 1000), NULL);
				APP_ERROR_CHECK(err_code);
				zb_err_code = ZB_SCHEDULE_ALARM(seek_network_stop, 0, NETWORK_SEEK_STOP_TIMER);
				APP_ERROR_CHECK(zb_err_code);		
				break;
			
			default:
				break;
		}	
	}
	
	if(network_status == NETWORK_STATUS_JOIN_REJOIN)
	{
		NRF_LOG_INFO("damon process %d",seek_network_damon_cnt);
		zb_err_code = ZB_SCHEDULE_ALARM(seek_network_damon_process, 0, NETWORK_DEOMON_PROCESS);
		ZB_ERROR_CHECK(zb_err_code);
	}
}

static void seek_network_init(uint8_t param)
{		
	seek_network_damon_process(0);
	//seek_network_start_loop(0);
}
		
/**@brief Callback function receiving finding procedure results.
 *
 * @param[in]   param   Reference to ZigBee stack buffer used to pass received data.
 */
static zb_void_t find_light_bulb_cb(zb_uint8_t param)
{
    zb_buf_t                   * p_buf  = ZB_BUF_FROM_REF(param);                              // Resolve buffer number to buffer address
    zb_zdo_match_desc_resp_t   * p_resp = (zb_zdo_match_desc_resp_t *) ZB_BUF_BEGIN(p_buf);    // Get the begining of the response
    zb_apsde_data_indication_t * p_ind  = ZB_GET_BUF_PARAM(p_buf, zb_apsde_data_indication_t); // Get the pointer to the parameters buffer, which stores APS layer response
    zb_uint8_t                 * p_match_ep;
    zb_ret_t                     zb_err_code;

    if ((p_resp->status == ZB_ZDP_STATUS_SUCCESS) && (p_resp->match_len > 0) && (!m_device_ctx.bulb_params.short_addr))
    {
        /* Match EP list follows right after response header */
        p_match_ep = (zb_uint8_t *)(p_resp + 1);

        /* We are searching for exact cluster, so only 1 EP may be found */
        m_device_ctx.bulb_params.endpoint   = *p_match_ep;
        m_device_ctx.bulb_params.short_addr = p_ind->src_addr;

        NRF_LOG_INFO("Found bulb addr: %d ep: %d", m_device_ctx.bulb_params.short_addr, m_device_ctx.bulb_params.endpoint);

        zb_err_code = ZB_SCHEDULE_ALARM_CANCEL(find_light_bulb_timeout, ZB_ALARM_ANY_PARAM);
        ZB_ERROR_CHECK(zb_err_code);

        bsp_board_led_on(BULB_FOUND_LED);
    }

    if (param)
    {
        ZB_FREE_BUF_BY_REF(param);
    }
}


/**@brief Function for sending ON/OFF and Level Control find request.
 *
 * @param[in]   param   Non-zero reference to ZigBee stack buffer that will be used to construct find request.
 */
static zb_void_t find_light_bulb(zb_uint8_t param)
{
    zb_buf_t                  * p_buf = ZB_BUF_FROM_REF(param); // Resolve buffer number to buffer address
    zb_zdo_match_desc_param_t * p_req;

    /* Initialize pointers inside buffer and reserve space for zb_zdo_match_desc_param_t request */
    UNUSED_RETURN_VALUE(ZB_BUF_INITIAL_ALLOC(p_buf, sizeof(zb_zdo_match_desc_param_t) + (1) * sizeof(zb_uint16_t), p_req));

    p_req->nwk_addr         = ZB_NWK_BROADCAST_RX_ON_WHEN_IDLE; // Send to all non-sleepy devices
    p_req->addr_of_interest = ZB_NWK_BROADCAST_RX_ON_WHEN_IDLE; // Get responses from all non-sleepy devices
    p_req->profile_id       = ZB_AF_HA_PROFILE_ID;              // Look for Home Automation profile clusters

    /* We are searching for 2 clusters: On/Off and Level Control Server */
    p_req->num_in_clusters  = 2;
    p_req->num_out_clusters = 0;
    /*lint -save -e415 // Suppress warning 415 "likely access of out-of-bounds pointer" */
    p_req->cluster_list[0]  = ZB_ZCL_CLUSTER_ID_ON_OFF;
    p_req->cluster_list[1]  = ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL;
    /*lint -restore */

    m_device_ctx.bulb_params.short_addr = 0x00; // Reset short address in order to parse only one response.
    UNUSED_RETURN_VALUE(zb_zdo_match_desc_req(param, find_light_bulb_cb));
}


/**@brief Finding procedure timeout handler.
 *
 * @param[in]   param   Reference to ZigBee stack buffer that will be used to construct find request.
 */
static zb_void_t find_light_bulb_timeout(zb_uint8_t param)
{
    zb_ret_t zb_err_code;

    if (param)
    {
        NRF_LOG_INFO("Bulb not found, try again");
        zb_err_code = ZB_SCHEDULE_ALARM(find_light_bulb, param, MATCH_DESC_REQ_START_DELAY);
        ZB_ERROR_CHECK(zb_err_code);
        zb_err_code = ZB_SCHEDULE_ALARM(find_light_bulb_timeout, 0, MATCH_DESC_REQ_TIMEOUT);
        ZB_ERROR_CHECK(zb_err_code);
    }
    else
    {
        zb_err_code = ZB_GET_OUT_BUF_DELAYED(find_light_bulb_timeout);
        ZB_ERROR_CHECK(zb_err_code);
    }
}


/**@brief Callback for detecting button press duration.
 *
 * @param[in]   button   BSP Button that was pressed.
 */
static zb_void_t light_switch_button_handler(zb_uint8_t button)
{
    zb_time_t current_time;
    zb_bool_t short_expired;
    zb_bool_t on_off;
    zb_ret_t zb_err_code;

    current_time = ZB_TIMER_GET();

    if (button == LIGHT_SWITCH_BUTTON_ON)
    {
        on_off = ZB_TRUE;
    }
    else
    {
        on_off = ZB_FALSE;
    }

    if (ZB_TIME_SUBTRACT(current_time, m_device_ctx.button.timestamp) > LIGHT_SWITCH_BUTTON_THRESHOLD)
    {
        short_expired = ZB_TRUE;
    }
    else
    {
        short_expired = ZB_FALSE;
    }

    /* Check if button was released during LIGHT_SWITCH_BUTTON_SHORT_POLL_TMO. */
    if (!bsp_button_is_pressed(button))
    {
        if (!short_expired)
            {
                /* Allocate output buffer and send on/off command. */
                zb_err_code = ZB_GET_OUT_BUF_DELAYED2(light_switch_send_on_off, on_off);
                ZB_ERROR_CHECK(zb_err_code);
            }

        /* Button released - wait for accept next event. */
        m_device_ctx.button.in_progress = ZB_FALSE;
    }
    else
    {
        if (short_expired)
        {
            /* The button is still pressed - allocate output buffer and send step command. */
            zb_err_code = ZB_GET_OUT_BUF_DELAYED2(light_switch_send_step, on_off);
            ZB_ERROR_CHECK(zb_err_code);
            zb_err_code = ZB_SCHEDULE_ALARM(light_switch_button_handler, button, LIGHT_SWITCH_BUTTON_LONG_POLL_TMO);
            ZB_ERROR_CHECK(zb_err_code);
        }
        else
        {
            /* Wait another LIGHT_SWITCH_BUTTON_SHORT_POLL_TMO, until LIGHT_SWITCH_BUTTON_THRESHOLD will be reached. */
            zb_err_code = ZB_SCHEDULE_ALARM(light_switch_button_handler, button, LIGHT_SWITCH_BUTTON_SHORT_POLL_TMO);
            ZB_ERROR_CHECK(zb_err_code);
        }
    }
}


/**@brief Callback for button events.
 *
 * @param[in]   evt      Incoming event from the BSP subsystem.
 */
static char network_button_count = 0;
static void network_button_count_handler(zb_uint8_t param)
{	
	zb_ret_t zb_err_code;
	network_button_count = 0;
	NRF_LOG_INFO("key_value = %d", param);
		
	if(param == 4)
	{
		if(network_status == NETWORK_STATUS_LEAVE)
		{
			network_status = NETWORK_STATUS_JOIN_REJOIN;
			fds_write_handler(NETWORK_STATUS_JOIN_REJOIN);
			NRF_LOG_INFO("*starting join network*");
			
			zb_err_code = ZB_SCHEDULE_ALARM(seek_network_init, 0, ZB_TIME_ONE_SECOND);
			ZB_ERROR_CHECK(zb_err_code);
		}
		else
		{
			NRF_LOG_INFO("*already join network or seek network*");
		}
	}
}

static void factory_settings(uint8_t param)
{ 
	NVIC_SystemReset();	
}	
static zb_void_t network_button_set_handler(zb_uint8_t button)
{
    zb_time_t current_time;
    zb_button_t short_expired;
    zb_ret_t zb_err_code;

    current_time = ZB_TIMER_GET();

    if (ZB_TIME_SUBTRACT(current_time, m_device_ctx.button.timestamp) > NETWORK_BUTTON_LONG_TIME)
    {
        short_expired = ZB_BUTTON_TIME_LONG;
    }
	else if (ZB_TIME_SUBTRACT(current_time, m_device_ctx.button.timestamp) > NETWORK_BUTTON_THRESHOLD_TIME)
    {
        short_expired = ZB_BUTTON_TIME_MIDDLE;
    }
	else 
    {
        short_expired = ZB_BUTTON_TIME_SHORT;
    }

    /* Check if button was released during LIGHT_SWITCH_BUTTON_SHORT_POLL_TMO. */
    if (!bsp_button_is_pressed(button))
    {
        if (short_expired == ZB_BUTTON_TIME_SHORT)
        {		
			network_button_count++;
			zb_err_code = ZB_SCHEDULE_ALARM(network_button_count_handler, network_button_count, NETWORK_BUTTON_SHORT_TIME);
			ZB_ERROR_CHECK(zb_err_code);
			zb_err_code = ZB_SCHEDULE_ALARM_CANCEL(network_button_count_handler, ZB_ALARM_ALL_CB);
			ZB_ERROR_CHECK(zb_err_code);
			zb_err_code = ZB_SCHEDULE_ALARM(network_button_count_handler, network_button_count, NETWORK_BUTTON_SHORT_TIME);
			ZB_ERROR_CHECK(zb_err_code);		
        }
		else if (short_expired == ZB_BUTTON_TIME_MIDDLE)
		{
			NRF_LOG_INFO("*long press time over threshold*");
			network_button_count = 0;
		}
        /* Button released - wait for accept next event. */
        m_device_ctx.button.in_progress = ZB_FALSE;
    }
    else
    {
        if (short_expired == ZB_BUTTON_TIME_LONG)
        {	
			network_button_count = 0;
			network_status = NETWORK_STATUS_LEAVE;
			NRF_LOG_INFO("*leave network*");
								
			fds_write_handler(NETWORK_STATUS_LEAVE);
			zb_err_code = ZB_SCHEDULE_ALARM(factory_settings, 0, ZB_TIME_ONE_SECOND);
			ZB_ERROR_CHECK(zb_err_code);
		
			/* Button released - wait for accept next event. */
			//m_device_ctx.button.in_progress = ZB_FALSE;			
        }
        else
        {
			zb_err_code = ZB_SCHEDULE_ALARM(network_button_count_handler, network_button_count,NETWORK_BUTTON_SHORT_TIME);
			ZB_ERROR_CHECK(zb_err_code);
			zb_err_code = ZB_SCHEDULE_ALARM_CANCEL(network_button_count_handler, ZB_ALARM_ALL_CB);
			ZB_ERROR_CHECK(zb_err_code);
            /* Wait another LIGHT_SWITCH_BUTTON_SHORT_POLL_TMO, until LIGHT_SWITCH_BUTTON_THRESHOLD will be reached. */
            zb_err_code = ZB_SCHEDULE_ALARM(network_button_set_handler, button, NETWORK_BUTTON_LONG_POLL_TMO);
            ZB_ERROR_CHECK(zb_err_code);
        }
    }
}
static void buttons_handler(bsp_event_t evt)
{
    zb_ret_t zb_err_code;
    zb_uint32_t button;

    switch(evt)
    {
        case BSP_EVENT_KEY_0:
            button = LIGHT_SWITCH_BUTTON_ON;
            break;

        case BSP_EVENT_KEY_1:
            button = LIGHT_SWITCH_BUTTON_OFF;
            break;
		
		case BSP_EVENT_KEY_3:
			NRF_LOG_INFO("KEY--3");
            button = NETWORK_BUTTON_SET;
            break;

        default:
            NRF_LOG_INFO("Unhandled BSP Event received: %d", evt);
            return;
    }

    if (!m_device_ctx.button.in_progress)
    {
        m_device_ctx.button.in_progress = ZB_TRUE;
        m_device_ctx.button.timestamp = ZB_TIMER_GET();
		
		if(evt == BSP_EVENT_KEY_3)
		{
			zb_err_code = ZB_SCHEDULE_ALARM(network_button_set_handler, button, NETWORK_BUTTON_SHORT_POLL_TMO);
			ZB_ERROR_CHECK(zb_err_code);
		}
		else 
		{
			if (!m_device_ctx.bulb_params.short_addr)
			{
				/* No bulb found yet. router */
				//return;
			}
			zb_err_code = ZB_SCHEDULE_ALARM(light_switch_button_handler, button, LIGHT_SWITCH_BUTTON_SHORT_POLL_TMO);
			//zb_err_code = ZB_SCHEDULE_ALARM(fds_test, button, LIGHT_SWITCH_BUTTON_SHORT_POLL_TMO);
			ZB_ERROR_CHECK(zb_err_code);
		}
    }
}


/**@brief Function for initializing LEDs and buttons.
 */
static void leds_buttons_init(void)
{
    ret_code_t error_code;

    /* Initialize LEDs and buttons - use BSP to control them. */
    error_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, buttons_handler);
    APP_ERROR_CHECK(error_code);
    /* By default the bsp_init attaches BSP_KEY_EVENTS_{0-4} to the PUSH events of the corresponding buttons. */

    bsp_board_leds_off();
}


/**@brief Function to set the Sleeping Mode according to the SLEEPY_ON_BUTTON state.
*/
static zb_void_t sleepy_device_setup(void)
{
    zb_set_rx_on_when_idle(bsp_button_is_pressed(SLEEPY_ON_BUTTON) ? ZB_FALSE : ZB_TRUE);
}

/**@brief Function for initializing the Zigbee Stack
 */
static void zigbee_init(zb_uint8_t param)
{
    zb_ieee_addr_t ieee_addr;

    /* Set ZigBee stack logging level and traffic dump subsystem. */
    ZB_SET_TRACE_LEVEL(ZIGBEE_TRACE_LEVEL);
    ZB_SET_TRACE_MASK(ZIGBEE_TRACE_MASK);
    ZB_SET_TRAF_DUMP_OFF();

    /* Initialize ZigBee stack. */
    ZB_INIT("light_switch");

    /* Set device address to the value read from FICR registers. */
    zb_osif_get_ieee_eui64(ieee_addr);
    zb_set_long_address(test_address);

    /* Set up Zigbee protocol main parameters. */
    zb_set_network_ed_role(IEEE_CHANNEL_MASK);
	if(network_status == NETWORK_STATUS_LEAVE)
	{
		zb_set_nvram_erase_at_start(ZB_TRUE);
	}
	else if(network_status == NETWORK_STATUS_JOIN_REJOIN)
	{
		zb_set_nvram_erase_at_start(ZB_FALSE);
	}

    zb_set_ed_timeout(ED_AGING_TIMEOUT_64MIN);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));
    sleepy_device_setup();

    /* Initialize application context structure. */
	UNUSED_RETURN_VALUE(ZB_MEMSET(&dev_ctx, 0, sizeof(dev_ctx)));
    UNUSED_RETURN_VALUE(ZB_MEMSET(&m_device_ctx, 0, sizeof(light_switch_ctx_t)));

	/* Register callback for handling ZCL commands. */
    //ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb);
	
    /* Register dimmer switch device context (endpoints). */
    ZB_AF_REGISTER_DEVICE_CTX(&door_lock_ctx);
	
	/* Register callback for handling endpoint. */
	ZB_AF_SET_ENDPOINT_HANDLER(HA_DOOR_LOCK_ENDPOINT, air_recevie_event_callback);

    bulb_clusters_attr_init();
}

/**@brief ZigBee stack event handler.
 *
 * @param[in]   param   Reference to ZigBee stack buffer used to pass arguments (signal).
 */
static void join_succeed_handler(uint8_t param)
{
	zb_ret_t                       zb_err_code;
	ret_code_t  				   err_code;
	
	seek_network_damon_cnt = 0;		
	//2min之内,如果搜到，则取消两分钟之后的定时器设置
	zb_err_code = ZB_SCHEDULE_ALARM_CANCEL(seek_network_stop, ZB_ALARM_ANY_PARAM);
	APP_ERROR_CHECK(zb_err_code);	
	
	err_code = app_timer_stop(seek_network_start_timer);
	APP_ERROR_CHECK(err_code);	
	
	zb_led_indicate(BSP_BOARD_LED_2, 1, LED_FILL_IN_DUTY_TIMER(1970), LED_FILL_IN_CYCLE_TIMER(2000));
}

void zboss_signal_handler(zb_uint8_t param)
{
    zb_zdo_app_signal_hdr_t      * p_sg_p         = NULL;
    zb_zdo_signal_leave_params_t * p_leave_params = NULL;
    zb_zdo_app_signal_type_t       sig            = zb_get_app_signal(param, &p_sg_p);
    zb_ret_t                       status         = ZB_GET_APP_SIGNAL_STATUS(param);
    zb_ret_t                       zb_err_code;

    switch(sig)
    {
		case ZB_ZDO_SIGNAL_SKIP_STARTUP:
		{
			/* install code */
			#if 0
			zb_ret_t ret;
			ret = zb_secur_ic_set(zb_ic);

			if (ret == RET_OK)
			{
				NRF_LOG_INFO("Add ic Ok");
			}
			else
			{
				NRF_LOG_INFO("Add ic Fail");
			}
			#endif
        }
        break;
		
        case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        case ZB_BDB_SIGNAL_DEVICE_REBOOT:
            if (status == RET_OK && network_status == NETWORK_STATUS_JOIN_REJOIN)
            {			
                NRF_LOG_INFO("Joined network successfully");
				
				zb_err_code = ZB_SCHEDULE_ALARM(join_succeed_handler, 0, ZB_TIME_ONE_SECOND);
				ZB_ERROR_CHECK(zb_err_code);
				
                if (m_device_ctx.bulb_params.short_addr == 0x0000)
                {
//                    zb_err_code = ZB_SCHEDULE_ALARM(find_light_bulb, param, MATCH_DESC_REQ_START_DELAY);
//                    ZB_ERROR_CHECK(zb_err_code);
//                    zb_err_code = ZB_SCHEDULE_ALARM(find_light_bulb_timeout, 0, MATCH_DESC_REQ_TIMEOUT);
//                    ZB_ERROR_CHECK(zb_err_code);
//                    param = 0; // Do not free buffer - it will be reused by find_light_bulb callback
                }
            }
            else
            {
                NRF_LOG_ERROR("Failed to join network");
                //bsp_board_led_off(ZIGBEE_NETWORK_STATE_LED);	
//				zb_err_code = ZB_SCHEDULE_ALARM(light_switch_retry_join, ZB_NWK_LEAVE_TYPE_RESET, NETWORK_SEEK_FREQUENCY);
//				ZB_ERROR_CHECK(zb_err_code);
            }
            break;

        case ZB_ZDO_SIGNAL_LEAVE:
            if (status == RET_OK)
            {
                //bsp_board_led_off(ZIGBEE_NETWORK_STATE_LED);
                p_leave_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_leave_params_t);
                NRF_LOG_INFO("Network left. Leave type: %d", p_leave_params->leave_type);
            }
            else
            {
                NRF_LOG_ERROR("Unable to leave network. Status: %d", status);
            }
            break;

        case ZB_COMMON_SIGNAL_CAN_SLEEP:
            {
                zb_zdo_signal_can_sleep_params_t *can_sleep_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_can_sleep_params_t);
                NRF_LOG_INFO("Can sleep for %ld ms", can_sleep_params->sleep_tmo);
                zb_sleep_now();
            }
            break;

        case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
            if (status != RET_OK)
            {
                NRF_LOG_WARNING("Production config is not present or invalid");
            }
            break;

        default:
            /* Unhandled signal. For more information see: zb_zdo_app_signal_type_e and zb_ret_e */
            NRF_LOG_INFO("Unhandled signal %d. Status: %d", sig, status);
    }

    if (param)
    {
        ZB_FREE_BUF_BY_REF(param);
    }
}


/***************************************************************************************************
* @section Main
**************************************************************************************************/
/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}
/**@brief   Sleep until an event is received. */
static void power_manage(void)
{
#ifdef SOFTDEVICE_PRESENT
    (void) sd_app_evt_wait();
#else
    __WFE();
#endif
}
/**@brief Function for application main entry.
 */
int main(void)
{
    ret_code_t err_code;
    zb_ret_t   zb_err_code;
	
	uart_init();
    /* Initialize loging system and GPIOs. */
    log_init();
    timer_init();
    leds_buttons_init();
	power_management_init();
	
	// Initialize the async SVCI interface to bootloader before any interrupts are enabled.
//    err_code = ble_dfu_buttonless_async_svci_init();
//    APP_ERROR_CHECK(err_code);

    /* Initialize fds system and register. */
    (void) fds_register(fds_evt_handler);
    err_code = fds_init();
    APP_ERROR_CHECK(err_code);
    /* Wait for fds to initialize. */
    while (!fds_init_flag)
    {
        power_manage();
    }
	 
	/* read flash network data */
	fds_read_handler(&network_status);
    /* Bluetooth initialization. */
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();

    /* Initialize Zigbee stack. */
    zigbee_init(network_status);

    /* Start execution. */
    NRF_LOG_INFO("BLE Zigbee dynamic light switch example started.");

    err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);

	/** Start Zigbee Stack. */
	NRF_LOG_INFO("flash_data=%02x", network_status);
	if(network_status == NETWORK_STATUS_JOIN_REJOIN)
	{
		NRF_LOG_INFO("zboss_start");	
		zb_err_code = zboss_start_no_autostart();
		ZB_ERROR_CHECK(zb_err_code);		
		zb_err_code = ZB_SCHEDULE_ALARM(seek_network_init, 0, ZB_TIME_ONE_SECOND);
		ZB_ERROR_CHECK(zb_err_code);
	}
	else
	{
		NRF_LOG_INFO("zboss_start_no_autostart");
		zb_err_code = zboss_start_no_autostart();
		ZB_ERROR_CHECK(zb_err_code);

		zb_led_indicate(BSP_BOARD_LED_2, 4, LED_FILL_IN_DUTY_TIMER(250), LED_FILL_IN_CYCLE_TIMER(500));		
	}
	
	sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
    while(1)
    {
        zboss_main_loop_iteration();
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
		nrf_pwr_mgmt_run();
    }
}

/***************************************************************************************************
 * @section Flash fds related functions.
 **************************************************************************************************/
static void fds_evt_handler(fds_evt_t const * p_evt)
{
    switch (p_evt->id)
    {
        case FDS_EVT_INIT:
            if (p_evt->result == FDS_SUCCESS)
            {
                fds_init_flag = true;
				NRF_LOG_INFO("Initialize fds system and register succeed");
            }
            break;

        case FDS_EVT_WRITE:
		case FDS_EVT_UPDATE:
        {
            if (p_evt->result == FDS_SUCCESS)
            {
//                NRF_LOG_INFO("Record ID:\t0x%04x",  p_evt->write.record_id);
//                NRF_LOG_INFO("File ID:\t0x%04x",    p_evt->write.file_id);
//                NRF_LOG_INFO("Record key:\t0x%04x", p_evt->write.record_key);
            }
        } break;

        case FDS_EVT_DEL_RECORD:
		case FDS_EVT_DEL_FILE:
        {
            if (p_evt->result == FDS_SUCCESS)
            {
//                NRF_LOG_INFO("Record ID:\t0x%04x",  p_evt->del.record_id);
//                NRF_LOG_INFO("File ID:\t0x%04x",    p_evt->del.file_id);
//                NRF_LOG_INFO("Record key:\t0x%04x", p_evt->del.record_key);
            }
        } break;
		
		case FDS_EVT_GC:
		{
			if (p_evt->result == FDS_SUCCESS)
			{
				NRF_LOG_INFO("fds garbage collection");
			}
		}break;
		
        default:
            break;
    }
}
/*****************************************************************/
static zb_void_t fds_read_handler(zb_uint8_t * param)
{
	ret_code_t rc;	
	
	fds_record_desc_t desc = {0};
    fds_find_token_t  tok  = {0};
    rc = fds_record_find(FDS_TEST_FILE, FDS_TEST_KEY, &desc, &tok);

    if(rc == FDS_SUCCESS)
    {
        fds_flash_record_t config = {0};
        /* Open the record and read its contents. */
        rc = fds_record_open(&desc, &config);
        APP_ERROR_CHECK(rc);

        /* Copy the configuration from flash into m_dummy_cfg. */
        memcpy(&fds_test_cfg, config.p_data, sizeof(fds_type_t));
		*param = fds_test_cfg.status_flag;
        /* Close the record when done reading. */
        rc = fds_record_close(&desc);
        APP_ERROR_CHECK(rc);
    }
	else 
		*param = NETWORK_STATUS_LEAVE;
}
static zb_void_t fds_write_handler(zb_uint8_t param)
{
	//static uint32_t fds_cnt = 0;
	ret_code_t rc;
	
	/* Update count and value*/
	fds_test_cfg.status_flag = param;
	
	fds_record_desc_t desc = {0};
    fds_find_token_t  tok  = {0};
    rc = fds_record_find(FDS_TEST_FILE, FDS_TEST_KEY, &desc, &tok);

    if (rc == FDS_SUCCESS)
    {
        NRF_LOG_INFO("update config count %d", fds_test_cfg.status_flag);
        /* Write the updated record to flash. */
        rc = fds_record_update(&desc, &fds_test_record);
		if(rc == FDS_ERR_NO_SPACE_IN_FLASH)
		{
			NRF_LOG_INFO("update flash overflow");
			rc = fds_gc();
		}
        APP_ERROR_CHECK(rc);
    }
    else
    {
        /* System config not found; write a new one. */
        NRF_LOG_INFO("write config count %d", fds_test_cfg.status_flag);
        rc = fds_record_write(&desc, &fds_test_record);
		if(rc == FDS_ERR_NO_SPACE_IN_FLASH)
		{
			NRF_LOG_INFO("write flash overflow");
			rc = fds_gc();
		}
        APP_ERROR_CHECK(rc);
    }
}


/***************************************************************************************************
 * @section zigbee network status indicate ligth
 **************************************************************************************************/
uint32_t led_idx_tmp = 0;
uint8_t led_cnt_tmp = 0;
uint8_t led_duty_timer_tmp = 0;
uint8_t led_cycle_timer_tmp = 0;
static void zb_led_indicate_off(zb_uint8_t param)
{
	bsp_board_led_off(param);
	return;
}

static void zb_led_indicate_callback(zb_uint8_t param)
{
	zb_led_indicate(led_idx_tmp, led_cnt_tmp, led_duty_timer_tmp, led_cycle_timer_tmp);
	return;
}
	
static void zb_led_indicate(uint32_t led_idx, uint8_t led_cnt, uint8_t duty_timer, uint8_t cycle_timer)
{
	zb_ret_t   zb_err_code;
	if(duty_timer > cycle_timer || led_cnt <= 0)
	{
		return;
	}
	led_idx_tmp = led_idx;
	led_cnt_tmp = led_cnt;
	led_duty_timer_tmp = duty_timer;
	led_cycle_timer_tmp = cycle_timer;
	
	switch(led_idx_tmp)
	{
		case BSP_BOARD_LED_0:
		case BSP_BOARD_LED_1:
		case BSP_BOARD_LED_2:
		case BSP_BOARD_LED_3:
		{
			bsp_board_led_on(led_idx_tmp);
			zb_err_code = ZB_SCHEDULE_ALARM(zb_led_indicate_off, led_idx_tmp, led_duty_timer_tmp);
			ZB_ERROR_CHECK(zb_err_code);
		}
		break;
		
		default:
			break;
	}
	
	if(--led_cnt_tmp)
	{
		zb_err_code = ZB_SCHEDULE_ALARM(zb_led_indicate_callback, led_cnt_tmp, led_cycle_timer_tmp);
		ZB_ERROR_CHECK(zb_err_code);
	}
}
/**
 * @}
 */




