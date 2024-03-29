#ifndef __DQ_SDK_MAIN_C__
#define __DQ_SDK_MAIN_C__
#include "mmi_feature.h"
#ifdef __LOCK_VIRTUAL_PASSWORD__
#include "dq_sdk_main.h"
#include "dq_sdk_cb.h"
#include "dq_sdk_crc.h"
//#include "dq_sdk_aes.h"
#include "mmi_decode.h"
#include "mcu02_sfr.h"
#include "mmi_time.h"

#ifndef __WIN32_ENV_SUPPORT__
// #include "nrf_log.h"
#endif

#ifdef __LOCK_USE_MALLOC__
typedef enum
{
	DQ_FS_MEM_INVALID,
	DQ_FS_MEM_PWD,
	DQ_FS_MEM_FP,
	DQ_FS_MEM_RFID,
	DQ_FS_MEM_APP_PWD,
	DQ_FS_MEM_APP_FP,
	DQ_FS_MEM_APP_RFID,
} dq_fs_storage_type;

void *mmi_dq_fs_get_storage(dq_fs_storage_type type, uint8_t *ret);
uint8_t mmi_dq_fs_free_storage(dq_fs_storage_type type, void **p);

#endif

//const data
#define DQ_OTP_AES_KEY_LEN 16
#define DQ_OTP_CMD_NO_CRC_LENGTH 32

//#define DQ_OTP_START_TIME_SEC		1546272000//2019.01.01.00.00

static const uint8_t DQ_OTP_AES_GBL_KEY[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

static unsigned char DQ_OTP_SYS_TIME_GET = 0;

static uint8_t g_dq_dfu_permit = 0;

// typedef struct otp_exchg_num
// {
// 	unsigned char num;
// 	unsigned char exchg_num[5];
// } otp_exchg_num;

typedef struct otp_base_setting_info
{
	unsigned char init_suc_flag;
	unsigned char reserve;
	unsigned char otp_dev_key[16];
	unsigned char otp_sec_key_8[4];
	unsigned char otp_exg_key_8[4];
	unsigned char otp_sec_key_9[5];
	unsigned char otp_exg_key_9[5];
	//unsigned short otp_empty_hour;
	otp_exchg_num g_pwd_signed_data[10];
	uint32_t otp_start_hour;
} otp_base_setting_info;

typedef struct otp_base_log_info
{
	uint32_t sys_time;
	uint8_t app_pwd[5];
	uint8_t type;
	uint8_t sys_time_correct;
	uint8_t flag;
} otp_base_log_info;

typedef struct otp_base_lock_chg_info
{
	unsigned char lock_pwd_chg_flag;
	unsigned char lock_fp_chg_flag;
	unsigned char lock_rf_chg_flag;
	unsigned char lock_log_chg_flag;
	unsigned char lock_opt_chg_flag;
	unsigned char lock_door_aud_flag;
	unsigned char lock_con_flag;
} otp_base_lock_chg_info;

//enum
typedef enum
{
	//init syn
	CMD_OTP_SYN_AES_16,		//0x00,0101
	CMD_OTP_SYN_SEC_8,		//0x01,0102
	CMD_OTP_SYN_SEC_9,		//0x02,0103
	CMD_OTP_SYN_START_TIME, //0x03,0104
	CMD_OTP_SYN_PWD_STR,	//0x04,0105
							//ekey
	CMD_OTP_EKEY_OPEN_LOCK, //0x05,0201
							//pwd
	CMD_OTP_USER_DEF_PWD,	//0X06,//0211
	CMD_OTP_DEL_PWD,		//0x07,0212
	CMD_OTP_CLR_PWD,		//0x08,0213
	CMD_OTP_SYN_PWD,		//0214
	CMD_OTP_SYN_TEMP_PWD,	//0215

	//fp
	CMD_OTP_ADD_FP, //0x09,0221
	CMD_OTP_DEL_FP, //0x0A,0222
	CMD_OTP_CLR_FP, //0x0B,0223
	CMD_OTP_SYN_FP, //0224

	//RFID
	CMD_OTP_ADD_RFID,		 //0x0C,0231
	CMD_OTP_DEL_RFID,		 //0x0D,0232
	CMD_OTP_CLR_RFID,		 //0x0E,0233
	CMD_OTP_SYN_RFID,		 //0234
	CMD_OTP_ADD_RFID_BY_APP, //0235
							 //LOG
	CMD_OTP_UPDATE_LOG,		 //0x0F,0301

	//lock info
	CMD_OTP_GET_LOCK_CONFIG,	//0x10,0401
	CMD_OTP_GET_LOCK_BASE_INFO, //0x11,0402
	CMD_OTP_CHK_ADMIN_STATUS,	//0x12,0403
	CMD_OTP_VERY_ADMIN_STA,		//0x13,0404
	CMD_OTP_INIT_SUCESS,		//0x14,0405
	CMD_OTP_SYS_ENTER_DFU,		//0x16,0406
	CMD_OTP_GET_SYS_TIME,		//0x17,0407
	CMD_OTP_MAX_NUM,

	CMD_OTP_INVALID = 0xFF
} OTP_TYPE;

typedef enum
{
	OTP_BASE_SUCESS = 0x00,
	OTP_BASE_FAIL = 0x01,
	OTP_BASE_ERR_UNKNOW = 0x02,
	OTP_BASE_BONDED = 0x03,
	OTP_BASE_UNBOND = 0x04,
	OTP_BASE_NO_MEM = 0x05,
	OTP_BASE_NOT_FOUND = 0x06,
	OTP_BASE_TYPE_ERROR = 0x07,
	OTP_BASE_EXIST = 0x08,
	OTP_BASE_NOT_PAIR = 0x09,
	OTP_BASE_FULL = 0x0A,
	OTP_BASE_EMPTY = 0x0B,
	OTP_BASE_TIMEOUT = 0x0C,
	OTP_BASE_HAND_EXIT = 0x0D,
	OTP_BASE_BUSY = 0x0E,
	OTP_BASE_USED = 0x0F,
	OTP_BASE_UNUSED = 0x10,
	OTP_BASE_WAIT_NEXT_CMD = 0x11,
	OTP_BASE_MUL_ERROR = 0x20,
	OTP_BASE_LOW_VBAT = 0x21,
	OTP_BASE_RET_INVALID,
} OTP_BASE_RET;

typedef enum
{
	OTP_PWD_TYPE_PER = 0x01,
	OTP_PWD_TYPE_SIN = 0x02,
	OTP_PWD_TYPE_LIM = 0x03,
	OTP_PWD_TYPE_LOOP = 0x04,
	OTP_PWD_TYPE_CLR = 0x05,
	OTP_PWD_TYPE_INVALID = 0xFF
} OTP_PWD_TYPE;

typedef struct
{
	unsigned char cmd_h;
	unsigned char cmd_l;
	OTP_TYPE cmd;
} otp_cmd_info;

const otp_cmd_info cmd_info[] =
	{
		{0x01, 0x01, CMD_OTP_SYN_AES_16},
		{0x01, 0x02, CMD_OTP_SYN_SEC_8},
		{0x01, 0x03, CMD_OTP_SYN_SEC_9},
		{0x01, 0x04, CMD_OTP_SYN_START_TIME},
		{0x01, 0x05, CMD_OTP_SYN_PWD_STR},

		{0x02, 0x01, CMD_OTP_EKEY_OPEN_LOCK},
		{0x02, 0x11, CMD_OTP_USER_DEF_PWD},
		{0x02, 0x12, CMD_OTP_DEL_PWD},
		{0x02, 0x13, CMD_OTP_CLR_PWD},
		{0x02, 0x14, CMD_OTP_SYN_PWD},
		{0x02, 0x15, CMD_OTP_SYN_TEMP_PWD},

		{0x02, 0x21, CMD_OTP_ADD_FP},
		{0x02, 0x22, CMD_OTP_DEL_FP},
		{0x02, 0x23, CMD_OTP_CLR_FP},
		{0x02, 0x24, CMD_OTP_SYN_FP},

		{0x02, 0x31, CMD_OTP_ADD_RFID},
		{0x02, 0x32, CMD_OTP_DEL_RFID},
		{0x02, 0x33, CMD_OTP_CLR_RFID},
		{0x02, 0x34, CMD_OTP_SYN_RFID},
		{0x02, 0x35, CMD_OTP_ADD_RFID_BY_APP},

		{0x03, 0x01, CMD_OTP_UPDATE_LOG},
		{0x04, 0x01, CMD_OTP_GET_LOCK_CONFIG},
		{0x04, 0x02, CMD_OTP_GET_LOCK_BASE_INFO},
		{0x04, 0x03, CMD_OTP_CHK_ADMIN_STATUS},
		//lock
		{0x04, 0x04, CMD_OTP_VERY_ADMIN_STA},
		{0x04, 0x05, CMD_OTP_INIT_SUCESS},
		{0x04, 0x06, CMD_OTP_SYS_ENTER_DFU},
		{0x04, 0x07, CMD_OTP_GET_SYS_TIME},
};

//variable
dq_otp_init_t g_dq_otp_init;

otp_base_setting_info otp_set_info;
#ifdef __LOCK_USE_MALLOC__
otp_base_app_pwd_info *g_dq_app_pwd_info; //100

otp_base_app_fp_info *g_dq_app_fp_info; //10
otp_base_app_rfid_info *g_dq_app_rfid_info;
#else
otp_base_app_pwd_info g_dq_app_pwd_info[DQ_OTP_APP_PWD_NUM]; //100

otp_base_app_fp_info g_dq_app_fp_info[DQ_OTP_APP_FP_NUM]; //10
otp_base_app_rfid_info g_dq_app_rfid_info[DQ_OTP_APP_RFID_NUM];
#endif
otp_base_lock_chg_info g_dq_otp_lock_chg;
otp_base_log_info otp_lock_log[DQ_OTP_LOG_NUM];
otp_base_log_info otp_lock_temp_log[2];

unsigned char otp_log_head, otp_log_end;

//static uint16_t 		serialId = 0;
static uint8_t serialId = 0;

static uint8_t dq_dev_key_flag = 0;
static OTP_TYPE g_dq_otp_type = CMD_OTP_INVALID;
//static unsigned char 	g_dq_cmd_reply_data[DQ_OTP_CMD_NO_CRC_LENGTH/2];
static OTP_BASE_RET g_dq_cmd_repleys_status;
//static uint8_t 			g_dq_reset_index = 0;
static uint16_t g_dq_fp_index = 0;
static uint16_t g_dq_rfid_index = 0;
static uint32_t g_dq_add_temp_start_time = 0;
static uint32_t g_dq_add_temp_end_time = 0;

static uint8_t g_dq_link_error = 0;

#ifdef __WIN32_ENV_SUPPORT__
static otp_exchg_num temp_pwd_signed_data[10] =
	{
		0, {0x43, 0x16, 0x98, 0x25, 0x07}, 1, {0x26, 0x41, 0x53, 0x89, 0x70}, 2, {0x92, 0x35, 0x64, 0x70, 0x81}, 3, {0x72, 0x59, 0x03, 0x18, 0x46}, 4, {0x02, 0x85, 0x19, 0x63, 0x74}, 5, {0x12, 0x05, 0x69, 0x43, 0x87}, 6, {0x21, 0x69, 0x50, 0x37, 0x84}, 7, {0x10, 0x63, 0x52, 0x74, 0x89}, 8, {0x09, 0x58, 0x26, 0x17, 0x34}, 9, {0x18, 0x95, 0x42, 0x76, 0x30}};
#endif

const char dq_otp_sdk_ver[] = "sdk v1.0.0";

#define DQ_OTP_INVALID_INDEX 0xFFFF
#define DQ_OTP_PWD_NOT_USED 0x11
#define DQ_OTP_PWD_USED 0x10
//function
static OTP_BASE_RET dq_otp_syn_aes_16_key(unsigned char *p_data);
static void dq_otp_cmd_dataOperation(uint8_t *p_data, uint16_t data_len);
/*
parameter: 
	current status machine
return :
	none
*/
void dq_otp_reback_pwd_by_sec(OTP_TYPE type, unsigned char *old_pwd, unsigned char *new_pwd);

unsigned char dq_otp_get_sys_time_hour(unsigned short *sys_hour);

uint32_t dq_otp_get_lim_pwd_end_time(unsigned char *dec_pwd, unsigned char len);
extern unsigned short dq_otp_get_pwd_time_hour(unsigned char *in_pwd, unsigned char len, unsigned char offset);
static void dq_otp_find_log_head_end(void);
extern void dq_otp_send_log(void);
extern uint8_t dq_otp_check_and_update_log_time(uint32_t time);
#ifdef __WIN32_ENV_SUPPORT__
int main(void)
{
	//0104 5D134A30 FFFF FFFF FFFF FFFF FF 99
	//uint8_t test_temp_data[] = {0x01,0x04,0x5D,0x13,0x4A,0x30,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x99,0x3,0x81};
	//0105 00 4316982507 01 2641538970 FF 99
	//uint8_t test_temp_data[] = {0x01,0x05,0x00,0x43,0x16,0x98,0x25,0x07,0x01,0x26,0x41,0x53,0x89,0x70,0xFF,0x99,0x3,0x81};
	//0201 01 5C89B5B8 5CEA6BB0 7F FF FF FF 99
	//uint8_t test_temp_data[] = {0x02,0x01,0x04,0x5C,0x89,0xB5,0xB8,0x5C,0xEA,0x6B,0xB0,0x4F,0xFF,0xFF,0xFF,0x99,0x3,0x81};
	//0211 93 62 39 60 5F  87 68 62 74 FFFF FFFF 99
	//uint8_t test_temp_data[] = {0x02,0x11,0x93,0x62,0x39,0x60,0x5F,0x87,0x68,0x62,0x74,0xFF,0xFF,0xFF,0xFF,0x99,0x3,0x81};
	//0212 8866 7687 3F FFFF FFFF FFFF FFFF 99
	//uint8_t test_temp_data[] = {0x02,0x12,0x88,0x62,0x78,0x87,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x99,0x3,0x81};
	//0213 5C90 628C FFFF FFFF FFFF FFFF FF 99
	//uint8_t test_temp_data[] = {0x02,0x13,0x5C,0x90,0x62,0x8C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x99,0x3,0x81};
	//0221 5C9A0030 5D134A30 00 60 FFFF FF 99
	uint8_t test_temp_data[] = {0x02, 0x21, 0x5C, 0x9A, 0x00, 0x30, 0x5D, 0x13, 0x4A, 0x30, 0x43, 0x56, 0xFF, 0xFF, 0xFF, 0x99, 0x3, 0x81};
	dq_sdk_otp_init();
	dq_otp_dataCheck(test_temp_data, 18);
	return 1;
}
#endif

/*
parameter: 
	none
return :
	none
*/
void dq_sdk_ByteToHexStr(const uint8_t *source, char *dest, int sourceLen)
{
	short i;
	uint8_t highByte, lowByte;
	for (i = 0; i < sourceLen; i++)
	{
		highByte = source[i] >> 4;
		lowByte = source[i] & 0x0f;
		highByte += 0x30;
		if (highByte > 0x39)
			dest[i * 2] = highByte + 0x07;
		else
			dest[i * 2] = highByte;
		lowByte += 0x30;
		if (lowByte > 0x39)
			dest[i * 2 + 1] = lowByte + 0x07;
		else
			dest[i * 2 + 1] = lowByte;
	}
	return;
}
/*
parameter: 
	none
return :
	none
*/
void dq_sdk_IntToByteStr(unsigned int p_data, char *dest, int sourceLen)
{
	unsigned char i = 0;
	unsigned int temp_data = p_data;
	for (i = 0; i < sourceLen; i++)
	{
		dest[sourceLen - i - 1] = temp_data % 10;
		temp_data = temp_data / 10;
	}
	return;
}

/*
parameter: 
	none
return :
	none
*/
void dq_sdk_CharToHexByte(const uint8_t *source, char *dest, int sourceLen)
{
	short i;
	uint8_t highByte, lowByte;
	for (i = 0; i < sourceLen; i++)
	{
		highByte = source[i] >> 4;
		lowByte = source[i] & 0x0f;
		dest[i * 2] = highByte;
		if (lowByte != 0x0F)
			dest[i * 2 + 1] = lowByte;
	}
	return;
}

/*
parameter: 
	none
return :
	none
*/
unsigned int dq_sdk_CharToInt(const uint8_t *source, int sourceLen)
{
	short i;
	unsigned int int_ret = 0;
	for (i = 0; i < sourceLen; i++)
	{
		int_ret = int_ret * 10 + source[i];
	}
	return int_ret;
}
/*
parameter: 
	none
return :
	none
*/
void dq_sdk_HexStrToByte(const char *source, uint8_t *dest, int sourceLen)
{
	short i;
	uint8_t highByte, lowByte;
	for (i = 0; i < sourceLen; i += 2)
	{
		highByte = (uint8_t)toupper(source[i]);
		lowByte = (uint8_t)toupper(source[i + 1]);
		if (highByte > 0x39)
			highByte -= 0x37;
		else
			highByte -= 0x30;
		if (lowByte > 0x39)
			lowByte -= 0x37;
		else
			lowByte -= 0x30;
		dest[i / 2] = (highByte << 4) | lowByte;
	}
	return;
}
/*
parameter: 
	none
return :
	none
*/
void dq_sdk_HexCharToByte(const char *source, uint8_t *dest, int sourceLen)
{
	short i;
	uint8_t highByte, lowByte;
	for (i = 0; i < sourceLen; i += 2)
	{
		highByte = (uint8_t)toupper(source[i]);
		if ((sourceLen % 2 != 0) && (i == ((sourceLen / 2) * 2)))
			lowByte = 0x0F;
		else
			lowByte = (uint8_t)toupper(source[i + 1]);
		dest[i / 2] = (highByte << 4) | lowByte;
	}
	return;
}

/*
parameter: 
	none
return :
	none
*/
void dq_sdk_ByteCharToString(char *source, char *dest, int sourcelen)
{
	unsigned char i = 0, j = 0, k = 0;
	for (i = 0; i < sourcelen; i++)
	{
		if (source[i] != 0x00)
			break;
	}
	for (k = i; k < sourcelen; k++)
	{
		if (source[k] != 0x00)
		{
			dest[j++] = source[k];
		}
		else
			break;
	}
	return;
}

/*
parameter: 
	none
return :
	none
*/
// int dq_sdk_HexcharToInt(uint8_t *source, int length)
// {
// 	int int_data = 0;
// 	char dest[20];
// 	uint8_t ret = 0;
// 	//dest = malloc(length*2*sizeof(unsigned char));

// 	memset(dest, 0x00, sizeof(dest));
// 	dq_sdk_ByteToHexStr(source, dest, length);
// 	ret = sscanf((const char *)dest, "%x", &int_data);

// 	//free(dest);
// 	if (ret == 1)
// 		return int_data;
// 	else
// 		return 0;
// }
/*
parameter: 
	current status machine
return :
	none
*/
time_t dq_otp_get_pwd_time_sec(unsigned char year, unsigned char month, unsigned char day, unsigned char hour)
{
	time_t time_sec;
	struct tm tm_now;

	tm_now.tm_year = year;
	tm_now.tm_mon = month;
	tm_now.tm_mday = day;
	tm_now.tm_hour = hour;
	tm_now.tm_min = 0;
	tm_now.tm_sec = 0;

	// time_sec = mktime(&tm_now);
	return time_sec;
}
/*
parameter: 
	current status machine
return :
	none
*/
time_t dq_otp_get_sys_time_sec(void)
{
	time_t now;
	struct tm tm_now;
	g_dq_otp_init.get_lock_time_info(&tm_now);

	// now = mktime(&tm_now);
	return now;
}

/*
parameter: 
	current status machine
return :
	none
*/
// unsigned char dq_otp_get_pwd_time_ymd(unsigned int pwd_hour, unsigned char *year, unsigned char *month, unsigned char *day, unsigned char *hour)
// {
// 	time_t now = pwd_hour * 3600 + otp_set_info.otp_start_hour; //DQ_OTP_START_TIME_SEC;
// 	struct tm *tm_now;
// 	localtime(&now,tm_now);
// 	*year = tm_now->tm_year;
// 	*month = tm_now->tm_mon;
// 	*day = tm_now->tm_mday;
// 	*hour = tm_now->tm_hour;
// 	return 1;
// }

/*
parameter: 
	current status machine
return :
	none
*/
unsigned char dq_otp_get_sys_time_ymd(unsigned char *year, unsigned char *month, unsigned char *day, unsigned char *hour)
{
	struct tm tm_sys;
	g_dq_otp_init.get_lock_time_info(&tm_sys);
	*year = tm_sys.tm_year;
	*month = tm_sys.tm_mon;
	*day = tm_sys.tm_mday;
	*hour = tm_sys.tm_hour;
	return 1;
}

/*
parameter: 
	current status machine
return :
	none
*/
// unsigned char dq_otp_get_sys_local_time_week(void)
// {
// 	time_t now;
// 	struct tm tm_sys, *tm_local;
// 	g_dq_otp_init.get_lock_time_info(&tm_sys);
// 	now = mktime(&tm_sys);
// 	g_dq_otp_init.time_zone_pro(&now);
// 	tm_local = localtime(&now);
// 	return tm_local->tm_wday;
// }

/*
parameter: 
	none
return :
	none
*/
void dq_sdk_otp_init(void)
{
	uint8_t ret;
#ifndef __LOCK_USE_MALLOC__
	uint8_t i = 0;
#endif
	uint8_t admin_state = 0;

	g_dq_otp_init.fds_read = dq_otp_fds_read_cb;
	g_dq_otp_init.fds_write = dq_otp_fds_write_cb;
	g_dq_otp_init.data_send_bt = dq_otp_data_send_by_bt_cb;
	//g_dq_otp_init.data_send_nb = dq_otp_data_send_by_nb_cb;

	g_dq_otp_init.set_lock_fp = dq_otp_set_lock_fp_cb;
	g_dq_otp_init.set_lock_pwd = dq_otp_set_lock_pwd_cb;
	g_dq_otp_init.set_lock_rfid = dq_otp_set_lock_rfid_cb;

	g_dq_otp_init.stop_set_lock_fp = dq_otp_stop_set_lock_fp_cb;
	g_dq_otp_init.stop_set_lock_rfid = dq_otp_stop_set_lock_rfid_cb;
	g_dq_otp_init.stop_verify_admin_status = dq_otp_stop_verify_admin_status_cb;

	g_dq_otp_init.clear_lock_fp = dq_otp_clear_lock_fp_cb;
	g_dq_otp_init.clear_lock_pwd = dq_otp_clear_lock_pwd_cb;
	g_dq_otp_init.clear_lock_rf = dq_otp_clear_lock_rf_cb;

	//g_dq_otp_init.del_lock_fp = dq_otp_get_lock_fp_index_cb;
	//g_dq_otp_init.get_lock_pwd_index = dq_otp_get_lock_pwd_index_cb;
	g_dq_otp_init.del_lock_pwd = dq_otp_del_lock_pwd_cb;
	g_dq_otp_init.del_lock_fp = dq_otp_del_lock_fp_cb;
	g_dq_otp_init.del_lock_rf = dq_otp_del_lock_rf_cb;

	g_dq_otp_init.add_lock_rfid_by_app = dq_otp_add_lock_rfid_by_app_cb;
	//lock info
	g_dq_otp_init.get_lock_bat_level = dq_otp_get_lock_bat_info_cb;
	g_dq_otp_init.get_lock_ver_info = dq_otp_get_lock_ver_info_cb;
	g_dq_otp_init.set_lock_open_aud = dq_otp_set_lock_open_aud_cb;
	g_dq_otp_init.get_lock_mem_info = dq_otp_get_lock_mem_info_cb;
	g_dq_otp_init.get_admin_status = dq_otp_get_admin_status_cb;
	g_dq_otp_init.verify_admin_status = dq_otp_verify_admin_status_cb;
	g_dq_otp_init.set_system_time = dq_otp_set_system_time_cb;
	g_dq_otp_init.set_system_time_zone = dq_otp_set_system_time_zone_cb;
	g_dq_otp_init.get_lock_time_info = dq_otp_get_lock_time_info_cb;

	//init sucess
	g_dq_otp_init.init_sucess = dq_otp_set_init_sucess_cb;
	g_dq_otp_init.init_sucess_reset_lock = dq_otp_set_init_sucess_reset_lock_cb;

	g_dq_otp_init.open_lock = dq_otp_set_lock_open;

	g_dq_otp_init.check_admin_pwd = dq_otp_lock_check_admin_pwd;
	g_dq_otp_init.check_lock_state = dq_otp_lock_check_lock_state;
	g_dq_otp_init.input_pwd_cb = dq_otp_lock_input_pwd_cb;

	g_dq_otp_init.time_zone_pro = dq_otp_lock_time_zone_pro;

	g_dq_cmd_repleys_status = OTP_BASE_SUCESS;

	//setinfo
	g_dq_dfu_permit = 0;

	memset(&otp_set_info, 0xFF, sizeof(otp_set_info));
	//g_dq_otp_init.fds_read(DQ_OTP_FILE_ID_SET, (unsigned char *)&otp_set_info, sizeof(otp_set_info), &ret);
#ifdef __WIN32_ENV_SUPPORT__
	{
		unsigned char temp_exg_key_9[] = {0x93, 0x42, 0x67, 0x15, 0x8F};
		unsigned char temp_sec_key_9[] = {0x15, 0x02, 0x23, 0x28, 0x6F};
		unsigned char temp_exg_key_8[] = {0x43, 0x26, 0x71, 0x58};
		unsigned char temp_sec_key_8[] = {0x50, 0x72, 0x29, 0x98};
		memset(otp_set_info.otp_exg_key_8, 0x00, sizeof(otp_set_info.otp_exg_key_8));
		memset(otp_set_info.otp_sec_key_8, 0x00, sizeof(otp_set_info.otp_sec_key_8));
		memset(otp_set_info.otp_exg_key_9, 0x00, sizeof(otp_set_info.otp_exg_key_9));
		memset(otp_set_info.otp_sec_key_9, 0x00, sizeof(otp_set_info.otp_sec_key_9));
		memcpy(otp_set_info.otp_exg_key_8, temp_exg_key_8, 4);
		memcpy(otp_set_info.otp_sec_key_8, temp_sec_key_8, 4);
		memcpy(otp_set_info.otp_exg_key_9, temp_exg_key_9, 5);
		memcpy(otp_set_info.otp_sec_key_9, temp_sec_key_9, 5);
		for (i = 0; i < 10; i++)
		{
			otp_set_info.g_pwd_signed_data[i].num = i;
			memcpy(otp_set_info.g_pwd_signed_data[i].exchg_num, temp_pwd_signed_data[i].exchg_num, 5);
		}
	}
//NRF_LOG_PRINTF_DEBUG("dq_sdk_otp_init entry otp_set_info.init_suc_flag = %d \n",otp_set_info.init_suc_flag);
#endif
	//g_dq_otp_init.get_admin_status(&admin_state, &ret);
	if (otp_set_info.init_suc_flag == 1 && admin_state != 0)
	{
		// extern uint8_t wdt_flag;
		uint8_t wdt_flag;
		dq_dev_key_flag = 1;
		//LOG
		memset(&otp_lock_log, 0xFF, sizeof(otp_lock_log));
		//g_dq_otp_init.fds_read(DQ_OTP_FILE_ID_LOG, (uint8_t *)&otp_lock_log, sizeof(otp_lock_log), &ret);
		dq_otp_find_log_head_end();
		//init ekey
#ifndef __LOCK_USE_MALLOC__
		memset(&g_dq_app_pwd_info, 0xFF, sizeof(g_dq_app_pwd_info));
		//g_dq_otp_init.fds_read(DQ_OTP_FILE_ID_PWD_APP, (uint8_t *)&g_dq_app_pwd_info, sizeof(otp_base_app_pwd_info) * DQ_OTP_APP_PWD_NUM, &ret);

		memset(&g_dq_app_fp_info, 0xFF, sizeof(g_dq_app_fp_info));
		//g_dq_otp_init.fds_read(DQ_OTP_FILE_ID_FP, (uint8_t *)&g_dq_app_fp_info, sizeof(otp_base_app_fp_info) * DQ_OTP_APP_FP_NUM, &ret);

		memset(&g_dq_app_rfid_info, 0xFF, sizeof(g_dq_app_rfid_info));
		//g_dq_otp_init.fds_read(DQ_OTP_FILE_ID_RFID, (uint8_t *)&g_dq_app_rfid_info, sizeof(otp_base_app_rfid_info) * DQ_OTP_APP_RFID_NUM, &ret);
#endif
		g_dq_otp_init.init_sucess(1);

		if (wdt_flag == 1)
		{
			//NRF_LOG_PRINTF_DEBUG("###wdt_flag   1\n");
			dq_otp_add_alarm_log(DQ_ALART_LOG_ERROR_RESET);
		}
	}
	else
	{
		dq_dev_key_flag = 0;

		memset(&otp_set_info, 0xFF, sizeof(otp_set_info));
		//LOG
		memset(otp_lock_log, 0xFF, sizeof(otp_lock_log));
		otp_log_head = 0;
		otp_log_end = 0;
		//init ekey

#ifndef __LOCK_USE_MALLOC__
		for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
		{
			//g_dq_app_pwd_info[i].pwd_index = 0xFF;
			memset(g_dq_app_pwd_info[i].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].app_pwd));
			memset(g_dq_app_pwd_info[i].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].replace_pwd));
			g_dq_app_pwd_info[i].pwd_type = 0xFF;
			g_dq_app_pwd_info[i].pwd_flag = 0xFF;
			//g_dq_app_pwd_info[i].pwd_end_time = 0xFFFFFFFF;
		}

		for (i = 0; i < DQ_OTP_APP_FP_NUM; i++)
		{
			g_dq_app_fp_info[i].fp_start_time = 0xFFFFFFFF;
			g_dq_app_fp_info[i].fp_end_time = 0xFFFFFFFF;
		}

		for (i = 0; i < DQ_OTP_APP_RFID_NUM; i++)
		{
			g_dq_app_rfid_info[i].rfid_start_time = 0xFFFFFFFF;
			g_dq_app_rfid_info[i].rfid_end_time = 0xFFFFFFFF;
		}
