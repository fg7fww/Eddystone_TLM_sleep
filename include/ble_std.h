/* ----------------------------------------------------------------------------
 * Copyright (c) 2015-2017 Semiconductor Components Industries, LLC (d/b/a
 * ON Semiconductor), All Rights Reserved
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 * This module is derived in part from example code provided by RivieraWaves
 * and as such the underlying code is the property of RivieraWaves [a member
 * of the CEVA, Inc. group of companies], together with additional code which
 * is the property of ON Semiconductor. The code (in whole or any part) may not
 * be redistributed in any form without prior written permission from
 * ON Semiconductor.
 *
 * The terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 *
 * This is Reusable Code.
 *
 * ----------------------------------------------------------------------------
 * ble_std.h
 * - Bluetooth standard header
 * ----------------------------------------------------------------------------
 * $Revision: 1.18 $
 * $Date: 2017/12/12 17:32:49 $
 * ------------------------------------------------------------------------- */

#ifndef BLE_STD_H
#define BLE_STD_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

/* Number of APP Task Instances */
#define APP_IDX_MAX                     1

/* Advertising channel map - 37, 38, 39 */
#define APP_ADV_CHMAP                   0x07

/*Advertisement connectable mode */
#define ADV_CONNECTABLE_MODE            0

/*Advertisement non-connectable mode */
#define ADV_NON_CONNECTABLE_MODE        1

/*Advertisement connectivity mode
 * Options are:
 *     - ADV_CONNECTABLE_MODE
 *     - ADV_NON_CONNECTABLE_MODE */
#define APP_ADV_CONNECTABILITY_MODE     ADV_NON_CONNECTABLE_MODE

/* Define the advertisement interval for connectable mode (units of 625us)
 * Notes: the interval can be 20ms up to 10.24s */
#ifdef CFG_ADV_INTERVAL_MS
#define ADV_INT_CONNECTABLE_MODE        (CFG_ADV_INTERVAL_MS/0.625)
#else
#define ADV_INT_CONNECTABLE_MODE        64
#endif

/* Define the advertisement interval for non-connectable mode (units of 625us)
 * Notes: the minimum interval for non-connectable advertising should be 100ms */
#ifdef CFG_ADV_INTERVAL_MS
#define ADV_INT_NON_CONNECTABLE_MODE    (CFG_ADV_INTERVAL_MS/0.625)
#else
#define ADV_INT_NON_CONNECTABLE_MODE    160
#endif

/* Set the advertisement interval */
#if (APP_ADV_CONNECTIVITY_MODE == ADV_CONNECTABLE_MODE)
#define APP_ADV_INT_MIN                 ADV_INT_CONNECTABLE_MODE
#define APP_ADV_INT_MAX                 ADV_INT_CONNECTABLE_MODE
#elif (APP_ADV_CONNECTIVITY_MODE == ADV_NON_CONNECTABLE_MODE)
#define APP_ADV_INT_MIN                 ADV_INT_NON_CONNECTABLE_MODE
#define APP_ADV_INT_MAX                 ADV_INT_NON_CONNECTABLE_MODE
#endif

/* Bluetooth device address type */
#define BD_TYPE_PUBLIC                  0
#define BD_TYPE_PRIVATE                 1
#define BD_ADDRESS_TYPE                 BD_TYPE_PUBLIC

/* Length of Bluetooth address (in octets) */
#define BDADDR_LENGTH                   6

/* Non-resolvable private Bluetooth device address */
#define PRIVATE_BDADDR                  {0x94,0x11,0x22,0xff,0xff,0xa5}

/* Public Bluetooth device address */
#define APP_PUBLIC_BDADDR               {0x11,0x22,0x22,0x44,0x44,0x33}


/* RF Oscillator accuracy in ppm */
#define RADIO_CLOCK_ACCURACY            20

/* Slave preferred connection parameters */
#define PREF_SLV_MIN_CON_INTERVAL       8
#define PREF_SLV_MAX_CON_INTERVAL       10
#define PREF_SLV_LATENCY                0
#define PREF_SLV_SUP_TIMEOUT            200

/* Set the device name */
#define APP_DEVICE_NAME_LENGTH_MAX      20
#define APP_DFLT_DEVICE_NAME            "OnSemi RSL10"
#define APP_DEVICE_NAME_FLAG            '\x09'

/* ON SEMICONDUCTOR Company ID */
#define APP_COMPANY_ID_DATA             { 0x4, 0xff, 0x62, 0x3, 0x3 }
#define APP_COMPANY_ID_DATA_LEN         (0x4 + 1)

/* Vendor specific scan response data (ON SEMICONDUCTOR Company ID) */
#define APP_SCNRSP_DATA                 APP_COMPANY_ID_DATA
#define APP_SCNRSP_DATA_LEN             APP_COMPANY_ID_DATA_LEN

/* GAPM configuration definitions */
#define RENEW_DUR                       15000
#define MTU_MAX                         0x200
#define MPS_MAX                         0x200
#define ATT_CFG                         0x80
#define TX_OCT_MAX                      0x1b
#define TX_TIME_MAX                     (14 * 8 + TX_OCT_MAX * 8)

