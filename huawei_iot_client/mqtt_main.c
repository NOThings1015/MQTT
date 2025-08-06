/*********************************************************************************
 *      Copyright:  (C) 2025 Dameng Studio
 *                  All rights reserved.
 *
 *       Filename:  mqtt_main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2025年08月05日)
 *         Author:  Yang jiayu <3381322051@qq.com>
 *      ChangeLog:  1, Release initial version on "2025年08月05日 18时11分24秒"
 *                 
 ********************************************************************************/
#include "network.h"
#include "mqtt_client.h"
#include <openssl/ssl.h>
#include "app_logic.h"
#include <stdio.h>
#include "config.h"

int main() 
{
	// 1. 初始化网络
	if (network_init() != 0) 
	{
		fprintf(stderr, "Network initialization failed\n");
		return 1;
	}

	// 2. 建立网络连接
	if (network_connect() != 0) 
	{
		fprintf(stderr, "Network connection failed\n");
		network_cleanup();
		return 1;
	}


	// 3. 初始化MQTT客户端
	if (mqtt_client_init() != 0) 
	{
		fprintf(stderr, "MQTT client initialization failed\n");
		network_cleanup();
		return 1;
	}


	// 4. 连接华为云平台
	if (mqtt_connect_to_huawei() != 0) 
	{
		fprintf(stderr, "Connection to Huawei Cloud failed\n");
		mqtt_cleanup();
		network_cleanup();
		return 1;
	}


	// 5. 订阅主题
	if (mqtt_subscribe_to_topics() != 0) 
	{
		fprintf(stderr, "Topic subscription failed\n");
		mqtt_cleanup();
		network_cleanup();
		return 1;
	}

	
	// 6. 运行主循环
	mqtt_run_main_loop();


	// 7. 清理资源
	mqtt_cleanup();
	network_cleanup();

	return 0;
}

