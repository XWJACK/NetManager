//
//  xSocketPing.h
//  NetManager
//
//  Created by XWJACK on 1/12/16.
//  Copyright © 2016 XWJACK. All rights reserved.
//

#ifndef xSocketPing_h
#define xSocketPing_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>//基本系统数据类型
#include <sys/socket.h>
#include <netinet/in.h>
//get current of time
#include <sys/time.h>
#include <arpa/inet.h>//inet_ntoa将一个IP转换成一个互联网标准点分格式的字符串
#include <unistd.h>//close(int)
#include <netdb.h>//定义了与网络有关的结构、变量类型、宏、函数等
//ip packet structure
#include <netinet/ip.h>
//icmp packet structure
#include <netinet/ip_icmp.h>

//time to live
int ttl = 64;
//icmp data size ,icmp header 8bytes,data size 56bytes,the maximum of packet size 64bytes
int dataSize = 56;
//packet number
int sendPacketNumber;
int recvPacketNumber;
//ip address
char *ipAddr;

//send packet of time
struct timeval *tvSend;
//receive packet of time
struct timeval tvRecv;

//Socket address, internet style.
//the destination address
struct sockaddr_in dstAddr;
//the receive address
struct sockaddr_in recvAddr;

//send icmp buffer
char sendBuffer[1024] = {0};
//receive icmp replay buffer
char recvBuffer[1024] = {0};

//the current process of id
int pid;
//socket
int socketfd = 0;


void statistics(char* back);
unsigned short checkSum(unsigned short *buffer, int size);
double timeSubtract(struct timeval *recvTimeStamp, struct timeval *sendTimeStamp);
int fillPacket(int packetSequence);
int sendPacket(int packetSequence);
void settingIP();
void getPid();
int createSocket();
void settingSocket(struct timeval timeout);
void destorySocket();
void unPacket(char* packetBuffer,char* back, long size);
void receivePacket(char* back);
void ping(char *ipAddress, int number, struct timeval timeout);


#endif /* xSocketPing_h */