#endif
		g_dq_otp_init.init_sucess(0);
	}
	return;
}
/*
parameter: 
	none
return :
	none
*/
OTP_TYPE dq_otp_get_cmd_type(unsigned char *p_data)
{
	unsigned char i = 0;

	for (i = 0; i < CMD_OTP_MAX_NUM; i++)
	{
		if ((cmd_info[i].cmd_h == p_data[0]) && (cmd_info[i].cmd_l == p_data[1]))
			break;
	}
	if (i < CMD_OTP_MAX_NUM)
		return cmd_info[i].cmd;
	else
		return CMD_OTP_INVALID;
}

/*
parameter: 
	none
return :
	none
*/
void dq_otp_setSerialId(uint8_t *p_data)
{
	//char serialData[4+1];
	//uint8_t bytes[2];

	//memset(serialData,0,4+1);
	//sprintf((char *)serialData,(const char *)"%04x",serialId);
	//dq_sdk_HexStrToByte(serialData,bytes,4);

	//data[14] = bytes[0];
	//data[15] = bytes[1];

	//data[14] = (serialId>>8)&0xFF;
	p_data[14] = 0xFF;
	p_data[15] = serialId & 0xFF;
	return;
}

/*
parameter: 
	none
return :
	none
*/
void dq_otp_getCRCBytes(uint8_t *in, uint8_t *out)
{
	unsigned short crc = CRC16_CCITT_FALSE(in, DQ_OTP_CMD_NO_CRC_LENGTH / 2);
	out[0] = (0xff00 & crc) >> 8;
	out[1] = 0xff & crc;
	return;
}

/*
parameter: 
	none
return :
	none
*/
void dq_otp_getCmdEncryptData(uint8_t *in, uint8_t *out)
{
	//if (dq_dev_key_flag == 1)
		//DQ_AES128_ECB_encrypt(in, otp_set_info.otp_dev_key, out);
	//else
		//DQ_AES128_ECB_encrypt(in, DQ_OTP_AES_GBL_KEY, out);

	return;
}

/*
parameter: 
	none
return :
	none
*/
void dq_otp_getCmdDecryptData(uint8_t *in, uint8_t *out)
{
	static uint8_t encrypt[DQ_OTP_CMD_NO_CRC_LENGTH / 2 + 1];
	memset(encrypt, 0, DQ_OTP_CMD_NO_CRC_LENGTH / 2 + 1);
	memcpy(encrypt, in, DQ_OTP_CMD_NO_CRC_LENGTH / 2);
	//if (dq_dev_key_flag == 1)
		//DQ_AES128_ECB_decrypt(encrypt, otp_set_info.otp_dev_key, out);
	//else
		//DQ_AES128_ECB_decrypt(encrypt, DQ_OTP_AES_GBL_KEY, out);
	return;
}

/*
parameter: 
	none
return :
	none
*/
void dq_otp_notify(uint8_t *p_data)
{
	uint8_t crc_data[2];
	uint8_t encrypt_data[DQ_OTP_CMD_NO_CRC_LENGTH / 2 + 1];
	uint8_t notify_data[DQ_OTP_CMD_NO_CRC_LENGTH / 2 + DQ_OTP_CMD_NO_CRC_LENGTH / 2 + 1];
	uint32_t ret = 0;

	memset(encrypt_data, 0, DQ_OTP_CMD_NO_CRC_LENGTH / 2 + 1);
	memset(notify_data, 0, DQ_OTP_CMD_NO_CRC_LENGTH / 2 + DQ_OTP_CMD_NO_CRC_LENGTH / 2 + 1);

	dq_otp_getCmdEncryptData(p_data, encrypt_data);
	dq_otp_getCRCBytes(p_data, crc_data);

	memcpy(notify_data, encrypt_data, DQ_OTP_CMD_NO_CRC_LENGTH / 2);
	notify_data[16] = crc_data[0];
	notify_data[17] = crc_data[1];

	//g_dq_otp_init.data_send_bt(notify_data, DQ_OTP_CMD_NO_CRC_LENGTH / 2 + 2, &ret);

	serialId++;
	return;
}

/*
parameter: 
	none
return :
	none
*/
void dq_otp_device_key_unknow(uint8_t instruct_h, uint8_t instruct_l, uint8_t result_code)
{
	uint8_t notify_data[DQ_OTP_CMD_NO_CRC_LENGTH / 2];
	unsigned char i = 0;
	uint32_t ret = 0;

	memset(notify_data, 0x00, DQ_OTP_CMD_NO_CRC_LENGTH / 2);
	//instruct
	notify_data[0] = instruct_h;
	notify_data[1] = instruct_l;
	//result
	notify_data[2] = result_code;
	//padding
	notify_data[3] = 0xFF;
#ifndef __WIN32_ENV_SUPPORT__
	for (i = 0; i < 16; i++)
	{
		//NRF_LOG_PRINTF_DEBUG("dq_otp_device_key_unknow notify_data[%d] = 0x%x \n", i, notify_data[i]);
	}
#endif
	//g_dq_otp_init.data_send_bt(notify_data, 4, &ret);
	return;
}

/*
parameter: 
	none
return :
	none
*/
void dq_otp_common_reply(uint8_t instruct_h, uint8_t instruct_l, uint8_t result_code)
{
	uint8_t notify_data[DQ_OTP_CMD_NO_CRC_LENGTH / 2];
	unsigned char i = 0;
	memset(notify_data, 0, DQ_OTP_CMD_NO_CRC_LENGTH / 2);
	//instruct
	notify_data[0] = instruct_h;
	notify_data[1] = instruct_l;
	//result
	notify_data[2] = result_code;
	//padding
	notify_data[3] = 0xFF;
	notify_data[4] = 0xFF;
	notify_data[5] = 0xFF;
	notify_data[6] = 0xFF;
	notify_data[7] = 0xFF;
	notify_data[8] = 0xFF;
	notify_data[9] = 0xFF;
	notify_data[10] = 0xFF;
	notify_data[11] = 0xFF;
	notify_data[12] = 0xFF;
	notify_data[13] = 0xFF;

	//serialId
	dq_otp_setSerialId(notify_data);
	for (i = 0; i < 16; i++)
	{
		//NRF_LOG_PRINTF_DEBUG("dq_otp_common_reply notify_data[%d] = 0x%x \n", i, notify_data[i]);
	}
	dq_otp_notify(notify_data);
	return;
}

/*
parameter: 
	none
return :
	none
*/
void dq_otp_send_data_reply(uint8_t instruct_h, uint8_t instruct_l, uint8_t result_code, uint8_t *p_data, uint8_t data_len)
{
	uint8_t notify_data[DQ_OTP_CMD_NO_CRC_LENGTH / 2];
	uint8_t i = 0;
	memset(notify_data, 0, DQ_OTP_CMD_NO_CRC_LENGTH / 2);
	//instruct
	notify_data[0] = instruct_h;
	notify_data[1] = instruct_l;
	//result
	notify_data[2] = result_code;
	//padding
	if (data_len > 11)
		return;
	for (i = 0; i < data_len; i++)
	{
		notify_data[3 + i] = *p_data++;
	}
	for (i = data_len; i < 11; i++)
	{
		notify_data[3 + i] = 0xFF;
	}
	dq_otp_setSerialId(notify_data);

	for (i = 0; i < 11; i++)
	{
		//NRF_LOG_PRINTF_DEBUG("dq_otp_send_data_reply notify_data[%d] = 0x%x \n", i, notify_data[i]);
	}
	//serialId
	dq_otp_notify(notify_data);
	return;
}
/*
parameter: 
	none
return :
	none
*/
void dq_otp_send_data_no_result_reply(uint8_t instruct_h, uint8_t instruct_l, uint8_t *p_data, uint8_t data_len)
{
	uint8_t notify_data[DQ_OTP_CMD_NO_CRC_LENGTH / 2];
	uint8_t i = 0;
	memset(notify_data, 0, DQ_OTP_CMD_NO_CRC_LENGTH / 2);
	//instruct
	notify_data[0] = instruct_h;
	notify_data[1] = instruct_l;
	//padding
	if (data_len > 12)
		return;
	for (i = 0; i < data_len; i++)
	{
		notify_data[2 + i] = *p_data++;
	}
	for (i = data_len; i < 12; i++)
	{
		notify_data[2 + i] = 0xFF;
	}
	//serialId
	dq_otp_setSerialId(notify_data);
	dq_otp_notify(notify_data);
	return;
}

//Abnormal disconnection
void dq_otp_disconnect_pro(void)
{
	uint8_t flag = 0;
	uint8_t result;
	if (otp_set_info.init_suc_flag == 0xFF)
	{
		dq_dev_key_flag = 0;
		memset(&otp_set_info, 0xFF, sizeof(otp_set_info));
	}

	flag = dq_otp_app_get_operate_lock_flag();
	if (flag == 1)
	{
		switch (g_dq_otp_type)
		{
		case CMD_OTP_VERY_ADMIN_STA:
			g_dq_otp_init.stop_verify_admin_status(&result);
			break;
		case CMD_OTP_ADD_FP:
			g_dq_link_error = 1;
			g_dq_otp_init.stop_set_lock_fp(&result);
			break;
		case CMD_OTP_ADD_RFID:
			g_dq_link_error = 1;
			g_dq_otp_init.stop_set_lock_rfid(&result);
			break;
		}
	}

	//if(g_dq_dfu_permit == 2)
	//	g_dq_dfu_permit = 0;
}

uint8_t dq_otp_get_link_state(void)
{
	return g_dq_link_error;
}

void dq_otp_lock_timeout(void)
{
	uint8_t flag = 0;

	flag = dq_otp_app_get_operate_lock_flag();
	if (flag == 1)
	{
		switch (g_dq_otp_type)
		{
		case CMD_OTP_VERY_ADMIN_STA:
			dq_otp_app_set_operate_lock_flag(0);
			dq_otp_check_lock_admin_pwd_fp_ret(1);
			break;
#if defined(__LOCK_FP_SUPPORT__) || defined(__LOCK_FP_SUPPORT2__) || defined(__LOCK_FP_SUPPORT1_2__)
		case CMD_OTP_ADD_FP:
			dq_otp_app_add_lock_fp_result(1, 0);
			break;
#endif
#ifdef __LOCK_RFID_CARD_SUPPORT__
		case CMD_OTP_ADD_RFID:
			dq_otp_app_add_lock_rfid_result(1, 0);
			break;
#endif
		}
	}
}

/*
parameter: 
	none
return :
	none
*/
void dq_otp_dataCheck(uint8_t *p_data, uint16_t length)
{
#ifndef __WIN32_ENV_SUPPORT__
	uint8_t decrypt_data[DQ_OTP_CMD_NO_CRC_LENGTH / 2 + 1];
	uint8_t crc_data[2];
	bool isPassCRC = false;

//	NRF_LOG_PRINTF_DEBUG("dq_otp_dataCheck length\n");

	memset(decrypt_data, 0, DQ_OTP_CMD_NO_CRC_LENGTH / 2 + 1);
	dq_otp_getCmdDecryptData(p_data, decrypt_data);
	dq_otp_getCRCBytes(decrypt_data, crc_data);
	if (crc_data[0] == p_data[length - 2] && crc_data[1] == p_data[length - 1])
	{
		isPassCRC = true;
	}
	else
	{
		isPassCRC = false;
	}
	if (isPassCRC)
	{
		dq_otp_cmd_dataOperation(decrypt_data, length);
	}
	else
	{
		uint8_t p_data[] = "efef0001";
		uint32_t ret = 0;
//		g_dq_otp_init.data_send_bt(p_data, 8, &ret);
	}
#else
	dq_otp_cmd_dataOperation(p_data, length);
#endif
	return;
}

/*
parameter: 
	none
return :
	none
*/
//0101 84757AEBE3987D3E 00 FFFF FFFF 99
static OTP_BASE_RET dq_otp_syn_aes_16_key(unsigned char *p_data)
{
	unsigned char i = 0;
	unsigned char c_aes_key[8];
	int end_flag = p_data[10];

	memset(c_aes_key, 0x00, sizeof(c_aes_key));
	for (i = 2; i < 10; i++)
	{
		c_aes_key[i - 2] = p_data[i];
#ifndef __WIN32_ENV_SUPPORT__
//		NRF_LOG_PRINTF_DEBUG("dq_otp_syn_aes_16_key c_aes_key[%d] = 0x%x \n", i - 2, c_aes_key[i - 2]);
#endif
	}
	if (end_flag == 0)
	{
		memset(otp_set_info.otp_dev_key, 0x00, sizeof(otp_set_info.otp_dev_key));
		memcpy(otp_set_info.otp_dev_key, c_aes_key, sizeof(c_aes_key));
	}
	else if (end_flag == 1)
	{
		memcpy(otp_set_info.otp_dev_key + 8, (const char *)c_aes_key, sizeof(c_aes_key));
		dq_dev_key_flag = 1;
	}
	return OTP_BASE_SUCESS;
}

