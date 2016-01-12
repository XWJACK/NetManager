//
//  xSocketPing.c
//  NetManager
//
//  Created by 许文杰 on 1/12/16.
//  Copyright © 2016 许文杰. All rights reserved.
//

#include "xSocketPing.h"



//statistics
void statistics(char* back){
    double percent = ((double)sendPacketNumber - (double)recvPacketNumber) / (double)sendPacketNumber * 100;
    sprintf(back, "---%s ping statistics---\n%d packets trasmitted, %d packet received, %0.1f%% packet loss",inet_ntoa(dstAddr.sin_addr),sendPacketNumber,recvPacketNumber,percent);
}
//check sum
unsigned short checkSum(unsigned short *buffer, int size){
    unsigned long checkSum = 0;
    while (size > 1) {
        checkSum += *buffer++;
        size -= sizeof(unsigned short);//unsigned short is 2 bytes = 16 bits
    }
    //if size is odd number
    if (size == 1){
        checkSum += *(unsigned short *)buffer;
    }
    checkSum = (checkSum >> 16) + (checkSum & 0xFFFF);
    checkSum += (checkSum >> 16);
    return ~checkSum;
}

//calculate time difference
double timeSubtract(struct timeval *recvTimeStamp, struct timeval *sendTimeStamp){
    //calculate seconds
    long timevalSec = recvTimeStamp->tv_sec - sendTimeStamp->tv_sec;
    //calculate microsends
    long timevalUsec = recvTimeStamp->tv_usec - sendTimeStamp->tv_usec;
    
    //if microsends less then zero
    if (timevalUsec < 0) {
        timevalSec -= 1;
        timevalUsec = - timevalUsec;
    }
    return (timevalSec * 1000.0 + timevalUsec) / 1000.0;
}

//fill icmp packet and return size of packet
int fillPacket(int packetSequence){
    int packetSize = 0;
    struct icmp *icmpHeader = (struct icmp *)sendBuffer;
    
    icmpHeader->icmp_type = ICMP_ECHO;
    icmpHeader->icmp_code = 0;
    icmpHeader->icmp_cksum = 0;
    icmpHeader->icmp_id = pid;
    icmpHeader->icmp_seq = packetSequence;
    packetSize = dataSize + 8;
    tvSend = (struct timeval *)icmpHeader->icmp_data;
    gettimeofday(tvSend, NULL);//get current of time
    icmpHeader->icmp_cksum = checkSum((unsigned short *)icmpHeader, packetSize);
    return packetSize;
}

//send icmp packet to dstAddr
int sendPacket(int packetSequence){
    int packSize = 0;
    packSize = fillPacket(packetSequence);
    if ((sendto(socketfd, sendBuffer, packSize, 0, (struct sockaddr *)&dstAddr, sizeof(dstAddr))) < 0) {
        printf("Send icmp packet Error\n");
        sendPacketNumber--;
        recvPacketNumber--;
        return -1;
    }
    return 0;
}

//setting ip address
void settingIP(){
    //initialize
    bzero(&dstAddr,sizeof(dstAddr));
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_addr.s_addr = inet_addr(ipAddr);
}

//get current process id
void getPid(){
    pid = getpid();
}

//create socket
int createSocket(){
    //原始套接字SOCK_RAW需要使用root权限,所以改用SOCK_DGRAM
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP)) < 0) {
        printf("Create Socket Error\n");
        return -1;
    }
    return 0;
}

//setting socket
void settingSocket(int timeout){
    int size = 50 * 1024;
    //setting timeout seconds or you can set it by microseconds
    struct timeval timeOut;
    timeOut.tv_sec = timeout;
    //扩大套接字接收缓冲区到50K这样做主要为了减小接收缓冲区溢出的可能性,若无意中ping一个广播地址或多播地址,将会引来大量应答
    setsockopt(socketfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
    setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeOut, sizeof(timeOut));
}

//destory socket
void destorySocket(){
    close(socketfd);
}

//unpacket
void unPacket(char* packetBuffer,char* back, long size){
    struct ip *ipHeader = NULL;
    struct icmp *icmpHeader = NULL;
    
    double rtt;//往返时间
    int ipHeaderLength;//ip header length
    
    ipHeader = (struct ip *)packetBuffer;
    ipHeaderLength = ipHeader->ip_hl<<2;//求ip报头长度,即ip报头的长度标志乘4
    icmpHeader = (struct icmp *)(packetBuffer + ipHeaderLength);//越过IP头，point to ICMP header
    size -= ipHeaderLength;
    if (size < 8){
        back = "Unpacket Error:packet size minmum 8 bytes\n";
    }
    
    if ((icmpHeader->icmp_type == ICMP_ECHOREPLY) && (icmpHeader->icmp_id == pid)) {
        tvSend = (struct timeval *)icmpHeader->icmp_data;
        gettimeofday(&tvRecv, NULL);
        //以毫秒为单位计算rtt
        rtt = timeSubtract(&tvRecv, tvSend);
        sprintf(back,"%ld bytes from %s: icmp_seq=%u ttl=%d time=%.1f ms\n",size,inet_ntoa(recvAddr.sin_addr),icmpHeader->icmp_seq,ipHeader->ip_ttl,rtt);
    }else{
        back = "Unpacket Error\n";
    }
}

//receive packet
void receivePacket(char* back){
    //claculate packet size
    int packetSize = sizeof(recvAddr);
    long size;
    if ((size = recvfrom(socketfd, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr *)&recvAddr, (socklen_t *)&packetSize)) < 0) {
        sprintf(back,"Receive timeout\n");
        recvPacketNumber--;
    }else{
        gettimeofday(&tvRecv, NULL);
        //char temp[100] = {0};
        unPacket(recvBuffer, back, size);
        //printf("%s\n",temp);
    }
}

void ping(char *ipAddress, int number, int timeout){
    int packetnumber = 0;
    ipAddr = ipAddress;
    sendPacketNumber = number;
    recvPacketNumber = number;
    
    settingIP();
    getPid();
    if (createSocket() != -1){
        settingSocket(timeout);
        printf("PING %s: %d bytes of data.\n",ipAddress,dataSize);
        while(packetnumber < number){
            if (sendPacket(packetnumber) != -1){
                char back[100] = {0};
                receivePacket(back);
                printf("%s",back);
            }
            sleep(1);
            packetnumber++;
        }
        char back[100] = {0};
        statistics(back);
        printf("%s\n",back);
        destorySocket();
    }
}