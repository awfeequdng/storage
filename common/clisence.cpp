#include "clisence.h"


#include <iostream>

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>

//using namespace std;



#include "HashMethod/MD5.h"
#include "HashMethod/CRC32.h"
#include "HashMethod/Checksum32.h"

#include "PrintMacros.h"

#define IDE_ATAPI_IDENTIFY 0xA1   // 读取ATAPI设备的命令
#define IDE_ATA_IDENTIFY   0xEC   // 读取ATA设备的命

//// 获取本地mac
//std::string getLocalMac()
//{
//    int sock_mac;
//    struct ifreq ifr_mac;
//    char mac_addr[30];
//    sock_mac = socket(AF_INET, SOCK_STREAM, 0);
//    if (sock_mac == -1)
//    {
//        perror("create socket falise...mac\n");
//        return "";
//    }

//    memset(&ifr_mac, 0, sizeof(ifr_mac));
//    strncpy(ifr_mac.ifr_name, "eth0", sizeof(ifr_mac.ifr_name) - 1);

//    if ((ioctl(sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)
//    {
//        printf("mac ioctl error\n");
//        close(sock_mac);
//        return "";
//    }

//    sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
//        (unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],
//        (unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],
//        (unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],
//        (unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],
//        (unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],
//        (unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]
//    );

//    close(sock_mac);

//    return mac_addr;

//}


CLisence::CLisence()
{
    memset(m_byMac,0,sizeof(m_byMac));
    memset(m_byCPUID,0,sizeof(m_byCPUID));
    memset(m_byHDDSerialNumber,0,sizeof(m_byHDDSerialNumber));
    memset(m_byLock,0,sizeof(m_byLock));
    memset(m_byKey,0,sizeof(m_byKey));
}

BOOL CLisence::GetMacAddress()
{
    int sock_mac;
    struct ifreq ifr_mac;
    char mac_addr[30];
    sock_mac = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_mac == -1)
    {
        ERROR("create socket falise...mac\n");
        return "";
    }

    memset(&ifr_mac, 0, sizeof(ifr_mac));
    strncpy(ifr_mac.ifr_name, "eth0", sizeof(ifr_mac.ifr_name) - 1);

    if ((ioctl(sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)
    {
        ERROR("mac ioctl error\n");
        close(sock_mac);
        return "";
    }

    sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
        (unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],
        (unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],
        (unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],
        (unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],
        (unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],
        (unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]
    );
//    cout<<mac_addr
    memcpy(m_byMac,ifr_mac.ifr_hwaddr.sa_data,MAC_LEN);
    DEBUG("mac address = "<<mac_addr);

    close(sock_mac);
}