/*
parameter: 
	none
return :
	none
*/
//0102 73638150 35817426 FFFF FFFF FF 99
static OTP_BASE_RET dq_otp_syn_sec_8_key(unsigned char *p_data)
{
	uint8_t i = 0;
	memset(otp_set_info.otp_sec_key_8, 0xFF, sizeof(otp_set_info.otp_sec_key_8));
	memset(otp_set_info.otp_exg_key_8, 0xFF, sizeof(otp_set_info.otp_exg_key_8));
	for (i = 0; i < 4; i++)
	{
		otp_set_info.otp_sec_key_8[i] = p_data[i + 2];
	}
	for (i = 0; i < 4; i++)
	{
		otp_set_info.otp_exg_key_8[i] = p_data[i + 6];
	}
	return OTP_BASE_SUCESS;
}
/*
parameter: 
	none
return :
	none
*/
//0103 736381504F 358174265F FFFF FF 99
static OTP_BASE_RET dq_otp_syn_sec_9_key(unsigned char *p_data)
{
	uint8_t i = 0;
	memset(otp_set_info.otp_sec_key_9, 0xFF, sizeof(otp_set_info.otp_sec_key_9));
	memset(otp_set_info.otp_exg_key_9, 0xFF, sizeof(otp_set_info.otp_exg_key_9));
	for (i = 0; i < 5; i++)
	{
		otp_set_info.otp_sec_key_9[i] = p_data[i + 2];
	}
	otp_set_info.otp_sec_key_9[4] |= 0x0F;

	for (i = 0; i < 5; i++)
	{
		otp_set_info.otp_exg_key_9[i] = (p_data[i + 6] << 4) + ((p_data[i + 7] >> 4) & 0x0F);
	}

	return OTP_BASE_SUCESS;
}
/*
parameter: 
	none
return :
	none
*/
//0104 5D134A30 FFFF FFFF FFFF FFFF FF 99
static OTP_BASE_RET dq_otp_syn_start_time(unsigned char *p_data)
{
	uint8_t i = 0;
	unsigned char start_time[5];

	memset(start_time, 0x00, sizeof(start_time));
	for (i = 0; i < 5; i++)
	{
		start_time[i] = p_data[i + 2];
	}
	if (otp_set_info.otp_start_hour == 0xFFFFFFFF)
	{
		otp_set_info.otp_start_hour = dq_sdk_HexcharToInt(start_time, 4);
		return OTP_BASE_SUCESS;
	}
	else
	{
		uint8_t ret;
		otp_set_info.otp_start_hour = dq_sdk_HexcharToInt(start_time, 4);
		g_dq_cmd_repleys_status = OTP_BASE_SUCESS;
//		g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_SET, (unsigned char *)&otp_set_info, sizeof(otp_base_setting_info), &ret, dq_otp_fds_write_common_cb);
		return OTP_BASE_BUSY;
	}
}

/*
parameter: 
	none
return :
	none
*/
//0105 00 4316982507 01 2641538970 FF 99
static OTP_BASE_RET dq_otp_syn_exchange_num(unsigned char *p_data)
{
	uint8_t i = 0, j = 0;
	uint8_t num = 0;
	unsigned char exg_num[5];
	OTP_BASE_RET ret_val = OTP_BASE_SUCESS;

	for (j = 0; j < 2; j++)
	{
		num = p_data[2 + 6 * j];
		if (num > 9)
		{
			ret_val = OTP_BASE_FAIL;
			break;
		}
		otp_set_info.g_pwd_signed_data[num].num = num;
		memset(exg_num, 0x00, sizeof(exg_num));
		for (i = 0; i < 5; i++)
		{
			otp_set_info.g_pwd_signed_data[num].exchg_num[i] = p_data[3 + i + j * 6];
		}
	}
	return ret_val;
}
/*
parameter: 
	none
return :
	none
*/
//0404 0060 FFFF FFFF FFFF FFFF FFFF 8899
static void dq_otp_lock_init_complete(uint8_t *p_data)
{
	uint8_t ret = 0;

	otp_set_info.init_suc_flag = 1;
	g_dq_cmd_repleys_status = OTP_BASE_SUCESS;
//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_SET, (unsigned char *)&otp_set_info, sizeof(otp_base_setting_info), &ret, dq_otp_fds_write_common_cb);
	return;
}
#if 0
/*
parameter: 
	none
return :
	none
*/
static uint8_t dq_otp_check_ekey_week(uint8_t ekey_week)
{
	unsigned char sys_week;
	unsigned short bit_week = 1;
	uint8_t week_off;
	sys_week = dq_otp_get_sys_time_week();
	week_off = bit_week << sys_week;
	if((week_off&ekey_week) > 0)
		return 1;
	else
		return 0;
}
/*
parameter: 
	none
return :
	none
*/
static uint8_t dq_otp_check_per_ekey_verify(time_t start_time,time_t end_time,time_t sys_time)
{
	if((sys_time >= start_time)&&(sys_time <= end_time))
		return 1;
	else
		return 0;
}
/*
parameter: 
	none
return :
	none
*/
static uint8_t dq_otp_check_sin_ekey_verify(time_t start_time,time_t end_time,time_t sys_time)
{
	if((sys_time >= start_time)&&(sys_time <= end_time))
		return 1;
	else
		return 0;
}

/*
parameter: 
	none
return :
	none
*/
static uint8_t dq_otp_check_lim_ekey_verify(time_t start_time,time_t end_time,time_t sys_time)
{
	if((sys_time >= start_time)&&(sys_time <= end_time))
		return 1;
	else
		return 0;
}
/*
parameter: 
	none
return :
	none
*/
static uint8_t dq_otp_check_loop_ekey_verify(time_t start_time,time_t end_time,time_t sys_time,uint8_t week_info)
{	
	uint8_t ret_val = 0;
	uint32_t start_hour = start_time%(3600*24);
	uint32_t end_hour = end_time%(3600*24);
	uint32_t sys_hour = sys_time%(3600*24);

	ret_val = dq_otp_check_ekey_week(week_info);
	if(ret_val == 0)
		return 0;
	if((sys_time >= start_time)&&(sys_time <= end_time))
	{
		if((sys_hour >= start_hour)&&(sys_hour <= end_hour))
			return 1;
	}
	return 0;
}
#endif

/*
parameter: 
	none
return :
	none
*/
//0201 01 5C89B5B8 5CEA6BB0 7F FF FF FF 99
static OTP_BASE_RET dq_otp_check_ekey_verify(uint8_t *p_data)
{
#if 1
	uint8_t check_byte[2];
	uint16_t check_sum = 0;
	uint8_t i;
	if (dq_otp_get_lock_state())
		return OTP_BASE_FAIL;

	for (i = 0; i <= 10; i++)
	{
		check_sum += p_data[i];
	}

	check_byte[0] = (0xff00 & check_sum) >> 8;
	check_byte[1] = 0xff & check_sum;

	if (check_byte[0] == p_data[12] && check_byte[1] == p_data[11])
		return OTP_BASE_SUCESS;
	else
		return OTP_BASE_FAIL;

#else
	uint8_t i = 0;
	uint8_t ekey_time[5];
	uint32_t ekey_start_time = 0;
	uint32_t ekey_end_time = 0;
	time_t sys_time_sec;
	uint8_t week_info = pdata[11];
	uint8_t ekey_type = pdata[2];
	uint8_t ret_val = 0;

	memset(ekey_time, 0x00, sizeof(ekey_time));
	for (i = 0; i < 4; i++)
	{
		ekey_time[i] = pdata[i + 3];
	}
	ekey_start_time = dq_sdk_HexcharToInt(ekey_time, 4);

	memset(ekey_time, 0x00, sizeof(ekey_time));
	for (i = 0; i < 4; i++)
	{
		ekey_time[i] = pdata[i + 7];
	}
	ekey_end_time = dq_sdk_HexcharToInt(ekey_time, 4);

	sys_time_sec = dq_otp_get_sys_time_sec();

	if (ekey_type == 0x01) //per ekey
	{
		ret_val = dq_otp_check_per_ekey_verify(ekey_start_time, ekey_end_time, sys_time_sec);
	}
	else if (ekey_type == 0x02) //sin ekey
	{
		ret_val = dq_otp_check_sin_ekey_verify(ekey_start_time, ekey_end_time, sys_time_sec);
	}
	else if (ekey_type == 0x03) //lim ekey
	{
		ret_val = dq_otp_check_lim_ekey_verify(ekey_start_time, ekey_end_time, sys_time_sec);
	}
	else if (ekey_type == 0x04) //loop ekey
	{
		ret_val = dq_otp_check_loop_ekey_verify(ekey_start_time, ekey_end_time, sys_time_sec, week_info);
	}
	if (ret_val == 1)
		return OTP_BASE_SUCESS;
	else
		return OTP_BASE_FAIL;

#endif
}

/*
parameter: 
	none
return :
	none
*/
static uint8_t dq_otp_get_pwd_type(uint8_t *pwd)
{
	if (pwd[0] <= 5)
	{
		return OTP_PWD_TYPE_LIM;
	}
	else if (pwd[0] == 6)
	{
		return OTP_PWD_TYPE_LOOP;
	}
	else if (pwd[0] == 7)
	{
		return OTP_PWD_TYPE_CLR;
	}
	else if (pwd[0] == 8)
	{
		return OTP_PWD_TYPE_SIN;
	}
	else if (pwd[0] == 9)
	{
		return OTP_PWD_TYPE_PER;
	}
	return OTP_PWD_TYPE_INVALID;
}

/*
parameter: 
	none
return :
	none
*/
static uint8_t dq_otp_get_pwd_len(uint8_t *pwd, uint8_t len)
{
	uint8_t i = 0;
	uint8_t pwd_l = 0;
	uint8_t pwd_h = 0;
	uint8_t pwd_len = 0;

	for (i = 0; i < len; i++)
	{
		pwd_h = (pwd[i] & 0xF0) >> 4;
		if (pwd_h == 0x0F)
			break;
		else
			pwd_len++;
		pwd_l = (pwd[i] & 0x0F);
		if (pwd_l == 0x0F)
			break;
		else
			pwd_len++;
	}
	return pwd_len;
}
/*
parameter: 
	none
return :
	none
*/
static uint8_t dq_otp_get_empty_pwd_index(void)
{
	uint8_t i = 0;
	unsigned short sys_hour = 0;

	dq_otp_get_sys_time_hour(&sys_hour);

	for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		switch (g_dq_app_pwd_info[i].pwd_type)
		{
		case OTP_PWD_TYPE_PER:
			if (g_dq_app_pwd_info[i].pwd_flag == 2)
			{
				unsigned char dec_pwd_char[15];
				unsigned short pwd_hour;
				unsigned char len = dq_otp_get_pwd_len(g_dq_app_pwd_info[i].app_pwd, 5);
				memset(dec_pwd_char, 0x00, sizeof(dec_pwd_char));
				dq_otp_enc_pwd(g_dq_app_pwd_info[i].app_pwd, len, otp_set_info.otp_sec_key_8, otp_set_info.otp_exg_key_8, dec_pwd_char);
				pwd_hour = dq_otp_get_pwd_time_hour(dec_pwd_char, 5, 1);
				if (sys_hour >= (pwd_hour + 24))
				{
					memset(g_dq_app_pwd_info[i].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].app_pwd));
					memset(g_dq_app_pwd_info[i].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].replace_pwd));
					g_dq_app_pwd_info[i].pwd_flag = 0xFF;
					g_dq_app_pwd_info[i].pwd_type = 0xFF;
				}
			}
			break;
		case OTP_PWD_TYPE_SIN:
		{
			unsigned char dec_pwd_char[15];
			unsigned short pwd_hour;
			unsigned char len = dq_otp_get_pwd_len(g_dq_app_pwd_info[i].app_pwd, 5);
			memset(dec_pwd_char, 0x00, sizeof(dec_pwd_char));
			dq_otp_enc_pwd(g_dq_app_pwd_info[i].app_pwd, len, otp_set_info.otp_sec_key_8, otp_set_info.otp_exg_key_8, dec_pwd_char);
			pwd_hour = dq_otp_get_pwd_time_hour(dec_pwd_char, 5, 1);

			if (pwd_hour + 6 <= sys_hour)
			{
				memset(g_dq_app_pwd_info[i].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].app_pwd));
				memset(g_dq_app_pwd_info[i].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].replace_pwd));
				g_dq_app_pwd_info[i].pwd_flag = 0xFF;
				g_dq_app_pwd_info[i].pwd_type = 0xFF;
			}
		}
		break;
		case OTP_PWD_TYPE_LIM:
		{
			unsigned char dec_pwd_char[15];
			unsigned short pwd_hour;
			unsigned short end_time;
			unsigned char len = dq_otp_get_pwd_len(g_dq_app_pwd_info[i].app_pwd, 5);
			memset(dec_pwd_char, 0x00, sizeof(dec_pwd_char));
			if (len == 8)
				dq_otp_enc_pwd(g_dq_app_pwd_info[i].app_pwd, len, otp_set_info.otp_sec_key_8, otp_set_info.otp_exg_key_8, dec_pwd_char);
			else if (len == 9)
				dq_otp_enc_pwd(g_dq_app_pwd_info[i].app_pwd, len, otp_set_info.otp_sec_key_9, otp_set_info.otp_exg_key_9, dec_pwd_char);
			pwd_hour = dq_otp_get_pwd_time_hour(dec_pwd_char, 5, 0);

			end_time = dq_otp_get_lim_pwd_end_time(dec_pwd_char, len) / 3600;
			if ((g_dq_app_pwd_info[i].pwd_flag == 2 && pwd_hour + 24 <= sys_hour) || (end_time < sys_hour))
			{
				memset(g_dq_app_pwd_info[i].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].app_pwd));
				memset(g_dq_app_pwd_info[i].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].replace_pwd));
				g_dq_app_pwd_info[i].pwd_flag = 0xFF;
				g_dq_app_pwd_info[i].pwd_type = 0xFF;
			}
		}
		break;
		case OTP_PWD_TYPE_LOOP:
			if (g_dq_app_pwd_info[i].pwd_flag == 2)
			{
				unsigned char dec_pwd_char[15];
				unsigned short pwd_hour;
				unsigned char len = dq_otp_get_pwd_len(g_dq_app_pwd_info[i].app_pwd, 5);
				memset(dec_pwd_char, 0x00, sizeof(dec_pwd_char));
				dq_otp_enc_pwd(g_dq_app_pwd_info[i].app_pwd, len, otp_set_info.otp_sec_key_9, otp_set_info.otp_exg_key_9, dec_pwd_char);
				pwd_hour = dq_otp_get_pwd_time_hour(dec_pwd_char, 5, 1);

				if (pwd_hour + 24 <= sys_hour)
				{
					memset(g_dq_app_pwd_info[i].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].app_pwd));
					memset(g_dq_app_pwd_info[i].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].replace_pwd));
					g_dq_app_pwd_info[i].pwd_flag = 0xFF;
					g_dq_app_pwd_info[i].pwd_type = 0xFF;
				}
			}
			break;
		}
	}

	for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		if (g_dq_app_pwd_info[i].pwd_type == 0xFF)
			break;
	}
	return i;
}

/*
parameter: 
	none
return :
	none
*/
//0211 21 83 13 18 5F  87 68 62 74 FFFF FFFF 99
static OTP_BASE_RET dq_otp_add_user_def_pwd(uint8_t *p_data)
{
	uint8_t i = 0;
	uint8_t pwd_index;
	uint8_t pwd_info[5];
	uint8_t pwd_len;
	uint8_t pwd_char[10];
	uint8_t ret = 0;

	if ((g_dq_otp_init.check_admin_pwd(&p_data[i + 7])) != 0xFF)
	{
		return OTP_BASE_EXIST;
	}
	if (dq_otp_check_replace_password(&p_data[i + 7]) == 1)
		return OTP_BASE_EXIST;

	for (i = 0; i < 5; i++)
	{
		pwd_info[i] = p_data[i + 2];
	}

#ifdef __LOCK_USE_MALLOC__
	g_dq_app_pwd_info = (otp_base_app_pwd_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_PWD, &ret);
#endif
	for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		if (memcmp(pwd_info, g_dq_app_pwd_info[i].app_pwd, 5) == 0)
			break;
	}
	if (i < DQ_OTP_APP_PWD_NUM)
	{
		if (g_dq_app_pwd_info[i].pwd_flag == 2)
			return OTP_BASE_FAIL;

		pwd_index = i;
		for (i = 0; i < 4; i++)
		{
			g_dq_app_pwd_info[pwd_index].replace_pwd[i] = p_data[i + 7];
		}
		g_dq_app_pwd_info[pwd_index].replace_pwd[4] = 0xFF;
	}
	else
	{
		pwd_index = dq_otp_get_empty_pwd_index();

		if (pwd_index == DQ_OTP_APP_PWD_NUM)
			return OTP_BASE_NO_MEM;

		//g_dq_app_pwd_info[pwd_index].pwd_index = pwd_index;

		//memset(pwd_info,0xFF,sizeof(pwd_info));
		for (i = 0; i < 5; i++)
		{
			g_dq_app_pwd_info[pwd_index].app_pwd[i] = p_data[i + 2];
		}
		//memset(pwd_info,0xFF,sizeof(pwd_info));
		for (i = 0; i < 4; i++)
		{
			g_dq_app_pwd_info[pwd_index].replace_pwd[i] = p_data[i + 7];
		}
		g_dq_app_pwd_info[pwd_index].replace_pwd[4] = 0xFF;
		memset(pwd_char, 0xFF, sizeof(pwd_char));
		pwd_len = dq_otp_get_pwd_len(g_dq_app_pwd_info[pwd_index].app_pwd, 5);
		if (pwd_len == 8)
			dq_otp_enc_pwd(g_dq_app_pwd_info[pwd_index].app_pwd, pwd_len, otp_set_info.otp_sec_key_8, otp_set_info.otp_exg_key_8, pwd_char);
		else if (pwd_len == 9)
			dq_otp_enc_pwd(g_dq_app_pwd_info[pwd_index].app_pwd, pwd_len, otp_set_info.otp_sec_key_9, otp_set_info.otp_exg_key_9, pwd_char);
		g_dq_app_pwd_info[pwd_index].pwd_type = dq_otp_get_pwd_type(pwd_char);
		if (g_dq_app_pwd_info[pwd_index].pwd_type == OTP_PWD_TYPE_PER)
		{
			unsigned short pwd_hour;
			unsigned short sys_hour = 0;

			dq_otp_get_sys_time_hour(&sys_hour);
			pwd_hour = dq_otp_get_pwd_time_hour(pwd_char, 5, 1);
			if (sys_hour >= (pwd_hour + 24))
			{
				memset(g_dq_app_pwd_info[pwd_index].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[pwd_index].app_pwd));
				memset(g_dq_app_pwd_info[pwd_index].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[pwd_index].replace_pwd));
				g_dq_app_pwd_info[pwd_index].pwd_flag = 0xFF;
				g_dq_app_pwd_info[pwd_index].pwd_type = 0xFF;
				return OTP_BASE_FAIL;
			}
		}
		else if (g_dq_app_pwd_info[pwd_index].pwd_type == OTP_PWD_TYPE_LIM)
		{
			unsigned short pwd_hour;
			unsigned short sys_hour = 0;
			unsigned short end_time;
			dq_otp_get_sys_time_hour(&sys_hour);
			pwd_hour = dq_otp_get_pwd_time_hour(pwd_char, 5, 0);

			end_time = dq_otp_get_lim_pwd_end_time(pwd_char, pwd_len) / 3600;
			if ((pwd_hour + 24 <= sys_hour) || (end_time < sys_hour))
			{
				memset(g_dq_app_pwd_info[pwd_index].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[pwd_index].app_pwd));
				memset(g_dq_app_pwd_info[pwd_index].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[pwd_index].replace_pwd));
				g_dq_app_pwd_info[pwd_index].pwd_flag = 0xFF;
				g_dq_app_pwd_info[pwd_index].pwd_type = 0xFF;
				return OTP_BASE_FAIL;
			}
		}

		g_dq_app_pwd_info[pwd_index].pwd_flag = 1;
		//if(g_dq_app_pwd_info[pwd_index].pwd_type == OTP_PWD_TYPE_LIM)
		//	g_dq_app_pwd_info[pwd_index].pwd_end_time=dq_otp_get_lim_pwd_end_time(pwd_char, pwd_len);
		//if(g_dq_app_pwd_info[pwd_index].pwd_type == OTP_PWD_TYPE_CLR)
		//	g_dq_app_pwd_info[pwd_index].pwd_used_flag = 1;//��ֹupdate��ɾ��һ�����
	}
	//save pwd info
	g_dq_cmd_repleys_status = OTP_BASE_SUCESS;
//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_PWD_APP, (unsigned char *)g_dq_app_pwd_info, sizeof(otp_base_app_pwd_info) * DQ_OTP_APP_PWD_NUM, &ret, dq_otp_fds_write_common_cb);
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
	return OTP_BASE_SUCESS;
}

/*
parameter: 
	none
return :
	none
*/
//0212 8866 7687 3F FFFF FFFF FFFF FFFF 99
static OTP_BASE_RET dq_otp_del_lock_pwd(uint8_t *p_data)
{
	uint8_t i = 0;
	uint8_t lock_pwd[5];
	uint8_t ret = 0;
#ifdef __LOCK_USE_MALLOC__
	g_dq_app_pwd_info = (otp_base_app_pwd_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_PWD, &ret);
#endif
	memset(lock_pwd, 0xFF, sizeof(lock_pwd));
	for (i = 0; i < 5; i++)
	{
		lock_pwd[i] = p_data[i + 2];
	}
	for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		if (!memcmp((const char *)g_dq_app_pwd_info[i].app_pwd, (const char *)lock_pwd, 5))
			break;
		if (!memcmp((const char *)g_dq_app_pwd_info[i].replace_pwd, (const char *)lock_pwd, 5))
			break;
	}
	if (i < DQ_OTP_APP_PWD_NUM)
	{
		memset(g_dq_app_pwd_info[i].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].app_pwd));
		memset(g_dq_app_pwd_info[i].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].replace_pwd));
		g_dq_app_pwd_info[i].pwd_flag = 0xFF;
		g_dq_app_pwd_info[i].pwd_type = 0xFF;
		//save

		g_dq_cmd_repleys_status = OTP_BASE_SUCESS;
