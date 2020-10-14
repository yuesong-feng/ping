#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef struct
{
    u8 version_and_headerlen;
    u8 type_of_service;
    u16 packet_len;
    u16 id;
    u16 slice_info;
    u8 TTL;
    u8 protocol;
    u16 checksum;
    u32 source_ip;
    u32 dest_ip;
} IP_HEADER;

typedef struct
{
    u8 Type;
    u8 Code;
    u16 Checksum;
    u16 Id;
    u16 Seq;
} ICMP_HEADER;

int main(int argc, char const *argv[])
{
    ICMP_HEADER test;

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd == -1)
    {
        printf("create socket error!\n");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("61.135.169.191");
    char buf[256];
    ICMP_HEADER sendpack;
    sendpack.Code=0;
    
    int send = sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (send == -1)
    {
        printf("send error!\n");
        return -1;
    }
    else
    {
        printf("you have sent %d bytes of data\n", send);
    }

    char recvpack[1024];
    memset(recvpack, 0, sizeof(recvpack));
    int len = sizeof(server_addr);
    int recv = recvfrom(sockfd, recvpack, sizeof(recvpack), 0, (struct sockaddr *)&server_addr, &len);

    return 0;
}
