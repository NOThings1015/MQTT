/********************************************************************************
 *      Copyright:  (C) 2025 Dameng Studio
 *                  All rights reserved.
 *
 *       Filename:  mqtt_client.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2025年08月05日)
 *         Author:  Yang jiayu <3381322051@qq.com>
 *      ChangeLog:  1, Release initial version on "2025年08月05日 16时30分45秒"
 *                 
 ********************************************************************************/
#ifndef MQTT_CLIENT_H_
#define MQTT_CLIENT_H_

#include "core_mqtt.h"
#include "transport_interface.h"


int mqtt_client_init(void);// 初始化MQTT客户端


int mqtt_connect_to_huawei(void);// 连接华为云


int mqtt_subscribe_to_topics(void);// 订阅主题


void mqtt_run_main_loop(void);// 运行主循环


MQTTContext_t *get_mqtt_context(void);// 获取MQTT上下文


void mqtt_cleanup(void);// 清理MQTT资源

#endif // MQTT_CLIENT_H

