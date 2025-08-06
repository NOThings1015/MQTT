/*********************************************************************************
 *      Copyright:  (C) 2025 Dameng Studio
 *                  All rights reserved.
 *
 *       Filename:  mqtt_client.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2025年08月05日)
 *         Author:  Yang jiayu <3381322051@qq.com>
 *      ChangeLog:  1, Release initial version on "2025年08月05日 16时33分08秒"
 *                 
 ********************************************************************************/
#include "mqtt_client.h"
#include "network.h"
#include "config.h"
#include "app_logic.h"
#include <stdio.h>
#include <string.h>

static MQTTContext_t mqttContext = {0};
static uint8_t networkBuffer[NETWORK_BUFFER_SIZE];
static TransportInterface_t transport;


static uint32_t getTimeFunc(void);// 时间函数声明

int mqtt_client_init(void) 
{
	// 获取网络上下文
	struct NetworkContext *networkContext = get_network_context();
	// 配置传输接口
	transport.pNetworkContext = networkContext; // 关联网络上下文:提供华为云连接的实际状态和资源
	transport.recv = network_recv; // 设置数据接收回调函数
	transport.send = network_send; // 设置数据发送回调函数
	transport.writev = NULL;  // 不使用分散/聚集IO,显式设置为 NULL

	// 准备网络缓冲区  
	MQTTFixedBuffer_t fixedBuffer = {
		.pBuffer = networkBuffer,
		.size = NETWORK_BUFFER_SIZE
	};

	// 初始化MQTT上下文
	MQTTStatus_t status = MQTTBadParameter;
	status = MQTT_Init( &mqttContext, 
						&transport, 
						getTimeFunc, 
						eventCallback, 
						&fixedBuffer);
	if (status != MQTTSuccess) 
	{
		fprintf(stderr, "MQTT_Init failed: %d\n", status);
		return -1;
	}

	return 0;
}
		


int mqtt_connect_to_huawei(void) 
{	   
	// 设置连接信息
	MQTTConnectInfo_t connectInfo = {
		.pClientIdentifier = CLIENT_ID,
		.clientIdentifierLength = strlen(CLIENT_ID),
		.pUserName = USERNAME,
		.userNameLength = strlen(USERNAME),
		.pPassword = PASSWORD,
		.passwordLength = strlen(PASSWORD),
		.keepAliveSeconds = MQTT_KEEP_ALIVE_SEC,
		.cleanSession = MQTT_CLEAN_SESSION
	};

	printf("华为云连接参数检查：\n");
	printf("  CLIENT_ID: %s (%zu)\n", CLIENT_ID, strlen(CLIENT_ID));
	printf("  USERNAME: %s (%zu)\n", USERNAME, strlen(USERNAME));
	printf("  PASSWORD: %s (%zu)\n", PASSWORD, strlen(PASSWORD));

	// 连接华为云
	bool sessionPresent = false;
	MQTTStatus_t status = MQTT_Connect(&mqttContext, &connectInfo, NULL, 
			CONNECT_TIMEOUT_MS, &sessionPresent);

	if (status != MQTTSuccess) 
	{
		fprintf(stderr, "MQTT_Connect failed: %d\n", status);
		return -1;
	}

	// 初始化时间戳
	mqttContext.lastPacketRxTime = getTimeFunc();
	mqttContext.lastPacketTxTime = getTimeFunc();
	printf("Connected to Huawei Cloud IoT platform\n");
	return 0;
}



int mqtt_subscribe_to_topics(void) 
{
	// 配置订阅信息
	MQTTSubscribeInfo_t subInfo = {
		.qos = MQTTQoS0,
		.pTopicFilter = TOPIC,
		.topicFilterLength = strlen(TOPIC)
	};

	// 订阅主题
	MQTTStatus_t status = MQTT_Subscribe(&mqttContext, &subInfo, 1, SUBSCRIBE_TIMEOUT_MS);
	if (status != MQTTSuccess) 
	{
		fprintf(stderr, "MQTT_Subscribe failed: %d\n", status);
		return -1;
	}

	printf("Subscribed to topic: %s\n", TOPIC);
	return 0;
}



void mqtt_run_main_loop(void) 
{
	while (1) 
	{
		MQTTStatus_t status = MQTT_ProcessLoop(&mqttContext);
		
		if (status != MQTTSuccess && status != MQTTNeedMoreBytes) 
		{
			fprintf(stderr, "MQTT_ProcessLoop failed: %d\n", status);
			break;
		}
	}
}


MQTTContext_t *get_mqtt_context(void) 
{
	    return &mqttContext;
}


void mqtt_cleanup(void) 
{
	    // MQTT库可能有自己的清理函数
		// 这里根据自己实际库实现
		
}


static uint32_t getTimeFunc(void) 
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}





