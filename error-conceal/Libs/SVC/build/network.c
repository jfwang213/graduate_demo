#include "network.h"
#include "type.h"
#include "svc_type_util.h"
#include "stdio.h"
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef WIN32
SOCKET server_socket;
SOCKET receive_socket;
#else
int server_socket;
int receive_socket;
#endif

struct sockaddr_in client_addr;
struct sockaddr_in server_addr;
static int nal_num = 0;
static int network_recv_end = 0;
const char *serverIpAddress = "127.0.0.1";
const short serverPortNumber = 12345;
int network_init()
{
#ifdef WIN32
	WSADATA  Ws;
#endif
	int ret;
	int addrLen  = sizeof(client_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(serverIpAddress);
	server_addr.sin_port = htons(serverPortNumber);
	memset(server_addr.sin_zero, 0x00, 8);
#ifdef WIN32
	if ( WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
	{
		printf("Init Windows Socket Failed::%d\n",GetLastError());
		return 0;
	}
#endif
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#ifdef WIN32
	if (INVALID_SOCKET == server_socket)
#else
    if (-1 == server_socket)
#endif
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
#ifdef WIN32
	if (INVALID_SOCKET == receive_socket)
#else
    if (-1 == receive_socket)
#endif
	{
		printf("accept error!\n");
		return 0;
	}
    printf("accept one client!\n");
    network_recv_end = 0;
	return 1;
}

void network_close()
{
#ifdef WIN32
	closesocket(receive_socket);
	closesocket(server_socket);
#else
    close(receive_socket);
    close(server_socket);
#endif
}
#ifdef WIN32
int recv_fix_len(SOCKET s, int len, unsigned char *buf)
#else
int recv_fix_len(int s, int len, unsigned char *buf)
#endif
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
    if (network_recv_end) {
        return NULL;
    }
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
    if (len == 0) {
        network_recv_end = 1;
        return NULL;
    }
	res = (NAL*)malloc(sizeof(NAL));
	init_nal(res,pdecoder_context);
	res->prbsp_memory = (unsigned char*)malloc(len+20);
	recv_fix_len(receive_socket,len,res->prbsp_memory);
	res->nal_byte_size = len;
	return res;
}