//		g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_PWD_APP, (unsigned char *)g_dq_app_pwd_info, sizeof(otp_base_app_pwd_info) * DQ_OTP_APP_PWD_NUM, &ret, dq_otp_fds_write_common_cb);
#ifdef __LOCK_USE_MALLOC__
		mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif

		return OTP_BASE_SUCESS;
	}
	else
	{
#ifdef __LOCK_USE_MALLOC__
		mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
		//return OTP_BASE_NOT_FOUND;
		//未找到默认为已删除
		return OTP_BASE_SUCESS;
	}
}
/*
parameter: 
	none
return :
	none
*/
//0213 5C90 628C FFFF FFFF FFFF FFFF FF 99
static OTP_BASE_RET dq_otp_clr_lock_pwd(uint8_t *p_data)
{
	uint8_t i = 0;
	//uint8_t sys_time[4];
	//uint32_t sys_time_int = 0;
	uint8_t ret_val = 0;
#ifdef __LOCK_USE_MALLOC__
	uint8_t ret = 0;
	g_dq_app_pwd_info = (otp_base_app_pwd_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_PWD, &ret);
#endif
	//memset(sys_time,0xFF,sizeof(sys_time));

	//for(i=0;i<4;i++)
	//{
	//	sys_time[i] = pdata[i+2];
	//}
	for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		memset(g_dq_app_pwd_info[i].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].app_pwd));
		memset(g_dq_app_pwd_info[i].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].replace_pwd));
		g_dq_app_pwd_info[i].pwd_flag = 0xFF;
		g_dq_app_pwd_info[i].pwd_type = 0xFF;
	}
	//need save the system time to startime

	//sys_time_int = dq_sdk_HexcharToInt(sys_time,4);

	//otp_set_info.otp_start_hour = sys_time_int;

	//g_dq_otp_init.set_system_time(sys_time_int,&ret_val);

	g_dq_cmd_repleys_status = OTP_BASE_SUCESS;
//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_PWD_APP, (unsigned char *)g_dq_app_pwd_info, sizeof(otp_base_app_pwd_info) * DQ_OTP_APP_PWD_NUM, &ret_val, dq_otp_fds_write_common_cb);
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
	return OTP_BASE_SUCESS;
}

static OTP_BASE_RET dq_otp_syn_lock_pwd(uint8_t *p_data)
{
	uint8_t i = 0, j = 0;
	uint8_t t = p_data[2];
	uint8_t num = 0;
	uint8_t _data[9];
#ifdef __LOCK_USE_MALLOC__
	uint8_t ret = 0;
	g_dq_app_pwd_info = (otp_base_app_pwd_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_PWD, &ret);
#endif
	for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		if (memcmp(g_dq_app_pwd_info[i].app_pwd, g_dq_app_pwd_info[i].replace_pwd, 5) != 0)
		{
			num++;
			if (num == t)
				break;
		}
	}
	if (i >= DQ_OTP_APP_PWD_NUM)
	{
		return OTP_BASE_FAIL;
	}

	for (j = 0; j < 5; j++)
	{
		_data[j] = g_dq_app_pwd_info[i].app_pwd[j];
	}
	for (j = 0; j < 4; j++)
	{
		_data[j + 5] = g_dq_app_pwd_info[i].replace_pwd[j];
	}
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
	dq_otp_send_data_reply(0x02, 0x14, OTP_BASE_SUCESS, (uint8_t *)_data, 9);
	return OTP_BASE_SUCESS;
}

static OTP_BASE_RET dq_otp_syn_lock_temp_pwd(uint8_t *p_data)
{
	uint8_t i = 0, j = 0;
	uint8_t t = p_data[2];
	uint8_t num = 0;
	uint8_t _data[10];
#ifdef __LOCK_USE_MALLOC__
	uint8_t ret = 0;
	g_dq_app_pwd_info = (otp_base_app_pwd_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_PWD, &ret);
#endif
	memset(_data, 0xFF, 10);
	for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		if (g_dq_app_pwd_info[i].pwd_type != 0xFF)
		{
			if (memcmp(g_dq_app_pwd_info[i].app_pwd, g_dq_app_pwd_info[i].replace_pwd, 5) == 0)
			{
				num++;
				if (num == (t * 2 - 1))
				{
					for (j = 0; j < 5; j++)
					{
						_data[j] = g_dq_app_pwd_info[i].app_pwd[j];
					}
					break;
				}
			}
		}
	}
	if (i >= DQ_OTP_APP_PWD_NUM)
	{
#ifdef __LOCK_USE_MALLOC__
		mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
		return OTP_BASE_FAIL;
	}

	for (i = i + 1; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		if (memcmp(g_dq_app_pwd_info[i].app_pwd, g_dq_app_pwd_info[i].replace_pwd, 5) == 0)
		{
			for (j = 0; j < 5; j++)
			{
				_data[j + 5] = g_dq_app_pwd_info[i].app_pwd[j];
			}
			break;
		}
	}
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
	dq_otp_send_data_reply(0x02, 0x15, OTP_BASE_SUCESS, (uint8_t *)_data, 10);
	return OTP_BASE_SUCESS;
}

/*
parameter: 
	none
return :
	none
*/
//static uint8_t dq_otp_get_empty_fp_index(void)
//{
//	uint8_t i = 0;
//	for(i=0;i<DQ_OTP_APP_FP_NUM;i++)
//	{
//		if(g_dq_app_fp_info[i].fp_index == 0xFFFF)
//			break;
//	}
//	return i;
//}

/*
parameter: 
	none
return :
	none
*/
#if defined(__LOCK_FP_SUPPORT__) || defined(__LOCK_FP_SUPPORT2__) || defined(__LOCK_FP_SUPPORT1_2__)

void dq_otp_app_add_lock_fp_result(uint8_t result_flag, uint16_t fp_index)
{
	uint8_t fp_index_char[2];
	uint8_t ret = 0;

	memset(fp_index_char, 0xFF, sizeof(fp_index_char));
	if (result_flag == 0)
	{
		fp_index_char[0] = (fp_index >> 8) & 0xFF;
		fp_index_char[1] = fp_index & 0xFF;
	}

	if (result_flag == 0)
		g_dq_cmd_repleys_status = OTP_BASE_SUCESS;
	else if (result_flag == 1)
		g_dq_cmd_repleys_status = OTP_BASE_TIMEOUT;
	else if (result_flag == 2)
		g_dq_cmd_repleys_status = OTP_BASE_FULL;
	else if (result_flag == 3)
		g_dq_cmd_repleys_status = OTP_BASE_HAND_EXIT;
	else if (result_flag == 5)
		g_dq_cmd_repleys_status = OTP_BASE_TYPE_ERROR;
	else
		g_dq_cmd_repleys_status = OTP_BASE_FAIL;

	if (result_flag == 0)
	{
		//save fp info
		g_dq_fp_index = fp_index;
#ifdef __LOCK_USE_MALLOC__
		g_dq_app_fp_info = (otp_base_app_fp_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_FP, &ret);
#endif
		g_dq_app_fp_info[fp_index].fp_start_time = g_dq_add_temp_start_time;
		g_dq_app_fp_info[fp_index].fp_end_time = g_dq_add_temp_end_time;
		g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_FP, (unsigned char *)g_dq_app_fp_info, sizeof(otp_base_app_fp_info) * DQ_OTP_APP_FP_NUM, &ret, dq_otp_fds_write_common_cb);
#ifdef __LOCK_USE_MALLOC__
		mmi_dq_fs_free_storage(DQ_FS_MEM_APP_FP, (void **)&g_dq_app_fp_info);
#endif
	}
	else
	{
		unsigned char i;
		g_dq_add_temp_start_time = 0;
		g_dq_add_temp_end_time = 0;
		for (i = 0; i < CMD_OTP_MAX_NUM; i++)
		{
			if (cmd_info[i].cmd == g_dq_otp_type)
				break;
		}
		dq_otp_app_set_operate_lock_flag(0);
		if (g_dq_link_error == 1)
		{
			g_dq_link_error = 0;
		}
		else
			dq_otp_common_reply(cmd_info[i].cmd_h, cmd_info[i].cmd_l, g_dq_cmd_repleys_status);
	}
}
/*
parameter: 
	none
return :
	none
*/
//0221 5C9A0030 5D134A30 00 60 FFFF FF 99
static OTP_BASE_RET dq_otp_add_lock_fp(uint8_t *p_data)
{
	uint8_t result = 0;
	uint8_t fp_time[4];
	uint8_t i = 0;
	uint16_t delay_time = 0;

	//g_dq_fp_index = dq_otp_get_empty_fp_index();

	//if(g_dq_fp_index == DQ_OTP_APP_FP_NUM)
	//	return OTP_BASE_NO_MEM;

	memset(fp_time, 0x00, sizeof(fp_time));
	for (i = 0; i < 4; i++)
	{
		fp_time[i] = p_data[i + 2];
	}
	g_dq_add_temp_start_time = dq_sdk_HexcharToInt(fp_time, 4);

	memset(fp_time, 0x00, sizeof(fp_time));
	for (i = 0; i < 4; i++)
	{
		fp_time[i] = p_data[i + 6];
	}
	g_dq_add_temp_end_time = dq_sdk_HexcharToInt(fp_time, 4);

	memset(fp_time, 0x00, sizeof(fp_time));
	for (i = 0; i < 2; i++)
	{
		fp_time[i] = p_data[i + 10];
	}
	delay_time = dq_sdk_HexcharToInt(fp_time, 2);

	g_dq_otp_init.set_lock_fp(delay_time, &result);
	return OTP_BASE_SUCESS;
}

void dq_otp_lock_fp_app_clr_result(void)
{
	uint8_t ret = 0;
	uint8_t i;
#ifdef __LOCK_USE_MALLOC__
	g_dq_app_fp_info = (otp_base_app_fp_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_FP, &ret);
#endif
	for (i = 0; i < DQ_OTP_APP_FP_NUM; i++)
	{
		g_dq_app_fp_info[i].fp_end_time = 0xFFFFFFFF;
		g_dq_app_fp_info[i].fp_start_time = 0xFFFFFFFF;
	}

	g_dq_cmd_repleys_status = OTP_BASE_SUCESS;
	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_FP, (unsigned char *)g_dq_app_fp_info, sizeof(otp_base_app_fp_info) * DQ_OTP_APP_FP_NUM, &ret, dq_otp_fds_write_common_cb);
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_FP, (void **)&g_dq_app_fp_info);
#endif
}

extern uint16_t lock_del_fp_index;
void dq_otp_lock_fp_app_del_result(void)
{
	uint8_t ret = 0;
#ifdef __LOCK_USE_MALLOC__
	g_dq_app_fp_info = (otp_base_app_fp_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_FP, &ret);
#endif
	g_dq_app_fp_info[lock_del_fp_index].fp_end_time = 0xFFFFFFFF;
	g_dq_app_fp_info[lock_del_fp_index].fp_start_time = 0xFFFFFFFF;

	g_dq_cmd_repleys_status = OTP_BASE_SUCESS;
	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_FP, (unsigned char *)g_dq_app_fp_info, sizeof(otp_base_app_fp_info) * DQ_OTP_APP_FP_NUM, &ret, dq_otp_fds_write_common_cb);
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_FP, (void **)&g_dq_app_fp_info);
#endif
}

void dq_otp_lock_fp_app_fail_result(void)
{

	g_dq_cmd_repleys_status = OTP_BASE_FAIL;
	dq_otp_fds_write_common_cb();
}

/*
parameter: 
	none
return :
	none
*/
//0222 8866 FFFF FFFF FFFF FFFF FFFF 8899
static OTP_BASE_RET dq_otp_del_lock_fp(uint8_t *p_data)
{
	uint8_t i = 0;
	uint8_t fp_index[2];
	uint16_t index = 0;
	uint8_t ret = 0;

	memset(fp_index, 0x00, sizeof(fp_index));
	for (i = 0; i < 2; i++)
	{
		fp_index[i] = p_data[i + 2];
	}

	index = dq_sdk_HexcharToInt(fp_index, 2);

	//for(i=0;i<DQ_OTP_APP_FP_NUM;i++)
	//{
	//	if(g_dq_app_fp_info[i].fp_index == index)
	//		break;
	//}
	//if(i == DQ_OTP_APP_FP_NUM)
	//	return OTP_BASE_NOT_FOUND;
	//else
	if (dq_otp_check_lock_fp(index) == 0)
	{
		g_dq_otp_init.del_lock_fp(index, &ret); //delete fp in lock

		//save fp info

		return (OTP_BASE_RET)ret;
	}
	else
		return OTP_BASE_NOT_FOUND;
}
/*
parameter: 
	none
return :
	none
*/
//0213 5C90 628C FFFF FFFF FFFF FFFF FF 99
static OTP_BASE_RET dq_otp_clr_lock_fp(uint8_t *p_data)
{
	uint8_t ret = 0;

	g_dq_otp_init.clear_lock_fp(&ret);

	//save fp info

	return OTP_BASE_SUCESS;
}

static OTP_BASE_RET dq_otp_syn_lock_fp(uint8_t *p_data)
{
	uint8_t i = 0, j = 0;
	uint8_t t = p_data[2];
	uint8_t num = 0;
	uint8_t _data[10];

	memset(_data, 0xFF, 10);
	for (i = 0; i < DQ_OTP_APP_FP_NUM; i++)
	{
		if (dq_otp_check_lock_fp(i) == 0)
		{
			num++;
			if ((num > (t - 1) * 5) && (num <= t * 5))
			{
				_data[j * 2] = 0;	 //((g_dq_app_fp_info[i].fp_index>>8)&0xFF);
				_data[j * 2 + 1] = i; //(g_dq_app_fp_info[i].fp_index&0xFF);
				j++;
				if (j >= 5)
					break;
			}
		}
	}
	if (j == 0)
	{
		return OTP_BASE_FAIL;
	}

	dq_otp_send_data_reply(0x02, 0x24, OTP_BASE_SUCESS, (uint8_t *)_data, 10);
	return OTP_BASE_SUCESS;
}

#endif
/*
parameter: 
	none
return :
	none
*/
//static uint8_t dq_otp_get_empty_rfid_index(void)
//{
//	uint8_t i = 0;
//	for(i=0;i<DQ_OTP_APP_RFID_NUM;i++)
//	{
//		if(g_dq_app_rfid_info[i].rfid_index == 0xFFFF)
//			break;
//	}
//	return i;
//}
/*
parameter: 
	none
return :
	none
*/
#ifdef __LOCK_RFID_CARD_SUPPORT__

void dq_otp_app_add_lock_rfid_result(uint8_t result_flag, uint16_t rfid_index)
{
	uint8_t rfid_index_char[2];
	uint8_t ret = 0;

	memset(rfid_index_char, 0xFF, sizeof(rfid_index_char));
	if (result_flag == 0)
	{
		rfid_index_char[0] = (rfid_index >> 8) & 0xFF;
		rfid_index_char[1] = rfid_index & 0xFF;
	}

	if (result_flag == 0)
		g_dq_cmd_repleys_status = OTP_BASE_SUCESS;
	else if (result_flag == 1)
		g_dq_cmd_repleys_status = OTP_BASE_TIMEOUT;
	else if (result_flag == 2)
		g_dq_cmd_repleys_status = OTP_BASE_FULL;
	else if (result_flag == 3)
		g_dq_cmd_repleys_status = OTP_BASE_HAND_EXIT;
	else if (result_flag == 5)
		g_dq_cmd_repleys_status = OTP_BASE_TYPE_ERROR;
	else
		g_dq_cmd_repleys_status = OTP_BASE_FAIL;

	if (result_flag == 0)
	{
		//save fp info
		g_dq_rfid_index = rfid_index;
#ifdef __LOCK_USE_MALLOC__
		g_dq_app_rfid_info = (otp_base_app_rfid_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_RFID, &ret);
#endif
		g_dq_app_rfid_info[rfid_index].rfid_start_time = g_dq_add_temp_start_time;
		g_dq_app_rfid_info[rfid_index].rfid_end_time = g_dq_add_temp_end_time;
//		g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_RFID, (unsigned char *)g_dq_app_rfid_info, sizeof(otp_base_app_rfid_info) * DQ_OTP_APP_RFID_NUM, &ret, dq_otp_fds_write_common_cb);
#ifdef __LOCK_USE_MALLOC__
		mmi_dq_fs_free_storage(DQ_FS_MEM_APP_RFID, (void **)&g_dq_app_rfid_info);
#endif
	}
	else
	{
		unsigned char i;
		g_dq_add_temp_start_time = 0;
		g_dq_add_temp_end_time = 0;

		for (i = 0; i < CMD_OTP_MAX_NUM; i++)
		{
			if (cmd_info[i].cmd == g_dq_otp_type)
				break;
		}
		dq_otp_app_set_operate_lock_flag(0);

		if (g_dq_link_error == 1)
		{
			g_dq_link_error = 0;
		}
		else
			dq_otp_common_reply(cmd_info[i].cmd_h, cmd_info[i].cmd_l, g_dq_cmd_repleys_status);
	}
}

/*
parameter: 
	none
return :
	none
*/
//0231 5C9A0030 5D134A30 00 60 FFFF FF 99
static OTP_BASE_RET dq_otp_add_lock_rfid(uint8_t *p_data)
{
	uint8_t result = 0;
	uint8_t rfid_time[4];
	uint8_t i = 0;
	uint16_t delay_time = 0;

	//g_dq_rfid_index = dq_otp_get_empty_rfid_index();

	//if(g_dq_rfid_index == DQ_OTP_APP_RFID_NUM)
	//	return OTP_BASE_NO_MEM;

	memset(rfid_time, 0x00, sizeof(rfid_time));
	for (i = 0; i < 4; i++)
	{
		rfid_time[i] = p_data[i + 2];
	}
	g_dq_add_temp_start_time = dq_sdk_HexcharToInt(rfid_time, 4);

	memset(rfid_time, 0x00, sizeof(rfid_time));
	for (i = 0; i < 4; i++)
	{
		rfid_time[i] = p_data[i + 6];
	}
	g_dq_add_temp_end_time = dq_sdk_HexcharToInt(rfid_time, 4);

	memset(rfid_time, 0x00, sizeof(rfid_time));
	for (i = 0; i < 2; i++)
	{
		rfid_time[i] = p_data[i + 10];
	}
	delay_time = dq_sdk_HexcharToInt(rfid_time, 2);

	g_dq_otp_init.set_lock_rfid(delay_time, &result);

	return OTP_BASE_SUCESS;
}

void dq_otp_lock_rfid_app_clr_result(void)
{
	uint8_t ret = 0;
	uint8_t i = 0;

#ifdef __LOCK_USE_MALLOC__
	g_dq_app_rfid_info = (otp_base_app_rfid_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_RFID, &ret);
#endif
	for (i = 0; i < DQ_OTP_APP_RFID_NUM; i++)
	{
		g_dq_app_rfid_info[i].rfid_end_time = 0xFFFFFFFF;
		g_dq_app_rfid_info[i].rfid_start_time = 0xFFFFFFFF;
		//g_dq_app_rfid_info[i].rfid_index = 0xFFFF;
	}

	g_dq_cmd_repleys_status = OTP_BASE_SUCESS;
//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_RFID, (unsigned char *)g_dq_app_rfid_info, sizeof(otp_base_app_rfid_info) * DQ_OTP_APP_RFID_NUM, &ret, dq_otp_fds_write_common_cb);
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_RFID, (void **)&g_dq_app_rfid_info);
#endif
}

extern uint16_t lock_del_rf_index;
void dq_otp_lock_rfid_app_del_result(void)
{
	uint8_t ret = 0;

#ifdef __LOCK_USE_MALLOC__
	g_dq_app_rfid_info = (otp_base_app_rfid_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_RFID, &ret);
#endif
	g_dq_app_rfid_info[lock_del_rf_index].rfid_end_time = 0xFFFFFFFF;
	g_dq_app_rfid_info[lock_del_rf_index].rfid_start_time = 0xFFFFFFFF;
	//g_dq_app_rfid_info[i].rfid_index = 0xFFFF;

	g_dq_cmd_repleys_status = OTP_BASE_SUCESS;
//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_RFID, (unsigned char *)g_dq_app_rfid_info, sizeof(otp_base_app_rfid_info) * DQ_OTP_APP_RFID_NUM, &ret, dq_otp_fds_write_common_cb);
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_RFID, (void **)&g_dq_app_rfid_info);
#endif
}

/*
parameter: 
	none
return :
	none
*/
//0232 8866 FFFF FFFF FFFF FFFF FFFF 8899
static OTP_BASE_RET dq_otp_del_lock_rfid(uint8_t *p_data)
{
	uint8_t i = 0;
	uint8_t rfid_index[2];
	uint16_t index = 0;
	uint8_t ret = 0;

	memset(rfid_index, 0x00, sizeof(rfid_index));
	for (i = 0; i < 2; i++)
	{
		rfid_index[i] = p_data[i + 2];
	}

	index = dq_sdk_HexcharToInt(rfid_index, 2);

	//for(i=0;i<DQ_OTP_APP_RFID_NUM;i++)
	//{
	//	if(g_dq_app_rfid_info[i].rfid_index == index)
	//		break;
	//}
	//if(i == DQ_OTP_APP_RFID_NUM)
	//	return OTP_BASE_NOT_FOUND;
	//else
	if (dq_otp_check_lock_rfid(index) == 0)
	{
		g_dq_otp_init.del_lock_rf(index, &ret);

		//save rfid info
		return OTP_BASE_SUCESS;
	}
	else
		return OTP_BASE_NOT_FOUND;
}

/*
parameter: 
	none
return :
	none
*/
//0233 FFFF FFFF FFFF FFFF FFFF FFFF FF 99
static OTP_BASE_RET dq_otp_clr_lock_rfid(uint8_t *p_data)
{
	uint8_t ret = 0;

	//save rfid info
	g_dq_otp_init.clear_lock_rf(&ret);

	return OTP_BASE_SUCESS;
}

