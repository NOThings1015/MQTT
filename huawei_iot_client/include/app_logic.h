/********************************************************************************
 *      Copyright:  (C) 2025 Dameng Studio
 *                  All rights reserved.
 *
 *       Filename:  app_logic.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2025年08月05日)
 *         Author:  Yang jiayu <3381322051@qq.com>
 *      ChangeLog:  1, Release initial version on "2025年08月05日 17时51分00秒"
 *                 
 ********************************************************************************/
#ifndef APP_LOGIC_H
#define APP_LOGIC_H

#include "core_mqtt.h"

// 事件回调
void eventCallback(MQTTContext_t *context, MQTTPacketInfo_t *packetInfo, 
		                  MQTTDeserializedInfo_t *deserializedInfo);

// 上报温湿度
void report_temperature_humidity(void);

// 发送命令响应
void send_command_response(char *request_id, int result_code);

#endif // APP_LOGIC_H

