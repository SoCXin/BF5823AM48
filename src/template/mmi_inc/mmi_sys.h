#ifndef __MMI_SYS_H__
#define __MMI_SYS_H__

#include "mmi_com.h"
#include "mmi_audio.h"
#include "mmi_feature.h"
#include "mmi_ms.h"
#include "mcu02_sfr.h"

typedef enum
{
	SYS_OPEN_BY_PASSWORD = 0x01,
	SYS_OPEN_BY_FP = 0x02,
	SYS_OPEN_BY_RFID = 0x04,
	SYS_OPEN_BY_ADMIN_PASSWORD = 0x20,
	SYS_OPEN_BY_USER_PASSWORD = 0x40,
	SYS_OPEN_BY_WIFI = 0x08,
	SYS_OPEN_BY_110 = 0x10,
	SYS_OPEN_BY_110_PASSWORD = 0x11,
	SYS_OPEN_BY_110_FP = 0x12,
} sys_open_type;

typedef enum
{
	SYS_OPEN_MODE_SIN = 0,
	SYS_OPEN_MODE_DBL = 1,
	SYS_OPEN_MODE_INVALID = 0xFF
} sys_open_mode;

typedef void (*timer2_delay_pro)(void);

/*
function: system enter sleep
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_init(void);

/*
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_lock_add_admin_suc(void);

/*
function: system enter sleep
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_enter_sleep(void);

/*
function: system wake up
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_wake_up(void);

/*
function: system wake up
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_door_open(sys_open_type type);

/*
function: 
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_door_open_fail(sys_open_type type);

/*
function: 
parameter: 
	none
return :
	none
*/
unsigned char mmi_dq_sys_door_state_check(void);

/*
function: system wake up
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_door_close(void);

/*
parameter: 
	none
return :
	none
*/
// #ifdef __LOCK_VIRTUAL_PASSWORD__
// void mmi_dq_sys_show_message_with_id(unsigned char tile_id, unsigned char bmp_id, unsigned char text_id, unsigned int time_msec, BASE_STATUS_MACHINE status);
// #else
void mmi_dq_sys_show_message_with_id(unsigned char text_id, unsigned long time_msec);
// #endif
/*
parameter: 
	none
return :
	none
*/
unsigned char mmi_dq_sys_get_rfid_flag(void);

/*
function: 
parameter: 
	none
return :
	none
*/
unsigned char mmi_dq_sys_get_wifi_check_flag(void);

/*
function: 
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_set_wifi_check(unsigned char type);

/*
function: 
parameter: 
	none
return :
	none
*/
unsigned char mmi_dq_sys_get_wifi_check_type(void);

/*
function: 
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_sleep_timer_reset(void);

/*
function: 
parameter: 
	none
return :
	none
*/
unsigned char mmi_dq_sys_get_timer2_flag(void);

/*
function: 
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_set_delay_event(unsigned int count, timer2_delay_pro pro);

/*
function: 
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_delay_event_pro(void);

/*
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_set_menu_father_id(unsigned char father_id);

/*
parameter: 
	none
return :
	none
*/
unsigned char mmi_dq_sys_get_menu_count(void);

/*
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_show_cur_menu_list(void);

/*
parameter: 
	none
return :
	current machine status
*/
void mmi_dq_sys_get_pre_menu_list(void);

/*
parameter: 
	none
return :
	none
*/
RET_VAL mmi_dq_sys_exe_menu_fun(unsigned char menu_id);

/*
function: 
parameter: 
	none
return :
	none
*/
unsigned char mmi_dq_sys_check_vbat(void);

/**
  * @brief  远程开门
  * @param  none
  * @return none
  * @note   none
  * @see    none
  */
void mmi_dq_sys_wifi_open(void);

/*
parameter: 
	none
return :
	none
*/
void mmi_dq_sys_add_admin_pwd(void);

//--------------------------------------------------------------------------------------------------------------------------------------------
void mmi_dq_sys_add_pwd(void);
void mmi_dq_sys_del_pwd(void);

void mmi_dq_sys_add_pwd_con(void);
void mmi_dq_sys_del_pwd_con(void);

#ifdef __LOCK_FP_SUPPORT__
void mmi_dq_sys_add_fp(void);
void mmi_dq_sys_del_fp(void);

void mmi_dq_sys_add_fp_con(void);
void mmi_dq_sys_del_fp_con(void);
#endif

#ifdef __LOCK_RFID_CARD_SUPPORT__
void mmi_dq_sys_add_rf(void);
void mmi_dq_sys_del_rf(void);

void mmi_dq_sys_add_rf_con(void);
void mmi_dq_sys_del_rf_con(void);
#endif

#ifdef __LOCK_FP_SUPPORT__
void mmi_dq_sys_chg_admin_fp_No1(void);
void mmi_dq_sys_chg_admin_fp_No2(void);
#endif
void mmi_dq_sys_chg_admin_pwd(void);

void mmi_dq_sys_restore_lock(void);

uint8_t mmi_dq_lock_random_vector_generate(uint8_t * p_buff, uint8_t size);

#endif //__MMI_SYS_H__