static OTP_BASE_RET dq_otp_syn_lock_rfid(uint8_t *p_data)
{
	uint8_t i = 0, j = 0;
	uint8_t t = p_data[2];
	uint8_t num = 0;
	uint8_t _data[10];

	memset(_data, 0xFF, 10);
	for (i = 0; i < DQ_OTP_APP_RFID_NUM; i++)
	{
		if (dq_otp_check_lock_rfid(i) == 0)
		{
			num++;
			if ((num > (t - 1) * 5) && (num <= t * 5))
			{
				_data[j * 2] = 0;	 //((g_dq_app_rfid_info[i].rfid_index>>8)&0xFF);
				_data[j * 2 + 1] = i; // (g_dq_app_rfid_info[i].rfid_index&0xFF);
				j++;
				if (j >= 5)
					break;
			}
		}
	}
	if (j == 0)
	{
		return OTP_BASE_FAIL;
	}

	dq_otp_send_data_reply(0x02, 0x34, OTP_BASE_SUCESS, (uint8_t *)_data, 10);
	return OTP_BASE_SUCESS;
}

static OTP_BASE_RET dq_otp_add_lock_rfid_by_app(uint8_t *p_data)
{
	uint8_t rfid_time[4];
	uint8_t i = 0;
	//uint16_t delay_time = 0;
	static uint8_t data_len = 0;
	static uint8_t _data[20] = {0};
	static uint8_t data_flag = 0;
	if (p_data[2] == 0)
	{
		memset(rfid_time, 0x00, sizeof(rfid_time));
		for (i = 0; i < 4; i++)
		{
			rfid_time[i] = p_data[i + 3];
		}
		g_dq_add_temp_start_time = dq_sdk_HexcharToInt(rfid_time, 4);

		memset(rfid_time, 0x00, sizeof(rfid_time));
		for (i = 0; i < 4; i++)
		{
			rfid_time[i] = p_data[i + 7];
		}
		g_dq_add_temp_end_time = dq_sdk_HexcharToInt(rfid_time, 4);
		data_len = p_data[11];
		memset(rfid_time, 0x00, sizeof(rfid_time));
		for (i = 0; i < 2; i++)
		{
			rfid_time[i] = p_data[i + 12];
		}
		//	delay_time = dq_sdk_HexcharToInt(rfid_time, 2);
		memset(_data, 0, 20);
		data_flag = 0;

		if (data_len > 20)
		{
			data_len = 0;
			dq_otp_common_reply(p_data[0], p_data[1], OTP_BASE_FAIL);
		}
		else
			dq_otp_common_reply(p_data[0], p_data[1], OTP_BASE_SUCESS);
	}
	else if (p_data[2] == data_flag + 1)
	{
		if (data_len > 12)
		{
			for (i = 0; i < 12; i++)
			{
				_data[data_flag * 12 + i] = p_data[i + 3];
			}
			data_len -= 12;
			data_flag++;
			dq_otp_common_reply(p_data[0], p_data[1], OTP_BASE_SUCESS);
		}
		else if (data_len > 0)
		{
			for (i = 0; i < data_len; i++)
			{
				_data[data_flag * 12 + i] = p_data[i + 3];
			}
			data_len = 0;
			data_flag = 0;

			g_dq_otp_init.add_lock_rfid_by_app(_data);
		}
		else
			dq_otp_common_reply(p_data[0], p_data[1], OTP_BASE_FAIL);
	}
	else
		dq_otp_common_reply(p_data[0], p_data[1], OTP_BASE_FAIL);

	return OTP_BASE_SUCESS;
}

#endif
/*
parameter: 
	none
return :
	none
*/
//0401 FFFF FFFF FFFF FFFF FFFF FFFF FF 99
static OTP_BASE_RET dq_otp_get_lock_config_info(uint8_t *pwd_num, uint8_t *fp_num, uint8_t *rf_num, uint16_t *lock_config)
{
//	g_dq_otp_init.get_lock_mem_info(pwd_num, fp_num, rf_num, lock_config);
	*pwd_num = DQ_OTP_APP_PWD_NUM;
	*fp_num = DQ_OTP_APP_FP_NUM;
	*rf_num = DQ_OTP_APP_RFID_NUM;

	return OTP_BASE_SUCESS;
}

/*
parameter: 
	none
return :
	none
*/
void dq_otp_check_and_update_time_cb(void)
{
	uint8_t ret_val = 0;
	uint8_t bat = 0;
	uint16_t sw = 0;

	uint8_t reply_data[3];
	dq_otp_app_return_idle();

//	g_dq_otp_init.get_lock_bat_level(&bat, &ret_val);

//	g_dq_otp_init.get_lock_ver_info(&sw, &ret_val);

	memset(reply_data, 0xFF, sizeof(reply_data));

	reply_data[0] = bat;
	reply_data[1] = (sw >> 8) & 0xFF;
	reply_data[2] = sw & 0xFF;
	dq_otp_send_data_reply(0x04, 0x02, OTP_BASE_SUCESS, reply_data, 3);
}

//0402 5D13 4A30 FFFF FFFF FFFF FFFF FF 99
//0402 00 58 1001 FFFF FFFF FFFF FFFF FFFF FF 99
static OTP_BASE_RET dq_otp_get_lock_base_info(uint8_t *p_data, uint8_t *bat_val, uint16_t *sw_ver)
{
	uint8_t i = 0;
	uint8_t sys_time[4];
	uint32_t sys_time_int = 0;
	uint32_t time = 0;
	uint8_t ret_val = 0;
	uint8_t bat = 0;
	uint16_t sw = 0;
	uint16_t west_d = 0;
	uint16_t east_d = 0;
	uint8_t time_zone_update_flag = 0;

	memset(sys_time, 0xFF, sizeof(sys_time));

	for (i = 0; i < 4; i++)
	{
		sys_time[i] = p_data[i + 2];
	}

	west_d = (p_data[7] << 8) + p_data[8];
	east_d = (p_data[9] << 8) + p_data[10];

	g_dq_otp_init.set_system_time_zone(west_d, east_d, &time_zone_update_flag);

	sys_time_int = dq_sdk_HexcharToInt(sys_time, 4);
	time = sys_time_int - dq_otp_get_sys_time_sec();

	g_dq_otp_init.set_system_time(sys_time_int, &ret_val);

	if (DQ_OTP_SYS_TIME_GET == 0)
	{
		DQ_OTP_SYS_TIME_GET = 1;

		if (dq_otp_check_and_update_log_time(time) != 1)
		{
			dq_otp_update_time();
		}
		return OTP_BASE_BUSY;
	}
	else if (time_zone_update_flag == 1)
	{

		dq_otp_update_time();
		return OTP_BASE_BUSY;
	}

//	g_dq_otp_init.get_lock_bat_level(&bat, &ret_val);

//	g_dq_otp_init.get_lock_ver_info(&sw, &ret_val);

	*bat_val = bat;
	*sw_ver = sw;
	return OTP_BASE_SUCESS;
}

unsigned char dq_otp_get_lock_sys_time_state(void)
{
	return DQ_OTP_SYS_TIME_GET;
}

/*
parameter: 
	none
return :
	none
*/
//0403 FFFF FFFF FFFF FFFF FFFF FFFF FF 99
static uint8_t dq_otp_get_lock_admin_status(uint8_t *p_data)
{
	uint8_t status;
	uint8_t retval;
//	g_dq_otp_init.get_admin_status(&status, &retval);
	return status;
}

/*
parameter: 
	none
return :
	none
*/
//0404 0060 FFFF FFFF FFFF FFFF FFFF 8899
static uint8_t dq_otp_check_lock_admin_pwd_fp(uint8_t *p_data)
{
	uint8_t i = 0;
	uint8_t time[2];
	uint16_t delay_time = 0;
	uint8_t retval = 0;

	memset(time, 0xFF, sizeof(time));
	for (i = 0; i < 2; i++)
	{
		time[i] = p_data[i + 2];
	}
	delay_time = dq_sdk_HexcharToInt(time, 2);

	g_dq_otp_init.verify_admin_status(delay_time, &retval);
	if (retval == 1)
		dq_otp_common_reply(0x04, 0x04, OTP_BASE_FAIL);
	return 0;
}
/*
parameter: 
	none
return :
	none
*/
void dq_otp_check_lock_admin_pwd_fp_ret(uint8_t result)
{
	if (result == 0)
		dq_otp_common_reply(0x04, 0x04, OTP_BASE_SUCESS);
	else if (result == 1)
		dq_otp_common_reply(0x04, 0x04, OTP_BASE_TIMEOUT);
	else if (result == 2)
		dq_otp_common_reply(0x04, 0x04, OTP_BASE_HAND_EXIT);
	else
		dq_otp_common_reply(0x04, 0x04, OTP_BASE_FAIL);
	return;
}

void dq_otp_sys_get_time(void)
{
	time_t time = dq_otp_get_sys_time_sec();
	uint8_t _data[4] = {0};
	_data[0] = (time >> 24) & 0xFF;
	_data[1] = (time >> 16) & 0xFF;
	_data[2] = (time >> 8) & 0xFF;
	_data[3] = time & 0xFF;
	dq_otp_send_data_reply(0x04, 0x07, OTP_BASE_SUCESS, _data, 4);
}

void dq_otp_sys_enter_dfu(void)
{
	g_dq_dfu_permit = 1;
}

uint8_t dq_otp_get_dfu_permit(void)
{
	return g_dq_dfu_permit;
}

void dq_otp_set_dfu_permit(uint8_t permit)
{
	g_dq_dfu_permit = permit;
}

/*
parameter: 
	none
return :
	none
*/
void dq_otp_cmd_dataOperation(uint8_t *p_data, uint16_t data_len)
{
	OTP_BASE_RET retval = OTP_BASE_SUCESS;

	uint8_t data_ret[20];
	uint8_t data_back[20];
	uint8_t i = 0;
	uint8_t ret = 0;

	memset(data_ret, 0x00, sizeof(data_ret));
	memset(data_back, 0x00, sizeof(data_back));
	for (i = 0; i < data_len; i++)
	{
		data_ret[i] = *p_data++;
	}

	ret = g_dq_otp_init.check_lock_state();
	if (ret == 1)
	{
		dq_otp_common_reply(data_ret[0], data_ret[1], OTP_BASE_MUL_ERROR);
		return;
	}
	else if (ret == 2)
	{
		dq_otp_common_reply(data_ret[0], data_ret[1], OTP_BASE_LOW_VBAT);
		return;
	}

	g_dq_otp_type = dq_otp_get_cmd_type(data_ret);

#ifndef __WIN32_ENV_SUPPORT__
	for (i = 0; i < data_len; i++)
	{
//		NRF_LOG_PRINTF_DEBUG("dq_otp_cmd_dataOperation data_ret[%d] = 0x%x \n", i, data_ret[i]);
	}
//	NRF_LOG_PRINTF_DEBUG("dq_otp_get_cmd_type otp_type = 0x%x \n", g_dq_otp_type);
#endif

	if (g_dq_otp_type == CMD_OTP_GET_LOCK_BASE_INFO || g_dq_otp_type == CMD_OTP_SYN_AES_16)
	{
		uint8_t _data;
		dq_otp_lock_random_vector_generate(&_data, 1);
		serialId = _data % 50;
//		NRF_LOG_PRINTF_DEBUG("dq_otp_cmd_dataOperation CMD_OTP_GET_LOCK_BASE_INFO serialID= 0x%x  0x%x \n", serialId, _data);
	}
	else if (data_ret[15] != serialId)
	{

//		NRF_LOG_PRINTF_DEBUG("dq_otp_cmd_dataOperation error serialID= 0x%x data_ret 0x%x \n", serialId, data_ret[15]);
		dq_otp_common_reply(data_ret[0], data_ret[1], OTP_BASE_FAIL);
		return;
	}

	switch (g_dq_otp_type)
	{
	case CMD_OTP_SYN_AES_16:
		retval = dq_otp_syn_aes_16_key(data_ret);
#ifndef __WIN32_ENV_SUPPORT__
//		NRF_LOG_PRINTF_DEBUG("dq_otp_syn_aes_16_key retval = %d \n", retval);
#endif
		if (retval == OTP_BASE_SUCESS)
		{
			if (dq_dev_key_flag == 0)
				dq_otp_device_key_unknow(data_ret[0], data_ret[1], retval);
			else
				dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_SYN_SEC_8:
		retval = dq_otp_syn_sec_8_key(data_ret);
		if (retval == OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_SYN_SEC_9:
		retval = dq_otp_syn_sec_9_key(data_ret);
		if (retval == OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_SYN_START_TIME:
		retval = dq_otp_syn_start_time(data_ret);
		if (retval == OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_SYN_PWD_STR:
		retval = dq_otp_syn_exchange_num(data_ret);
		dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		break;
	case CMD_OTP_EKEY_OPEN_LOCK:
		retval = dq_otp_check_ekey_verify(data_ret);
		if (retval == OTP_BASE_SUCESS)
		{
			g_dq_otp_init.open_lock();
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		else
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		break;
	case CMD_OTP_USER_DEF_PWD:
		retval = dq_otp_add_user_def_pwd(data_ret);
		if (retval != OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_DEL_PWD:
		retval = dq_otp_del_lock_pwd(data_ret);
		if (retval != OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_CLR_PWD:
		retval = dq_otp_clr_lock_pwd(data_ret);
		break;
	case CMD_OTP_SYN_PWD:
		retval = dq_otp_syn_lock_pwd(data_ret);
		if (retval != OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_SYN_TEMP_PWD:
		retval = dq_otp_syn_lock_temp_pwd(data_ret);
		if (retval != OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_ADD_FP:
#if defined(__LOCK_FP_SUPPORT__) || defined(__LOCK_FP_SUPPORT2__) || defined(__LOCK_FP_SUPPORT1_2__)
		retval = dq_otp_add_lock_fp(data_ret);
#else
		retval = OTP_BASE_FAIL;
#endif
		if (retval != OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_DEL_FP:
#if defined(__LOCK_FP_SUPPORT__) || defined(__LOCK_FP_SUPPORT2__) || defined(__LOCK_FP_SUPPORT1_2__)
		retval = dq_otp_del_lock_fp(data_ret);
#else
		retval = OTP_BASE_FAIL;
#endif
		if (retval != OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_CLR_FP:
#if defined(__LOCK_FP_SUPPORT__) || defined(__LOCK_FP_SUPPORT2__) || defined(__LOCK_FP_SUPPORT1_2__)
		retval = dq_otp_clr_lock_fp(data_ret);
#else
		retval = OTP_BASE_FAIL;
#endif
		if (retval != OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_SYN_FP:
#if defined(__LOCK_FP_SUPPORT__) || defined(__LOCK_FP_SUPPORT2__) || defined(__LOCK_FP_SUPPORT1_2__)
		retval = dq_otp_syn_lock_fp(data_ret);
#else
		retval = OTP_BASE_FAIL;
#endif
		if (retval != OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_ADD_RFID:
#ifdef __LOCK_RFID_CARD_SUPPORT__
		retval = dq_otp_add_lock_rfid(data_ret);
#else
		retval = OTP_BASE_FAIL;
#endif
		if (retval != OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_DEL_RFID:
#ifdef __LOCK_RFID_CARD_SUPPORT__
		retval = dq_otp_del_lock_rfid(data_ret);
#else
		retval = OTP_BASE_FAIL;
#endif
		if (retval != OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_CLR_RFID:
#ifdef __LOCK_RFID_CARD_SUPPORT__
		retval = dq_otp_clr_lock_rfid(data_ret);
#else
		retval = OTP_BASE_FAIL;
#endif
		if (retval != OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_SYN_RFID:
#ifdef __LOCK_RFID_CARD_SUPPORT__
		retval = dq_otp_syn_lock_rfid(data_ret);
#else
		retval = OTP_BASE_FAIL;
#endif
		if (retval != OTP_BASE_SUCESS)
		{
			dq_otp_common_reply(data_ret[0], data_ret[1], retval);
		}
		break;
	case CMD_OTP_ADD_RFID_BY_APP:
#ifdef __LOCK_RFID_CARD_SUPPORT__
		retval = dq_otp_add_lock_rfid_by_app(data_ret);
#else
		dq_otp_common_reply(data_ret[0], data_ret[1], OTP_BASE_FAIL);
#endif
		break;

	case CMD_OTP_UPDATE_LOG:
		dq_otp_send_log();
		break;
	case CMD_OTP_GET_LOCK_CONFIG:
	{
		uint8_t lock_mem_info[5];
		uint16_t lock_config;
		memset(lock_mem_info, 0xFF, sizeof(lock_mem_info));

		retval = dq_otp_get_lock_config_info(&lock_mem_info[2], &lock_mem_info[3], &lock_mem_info[4], &lock_config);
		lock_mem_info[0] = (lock_config >> 8) & 0xFF;
		lock_mem_info[1] = lock_config & 0xFF;

		if (retval == OTP_BASE_SUCESS)
		{
			dq_otp_send_data_reply(data_ret[0], data_ret[1], retval, lock_mem_info, 5);
		}
	}
	break;
	case CMD_OTP_GET_LOCK_BASE_INFO:
	{
		uint8_t reply_data[3];
		uint16_t sw_ver = 0;
		memset(reply_data, 0xFF, sizeof(reply_data));
		retval = dq_otp_get_lock_base_info(data_ret, &reply_data[0], &sw_ver);
		if (retval == OTP_BASE_SUCESS)
		{
			reply_data[1] = (sw_ver >> 8) & 0xFF;
			reply_data[2] = sw_ver & 0xFF;
			dq_otp_send_data_reply(data_ret[0], data_ret[1], retval, reply_data, 3);
		}
	}
	break;
	case CMD_OTP_CHK_ADMIN_STATUS:
	{
		uint8_t admin_status = 0;
		admin_status = dq_otp_get_lock_admin_status(data_ret);
		dq_otp_send_data_reply(data_ret[0], data_ret[1], OTP_BASE_SUCESS, &admin_status, 1);
	}
	break;
	case CMD_OTP_VERY_ADMIN_STA:
		dq_otp_check_lock_admin_pwd_fp(data_ret);
		break;
	case CMD_OTP_INIT_SUCESS:
		dq_otp_lock_init_complete(data_ret);
		break;
	case CMD_OTP_SYS_ENTER_DFU:
		dq_otp_sys_enter_dfu();
		dq_otp_common_reply(data_ret[0], data_ret[1], OTP_BASE_SUCESS);
		break;
	case CMD_OTP_GET_SYS_TIME:
		dq_otp_sys_get_time();
		break;
	case CMD_OTP_INVALID:
		dq_otp_common_reply(data_ret[0], data_ret[1], OTP_BASE_FAIL);
		break;
	default:
		break;
	}
	return;
}

/*
parameter: 
	current status machine
return :
	none
*/
void dq_otp_fds_write_common_cb(void)
{
	uint8_t i = 0;

	for (i = 0; i < CMD_OTP_MAX_NUM; i++)
	{
		if (cmd_info[i].cmd == g_dq_otp_type)
			break;
	}
#ifndef __WIN32_ENV_SUPPORT__
//	NRF_LOG_PRINTF_DEBUG("dq_otp_fds_write_common_cb g_dq_cmd_repleys_status = 0x%x,cmd_info[%d].cmd_h  = 0x%x,cmd_info[%d].cmd_l = 0x%x,g_dq_fp_index = %d\n", g_dq_cmd_repleys_status, i, cmd_info[i].cmd_h, i, cmd_info[i].cmd_l, g_dq_fp_index);
#endif
	if (g_dq_otp_type == CMD_OTP_INIT_SUCESS)
	{
		g_dq_otp_init.init_sucess(1);
		g_dq_otp_init.init_sucess_reset_lock(); //delete all data in lock pwd fp rfid
	}
	/*
	else if(g_dq_otp_type == CMD_OTP_CLR_PWD)
	{	
		g_dq_cmd_repleys_status = OTP_BASE_SUCESS;
		g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_SET,(unsigned char *)&otp_set_info,sizeof(otp_base_setting_info),&ret,dq_otp_fds_write_common_cb);
	}
	*/
	else if (g_dq_otp_type == CMD_OTP_ADD_FP)
	{
		uint8_t fp_index_char[2];

		if (dq_otp_app_get_operate_lock_flag() == 2)
		{
			return;
		}

		memset(fp_index_char, 0xFF, sizeof(fp_index_char));

		fp_index_char[0] = (g_dq_fp_index >> 8) & 0xFF;
		fp_index_char[1] = g_dq_fp_index & 0xFF;

		if (g_dq_link_error == 1)
		{
			uint16_t index;
			uint8_t ret;
			dq_otp_app_set_operate_lock_flag(2);

			g_dq_link_error = 0;
			index = g_dq_fp_index;
			g_dq_otp_init.del_lock_fp(index, &ret);
		}
		else
		{
			dq_otp_app_set_operate_lock_flag(0);
			dq_otp_send_data_reply(cmd_info[i].cmd_h, cmd_info[i].cmd_l, g_dq_cmd_repleys_status, (uint8_t *)fp_index_char, 2);
		}
	}
	else if (g_dq_otp_type == CMD_OTP_ADD_RFID)
	{
		uint8_t rfid_index_char[2];

		if (dq_otp_app_get_operate_lock_flag() == 2)
		{
			dq_otp_app_set_operate_lock_flag(0);

			return;
		}

		memset(rfid_index_char, 0xFF, sizeof(rfid_index_char));

		rfid_index_char[0] = (g_dq_rfid_index >> 8) & 0xFF;
		rfid_index_char[1] = g_dq_rfid_index & 0xFF;

		if (g_dq_link_error == 1)
		{
			uint16_t index;
			uint8_t ret;
			dq_otp_app_set_operate_lock_flag(2);

			g_dq_link_error = 0;
			index = g_dq_rfid_index;
			g_dq_otp_init.del_lock_rf(index, &ret);
		}
		else
		{
			dq_otp_app_set_operate_lock_flag(0);
			dq_otp_send_data_reply(cmd_info[i].cmd_h, cmd_info[i].cmd_l, g_dq_cmd_repleys_status, (uint8_t *)rfid_index_char, 2);
		}
	}
	else
	{
		dq_otp_common_reply(cmd_info[i].cmd_h, cmd_info[i].cmd_l, g_dq_cmd_repleys_status);
		dq_otp_app_return_idle();
	}

	return;
}

void dq_otp_app_init_suc(void)
{
	dq_otp_common_reply(0x04, 0x05, g_dq_cmd_repleys_status);
}

/*
parameter: 
	current status machine
return :
	none
*/
void dq_otp_fds_clear_init_set_param(dq_otp_write_data_cb write_data_cb)
{
	unsigned char ret = 0;
	dq_dev_key_flag = 0;
	memset(&otp_set_info, 0xFF, sizeof(otp_set_info));
//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_SET, (unsigned char *)&otp_set_info, sizeof(otp_base_setting_info), &ret, write_data_cb);
	return;
}
/*
parameter: 
	current status machine
return :
	none
*/
void dq_otp_fds_clear_app_pwd(dq_otp_write_data_cb write_data_cb)
{
	uint8_t i = 0;
	uint8_t ret = 0;
#ifdef __LOCK_USE_MALLOC__
	g_dq_app_pwd_info = (otp_base_app_pwd_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_PWD, &ret);
#endif

	for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		memset(g_dq_app_pwd_info[i].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].app_pwd));
		memset(g_dq_app_pwd_info[i].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].replace_pwd));
		g_dq_app_pwd_info[i].pwd_flag = 0xFF;
		g_dq_app_pwd_info[i].pwd_type = 0xFF;
	}
//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_PWD_APP, (unsigned char *)g_dq_app_pwd_info, sizeof(otp_base_app_pwd_info) * DQ_OTP_APP_PWD_NUM, &ret, write_data_cb);
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif

	return;
}

void dq_otp_fds_clear_app_pwd_by_password(void)
{
	uint8_t i = 0;
	uint8_t ret = 0;
	unsigned short sys_hour = 0;
#ifdef __LOCK_USE_MALLOC__
	g_dq_app_pwd_info = (otp_base_app_pwd_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_PWD, &ret);
#endif

	dq_otp_get_sys_time_hour(&sys_hour);

	for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		switch (g_dq_app_pwd_info[i].pwd_type)
		{
		case OTP_PWD_TYPE_SIN:
			if (g_dq_app_pwd_info[i].pwd_flag == 1)
			{
				unsigned char dec_pwd_char[15];
				unsigned short pwd_hour;
				unsigned char len = dq_otp_get_pwd_len(g_dq_app_pwd_info[i].app_pwd, 5);
				memset(dec_pwd_char, 0x00, sizeof(dec_pwd_char));
				dq_otp_enc_pwd(g_dq_app_pwd_info[i].app_pwd, len, otp_set_info.otp_sec_key_8, otp_set_info.otp_exg_key_8, dec_pwd_char);
				pwd_hour = dq_otp_get_pwd_time_hour(dec_pwd_char, 5, 1);

				if (pwd_hour + 6 <= sys_hour)
				{
					memset(g_dq_app_pwd_info[i].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].app_pwd));
					memset(g_dq_app_pwd_info[i].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].replace_pwd));
					g_dq_app_pwd_info[i].pwd_flag = 0xFF;
					g_dq_app_pwd_info[i].pwd_type = 0xFF;
				}
			}
			break;
		case OTP_PWD_TYPE_PER:
			if (g_dq_app_pwd_info[i].pwd_flag == 1)
			{
				unsigned char dec_pwd_char[15];
				unsigned short pwd_hour;
				unsigned char len = dq_otp_get_pwd_len(g_dq_app_pwd_info[i].app_pwd, 5);
				memset(dec_pwd_char, 0x00, sizeof(dec_pwd_char));
				dq_otp_enc_pwd(g_dq_app_pwd_info[i].app_pwd, len, otp_set_info.otp_sec_key_8, otp_set_info.otp_exg_key_8, dec_pwd_char);
				pwd_hour = dq_otp_get_pwd_time_hour(dec_pwd_char, 5, 1);

				if (pwd_hour + 24 <= sys_hour)
				{
					memset(g_dq_app_pwd_info[i].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].app_pwd));
					memset(g_dq_app_pwd_info[i].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].replace_pwd));
					g_dq_app_pwd_info[i].pwd_flag = 0xFF;
					g_dq_app_pwd_info[i].pwd_type = 0xFF;
				}
				else
					g_dq_app_pwd_info[i].pwd_flag = 2;
			}
			break;
		case OTP_PWD_TYPE_LIM:
			if (g_dq_app_pwd_info[i].pwd_flag == 1)
			{
				unsigned char dec_pwd_char[15];
				unsigned short pwd_hour;
				unsigned short end_time;
				unsigned char len = dq_otp_get_pwd_len(g_dq_app_pwd_info[i].app_pwd, 5);
				memset(dec_pwd_char, 0x00, sizeof(dec_pwd_char));
				if (len == 8)
					dq_otp_enc_pwd(g_dq_app_pwd_info[i].app_pwd, len, otp_set_info.otp_sec_key_8, otp_set_info.otp_exg_key_8, dec_pwd_char);
				else if (len == 9)
					dq_otp_enc_pwd(g_dq_app_pwd_info[i].app_pwd, len, otp_set_info.otp_sec_key_9, otp_set_info.otp_exg_key_9, dec_pwd_char);
				pwd_hour = dq_otp_get_pwd_time_hour(dec_pwd_char, 5, 0);

				end_time = dq_otp_get_lim_pwd_end_time(dec_pwd_char, len) / 3600;
				if (pwd_hour + 24 <= sys_hour || end_time < sys_hour)
				{
					memset(g_dq_app_pwd_info[i].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].app_pwd));
					memset(g_dq_app_pwd_info[i].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].replace_pwd));
					g_dq_app_pwd_info[i].pwd_type = 0xFF;
					g_dq_app_pwd_info[i].pwd_flag = 0xFF;
				}
				else
					g_dq_app_pwd_info[i].pwd_flag = 2;
			}
			break;
		case OTP_PWD_TYPE_LOOP:
			if (g_dq_app_pwd_info[i].pwd_flag == 1)
			{
				unsigned char dec_pwd_char[15];
				unsigned short pwd_hour;
				unsigned char len = dq_otp_get_pwd_len(g_dq_app_pwd_info[i].app_pwd, 5);
				memset(dec_pwd_char, 0x00, sizeof(dec_pwd_char));
				dq_otp_enc_pwd(g_dq_app_pwd_info[i].app_pwd, len, otp_set_info.otp_sec_key_9, otp_set_info.otp_exg_key_9, dec_pwd_char);
				pwd_hour = dq_otp_get_pwd_time_hour(dec_pwd_char, 5, 1);

				if (pwd_hour + 24 <= sys_hour)
				{
					memset(g_dq_app_pwd_info[i].app_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].app_pwd));
					memset(g_dq_app_pwd_info[i].replace_pwd, 0xFF, sizeof(g_dq_app_pwd_info[i].replace_pwd));
					g_dq_app_pwd_info[i].pwd_flag = 0xFF;
					g_dq_app_pwd_info[i].pwd_type = 0xFF;
				}
				else
					g_dq_app_pwd_info[i].pwd_flag = 2;
			}
			break;
		}
	}
