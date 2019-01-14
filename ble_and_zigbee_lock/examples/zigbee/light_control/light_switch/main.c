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
 * @defgroup zigbee_examples_light_switch main.c
 * @{
 * @ingroup zigbee_examples
 * @brief Dimmer switch for HA profile implementation.
 */

#include "zboss_api.h"
#include "zb_mem_config_min.h"
#include "zb_error_handler.h"
#include "wl_door_lock_config.h"

#include "app_timer.h"
#include "bsp.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "app_uart.h"
#include "nrf_uart.h"
#include "nrf_uarte.h"

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */

#define IEEE_CHANNEL_MASK                   (1l << ZIGBEE_CHANNEL)              /**< Scan only one, predefined channel to find the coordinator. */
#define LIGHT_SWITCH_ENDPOINT               1                                   /**< Source endpoint used to control light bulb. */
#define MATCH_DESC_REQ_START_DELAY          (2 * ZB_TIME_ONE_SECOND)            /**< Delay between the light switch startup and light bulb finding procedure. */
#define MATCH_DESC_REQ_TIMEOUT              (5 * ZB_TIME_ONE_SECOND)            /**< Timeout for finding procedure. */
#define MATCH_DESC_REQ_ROLE                 ZB_NWK_BROADCAST_RX_ON_WHEN_IDLE    /**< Find only non-sleepy device. */
#define ERASE_PERSISTENT_CONFIG             ZB_FALSE                            /**< Do not erase NVRAM to save the network parameters after device reboot or power-off. NOTE: If this option is set to ZB_TRUE then do full device erase for all network devices before running other samples. */
#define ZIGBEE_NETWORK_STATE_LED            BSP_BOARD_LED_2                     /**< LED indicating that light switch successfully joind ZigBee network. */
#define BULB_FOUND_LED                      BSP_BOARD_LED_3                     /**< LED indicating that light witch found a light bulb to control. */
#define LIGHT_SWITCH_BUTTON_ON              BSP_BOARD_BUTTON_0                  /**< Button ID used to switch on the light bulb. */
#define LIGHT_SWITCH_BUTTON_OFF             BSP_BOARD_BUTTON_1                  /**< Button ID used to switch off the light bulb. */
#define SLEEPY_ON_BUTTON                    BSP_BOARD_BUTTON_2                  /**< Button ID used to determine if we need the sleepy device behaviour (pressed means yes). */

#define LIGHT_SWITCH_DIMM_STEP              15                                  /**< Dim step size - increases/decreses current level (range 0x000 - 0xfe). */
#define LIGHT_SWITCH_DIMM_TRANSACTION_TIME  2                                   /**< Transition time for a single step operation in 0.1 sec units. 0xFFFF - immediate change. */

#define LIGHT_SWITCH_BUTTON_THRESHOLD       ZB_TIME_ONE_SECOND                      /**< Number of beacon intervals the button should be pressed to dimm the light bulb. */
#define LIGHT_SWITCH_BUTTON_SHORT_POLL_TMO  ZB_MILLISECONDS_TO_BEACON_INTERVAL(50)  /**< Delay between button state checks used in order to detect button long press. */
#define LIGHT_SWITCH_BUTTON_LONG_POLL_TMO   ZB_MILLISECONDS_TO_BEACON_INTERVAL(300) /**< Time after which the button state is checked again to detect button hold - the dimm command is sent again. */

#if !defined ZB_ED_ROLE
#error Define ZB_ED_ROLE to compile light switch (End Device) source code.
#endif


typedef struct light_switch_bulb_params_s
{
  zb_uint8_t  endpoint;
  zb_uint16_t short_addr;
} light_switch_bulb_params_t;

typedef struct light_switch_button_s
{
  zb_bool_t in_progress;
  zb_time_t timestamp;
} light_switch_button_t;

typedef struct light_switch_ctx_s
{
  light_switch_bulb_params_t bulb_params;
  light_switch_button_t      button;
} light_switch_ctx_t;

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
	//zb_ret_t                        zb_err_code;
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
/**********************************************************************************************/


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    ret_code_t err_code;

    // Initialize timer module.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
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
static zb_void_t light_switch_retry_join(zb_uint8_t leave_type)
{
    zb_bool_t comm_status;

    if (leave_type == ZB_NWK_LEAVE_TYPE_RESET)
    {
        comm_status = bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        ZB_COMM_STATUS_CHECK(comm_status);
    }
}

/**@brief Function for leaving current network and starting join procedure afterwards.
 *
 * @param[in]   param   Optional reference to ZigBee stack buffer to be reused by leave and join procedure.
 */
static zb_void_t light_switch_leave_and_join(zb_uint8_t param)
{
    if (ZB_JOINED())
    {
        /* Leave network. Joining procedure will be initiated inisde ZigBee stack signal handler. */
        light_switch_leave_nwk(param);
    }
    else
    {
        /* Already left network. Start joining procedure. */
        light_switch_retry_join(ZB_NWK_LEAVE_TYPE_RESET);

        if (param)
        {
            ZB_FREE_BUF_BY_REF(param);
        }
    }
}


/**@brief Callback for button events.
 *
 * @param[in]   evt      Incoming event from the BSP subsystem.
 */
static void buttons_handler(bsp_event_t evt)
{
    zb_uint32_t button;

    switch(evt)
    {
        case BSP_EVENT_KEY_0:
            button = LIGHT_SWITCH_BUTTON_ON;
            break;

        case BSP_EVENT_KEY_1:
            button = LIGHT_SWITCH_BUTTON_OFF;
            break;

        default:
            NRF_LOG_INFO("Unhandled BSP Event received: %d", evt);
            return;
    }

}

/**@brief Function for initializing LEDs and buttons.
 */
static zb_void_t leds_buttons_init(void)
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

/**@brief ZigBee stack event handler.
 *
 * @param[in]   param   Reference to ZigBee stack buffer used to pass arguments (signal).
 */
void zboss_signal_handler(zb_uint8_t param)
{
    zb_zdo_app_signal_hdr_t      * p_sg_p         = NULL;
    zb_zdo_signal_leave_params_t * p_leave_params = NULL;
    zb_zdo_app_signal_type_t       sig            = zb_get_app_signal(param, &p_sg_p);
    zb_ret_t                       status         = ZB_GET_APP_SIGNAL_STATUS(param);
    zb_ret_t                       zb_err_code;

    switch(sig)
    {
        case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        case ZB_BDB_SIGNAL_DEVICE_REBOOT:
            if (status == RET_OK)
            {
                NRF_LOG_INFO("Joined network successfully");
                bsp_board_led_on(ZIGBEE_NETWORK_STATE_LED);
            }
            else
            {
                NRF_LOG_ERROR("Failed to join network. Status: %d", status);
                bsp_board_led_off(ZIGBEE_NETWORK_STATE_LED);
                zb_err_code = ZB_SCHEDULE_ALARM(light_switch_leave_and_join, 0, ZB_TIME_ONE_SECOND*5);
                ZB_ERROR_CHECK(zb_err_code);
            }
            break;

        case ZB_ZDO_SIGNAL_LEAVE:
            if (status == RET_OK)
            {
                bsp_board_led_off(ZIGBEE_NETWORK_STATE_LED);
                p_leave_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_leave_params_t);
                NRF_LOG_INFO("Network left. Leave type: %d", p_leave_params->leave_type);
                light_switch_retry_join(p_leave_params->leave_type);
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

/**@brief Function for application main entry.
 */
int main(void)
{
    zb_ret_t       zb_err_code;
    zb_ieee_addr_t ieee_addr;

	uart_init();
    /* Initialize timers, loging system and GPIOs. */
    timers_init();
    log_init();
    leds_buttons_init();

    /* Set ZigBee stack logging level and traffic dump subsystem. */
    ZB_SET_TRACE_LEVEL(ZIGBEE_TRACE_LEVEL);
    ZB_SET_TRACE_MASK(ZIGBEE_TRACE_MASK);
    ZB_SET_TRAF_DUMP_OFF();

    /* Initialize ZigBee stack. */
    ZB_INIT("light_switch");

    /* Set device address to the value read from FICR registers. */
    zb_osif_get_ieee_eui64(ieee_addr);
    zb_set_long_address(ieee_addr);

    zb_set_network_ed_role(IEEE_CHANNEL_MASK);
    zb_set_nvram_erase_at_start(ERASE_PERSISTENT_CONFIG);

    zb_set_ed_timeout(ED_AGING_TIMEOUT_64MIN);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));
    sleepy_device_setup();
	/* 低功耗模式 */
	//zb_set_rx_on_when_idle(ZB_FALSE);

    /* Initialize application context structure. */
    UNUSED_RETURN_VALUE(ZB_MEMSET(&dev_ctx, 0, sizeof(dev_ctx)));

    /* Register callback for handling ZCL commands. */
    //ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb);

    /* Register dimmer switch device context (endpoints). */
    ZB_AF_REGISTER_DEVICE_CTX(&door_lock_ctx);
	
	/* Register callback for handling endpoint. */
	ZB_AF_SET_ENDPOINT_HANDLER(HA_DOOR_LOCK_ENDPOINT, air_recevie_event_callback);

    bulb_clusters_attr_init();

    /** Start Zigbee Stack. */
    zb_err_code = zboss_start();
    ZB_ERROR_CHECK(zb_err_code);

    while(1)
    {
        zboss_main_loop_iteration();
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
    }
}


/**
 * @}
 */
