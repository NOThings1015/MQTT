/*********************************************************************************
 *      Copyright:  (C) 2025 Dameng Studio
 *                  All rights reserved.
 *
 *       Filename:  network.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2025年08月05日)
 *         Author:  Yang jiayu <3381322051@qq.com>
 *      ChangeLog:  1, Release initial version on "2025年08月05日 15时25分50秒"
 *                 
 ********************************************************************************/
#include "network.h"
#include "config.h"
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>


static struct NetworkContext networkContext = {0};

int network_init(void)
{
	// 初始化OpenSSL
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

	// 创建SSL上下文
	networkContext.ctx = SSL_CTX_new(TLS_client_method());
	if (!networkContext.ctx)
	{
		fprintf(stderr, "SSL_CTX_new failed\n");
		return -1;
	}

	// 加载CA证书
	if (SSL_CTX_load_verify_locations(networkContext.ctx, CA_CERT_FILE, NULL) != 1) 
	{
		fprintf(stderr, "Failed to load CA certificate\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}

	return 0;

}



int network_connect(void)
{
	// 解析主机地址
	struct addrinfo hints = {.ai_family = AF_INET, 
							 .ai_socktype = SOCK_STREAM};
	struct addrinfo *res;
	if (getaddrinfo(HOST, NULL, &hints, &res) != 0)
	{
		fprintf(stderr, "getaddrinfo error\n");
		return -1;
	}

	// 创建套接字
	struct sockaddr_in serverAddr = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr
	};

	networkContext.socket = socket(AF_INET, SOCK_STREAM, 0);
	if (networkContext.socket < 0) 
	{
		perror("socket");
		freeaddrinfo(res);
		return -1;
	}

	 // 连接服务器
	if (connect(networkContext.socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) 
	{
		perror("connect");
		fprintf(stderr, "错误号: %d\n", errno);
		close(networkContext.socket);
		freeaddrinfo(res);
		return -1;
	}

	freeaddrinfo(res);


	// 创建SSL连接
	networkContext.ssl = SSL_new(networkContext.ctx);
	SSL_set_fd(networkContext.ssl, networkContext.socket);


	int ssl_connect_result = SSL_connect(networkContext.ssl);
	if (ssl_connect_result != 1) 
	{
		fprintf(stderr, "SSL_connect failed, 返回值: %d\n", ssl_connect_result);

		int ssl_err = SSL_get_error(networkContext.ssl, ssl_connect_result);
		fprintf(stderr, "SSL错误代码: %d\n", ssl_err);

		switch (ssl_err)
		{
			case SSL_ERROR_SSL:
				fprintf(stderr, "SSL协议错误:\n");
				ERR_print_errors_fp(stderr);
				break;
			case SSL_ERROR_SYSCALL:
				fprintf(stderr, "系统调用错误: %s\n", strerror(errno));
				if (errno == 0) 
				{			char buf[1];
					ssize_t recv_result = recv(networkContext.socket, buf, sizeof(buf), MSG_PEEK);
					if (recv_result == 0) 
					{
						fprintf(stderr, "华为云连接被对端关闭\n");
					} 
					else if (recv_result < 0) 
					{
						fprintf(stderr, "TCP连接错误: %s\n", strerror(errno));
					}
					else
					{
						fprintf(stderr, "华为云SSL内部错误\n");
					}
				}

				else
				{
					fprintf(stderr, "系统调用错误: %s\n", strerror(errno));
				}
				break;



			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_WRITE:
				fprintf(stderr, "SSL需要重试\n");
				break;
			default:
				fprintf(stderr, "未知SSL错误\n");
		}

		return -1;

	}
	char err_buf[512];
	unsigned long err_code;
	while ((err_code = ERR_get_error())) 
	{
		ERR_error_string_n(err_code, err_buf, sizeof(err_buf));
		fprintf(stderr, "OpenSSL错误: %s\n", err_buf);
	}


	//test
	if (SSL_is_init_finished(networkContext.ssl)) 
	{
		printf("华为云SSL握手完成\n");
	}
	else 
	{
		printf("华为云SSL握手未完成\n");
		ERR_print_errors_fp(stderr);
	}
	//

	// 设置为非阻塞模式
	int flags = fcntl(networkContext.socket, F_GETFL, 0);
	if (fcntl(networkContext.socket, F_SETFL, flags | O_NONBLOCK) < 0) 
	{
		perror("fcntl nonblock failed");
		return -1;
	}
	return 0;

}

int32_t network_recv(struct NetworkContext *context, void *buffer, size_t len) 
{
	if (!networkContext.ssl) 
		return -1;

	int received = SSL_read(context->ssl, buffer, len);

	if (received > 0) 
		return received;

	int ssl_err = SSL_get_error(context->ssl, received);
	switch (ssl_err) 
	{
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
			return 0;
		case SSL_ERROR_ZERO_RETURN:
			return 0;
		case SSL_ERROR_SYSCALL:
			if (errno == 0) 
			{
				fprintf(stderr, "Connection closed by peer\n");
			} 

			else 
			{
				fprintf(stderr, "SSL_ERROR_SYSCALL: %s\n", strerror(errno));
			}
			return -1;
		default:
			return -2;
	}
}



int32_t network_send(struct NetworkContext *context, const void *buffer, size_t len)
{
	if (!networkContext.ssl) 
		return -1;

	int sent = SSL_write(context->ssl, buffer, len);

	if (sent > 0) 
		return sent;

	int ssl_err = SSL_get_error(context->ssl, sent);
	switch (ssl_err) 
	{
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
			return 0;
		default:
			ERR_print_errors_fp(stderr);
			return -1;
	}
}



void network_cleanup(void) 
{
	if (networkContext.ssl) 
	{
		SSL_shutdown(networkContext.ssl);
		SSL_free(networkContext.ssl);
		networkContext.ssl = NULL;
	}

	if (networkContext.socket >= 0) 
	{
		close(networkContext.socket);
		networkContext.socket = -1;
	}

	if (networkContext.ctx) 
	{
		SSL_CTX_free(networkContext.ctx);
		networkContext.ctx = NULL;
	}
}


struct NetworkContext *get_network_context(void) 
{
	return &networkContext;
}