//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_PWD_APP, (unsigned char *)g_dq_app_pwd_info, sizeof(otp_base_app_pwd_info) * DQ_OTP_APP_PWD_NUM, &ret, NULL);
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
	return;
}
/*
parameter: 
	current status machine
return :
	none
*/
void dq_otp_fds_clear_log(dq_otp_write_data_cb write_data_cb)
{
	unsigned char ret = 0;
	unsigned char i;
	for (i = 0; i < DQ_OTP_LOG_NUM; i++)
	{
		memset(otp_lock_log[i].app_pwd, 0xFF, sizeof(otp_lock_log[i].app_pwd));
		otp_lock_log[i].sys_time = 0xFFFFFFFF;
		otp_lock_log[i].type = 0xFF;
		otp_lock_log[i].sys_time_correct = 0xFF;

		otp_lock_log[i].flag = 0xFF;
	}
	//memset(otp_lock_log,0xFF,sizeof(otp_lock_log));
	otp_log_head = 0;
	otp_log_end = 0;
	//LOG
//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_LOG, (unsigned char *)&otp_lock_log, sizeof(otp_base_log_info) * DQ_OTP_LOG_NUM, &ret, write_data_cb);
	return;
}
/*
parameter: 
	current status machine
return :
	none
*/
void dq_otp_fds_clear_app_fp(dq_otp_write_data_cb write_data_cb)
{
	unsigned char i = 0;
	unsigned char ret = 0;
#ifdef __LOCK_USE_MALLOC__
	g_dq_app_fp_info = (otp_base_app_fp_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_FP, &ret);
#endif
	for (i = 0; i < DQ_OTP_APP_FP_NUM; i++)
	{
		g_dq_app_fp_info[i].fp_end_time = 0xFFFFFFFF;
		g_dq_app_fp_info[i].fp_start_time = 0xFFFFFFFF;
		//g_dq_app_fp_info[i].fp_index = 0xFFFF;
	}
	//LOG
//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_FP, (unsigned char *)g_dq_app_fp_info, sizeof(otp_base_app_fp_info) * DQ_OTP_APP_FP_NUM, &ret, write_data_cb);
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_FP, (void **)&g_dq_app_fp_info);
#endif
	return;
}
/*
parameter: 
	current status machine
return :
	none
*/
void dq_otp_fds_clear_app_rf(dq_otp_write_data_cb write_data_cb)
{
	unsigned char i = 0;
	unsigned char ret = 0;
#ifdef __LOCK_USE_MALLOC__
	g_dq_app_rfid_info = (otp_base_app_rfid_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_RFID, &ret);
#endif
	for (i = 0; i < DQ_OTP_APP_RFID_NUM; i++)
	{
		g_dq_app_rfid_info[i].rfid_end_time = 0xFFFFFFFF;
		g_dq_app_rfid_info[i].rfid_start_time = 0xFFFFFFFF;
		//g_dq_app_rfid_info[i].rfid_index = 0xFFFF;
	}
	//LOG
//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_RFID, (unsigned char *)g_dq_app_rfid_info, sizeof(otp_base_app_rfid_info) * DQ_OTP_APP_RFID_NUM, &ret, write_data_cb);
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_RFID, (void **)&g_dq_app_rfid_info);
#endif
	return;
}

#if 1

static void dq_otp_find_log_head_end(void)
{
#if 1
	unsigned char i = 0;
	unsigned char index = 0xFF;

	for (i = 0; i < DQ_OTP_LOG_NUM; i++)
	{
		if (otp_lock_log[i].flag == 1)
		{
			index = i;
			break;
		}
	}
	if (index == 0xFF)
	{
		otp_log_end = 0;
		otp_log_head = 0;
	}
	else
	{
		otp_log_head = index;
		for (i = 0; i < DQ_OTP_LOG_NUM; i++)
		{
			if (otp_lock_log[i].flag == 2)
			{
				index = i;
				break;
			}
		}
		if (index < DQ_OTP_LOG_NUM - 1)
			otp_log_end = index + 1;
		else
			otp_log_end = 0;

		if (otp_log_end == otp_log_head)
		{
			if (otp_log_head < DQ_OTP_LOG_NUM - 1)
				otp_log_head++;
			else
				otp_log_head = 0;

			otp_lock_log[otp_log_head].flag = 1;
			otp_lock_log[otp_log_end].sys_time = 0xFFFFFFFF;
			otp_lock_log[otp_log_end].type = 0xFF;
			memset(otp_lock_log[otp_log_end].app_pwd, 0xFF, sizeof(otp_lock_log[otp_log_end].app_pwd));
			otp_lock_log[otp_log_end].flag = 0xFF;
		}
	}

#else
	unsigned char i = 0;
	unsigned char index = 0xFF;
	uint32_t time = 0xFFFFFFFF;
	for (i = 0; i < DQ_OTP_LOG_NUM; i++)
	{
		if (otp_lock_log[i].sys_time < time)
		{
			time = otp_lock_log[i].sys_time;
			index = i;
		}
	}
	if (index == 0xFF)
	{
		otp_log_end = 0;
		otp_log_head = 0;
	}
	else
	{
		otp_log_head = index;
		for (i = 0; i < DQ_OTP_LOG_NUM; i++)
		{
			if (otp_lock_log[i].sys_time != 0xFFFFFFFF && otp_lock_log[i].sys_time > time)
			{
				time = otp_lock_log[i].sys_time;
				index = i;
			}
		}
		if (index < DQ_OTP_LOG_NUM - 1)
			otp_log_end = index + 1;
		else
			otp_log_end = 0;

		if (otp_log_end == otp_log_head)
		{
			if (otp_log_head < DQ_OTP_LOG_NUM - 1)
				otp_log_head++;
			else
				otp_log_head = 0;
		}
	}
#endif
}

unsigned char dq_otp_add_log_index(void)
{
	unsigned char index;
	index = otp_log_end;
	if (otp_log_end < DQ_OTP_LOG_NUM - 1)
		otp_log_end++;
	else
		otp_log_end = 0;

	if (otp_log_end == otp_log_head)
	{
		if (otp_log_head < DQ_OTP_LOG_NUM - 1)
			otp_log_head++;
		else
			otp_log_head = 0;

		otp_lock_log[otp_log_end].sys_time = 0xFFFFFFFF;
		otp_lock_log[otp_log_end].type = 0xFF;
		memset(otp_lock_log[otp_log_end].app_pwd, 0xFF, sizeof(otp_lock_log[otp_log_end].app_pwd));
		otp_lock_log[otp_log_end].sys_time_correct = 0xFF;
		otp_lock_log[otp_log_end].flag = 0xFF;
	}

	if (index == 0)
	{
		otp_lock_log[DQ_OTP_LOG_NUM - 1].flag = 0;
		otp_lock_log[0].flag = 2;
	}
	else
	{
		otp_lock_log[index - 1].flag = 0;
		otp_lock_log[index].flag = 2;
	}
	otp_lock_log[otp_log_head].flag = 1;

	return index;
}

unsigned char dq_otp_get_log_index(void)
{
	unsigned char index;

	if (otp_log_head == otp_log_end)
		return 0xFF;

	index = otp_log_head;

	if (otp_log_head < DQ_OTP_LOG_NUM - 1)
		otp_log_head++;
	else
		otp_log_head = 0;

	otp_lock_log[index].flag = 0;
	otp_lock_log[otp_log_head].flag = 1;

	return index;
}

void dq_otp_add_exchange_temp_open_log(uint8_t src_id, uint8_t dest_id)
{
	unsigned char j;

	otp_lock_temp_log[dest_id].sys_time = otp_lock_temp_log[src_id].sys_time;
	otp_lock_temp_log[dest_id].type = otp_lock_temp_log[src_id].type;
	otp_lock_temp_log[dest_id].sys_time_correct = otp_lock_temp_log[src_id].sys_time_correct;

	memset(otp_lock_temp_log[dest_id].app_pwd, 0xFF, sizeof(otp_lock_temp_log[dest_id].app_pwd));

	for (j = 0; j < 5; j++)
	{
		otp_lock_temp_log[dest_id].app_pwd[j] = otp_lock_temp_log[src_id].app_pwd[j];
	}
}

void dq_otp_add_open_log_by_temp(uint8_t id)
{
	unsigned char i, j;

	i = dq_otp_add_log_index();

	otp_lock_log[i].sys_time = otp_lock_temp_log[id].sys_time;
	otp_lock_log[i].type = otp_lock_temp_log[id].type;
	otp_lock_log[i].sys_time_correct = otp_lock_temp_log[id].sys_time_correct;

	memset(otp_lock_log[i].app_pwd, 0xFF, sizeof(otp_lock_log[i].app_pwd));

	for (j = 0; j < 5; j++)
	{
		otp_lock_log[i].app_pwd[j] = otp_lock_temp_log[id].app_pwd[j];
	}
}

void dq_otp_del_temp_open_log(void)
{
	uint8_t i;
	for (i = 0; i < 2; i++)
	{
		otp_lock_temp_log[i].sys_time = 0xFFFFFFFF;
		otp_lock_temp_log[i].type = 0xFF;
		memset(otp_lock_temp_log[i].app_pwd, 0xFF, sizeof(otp_lock_temp_log[i].app_pwd));
		otp_lock_temp_log[i].sys_time_correct = 0xFF;
		otp_lock_temp_log[i].flag = 0xFF;
	}
}

unsigned char dq_otp_add_temp_open_log(uint8_t id, unsigned char type, unsigned char *pwd, unsigned char len)
{
	unsigned char j;

	otp_lock_temp_log[id].sys_time = dq_otp_get_sys_time_sec();
	otp_lock_temp_log[id].type = type;
	otp_lock_temp_log[id].sys_time_correct = dq_otp_get_lock_sys_time_state();

	memset(otp_lock_temp_log[id].app_pwd, 0xFF, sizeof(otp_lock_temp_log[id].app_pwd));

	for (j = 0; j < len; j++)
	{
		otp_lock_temp_log[id].app_pwd[j] = pwd[j];
	}
	return 0;
}

unsigned char dq_otp_add_open_log(unsigned char type, unsigned char *pwd, unsigned char len)
{
#if 1
	unsigned char i, j;
	//unsigned char password[5];

	i = dq_otp_add_log_index();

	otp_lock_log[i].sys_time = dq_otp_get_sys_time_sec();
	otp_lock_log[i].type = type;
	otp_lock_log[i].sys_time_correct = dq_otp_get_lock_sys_time_state();

	memset(otp_lock_log[i].app_pwd, 0xFF, sizeof(otp_lock_log[i].app_pwd));

	for (j = 0; j < len; j++)
	{
		otp_lock_log[i].app_pwd[j] = pwd[j];
	}
	return 0;
#else
	unsigned char i;
	memset(otp_temp_lock_log.sys_time, 0xFF, sizeof(otp_temp_lock_log.sys_time));
	memset(otp_temp_lock_log.app_pwd, 0xFF, sizeof(otp_temp_lock_log.app_pwd));
	otp_temp_lock_log.sys_time = dq_otp_get_sys_time_sec();
	otp_temp_lock_log.type = type;
	for (i = 0; i < len; i++)
	{
		otp_temp_lock_log.app_pwd[i] = pwd[i];
	}
#endif
}

unsigned char dq_otp_add_alarm_log(unsigned char type)
{
	//unsigned char ret = 0;
	unsigned char i;

	i = dq_otp_add_log_index();

	otp_lock_log[i].sys_time = dq_otp_get_sys_time_sec();
	otp_lock_log[i].type = type;
	otp_lock_log[i].sys_time_correct = dq_otp_get_lock_sys_time_state();

	//g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_LOG,(unsigned char *)&otp_lock_log,sizeof(otp_base_log_info)*DQ_OTP_LOG_NUM,&ret,NULL);

	return 0;
}
#if 0
unsigned char dq_otp_del_open_log(void)
{
	uint8_t temp_end, temp_end_next;
	if(otp_log_head == otp_log_end)
		return 1;

	temp_end = otp_log_end;
	
	do
	{
		if(temp_end>0)
			temp_end--;
		else
			temp_end = DQ_OTP_LOG_NUM-1;
		if(temp_end==otp_log_head)
			break;
	}
	while(!(otp_lock_log[temp_end].type >= DQ_OPEN_LOG_ADMIN_PASSWORD && otp_lock_log[temp_end].type<=DQ_OPEN_LOG_USER_RF));

	if(temp_end!=otp_log_head)
	{

		while(1)
		{
			if(temp_end<DQ_OTP_LOG_NUM-1)
				temp_end_next = temp_end+1;
			else
				temp_end_next = 0;

			if(temp_end_next!=otp_log_end)
			{
				otp_lock_log[temp_end].sys_time = otp_lock_log[temp_end_next].sys_time;
				otp_lock_log[temp_end].type = otp_lock_log[temp_end_next].type;
				memcpy(otp_lock_log[temp_end].app_pwd , otp_lock_log[temp_end_next].app_pwd,sizeof(otp_lock_log[temp_end].app_pwd ));
				otp_lock_log[temp_end].sys_time_correct = otp_lock_log[temp_end_next].sys_time_correct;;
				otp_lock_log[temp_end].flag = otp_lock_log[temp_end_next].flag;

				temp_end = temp_end_next;
			}
			else
			{
				otp_lock_log[temp_end].sys_time = 0xFFFFFFFF;
				otp_lock_log[temp_end].type = 0xFF;
				memset(otp_lock_log[temp_end].app_pwd , 0xFF,sizeof(otp_lock_log[temp_end].app_pwd ));
				otp_lock_log[temp_end].sys_time_correct = 0xFF;
				otp_lock_log[temp_end].flag = 0xFF;

				otp_log_end = temp_end;

				break;
			}
		}

	}


	
	if(otp_log_end != otp_log_head)
	{
		if(otp_log_end>0&&otp_log_head!=otp_log_end-1)
			otp_lock_log[otp_log_end-1].flag = 2;
		else if(otp_log_head!=DQ_OTP_LOG_NUM-1)		
			otp_lock_log[DQ_OTP_LOG_NUM-1].flag = 2;
	}
		
		
	return 0;
}
#endif

void dq_otp_save_open_log(dq_otp_write_data_cb write_data_cb)
{
	unsigned char ret = 0;

//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_LOG, (unsigned char *)&otp_lock_log, sizeof(otp_base_log_info) * DQ_OTP_LOG_NUM, &ret, write_data_cb);
	return;
}

void dq_otp_send_log(void)
{
#if 1
	unsigned char i;
	unsigned char index = dq_otp_get_log_index();
	unsigned char _data[10] = {0};
	uint32_t time = 0;

	if (index == 0xFF)
	{
		unsigned char ret = 0;
		if (dq_otp_lock_save_log_state())
		{
			g_dq_cmd_repleys_status = OTP_BASE_EMPTY;
//			g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_LOG, (unsigned char *)&otp_lock_log, sizeof(otp_base_log_info) * DQ_OTP_LOG_NUM, &ret, dq_otp_fds_write_common_cb);
		}
		else
			dq_otp_common_reply(0x03, 0x01, OTP_BASE_EMPTY);
		return;
	}

	time = otp_lock_log[index].sys_time;
	_data[0] = otp_lock_log[index].type;
	_data[1] = (time >> 24) & 0xFF;
	_data[2] = (time >> 16) & 0xFF;
	_data[3] = (time >> 8) & 0xFF;
	_data[4] = time & 0xFF;

	for (i = 0; i < 5; i++)
	{
		_data[5 + i] = otp_lock_log[index].app_pwd[i];
	}

	dq_otp_send_data_reply(0x03, 0x01, OTP_BASE_SUCESS, (uint8_t *)_data, 10);

	otp_lock_log[index].sys_time = 0xFFFFFFFF;
	otp_lock_log[index].type = 0xFF;
	memset(otp_lock_log[index].app_pwd, 0xFF, sizeof(otp_lock_log[otp_log_end].app_pwd));
	otp_lock_log[index].sys_time_correct = 0xFF;
	otp_lock_log[index].flag = 0xFF;
#endif
}

