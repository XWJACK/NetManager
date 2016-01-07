//
//  xSocketPing.c
//  NetManager
//
//  Created by 许文杰 on 1/7/16.
//  Copyright © 2016 许文杰. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>//基本系统数据类型
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>//用于获取当前时间
#include <arpa/inet.h>//inet_ntoa将一个IP转换成一个互联网标准点分格式的字符串
#include <unistd.h>//close(int)
#include <netdb.h>//定义了与网络有关的结构、变量类型、宏、函数等

#include <netinet/ip.h>//ip数据包结构
#include <netinet/ip_icmp.h>//icmp数据包结构

int data_size = 56;//icmp报头为8字节,数据部分为56字节,数据报长度最大为64字节
int pid = 0;//当前线程号
int ttl = 64;//生存时间
int socketfd = 0;//socket

//Socket address, internet style.
struct sockaddr_in dst_addr;
struct sockaddr_in recv_addr;

struct timeval *tvsend;//发送数据包的时间
struct timeval tvrecv;//接受数据包的时间,不用指针

char icmp_pkt[4096] = {0};//发送icmp缓冲区
char recv_pkt[1024] = {0};//接受icmp缓冲区,一般设置较大,防止缓冲区溢出


u_short checksum(u_short* buffer, int size);
double time_sub(struct timeval *recvtime,struct timeval *sendtime);
int pack(int number);
int send_icmp(struct sockaddr_in ip_addr, int packnumber);
void ping(char *ipAddress, int number);
void recv_icmp();
int unpack(char *pkt_buf, long size);
int CreateSocket();
void SettingSocket();
void DestorySocket();
void SettingIP(char *ip_addr);
void getPid();


//校检码设置,计算校检和经典函数
u_short checksum(u_short* buffer, int size){
    int cksum = 0;
    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(u_short);
    }
    if (size) {
        cksum += *(u_short*)buffer;
    }
    cksum = (cksum >> 16) + (cksum & 0xFFFF);
    cksum += (cksum >> 16);
    return ~cksum;
}

//计算时间差值
double time_sub(struct timeval *recvtime,struct timeval *sendtime){
    long sec = recvtime->tv_sec - sendtime->tv_sec;//计算秒
    long usec = recvtime->tv_usec - sendtime->tv_usec;//计算毫秒
    
    if(usec < 0){//判断毫秒差值是否小于0
        recvtime->tv_sec = sec - 1;
        recvtime->tv_usec = -usec;
    }
    return (sec * 1000.0 + usec) / 1000.0;
}

//填充ICMP数据报并放入icmp数据包缓冲区
int pack(int number){
    int packsize = 0;
    struct icmp *icmp_header = (struct icmp *)icmp_pkt;
    
    icmp_header->icmp_type = ICMP_ECHO;
    icmp_header->icmp_code = 0;
    icmp_header->icmp_cksum = 0;
    icmp_header->icmp_id = pid;
    icmp_header->icmp_seq = number;
    packsize = data_size + 8;//数据报大小等于icmp头部大小加上数据大小
    tvsend = (struct timeval *)icmp_header->icmp_data;//将当前时间戳写入数据中,数据部分前8字节是时间戳
    gettimeofday(tvsend, NULL);//获得当前的时间
    icmp_header->icmp_cksum = checksum((u_short *)icmp_header, packsize);//最后计算校检和
    return packsize;
}

//发送ICMP,向dst_addr的IP地址,发送第packnumber数据包
int send_icmp(struct sockaddr_in ip_addr, int packnumber){
    int packsize = 0;
    packsize = pack(packnumber);
    if ((sendto(socketfd, icmp_pkt, packsize, 0, (struct sockaddr *)&ip_addr, sizeof(ip_addr))) < 0) {
        printf("发送失败\n");
        return -1;
    }
    return 0;
}

//给ipAddress发送number个icmp包
void ping(char *ipAddress, int number){
    int packnumber = 0;
    
    SettingIP(ipAddress);//设置IP地址
    getPid();//获取用户进程
    if (CreateSocket() != -1){
        SettingSocket();
        printf("PING %s(%s):%d bytes of data.\n",ipAddress,inet_ntoa(dst_addr.sin_addr),data_size);
        while(packnumber < number){
            send_icmp(dst_addr, packnumber);
            recv_icmp();
            sleep(1);
            packnumber++;
        }
        DestorySocket();
    }
}

//接受数据包
void recv_icmp(){
    int recv_len = sizeof(recv_addr);
    long size;
    if ((size = recvfrom(socketfd, recv_pkt, sizeof(recv_pkt), 0, (struct sockaddr *)&recv_addr, (socklen_t *)&recv_len)) < 0) {
        printf("接收失败");
    }else{
        gettimeofday(&tvrecv, NULL);
        unpack(recv_pkt, size);
    }
}

//解包
int unpack(char *pkt_buf, long size){
    struct ip *ip_header = NULL;
    struct icmp *icmp_header = NULL;
    
    double rtt;//往返时间
    int ip_hdrlen;//ip头部长度,用于剥离ip数据报
    
    ip_header = (struct ip *)pkt_buf;
    ip_hdrlen = ip_header->ip_hl<<2;//求ip报头长度,即ip报头的长度标志乘4
    icmp_header = (struct icmp *)(pkt_buf + ip_hdrlen);//越过IP头，指向ICMP报头
    size -= ip_hdrlen;
    if (size < 8){
        printf("解包失败,数据包小\n");
        return -1;
    }
    
    if ((icmp_header->icmp_type == ICMP_ECHOREPLY) && (icmp_header->icmp_id == pid)) {
        tvsend = (struct timeval *)icmp_header->icmp_data;
        gettimeofday(&tvrecv, NULL);
        //以毫秒为单位计算rtt
        rtt = time_sub(&tvrecv, tvsend);
        printf("%ld bytes from %s: icmp_seq=%u ttl=%d time=%.1f ms\n",size,inet_ntoa(recv_addr.sin_addr),icmp_header->icmp_seq,ip_header->ip_ttl,rtt);
    }else{
        printf("解包失败\n");
        return -1;
    }
    return 0;
}

//创建socket
int CreateSocket(){
    //原始套接字SOCK_RAW需要使用root权限,所以改用SOCK_DGRAM
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP)) < 0) {
        printf("Socket创建失败\n");
        return -1;
    }
    return 0;
}
//设置socket
void SettingSocket(){
    int size = 50 * 1024;
    //扩大套接字接收缓冲区到50K这样做主要为了减小接收缓冲区溢出的可能性,若无意中ping一个广播地址或多播地址,将会引来大量应答
    setsockopt(socketfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
}
//销毁socket
void DestorySocket(){
    close(socketfd);
}

//设置IP地址
void SettingIP(char *ip_addr){
    
    bzero(&dst_addr,sizeof(dst_addr));//置字节字符串前n个字节为零且包括‘\0’
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_addr.s_addr = inet_addr(ip_addr);
}

//获取进程
void getPid(){
    pid = getpid();
}
