/*********************************************************************************
 *      Copyright:  (C) 2025 Dameng Studio
 *                  All rights reserved.
 *
 *       Filename:  app_logic.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2025年08月05日)
 *         Author:  Yang jiayu <3381322051@qq.com>
 *      ChangeLog:  1, Release initial version on "2025年08月05日 17时33分30秒"
 *                 
 ********************************************************************************/
#include "app_logic.h"
#include "config.h"
#include "mqtt_client.h"
#include <stdio.h>
#include <string.h>

void eventCallback(MQTTContext_t *context, MQTTPacketInfo_t *packetInfo, 
		MQTTDeserializedInfo_t *deserializedInfo) 
{
	if (packetInfo->type == MQTT_PACKET_TYPE_PINGRESP) 
	{
		printf("PINGRESP received\n");
	}
	
	if (packetInfo->type == MQTT_PACKET_TYPE_DISCONNECT)
	{
		printf("Received DISCONNECT packet from server\n");
	}

	if (packetInfo->type == MQTT_PACKET_TYPE_PUBLISH) 
	{
		const char *topic = deserializedInfo->pPublishInfo->pTopicName;
		int topicLen = (int)deserializedInfo->pPublishInfo->topicNameLength;
		const char *payload = deserializedInfo->pPublishInfo->pPayload;
		int payloadLen = (int)deserializedInfo->pPublishInfo->payloadLength;

		printf("Received message on topic: %.*s\n", topicLen, topic);
		printf("Message: %.*s\n", payloadLen, payload);

		// 华为云命令处理
		if (strstr(topic, "/sys/commands/") != NULL) 
		{
			printf("Command received! Parsing...\n");

			// 解析命令内容
			char *service_ptr = strstr(payload, "\"service_id\":\"");
			char *command_ptr = strstr(payload, "\"command_name\":\"");
			char *switch_ptr = strstr(payload, "\"Switch\":\"");
			char *request_ptr = strstr(topic, "request_id");

			// 提取命令请求ID 名称
			char request_id[128] = {0};
			if (request_ptr) 
			{
				sscanf(request_ptr + 11, "%52[^\{]", request_id);

			}

			printf("Parsed command: service_ptr=%s, command_ptr=%s, switch_ptr=%s\n",              service_ptr, command_ptr, switch_ptr);

			if (service_ptr && command_ptr && switch_ptr) 
			{
				printf("Enter temperature sample\n");

				// 提取服务ID
				char service_id[32] = {0};
				sscanf(service_ptr + 14, "%31[^\"]", service_id);

				// 提取命令名称
				char command_name[32] = {0};
				sscanf(command_ptr + 16, "%31[^\"]", command_name);

				// 提取开关状态
				char switch_state[8] = {0};
				sscanf(switch_ptr + 10, "%7[^\"]", switch_state);

				printf("Parsed command: service_id=%s, command_name=%s, Switch=%s\n", 
						service_id, command_name, switch_state);

				// 处理温湿度获取命令

			
				if (strcmp(service_id, "Get_TempAndHumi") == 0 && strcmp(command_name,             "Switch") == 0 && strcmp(switch_state, "ON") == 0)
				if (strcmp(command_name, "Switch") == 0) 
				{
					printf("Temperature report requested!\n");
					report_temperature_humidity();
					send_command_response(request_id, 0);
				}
			}
		}
	}
}


void report_temperature_humidity(void) 
{
	// 准备上报主题
	const char *report_topic = "$oc/devices/6888d7bfd582f20018408d5d_Temp_Humi/sys/properties/report";

	// 模拟传感器
	float temperature = 25.66;
	float humidity = 45.5;

	// 构建华为云JSON格式
	char report_payload[256];
	snprintf(report_payload, sizeof(report_payload),
			"{\"services\":[{"
			"\"service_id\":\"Get_TempAndHumi\","
			"\"properties\":{"
			"\"Temp_value\":%.2f,"
			"\"Humi_value\":%.2f"
			"}"
			"}]}",
			temperature, humidity);

	// 准备发布消息
	MQTTPublishInfo_t pubInfo = {
		.qos = MQTTQoS0,
		.retain = false,
		.pTopicName = report_topic,
		.topicNameLength = strlen(report_topic),
		.pPayload = report_payload,
		.payloadLength = strlen(report_payload)
	};

	// 发布消息
	MQTTStatus_t status = MQTT_Publish(get_mqtt_context(), &pubInfo, 0);
	if (status != MQTTSuccess) 
	{
		printf("Failed to publish properties report: %d\n", status);
	} 

	else 
	{
		printf("Properties report sent: temperature=%.2f, humidity=%.2f\n", temperature, humidity);
	
	}
}



void send_command_response(char *request_id, int result_code)
{
	    // 构造响应主题
	char response_topic[256];
	snprintf(response_topic, sizeof(response_topic),
			"$oc/devices/6888d7bfd582f20018408d5d_Temp_Humi/sys/commands/response/request_id=%s",
			request_id);

	// 构造响应内容
	char response_payload[64];
	snprintf(response_payload, sizeof(response_payload), "{\"result_code\":%d}", result_code);

	// 创建发布信息
	MQTTPublishInfo_t respInfo = {
		.qos = MQTTQoS0,
		.pTopicName = response_topic,
		.topicNameLength = (uint16_t)strlen(response_topic),
		.pPayload = response_payload,
		.payloadLength = strlen(response_payload)
	};

	// 发送响应
	MQTTStatus_t status = MQTT_Publish(get_mqtt_context(), &respInfo, 0);
	if(status != MQTTSuccess) 
	{
		printf("Failed to send command response: %d\n", status);
		return;
	}

	printf("Command response sent: topic=%s, payload=%s\n", response_topic, response_payload);
	
}
