uint8_t dq_otp_check_and_update_log_time(uint32_t time)
{

	unsigned char i = 0;
	uint8_t res = 0;

	for (i = 0; i < DQ_OTP_LOG_NUM; i++)
	{
		if (otp_lock_log[i].sys_time_correct == 0)
		{
			otp_lock_log[i].sys_time += time;
			otp_lock_log[i].sys_time_correct = 1;
			res = 1;
		}
	}
	for (i = 0; i < 2; i++)
	{
		if (otp_lock_temp_log[i].sys_time_correct == 0)
		{
			otp_lock_temp_log[i].sys_time += time;
			otp_lock_temp_log[i].sys_time_correct = 1;
			res = 1;
		}
	}

	if (res == 1)
		dq_otp_save_open_log(dq_otp_update_time);

	return res;
}

#endif

static char dq_app_pwd_flag = 0;
static char dq_app_pwd_for_open_ret = 0;
void dq_otp_app_pwd_save_cb(void)
{
	unsigned char ret = 0;
	dq_app_pwd_flag = 0;
	ret = dq_app_pwd_for_open_ret;
	dq_app_pwd_for_open_ret = 0;
	g_dq_otp_init.input_pwd_cb(ret);
}

unsigned char dq_otp_save_temp_pwd(unsigned char *in_pwd, OTP_PWD_TYPE type)
{
	uint8_t i = 0;
	uint8_t pwd_index;
	uint8_t ret = 0;
#ifdef __LOCK_USE_MALLOC__
	g_dq_app_pwd_info = (otp_base_app_pwd_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_PWD, &ret);
#endif
	pwd_index = dq_otp_get_empty_pwd_index();

	if (pwd_index == DQ_OTP_APP_PWD_NUM)
		return OTP_BASE_NO_MEM;

	//g_dq_app_pwd_info[pwd_index].pwd_index = pwd_index;
	for (i = 0; i < 5; i++)
	{
		g_dq_app_pwd_info[pwd_index].app_pwd[i] = in_pwd[i];
		g_dq_app_pwd_info[pwd_index].replace_pwd[i] = in_pwd[i];
	}

	g_dq_app_pwd_info[pwd_index].pwd_type = type;
	g_dq_app_pwd_info[pwd_index].pwd_flag = 1;

	//save pwd info
	dq_app_pwd_flag = 1;
//	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_PWD_APP, (unsigned char *)g_dq_app_pwd_info, sizeof(otp_base_app_pwd_info) * DQ_OTP_APP_PWD_NUM, &ret, dq_otp_app_pwd_save_cb);

#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif

	return OTP_BASE_SUCESS;
}
#if 0
unsigned char dq_otp_save_temp_pwd_with_endtime(unsigned char *in_pwd, OTP_PWD_TYPE type, uint32_t endtime)
{
	uint8_t i = 0;
	uint8_t pwd_index = dq_otp_get_empty_pwd_index();
	uint8_t ret = 0;

	if(pwd_index == DQ_OTP_APP_PWD_NUM)
		return OTP_BASE_NO_MEM;

	//g_dq_app_pwd_info[pwd_index].pwd_index = pwd_index;

	for(i=0;i<5;i++)
	{
		g_dq_app_pwd_info[pwd_index].app_pwd[i] = in_pwd[i];
		g_dq_app_pwd_info[pwd_index].replace_pwd[i] = in_pwd[i];
	}

	g_dq_app_pwd_info[pwd_index].pwd_type = type;
	g_dq_app_pwd_info[pwd_index].pwd_used_flag = 1;
	if(endtime != 0)
		g_dq_app_pwd_info[pwd_index].pwd_end_time = endtime;

	g_dq_app_pwd_info[pwd_index].pwd_del_flag = 0;
	

	//save pwd info
	dq_app_pwd_flag = 1;
	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_PWD_APP,(unsigned char *)&g_dq_app_pwd_info,sizeof(otp_base_app_pwd_info)*DQ_OTP_APP_PWD_NUM,&ret,dq_otp_app_pwd_save_cb);
	return OTP_BASE_SUCESS;

}
#endif
#if 0
void dq_otp_update_pwd_app(unsigned char index)
{
	uint8_t ret = 0;

	g_dq_app_pwd_info[index].pwd_used_flag = 1;
	switch(g_dq_app_pwd_info[index].pwd_type)
	{
		case OTP_PWD_TYPE_LIM:
		{
			unsigned char len;
			unsigned char dec_pwd_char[15];
			memset(dec_pwd_char,0x00,sizeof(dec_pwd_char));
			len = dq_otp_get_pwd_len(g_dq_app_pwd_info[index].app_pwd,5);

			if(len == 8)
				dq_otp_enc_pwd(g_dq_app_pwd_info[index].app_pwd,len,otp_set_info.otp_sec_key_8,otp_set_info.otp_exg_key_8,dec_pwd_char);
			else if(len == 9)
				dq_otp_enc_pwd(g_dq_app_pwd_info[index].app_pwd,len,otp_set_info.otp_sec_key_9,otp_set_info.otp_exg_key_9,dec_pwd_char);	
			g_dq_app_pwd_info[index].pwd_end_time=dq_otp_get_lim_pwd_end_time(dec_pwd_char, len);
		}
			break;
		case OTP_PWD_TYPE_SIN:
			break;
		case OTP_PWD_TYPE_PER:
		case OTP_PWD_TYPE_LOOP:
			break;
		default:
			break;
	}

	dq_app_pwd_flag = 1;
	g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_PWD_APP,(unsigned char *)&g_dq_app_pwd_info,sizeof(otp_base_app_pwd_info)*DQ_OTP_APP_PWD_NUM,&ret,dq_otp_app_pwd_save_cb);
	
}
#endif

/*
parameter: 
	current status machine
return :
	none
*/
void dq_otp_enc_pwd(unsigned char *pwd, unsigned char len, unsigned char *sec_key, unsigned char *exg_key, unsigned char *sec_pwd)
{
	unsigned char sec_key_char[10];
	unsigned char exg_key_char[10];
	unsigned char pwd_char[10];
	unsigned char sec_char[10];
	unsigned int int_sec_key = 0;
	unsigned int int_pwd = 0;
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned char hex_pwd_len = 0;
	unsigned char exchange_id = 0;
	unsigned char exchange_char[10];

	if (len == 8)
		hex_pwd_len = 4;
	else if (len == 9)
		hex_pwd_len = 5;

	memset(sec_key_char, 0x00, sizeof(sec_key_char));
	memset(exg_key_char, 0x00, sizeof(exg_key_char));
	memset(pwd_char, 0x00, sizeof(pwd_char));

	dq_sdk_CharToHexByte((const uint8_t *)exg_key, (char *)exg_key_char, hex_pwd_len);
	dq_sdk_CharToInt(exg_key_char, len);

	dq_sdk_CharToHexByte((const uint8_t *)sec_key, (char *)sec_key_char, hex_pwd_len);
	int_sec_key = dq_sdk_CharToInt(sec_key_char, len);

	dq_sdk_CharToHexByte((const uint8_t *)pwd, (char *)pwd_char, hex_pwd_len);

	for (i = 0; i < 59; i++)
	{
		int_pwd = dq_sdk_CharToInt((const uint8_t *)pwd_char, len);

		if (int_pwd < int_sec_key)
		{
			if (len == 8)
				int_pwd += 100000000;
			else if (len == 9)
				int_pwd += 1000000000;
			else if (len == 10)
				int_pwd += 10000000000;
		}
		int_pwd -= int_sec_key;
		memset(sec_char, 0x00, sizeof(sec_char));
		dq_sdk_IntToByteStr(int_pwd, (char *)sec_char, len);
		memset(pwd_char, 0x00, sizeof(pwd_char));
		for (j = 0; j < len; j++)
		{
			pwd_char[exg_key_char[j] - 1] = sec_char[j];
		}
	}
	memset(sec_char, 0x00, sizeof(sec_char));
	exchange_id = pwd_char[2];
	memset(exchange_char, 0x00, sizeof(exchange_char));
	dq_sdk_CharToHexByte((const uint8_t *)otp_set_info.g_pwd_signed_data[exchange_id].exchg_num, (char *)exchange_char, 5);
	for (i = 0; i < len; i++)
	{
		if (i == 2)
		{
			sec_char[i] = exchange_id;
		}
		else
		{
			for (j = 0; j < 10; j++)
			{
				if (pwd_char[i] == exchange_char[j])
				{
					sec_char[i] = j;
					break;
				}
			}
		}
	}
	for (i = 2; i < len; i++)
	{
		sec_char[i] = sec_char[i + 1];
	}
	sec_char[len - 1] = exchange_id;
	memcpy((char *)sec_pwd, (const char *)sec_char, len);
	return;
}
/*
parameter: 
	current status machine
return :
	none
*/
void dq_otp_fds_reset_ekey_record_cb(void)
{
#ifndef __WIN32_ENV_SUPPORT__
//	NRF_LOG_PRINTF_DEBUG("#### dq_otp_fds_reset_ekey_record_cb \n");
#endif
}

/*
parameter: 
	current status machine
return :
	none
*/
unsigned char dq_otp_check_app_rfid(unsigned char index)
{
	//unsigned char i = 0;
	unsigned char result = 1;
	uint32_t sys_time;

#ifdef __LOCK_USE_MALLOC__
	uint8_t ret;
	g_dq_app_rfid_info = (otp_base_app_rfid_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_RFID, &ret);
#endif
	sys_time = dq_otp_get_sys_time_sec();

	//for(i=0;i<DQ_OTP_APP_RFID_NUM;i++)
	//{
	//	if(g_dq_app_rfid_info[i].rfid_index == index)
	//	{
	if (g_dq_app_rfid_info[index].rfid_end_time == 0xFFFFFFFF || (sys_time >= g_dq_app_rfid_info[index].rfid_start_time && sys_time <= g_dq_app_rfid_info[index].rfid_end_time))
	{
		result = 0;
		//			break;
	}

	//	}
	//}
#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_RFID, (void **)&g_dq_app_rfid_info);
#endif

	return result;
}

unsigned char dq_otp_check_app_fp(unsigned char index)
{
	//unsigned char i = 0;
	unsigned char result = 1;
	uint32_t sys_time;
#ifdef __LOCK_USE_MALLOC__
	uint8_t ret;
	g_dq_app_fp_info = (otp_base_app_fp_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_FP, &ret);
#endif
	sys_time = dq_otp_get_sys_time_sec();

	//for(i=0;i<DQ_OTP_APP_FP_NUM;i++)
	//{
	//	if(g_dq_app_fp_info[i].fp_index == index)
	//	{
	if (g_dq_app_fp_info[index].fp_end_time == 0xFFFFFFFF || (sys_time >= g_dq_app_fp_info[index].fp_start_time && sys_time <= g_dq_app_fp_info[index].fp_end_time))
	{
		result = 0;
		//			break;
	}
	//	}
	//}

#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_FP, (void **)&g_dq_app_fp_info);
#endif
	return result;
}

/*
parameter: 
	current status machine
return :
	none
*/
unsigned char dq_otp_check_password(unsigned char *password)
{
	unsigned char i = 0;
	unsigned char k = 0;
#ifdef __LOCK_USE_MALLOC__
	uint8_t ret;
	g_dq_app_pwd_info = (otp_base_app_pwd_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_PWD, &ret);
#endif

	for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		if (g_dq_app_pwd_info[i].pwd_type != 0xFF)
		{
			for (k = 0; k < 5; k++)
			{
				if (password[k] != g_dq_app_pwd_info[i].app_pwd[k])
					break;
			}
			if (k == 5)
			{
				break;
			}
		}
	}

#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
	if (i < DQ_OTP_APP_PWD_NUM)
	{
		return 1;
	}

	return 0;
}

unsigned char dq_otp_check_replace_password(unsigned char *password)
{
	unsigned char i = 0;
	unsigned char k = 0;
#ifdef __LOCK_USE_MALLOC__
	uint8_t ret;
	g_dq_app_pwd_info = (otp_base_app_pwd_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_PWD, &ret);
#endif

	for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		if (g_dq_app_pwd_info[i].pwd_type != 0xFF)
		{
			for (k = 0; k < 5; k++)
			{
				if (password[k] != g_dq_app_pwd_info[i].replace_pwd[k])
					break;
			}
			if (k == 5)
			{
				break;
			}
		}
	}

#ifdef __LOCK_USE_MALLOC__
	mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
	if (i < DQ_OTP_APP_PWD_NUM)
	{
		return 1;
	}

	return 0;
}

unsigned char dq_otp_check_password_for_open(unsigned char *password, unsigned char len)
{
	unsigned char i = 0;
	unsigned char k = 0;
	unsigned char pwd_len = 0;
	unsigned char ret = 0;
	unsigned char ret_t = 0;
#ifdef __LOCK_USE_MALLOC__
	g_dq_app_pwd_info = (otp_base_app_pwd_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_PWD, &ret_t);
#endif

	for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
	{
		if (g_dq_app_pwd_info[i].pwd_type != 0xFF)
		{
			for (k = 0; k < 5; k++)
			{
				if (password[k] != g_dq_app_pwd_info[i].replace_pwd[k])
					break;
			}
			if (k == 5)
			{
				break;
			}
#if 0
			for(k=0;k<5;k++)
			{
				if(password[k] != g_dq_app_pwd_info[i].app_pwd[k])
					break;
			}
			if(k == 5)
			{
				break;
			}
#endif
		}
	}
	if (i < DQ_OTP_APP_PWD_NUM)
	{
		extern uint8_t mmi_dq_rtc_check_time(void);
		if (g_dq_app_pwd_info[i].pwd_flag == 2)
		{
#ifdef __LOCK_USE_MALLOC__
			mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
			g_dq_otp_init.input_pwd_cb(4);
			return 4;
		}
		if (g_dq_app_pwd_info[i].pwd_type == OTP_PWD_TYPE_SIN && g_dq_app_pwd_info[i].pwd_flag == 1)
		{
#ifdef __LOCK_USE_MALLOC__
			mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
			g_dq_otp_init.input_pwd_cb(4);
			return 4;
		}
		else if (g_dq_app_pwd_info[i].pwd_type == OTP_PWD_TYPE_PER && g_dq_app_pwd_info[i].pwd_flag == 1)
		{
#ifdef __LOCK_USE_MALLOC__
			mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
			ret = 1;
		}
		// 		else if (dq_otp_get_lock_sys_time_state() == 0 && mmi_dq_rtc_check_time() == 1)
		// 		{
		// #ifdef __LOCK_USE_MALLOC__
		// 			mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
		// #endif
		// 			g_dq_otp_init.input_pwd_cb(6);
		// 			return 6;
		// 		}
		else
		{
			uint8_t pwd[5] = {0xFF};
			memcpy(pwd, g_dq_app_pwd_info[i].app_pwd, 5);
#ifdef __LOCK_USE_MALLOC__
			mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
			pwd_len = dq_otp_get_pwd_len(pwd, 5);
			ret = dq_otp_temp_pwd_check(pwd, pwd_len);
			if (ret == 0)
				ret = 4;
		}

		if (ret == 1)
		{
#ifdef __LOCK_USE_MALLOC__
			g_dq_app_pwd_info = (otp_base_app_pwd_info *)mmi_dq_fs_get_storage(DQ_FS_MEM_APP_PWD, &ret_t);
#endif
			if (g_dq_app_pwd_info[i].pwd_flag == 0)
			{
				g_dq_app_pwd_info[i].pwd_flag = 1;
				dq_app_pwd_flag = 1;
//				g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_PWD_APP, (unsigned char *)g_dq_app_pwd_info, sizeof(otp_base_app_pwd_info) * DQ_OTP_APP_PWD_NUM, &ret, dq_otp_app_pwd_save_cb);
			}
#ifdef __LOCK_USE_MALLOC__
			mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
		}
	}
	else
	{
		for (i = 0; i < DQ_OTP_APP_PWD_NUM; i++)
		{
			if (g_dq_app_pwd_info[i].pwd_type != 0xFF)
			{
				for (k = 0; k < 5; k++)
				{
					if (password[k] != g_dq_app_pwd_info[i].app_pwd[k])
						break;
				}
				if (k == 5)
				{
					break;
				}
			}
		}
#ifdef __LOCK_USE_MALLOC__
		mmi_dq_fs_free_storage(DQ_FS_MEM_APP_PWD, (void **)&g_dq_app_pwd_info);
#endif
		if (i < DQ_OTP_APP_PWD_NUM)
			ret = 4;
		else
			ret = dq_otp_temp_pwd_check(password, len);
	}

	if (ret == 1)
		dq_otp_add_temp_open_log(0, DQ_OPEN_LOG_USER_PASSWORD, password, 5);
	else if (ret == 2)
	{
		dq_otp_add_alarm_log(DQ_ALART_LOG_EMPTY_PASSWORD);
	}

	if (dq_app_pwd_flag == 0)
		g_dq_otp_init.input_pwd_cb(ret);
	else
		dq_app_pwd_for_open_ret = ret;

	return ret;
}

/*
parameter: 
	current status machine
return :
	none
*/
unsigned short dq_otp_gen_random_key(unsigned char *old_key, unsigned char len, unsigned char mod_flag)
{
	unsigned char i = 0;
	unsigned short odd_val = 0;
	unsigned short even_val = 0;
	unsigned char ret_val = 0;
	for (i = 0; i < len; i++)
	{
		odd_val += (*(old_key + i)) * 2;
		i++;
		if (i == len)
			break;
		else
			even_val += (*(old_key + i)) * (*(old_key + i));
	}
	ret_val = (odd_val + even_val) % mod_flag;
	return ret_val;
}

/*
parameter: 
	current status machine
return :
	none
*/
unsigned short dq_otp_get_pwd_time_hour(unsigned char *in_pwd, unsigned char len, unsigned char offset)
{
	unsigned char time_h[5];
	unsigned char i = 0;
	unsigned int time_pwd = 0;

	memset(time_h, 0x00, sizeof(time_h));
	for (i = 0; i < len; i++)
	{
		time_h[i] = in_pwd[i + offset];
	}

	time_pwd = dq_sdk_CharToInt(time_h, 5);

	return time_pwd;
}

/*
parameter: 
	current status machine
return :
	none
*/
unsigned char dq_otp_get_sys_time_hour(unsigned short *sys_hour)
{
	unsigned int sec_time; //= (1552471200-DQ_OTP_START_TIME_SEC);
						   //	unsigned char week_info;

	//g_dq_otp_init.get_lock_time_info(&sec_time,&week_info);

	sec_time = dq_otp_get_sys_time_sec() - otp_set_info.otp_start_hour;

	*sys_hour = sec_time / 3600;

//	NRF_LOG_PRINTF_DEBUG("otp_set_info.otp_start_hour %d\n", otp_set_info.otp_start_hour);

//	NRF_LOG_PRINTF_DEBUG("sec_timer %d\n", sec_time);

	return 1;
}

unsigned char dq_otp_get_local_sys_time_hour(unsigned short *sys_hour)
{
	uint32_t sec_time; //= (1552471200-DQ_OTP_START_TIME_SEC);
					   //	unsigned char week_info;

	//g_dq_otp_init.get_lock_time_info(&sec_time,&week_info);

	sec_time = dq_otp_get_sys_time_sec();
	g_dq_otp_init.time_zone_pro(&sec_time);

	*sys_hour = (sec_time / 3600) % 24;
	return 1;
}

/*
parameter: 
	current status machine
return :
	none
*/
unsigned char dq_otp_check_time_by_hour(unsigned char *in_pwd, unsigned char len, unsigned short delay_time)
{
	unsigned short pwd_hour = 0;
	unsigned short sys_hour = 0;
	unsigned char ret_val = 0;
	pwd_hour = dq_otp_get_pwd_time_hour(in_pwd, len, 1);
	ret_val = dq_otp_get_sys_time_hour(&sys_hour);

//	NRF_LOG_PRINTF_DEBUG("dq_otp_check_time_by_hour %d  %d\n", pwd_hour, sys_hour);

	if (ret_val == 1)
	{
		//if(otp_set_info.otp_empty_hour == 0xFFFF || pwd_hour >= otp_set_info.otp_empty_hour)
		//{
		if (delay_time == 0)
			ret_val = 1;
		else
		{
			if ((sys_hour >= pwd_hour) && (sys_hour < (pwd_hour + delay_time)))
			{
				ret_val = 1;
			}
			else
			{
				ret_val = 0;
			}
		}
		//}
		//else
		//	ret_val = 0;
	}
	return ret_val;
}

/*
parameter: 
	current status machine
return :
	none
*/
unsigned char dq_otp_check_per_pwd_verify(unsigned char *dec_pwd, unsigned char *pwd_in, unsigned char len)
{
	unsigned char ret_val = 0;
	unsigned char new_pwd_flag = 0;

//	NRF_LOG_PRINTF_DEBUG("dq_otp_check_per_pwd_verify\n");
	if (dec_pwd[0] != 0x09)
		return 0;

	ret_val = dq_otp_check_password(pwd_in);
	if (ret_val == 0)
	{
		ret_val = dq_otp_check_time_by_hour(dec_pwd, 5, 24); //per pwd must in 24 hours
		new_pwd_flag = 1;
	}
	if (ret_val == 1)
	{
		if (new_pwd_flag == 1)
		{
			//save pwd
			if (dq_otp_save_temp_pwd(pwd_in, OTP_PWD_TYPE_PER) != 0)
				ret_val = 5;
		}
	}

	return ret_val;
}

