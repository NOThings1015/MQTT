/********************************************************************************
 *      Copyright:  (C) 2025 Dameng Studio
 *                  All rights reserved.
 *
 *       Filename:  network.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2025年08月05日)
 *         Author:  Yang jiayu <3381322051@qq.com>
 *      ChangeLog:  1, Release initial version on "2025年08月05日 16时12分46秒"
 *                 
 ********************************************************************************/
#ifndef NETWORK_H
#define NETWORK_H

#include <openssl/ssl.h>
#include <stdint.h>
#include <stddef.h>
#include <core_mqtt_config.h>

int network_init(void); // 网络初始化

int network_connect(void); // 网络连接


int32_t network_recv(struct NetworkContext *context, void *buffer, size_t len);// 网络数据接收


int32_t network_send(struct NetworkContext *context, const void *buffer, size_t len);// 网络发送函数


void network_cleanup(void); // 网络清理

struct NetworkContext *get_network_context(void); // 获取网络上下文

#endif // NETWORK_H
