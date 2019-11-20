#include<WinSock2.h>
#pragma comment ( lib, "ws2_32.lib" )
#include<stdio.h>
typedef struct ICMP_HEAD {
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned short id;
	unsigned short sequence;
}ICMP_HEAD, * PICMP_HEAD;
unsigned short CheckSum(PICMP_HEAD pICMP, int leng)
{
	long checksum = 0;
	unsigned short* l = (unsigned short*)pICMP;
	while (leng > 1)
	{
		checksum += *(l++);
		if (checksum & 0x8000000) {
			checksum = (checksum & 0xffff) + (checksum >> 16);
		}
		leng -= 2;
	}
	if (leng == 1) {
		checksum += *((unsigned char*)l);
	}
	while (checksum >> 16) {
		checksum = (checksum & 0xffff) + (checksum >> 16);
	}
	return (unsigned short)~checksum;
}
BOOL Ping(char* dest) {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { return FALSE; }
	SOCKET	Sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (Sock == INVALID_SOCKET)
	{
		printf("创建SOCKET失败！\n");
		return FALSE;
	}
	struct hostent* host = NULL;
	host = gethostbyname(dest);
	SOCKADDR_IN DestAddr;
	DestAddr.sin_family = AF_INET;
	DestAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
	DestAddr.sin_port = htons(0);
	int Timeout = 1000;
	int ret = setsockopt(Sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&Timeout, sizeof(Timeout));	
	for (int i = 0, num = 4, recvnum = 0; i < num; i++) {
		char SendPack[sizeof(ICMP_HEAD) + 32] = { 0 };
		PICMP_HEAD pICMP = (PICMP_HEAD)SendPack;
		pICMP->type = 8;
		pICMP->code = 0;
		pICMP->id = (unsigned short)GetCurrentProcessId();
		pICMP->checksum = 0;
		pICMP->sequence = i;
		memcpy((SendPack + sizeof(ICMP_HEAD)), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 32);
		pICMP->checksum = CheckSum((PICMP_HEAD)SendPack, sizeof(SendPack));
		struct sockaddr_in FromAddr;
		int sizeFrom = sizeof(SOCKADDR_IN);
		char RecvPack[1024] = { 0 };
		unsigned long t1 = GetTickCount64();
		int send = sendto(Sock, SendPack, sizeof(SendPack), 0, (SOCKADDR*)&DestAddr, sizeof(DestAddr));
		if (send == SOCKET_ERROR) {
			printf("Request time out\n");
		}
		int recv = recvfrom(Sock, RecvPack, sizeof(RecvPack), 0, (SOCKADDR*)&FromAddr, &sizeFrom);
		unsigned long t2 = GetTickCount64();
		if (recv == SOCKET_ERROR) {
			printf("Request time out\n");
		}
		else if (strcmp(inet_ntoa(FromAddr.sin_addr), inet_ntoa(DestAddr.sin_addr)) != 0) {
			printf("收到的不是相应的应答报文！\n");
		}
		else {
			recvnum++;
			printf("收到应答报文%d！\n报文数据大小：%d\n",i+1, recv);
			printf("IP地址：%d.%d.%d.%d\n", RecvPack[12] > 0 ? RecvPack[12] : 256 + RecvPack[12], RecvPack[13] > 0 ? RecvPack[13] : 256 + RecvPack[13], RecvPack[14] > 0 ? RecvPack[14] : 256 + RecvPack[14], RecvPack[15] > 0 ? RecvPack[15] : 256 + RecvPack[15]);
			printf("序号：%d.%d\n", RecvPack[26], RecvPack[27]);
			printf("响应时间：%lums\n\n", t2 - t1);
		}
		if (i == num - 1) {
			printf("用户名：%s\n统计信息：发出报文%d个，收到报文%d个，丢包%d个，丢包率为%.0f%%\n", dest, num, recvnum, num - recvnum, (float)(num - recvnum)*100 / num);
		}
	}
}
int main() {
	printf("输入格式：ping www.baidu.com或ping 182.61.200.7\n");
	char ip[1024] = { 0 };
	scanf("ping %s", ip);
	Ping(ip);
	system("pause");
	return 0;
}