/*
parameter: 
	current status machine
return :
	none
*/
unsigned char dq_otp_check_empty_pwd_verify(unsigned char *in_pwd, unsigned char len)
{
	unsigned short check_code = 0;
	unsigned short check_code2 = 0;
	unsigned char ver_code[2];
	unsigned char ret_val = 0;

	if (in_pwd[0] != 0x07)
		return 0;

	memset(ver_code, 0x00, sizeof(ver_code));

	check_code = dq_otp_gen_random_key(in_pwd, 6, 100);
	ver_code[0] = in_pwd[6];
	ver_code[1] = in_pwd[7];

	check_code2 = dq_sdk_CharToInt(ver_code, 2);
	if (check_code == check_code2)
	{
		ret_val = dq_otp_check_time_by_hour(in_pwd, 5, 2); //empty pwd must in 2 hours
		if (ret_val == 1)
		{
			//clear password
			//unsigned short pwd_hour = 0;
			//uint8_t ret = 0;
#ifdef __LOCK_VIRTUAL_PASSWORD__
			extern uint8_t input_empty_pwd_len;
			if (input_empty_pwd_len == 1)
#endif
			{
				//pwd_hour = dq_otp_get_pwd_time_hour(in_pwd,5,1);
				//otp_set_info.otp_empty_hour = pwd_hour;
				//g_dq_otp_init.fds_write(DQ_OTP_FILE_ID_SET,(unsigned char *)&otp_set_info,sizeof(otp_base_setting_info),&ret,dq_otp_fds_clear_app_pwd_by_password);
				dq_otp_fds_clear_app_pwd_by_password();
				return 2;
			}
		}
	}
	return 0;
}
/*
parameter: 
	current status machine
return :
	none
*/
#if 0
uint32_t dq_otp_get_sin_pwd_endtime(unsigned char *dec_pwd,unsigned char len)
{
	unsigned short pwd_hour = 0;
	time_t pwd_time_sec = 0;
	time_t end_time_sec = 0;
	unsigned char time_y,time_m,time_h,time_d;


	if(dec_pwd[0] != 0x08)
		return 0;
	pwd_hour = dq_otp_get_pwd_time_hour(dec_pwd,5,1);
	dq_otp_get_pwd_time_ymd(pwd_hour,&time_y,&time_m,&time_d,&time_h);
	pwd_time_sec = dq_otp_get_pwd_time_sec(time_y,time_m,time_d,time_h);

	end_time_sec = pwd_time_sec + 6*3600;
		
	return end_time_sec;
}
#endif
unsigned char dq_otp_check_sin_pwd_verify(unsigned char *dec_pwd, unsigned char *pwd_in, unsigned char len)
{
	unsigned char ret_val = 0;
//	NRF_LOG_PRINTF_DEBUG("dq_otp_check_sin_pwd_verify\n");

	if (dec_pwd[0] != 0x08)
		return 0;

	ret_val = dq_otp_check_time_by_hour(dec_pwd, 5, 6); //sin pwd must in 6 hours

	if (ret_val == 1)
	{
		ret_val = dq_otp_check_password(pwd_in);
		if (ret_val == 0)
		{
			//if not out 6 hour ,save pwd
			if (dq_otp_save_temp_pwd(pwd_in, OTP_PWD_TYPE_SIN) == 0) //, dq_otp_get_sin_pwd_endtime(dec_pwd, len));
				ret_val = 1;
			else
				ret_val = 5;
		}
	}
	return ret_val;
}

/*
parameter: 
	current status machine
return :
	none
*/
unsigned char dq_otp_adjust_sys_time_verify(unsigned char *in_pwd, unsigned char len)
{
	unsigned char dec_pwd_char[10];
	unsigned char vercode[2];
	unsigned short very_code, very_code2;
	//unsigned int time;
	//unsigned char ret_val;

	memset(dec_pwd_char, 0x00, sizeof(dec_pwd_char));
	//dq_otp_enc_pwd(in_pwd,8,otp_set_info.otp_sec_key,otp_set_info.otp_exg_key,dec_pwd_char);

	vercode[0] = in_pwd[8];
	vercode[1] = in_pwd[9];

	very_code = dq_sdk_CharToInt(vercode, 2);

	very_code2 = dq_otp_gen_random_key(in_pwd, 8, 100);

	if (very_code == very_code2)
	{
		//time = dq_sdk_CharToInt(dec_pwd_char, 8);
		//g_dq_otp_init.set_system_time(time*60,&ret_val);

		return 3;
	}
	return 0;
}
#if 0
extern void mmi_dq_rtc_set_time_test(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute);
unsigned char dq_otp_adjust_sys_time_verify_for_test(unsigned char *in_pwd,unsigned char len)
{
	unsigned char vercode[2];
	unsigned char year,month,day,hour,minute;
	unsigned char i;
	unsigned char pwd[11];

	dq_sdk_CharToHexByte(in_pwd, (char *)pwd, 10);

	for(i=0;i<len;i++)
		NRF_LOG_PRINTF_DEBUG("dq_otp_adjust_sys_time_verify_for_test%d :%d\n",i,pwd[i]);


	vercode[0] = pwd[0];
	vercode[1] = pwd[1];

	year = dq_sdk_CharToInt(vercode,2);

	vercode[0] = pwd[2];
	vercode[1] = pwd[3];
	month = dq_sdk_CharToInt(vercode,2);

	vercode[0] = pwd[4];
	vercode[1] = pwd[5];
	day = dq_sdk_CharToInt(vercode,2);

	vercode[0] = pwd[6];
	vercode[1] = pwd[7];
	hour = dq_sdk_CharToInt(vercode,2);	

	vercode[0] = pwd[8];
	vercode[1] = pwd[9];
	minute = dq_sdk_CharToInt(vercode,2);

	NRF_LOG_PRINTF_DEBUG("dq_otp_adjust_sys_time_verify_for_test:%d %d %d %d %d\n",year,month, day, hour, minute );
	
	mmi_dq_rtc_set_time_test(year, month, day, hour, minute);
	return 3;
}
#endif
/*
parameter: 
	current status machine
return :
	none
*/
uint32_t dq_otp_get_lim_pwd_end_time(unsigned char *dec_pwd, unsigned char len)
{
	unsigned int hour_time = 0;
	time_t pwd_time_sec = 0;
	time_t end_time_sec = 0;
	unsigned char time_y, time_m, time_h, time_d;
	unsigned int delay_time = 0;
	unsigned char i = 0;
	unsigned char time_buf[4];

	if (dec_pwd[0] > 5)
		return 0;

	hour_time = dq_otp_get_pwd_time_hour(dec_pwd, 5, 0);
//	dq_otp_get_pwd_time_ymd(hour_time, &time_y, &time_m, &time_d, &time_h);
	pwd_time_sec = dq_otp_get_pwd_time_sec(time_y, time_m, time_d, time_h);

	if (len == 8)
	{
		if (dec_pwd[5] <= 4) //T<= 20 day 0-499 hour
		{
			for (i = 0; i < 3; i++)
			{
				time_buf[i] = dec_pwd[5 + i];
			}
			delay_time = dq_sdk_CharToInt(time_buf, 3);

			end_time_sec = pwd_time_sec + delay_time * 3600;
		}
		else if ((dec_pwd[5] >= 5) && (dec_pwd[5] <= 8)) // 500-899 for day
		{
			for (i = 0; i < 3; i++)
			{
				time_buf[i] = dec_pwd[5 + i];
			}
			delay_time = dq_sdk_CharToInt(time_buf, 3);
			delay_time = (delay_time - 500) * 24;

			end_time_sec = pwd_time_sec + delay_time * 3600;
		}
		else if (dec_pwd[5] == 9) //900-999 for month
		{
			for (i = 0; i < 2; i++)
			{
				time_buf[i] = dec_pwd[6 + i];
			}
			delay_time = dq_sdk_CharToInt(time_buf, 2);
			time_y += (time_m + delay_time) / 12;
			time_m = (time_m + delay_time) % 12;
			end_time_sec = dq_otp_get_pwd_time_sec(time_y, time_m, time_d, time_h);
		}
	}
	else if (len == 9)
	{
		for (i = 0; i < 4; i++)
		{
			time_buf[i] = dec_pwd[5 + i];
		}
		delay_time = dq_sdk_CharToInt(time_buf, 4);
		end_time_sec = pwd_time_sec + delay_time * 3600;
	}

	return end_time_sec;
}

unsigned char dq_otp_check_lim_pwd_verify(unsigned char *dec_pwd, unsigned char *pwd_in, unsigned char len)
{
	unsigned char i = 0;
	unsigned char ret_val = 0, ret_code = 0;
	unsigned short sys_hour = 0;
	unsigned int hour_time = 0;
	unsigned char time_buf[4];
	unsigned int delay_time = 0;
	unsigned char time_y, time_m, time_h, time_d;
	time_t pwd_time_sec;
	time_t sys_time_sec;
	time_t end_time_sec;

//	NRF_LOG_PRINTF_DEBUG("dq_otp_check_lim_pwd_verify\n");
	if (dec_pwd[0] > 5)
		return 0;

	dq_otp_get_sys_time_hour(&sys_hour);
	hour_time = dq_otp_get_pwd_time_hour(dec_pwd, 5, 0);
	ret_val = dq_otp_check_password(pwd_in);

	//if(otp_set_info.otp_empty_hour != 0xFFFF&&hour_time < otp_set_info.otp_empty_hour)
	//	return 0;

	if (ret_val == 0) //not used
	{
		if ((sys_hour >= hour_time) && (sys_hour < hour_time + 24))
		{
			ret_code = 1;
		}
		else
		{
			return 0;
		}
	}
	memset(time_buf, 0x00, sizeof(time_buf));
	if (len == 8)
	{
		if (dec_pwd[5] <= 4) //T<= 20 day 0-499 hour
		{
			for (i = 0; i < 3; i++)
			{
				time_buf[i] = dec_pwd[5 + i];
			}
			delay_time = dq_sdk_CharToInt(time_buf, 3);
			if ((sys_hour >= hour_time) && (sys_hour <= (hour_time + delay_time)))
			{
				ret_code = 1;
			}
			else
			{
				ret_code = 0;
			}
		}
		else if ((dec_pwd[5] >= 5) && (dec_pwd[5] <= 8)) // 500-899 for day
		{
			for (i = 0; i < 3; i++)
			{
				time_buf[i] = dec_pwd[5 + i];
			}
			delay_time = dq_sdk_CharToInt(time_buf, 3);
			delay_time = (delay_time - 500) * 24;
			if ((sys_hour >= hour_time) && (sys_hour <= (hour_time + delay_time)))
			{
				ret_code = 1;
			}
			else
			{
				ret_code = 0;
			}
		}
		else if (dec_pwd[5] == 9) //900-999 for month
		{
//			dq_otp_get_pwd_time_ymd(hour_time, &time_y, &time_m, &time_d, &time_h);
			pwd_time_sec = dq_otp_get_pwd_time_sec(time_y, time_m, time_d, time_h);

			for (i = 0; i < 2; i++)
			{
				time_buf[i] = dec_pwd[6 + i];
			}
			delay_time = dq_sdk_CharToInt(time_buf, 2);
			time_y += (time_m + delay_time) / 12;
			time_m = (time_m + delay_time) % 12;
			end_time_sec = dq_otp_get_pwd_time_sec(time_y, time_m, time_d, time_h);
			sys_time_sec = dq_otp_get_sys_time_sec();
			if ((sys_time_sec >= pwd_time_sec) && (sys_time_sec <= end_time_sec))
				ret_code = 1;
			else
				ret_code = 0;
		}
	}
	else if (len == 9)
	{
		for (i = 0; i < 4; i++)
		{
			time_buf[i] = dec_pwd[5 + i];
		}
		delay_time = dq_sdk_CharToInt(time_buf, 4);
		if ((sys_hour >= hour_time) && (sys_hour <= hour_time + delay_time))
		{
			ret_code = 1;
		}
		else
		{
			ret_code = 0;
		}
	}
	if ((ret_val == 0) && (ret_code == 1))
	{
		//save pwd
		if (dq_otp_save_temp_pwd(pwd_in, OTP_PWD_TYPE_LIM) != 0) //, dq_otp_get_lim_pwd_end_time(dec_pwd, len));
			ret_code = 5;
	}
	return ret_code;
}

/*
parameter: 
	current status machine
return :
	none
*/
unsigned char dq_otp_loop_pwd_check_week_verify(unsigned char loop_flag, unsigned char week_info)
{
	if (loop_flag < 6) //week 1-6
	{

		if (loop_flag == (week_info - 1))
			return 1;
		else
			return 0;
	}
	else if (loop_flag == 0x06)
	{
		if (week_info == 0)
			return 1;
		else
			return 0;
	}
	else if (loop_flag == 0x07) //work day,1-5
	{
		if ((week_info >= 1) && (week_info <= 5))
			return 1;
		else
			return 0;
	}
	else if (loop_flag == 0x08) //week end 6.7
	{
		if ((week_info == 6) || (week_info == 0))
			return 1;
		else
			return 0;
	}
	else if (loop_flag == 0x09) //every day
	{
		if (week_info <= 6)
			return 1;
		else
			return 0;
	}
	return 1;
}
/*
parameter: 
	current status machine
return :
	none
*/
unsigned char dq_otp_check_loop_pwd_verify(unsigned char *dec_pwd, unsigned char *pwd_in, unsigned char len)
{
	unsigned char loop_val[2];
	unsigned char ret_val = 0;
	//	unsigned int	delay_time;
	unsigned char week;
	unsigned char end_time = 0;
	unsigned char week_info = dec_pwd[8];
	unsigned short sys_hour = 0;
	unsigned short sys_hour2 = 0;
	unsigned short pwd_hour = dq_otp_get_pwd_time_hour(dec_pwd, 5, 1);
	unsigned short pwd_hour2 = 0;
	uint32_t pwd_sec = 0;
	unsigned char new_pwd_flag = 0;

//	NRF_LOG_PRINTF_DEBUG("dq_otp_check_loop_pwd_verify\n");

	if (dec_pwd[0] != 0x06)
		return 0;

	//if(otp_set_info.otp_empty_hour != 0xFFFF&&pwd_hour < otp_set_info.otp_empty_hour)
	//	return 0;

	memset(loop_val, 0x00, sizeof(loop_val));

	loop_val[0] = dec_pwd[6];
	loop_val[1] = dec_pwd[7];

	end_time = dq_sdk_CharToInt(loop_val, 2);
	ret_val = dq_otp_check_password(pwd_in);
	if (ret_val == 0)
	{
		ret_val = dq_otp_get_sys_time_hour(&sys_hour);
		if (ret_val == 1)
		{
			if ((sys_hour >= pwd_hour) && (sys_hour < (pwd_hour + 24)))
			{
				ret_val = 1;
			}
			else
				ret_val = 0;
		}
		if (ret_val == 1)
			new_pwd_flag = 1;
	}
	else
		ret_val = dq_otp_get_sys_time_hour(&sys_hour);

	if (ret_val == 1)
	{
		pwd_sec = (pwd_hour * 3600 + otp_set_info.otp_start_hour);
		g_dq_otp_init.time_zone_pro(&pwd_sec);
		pwd_hour2 = (pwd_sec / 3600) % 24;
		dq_otp_get_local_sys_time_hour(&sys_hour2);
//		NRF_LOG_PRINTF_DEBUG("dq_otp_check_loop_pwd_verify pwd_hour2 %d  ; sys_hour2 %d ;   end_time %d\n", pwd_hour2, sys_hour2, end_time);
		if ((sys_hour2 >= pwd_hour2) && (sys_hour2 <= end_time))
		{
			// week = dq_otp_get_sys_local_time_week();
			// NRF_LOG_PRINTF_DEBUG("dq_otp_check_loop_pwd_verify week %d\n", week);

			ret_val = dq_otp_loop_pwd_check_week_verify(week_info, week);
		}
		else
			ret_val = 0;
	}
	if ((new_pwd_flag == 1) && (ret_val == 1))
	{
		//save the passowrd
		if (dq_otp_save_temp_pwd(pwd_in, OTP_PWD_TYPE_LOOP) != OTP_BASE_SUCESS)
			ret_val = 5;
	}
	return ret_val;
}
/*
Function:check the password user input right or not
parameter: 
	pwd_in: the password user input
return :
	if the password can be verify,return OTP_BASE_SUCESS
	if the password can't be verify,return OTP_BASE_PWD_NOT_FOUND
*/
uint8_t dq_otp_temp_pwd_check(unsigned char *pwd_in, unsigned char len)
{
	unsigned char hex_pwd_char[15];
	unsigned char dec_pwd_char[15];
	unsigned char sec_pwd_char[10];
	unsigned char ret_val = 0;
	unsigned char i = 0;

	memset(hex_pwd_char, 0x00, sizeof(hex_pwd_char));
	memset(dec_pwd_char, 0x00, sizeof(dec_pwd_char));
	memset(sec_pwd_char, 0x00, sizeof(sec_pwd_char));

	if ((len == 8) || (len == 9))
	{
		//dq_sdk_HexCharToByte((const char *)pwd_in,(unsigned char *)hex_pwd_char,len);
		if (len == 8)
			dq_otp_enc_pwd(pwd_in, len, otp_set_info.otp_sec_key_8, otp_set_info.otp_exg_key_8, dec_pwd_char);
		else if (len == 9)
			dq_otp_enc_pwd(pwd_in, len, otp_set_info.otp_sec_key_9, otp_set_info.otp_exg_key_9, dec_pwd_char);
		// else if (len == 10)
		// 	dq_otp_enc_pwd(pwd_in, len, otp_set_info.otp_sec_key_10, otp_set_info.otp_exg_key_10, dec_pwd_char);

		for (i = 0; i < len; i++)
		{
//			NRF_LOG_PRINTF_DEBUG("dq_otp_temp_pwd_check [%d]=  %d\n", i, dec_pwd_char[i]);
		}

		if (dec_pwd_char[0] <= 5)
		{
			ret_val = dq_otp_check_lim_pwd_verify(dec_pwd_char, pwd_in, len);
		}
		else if (dec_pwd_char[0] == 6)
		{
			ret_val = dq_otp_check_loop_pwd_verify(dec_pwd_char, pwd_in, len);
		}
		else if (dec_pwd_char[0] == 7)
		{
			ret_val = dq_otp_check_empty_pwd_verify(dec_pwd_char, len);
		}
		else if (dec_pwd_char[0] == 8)
		{
			ret_val = dq_otp_check_sin_pwd_verify(dec_pwd_char, pwd_in, len);
		}
		else if (dec_pwd_char[0] == 9)
		{
			ret_val = dq_otp_check_per_pwd_verify(dec_pwd_char, pwd_in, len);
		}
	}
#if 0
	else if(len == 10)//adjust system time
	{
#if 1
		ret_val = dq_otp_adjust_sys_time_verify(pwd_in,len);
#else
		
		ret_val = dq_otp_adjust_sys_time_verify_for_test(pwd_in,len);
#endif
	}
#endif
#if 0
	if(ret_val == 1)
		return OTP_BASE_SUCESS;
	else
		return OTP_BASE_NOT_PAIR;
#else
	return ret_val;
#endif
}

/*
parameter: 
	current status machine
return :
	none
*/
#if 0
//unsigned char user_input_pwd[] = {0x06,0x06,0x07,0x03,0x08,0x08,0x02,0x02};//66738822--01721940
//unsigned char user_input_pwd[] = {0x06,0x04,0x07,0x02,0x03,0x00,0x06,0x05,0x03};//647230653--017228761
//unsigned char user_input_pwd[] = {0x06,0x07,0x03,0x03,0x09,0x05,0x01,0x03};//67339513--01721051
//unsigned char user_input_pwd[] = {0x07,0x05,0x04,0x07,0x07,0x04,0x06,0x08,0x01};//754774681--601721189
//unsigned char user_input_pwd[] = {0x05,0x04,0x09,0x09,0x01,0x03,0x03,0x02};//54991332--70172170
//unsigned char user_input_pwd[] = {0x08,0x05,0x03,0x02,0x08,0x01,0x03,0x04};//85328134--
//unsigned char user_input_pwd[] = {0x6,0x5,0x01,0x07,0x01,0x05,0x06,0x07};//65171567--01724023
unsigned char user_input_pwd[] = {0x2,0x1,0x8,0x3,0x1,0x3,0x1,0x8,0x5};//218313185--
unsigned char temp_exg_key_8[] = {0x43,0x26,0x71,0x58};
unsigned char temp_sec_key_8[] = {0x50,0x72,0x29,0x98};
unsigned char temp_exg_key_9[] = {0x93,0x42,0x67,0x15,0x8F}; 
unsigned char temp_sec_key_9[] = {0x15,0x02,0x23,0x28,0x6F};
int main(void)
{
	unsigned char sec_pwd[9];
	unsigned char pwd_ret =0;
	unsigned short pwd_short_ret = 0;
	unsigned char pwd_len = 9;
	memset(sec_pwd,0xFF,sizeof(sec_pwd));
	memset(otp_set_info.otp_exg_key,0x00,sizeof(otp_set_info.otp_exg_key));
	memset(otp_set_info.otp_sec_key,0x00,sizeof(otp_set_info.otp_sec_key));
	if(pwd_len == 8)
	{
		memcpy(otp_set_info.otp_sec_key,temp_sec_key_8,4);
		memcpy(otp_set_info.otp_exg_key,temp_exg_key_8,4);
	}
	else if(pwd_len == 9)
	{
		memcpy(otp_set_info.otp_sec_key,temp_sec_key_9,5);
		memcpy(otp_set_info.otp_exg_key,temp_exg_key_9,5);
	}

	dq_otp_pwd_check(user_input_pwd,pwd_len);
}
#endif

#endif //__LOCK_VIRTUAL_PASSWORD__
#endif //__DQ_SDK_MAIN_C__
