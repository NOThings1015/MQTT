/********************************************************************************
 *      Copyright:  (C) 2025 Dameng Studio
 *                  All rights reserved.
 *
 *       Filename:  mqtt_config.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2025年08月05日)
 *         Author:  Yang jiayu <3381322051@qq.com>
 *      ChangeLog:  1, Release initial version on "2025年08月05日 15时22分42秒"
 *                 
 ********************************************************************************/
#ifndef MQ_CONFIG_H_
#define MQ_CONFIG_H_


#define HOST "ede5b7e7e9.st1.iotda-device.cn-north-4.myhuaweicloud.com"  //华为云IOT平台地址
#define PORT 8883 //MQTT over SSL/TLS 标准端口
#define CLIENT_ID "6888d7bfd582f20018408d5d_Temp_Humi_0_0_2025073006"  //设备唯一标识符ID
#define USERNAME "6888d7bfd582f20018408d5d_Temp_Humi" //连接用户名
#define PASSWORD "e0ac028a97a71ae7cc57d7d84fdf55a7ebfbcfd43606541bfaebef8bdc41dfe3" //连接密码 
#define TOPIC "$oc/devices/6888d7bfd582f20018408d5d_Temp_Humi/sys/#" //订阅的主题（华为云格式）
#define CA_CERT_FILE "./GlobalSign-rootca.pem" //从当前路径加载CA 根证书文件



#define NETWORK_BUFFER_SIZE 1024
#define CONNECT_TIMEOUT_MS 10000
#define SUBSCRIBE_TIMEOUT_MS 10000


#define MQTT_KEEP_ALIVE_SEC 60
#define MQTT_CLEAN_SESSION true


#endif // CONFIG_H