/* Define the available application states */
enum appm_state
{
    /* Initialization state */
    APPM_INIT,

    /* Database create state */
    APPM_CREATE_DB,

    /* Ready State */
    APPM_READY,

    /* Advertising state */
    APPM_ADVERTISING,

    /* Connecting state */
    APPM_CONNECTING,

    /* Connected state */
    APPM_CONNECTED,

    /* Number of defined states */
    APPM_STATE_MAX
};

/* List of message handlers that are used by the Bluetooth application manager */
#define BLE_MESSAGE_HANDLER_LIST                                              \
    DEFINE_MESSAGE_HANDLER(GAPM_CMP_EVT, GAPM_CmpEvt),                        \
    DEFINE_MESSAGE_HANDLER(GAPM_PROFILE_ADDED_IND, GAPM_ProfileAddedInd),     \
    DEFINE_MESSAGE_HANDLER(GAPC_CONNECTION_REQ_IND, GAPC_ConnectionReqInd),   \
    DEFINE_MESSAGE_HANDLER(GAPC_CMP_EVT, GAPC_CmpEvt),                        \
    DEFINE_MESSAGE_HANDLER(GAPC_DISCONNECT_IND, GAPC_DisconnectInd),          \
    DEFINE_MESSAGE_HANDLER(GAPC_GET_DEV_INFO_REQ_IND, GAPC_GetDevInfoReqInd), \
    DEFINE_MESSAGE_HANDLER(GAPC_PARAM_UPDATED_IND, GAPC_ParamUpdatedInd),     \
    DEFINE_MESSAGE_HANDLER(GAPC_PARAM_UPDATE_REQ_IND, GAPC_ParamUpdateReqInd) \


/* Standard declaration/description UUIDs in 16-byte format */
#define ATT_DECL_CHARACTERISTIC_128     { 0x03, 0x28, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00 }
#define ATT_DESC_CLIENT_CHAR_CFG_128    { 0x02, 0x29, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00 }
#define ATT_DESC_CHAR_USER_DESC_128     { 0x01, 0x29, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00 }

/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/

/* Support for the application manager and the application environment */
extern const struct ke_state_handler appm_default_handler;
extern ke_state_t appm_state[APP_IDX_MAX];

struct ble_env_tag
{
    /* Connection handle */
    uint16_t conhdl;

    /* Connection index */
    uint8_t conidx;

    /* Next service to initialize */
    uint8_t next_svc;

    /* Next service to enable */
    uint8_t next_svc_enable;

    /* Application state */
    uint8_t state;

    /* Connection parameters */
    uint16_t con_interval;
    uint16_t time_out;
    uint16_t con_latency;

    uint16_t updated_con_interval;
    uint16_t updated_latency;
    uint16_t updated_suo_to;

    uint32_t adv_count;
    uint32_t adv_time;

    /* I2C reception buffer */
    uint8_t i2c_rx_buffer[8];
    /* I2C reception buffer */
    uint8_t i2c_tx_buffer[8];

    uint32_t temperature;

    /* Battery service */
    uint16_t batt_lvl;

};

/* Support for the application manager and the application environment */
extern struct ble_env_tag ble_env;

/* Bluetooth Device Address */
extern uint8_t bdaddr[BDADDR_LENGTH];

/* ----------------------------------------------------------------------------
 * Function prototype definitions
 * --------------------------------------------------------------------------*/

/* Bluetooth baseband application support functions */
extern void BLE_Initialize(void);
extern bool Service_Add(void);
extern void Advertising_Start(void);
extern void Advertising_Update(void);
extern void BLE_SetStateEnable(void);
extern void BLE_SetServiceState(bool enable, uint8_t conidx);
extern bool Service_Enable(uint8_t conidx);

/* Bluetooth event and message handlers */
extern int GAPM_ProfileAddedInd(ke_msg_id_t const msgid,
                                struct gapm_profile_added_ind const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id);
extern int GAPM_CmpEvt(ke_msg_id_t const msgid,
                       struct gapm_cmp_evt const *param,
                       ke_task_id_t const dest_id,
                       ke_task_id_t const src_id);
extern int GAPC_CmpEvt(ke_msg_id_t const msgid,
                       struct gapc_cmp_evt const *param,
                       ke_task_id_t const dest_id,
                       ke_task_id_t const src_id);
extern int GAPC_GetDevInfoReqInd(ke_msg_id_t const msgid,
                                 struct gapc_get_dev_info_req_ind const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id);
extern int GAPC_DisconnectInd(ke_msg_id_t const msgid,
                              struct gapc_disconnect_ind const *param,
                              ke_task_id_t const dest_id,
                             ke_task_id_t const src_id);
extern int GAPC_ParamUpdatedInd(ke_msg_id_t const msgid,
                                struct gapc_param_updated_ind const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id);
extern int GAPC_ParamUpdateReqInd(ke_msg_id_t const msg_id,
                                  struct gapc_param_update_req_ind const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id);
extern int GAPC_ConnectionReqInd(ke_msg_id_t const msgid,
                                 struct gapc_connection_req_ind const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif

#endif /* BLE_STD_H */
