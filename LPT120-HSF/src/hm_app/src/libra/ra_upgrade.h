/**
* @file         ra_upgrade.h
* @brief        OTA升级接口
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __RA_UPGRADE_H__
#define __RA_UPGRADE_H__
#include "ra_types.h"


/**
* ra_dev_upgrade_success_callback_t 函数原型定义
*/
typedef void(*ra_dev_upgrade_success_callback_t)(void);

/**
* ra_dev_upgrade_failed_callback_t 函数原型定义
*/
typedef void(*ra_dev_upgrade_failed_callback_t)(int error_code);

/**
* 启动模块OTA升级
* (执行完毕不自动重启)
* @param[in]   dev_tid  模块TID
* @param[in]   bin_url  固件下载地址(最大长度1024)
* @param[in]   md5      固件MD5校验值
* @param[in]   bin_type 固件类型
* @retval	0        开始升级
* @retval	-3       参数错误
* @note OTA升级成功后须调用ra_system_upgrade_reboot运行新版本固件
*/
int ra_start_dev_upgrade(char *dev_tid, char *bin_url, char *md5, char *bin_type);

/**
* 升级成功后系统重启
* @note 该函数可在ra_register_dev_upgrade_success_callback函数中调用
*/
void ra_system_upgrade_reboot(void);

/**
* 注册升级成功回调函数
* @param[in]  callback 升级成功回调函数
*/
void ra_register_dev_upgrade_success_callback(ra_dev_upgrade_success_callback_t callback);

/**
* 注册升级失败回调函数
* @param[in]  callback 升级失败回调函数
*/
void ra_register_dev_upgrade_failed_callback(ra_dev_upgrade_failed_callback_t callback);

#endif