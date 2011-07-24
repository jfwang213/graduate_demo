#include "network.h"
#include "type.h"
#include "svc_type_util.h"
#include "stdio.h"
#ifdef WIN32
#include <winsock2.h>

SOCKET server_socket;
struct sockaddr_in server_addr;
SOCKET receive_socket;
struct sockaddr_in client_addr;
static int nal_num = 0;
int network_init()
{
	WSADATA  Ws;
	int ret;
	int addrLen  = sizeof(client_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(12345);
	memset(server_addr.sin_zero, 0x00, 8);
	if ( WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
	{
		printf("Init Windows Socket Failed::%d\n",GetLastError());
		return 0;
	}
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server_socket)
	{
		printf("socket create error\n");
		return 0;
	}
	ret = bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if ( 0 != ret)
	{
		printf("bind error!\n");
		return 0;
	}
	ret = listen(server_socket,10);
	if (0 != ret)
	{
		printf("listen error!\n");
		return 0;
	}
	printf("waiting for client socket!\n");
	receive_socket = accept(server_socket,(struct sockaddr*)&client_addr,&addrLen);
	if (INVALID_SOCKET == receive_socket)
	{
		printf("accept error!\n");
		return 0;
	}
	return 1;
}

void network_close()
{
	closesocket(receive_socket);
	closesocket(server_socket);
}

int recv_fix_len(SOCKET s,int len,unsigned char *buf)
{
	int now = 0;
	int try_num = 0;
	const int max_try_num  = 10;
	while (now<len && try_num < max_try_num)
	{
		try_num++;
		now += recv(s,buf,len-now,0);
	}
	return max_try_num != try_num;
}

NAL* get_one_nal_from_network(decoder_context *pdecoder_context)
{
	int len,i;
	unsigned char buf[10];
	NAL* res = NULL;
	if(!recv_fix_len(receive_socket,4,buf))
	{
		return NULL;
	}
	nal_num ++;
	len = 0;
	for (i = 0;i<4;i++)
	{
		len=(len<<8)+buf[i];
	}
	res = (NAL*)malloc(sizeof(NAL));
	init_nal(res,pdecoder_context);
	res->prbsp_memory = (unsigned char*)malloc(len+20);
	recv_fix_len(receive_socket,len,res->prbsp_memory);
	res->nal_byte_size = len;
	return res;
}

#endif