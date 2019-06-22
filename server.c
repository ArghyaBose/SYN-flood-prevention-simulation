#include <stdio.h>
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <string.h>

#include <netdb.h>
#include <netpacket/packet.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

#define BUF_SIZE 1024 * 5


static const int err = -1;
static int _recvBufSize = BUF_SIZE;
static char _recvBuf[BUF_SIZE] = {0};
static long _index = 0 ;
typedef struct ip_info
{
    unsigned long acc_ip;
    int acc_times;
    struct timeval acc_1st_time;
    struct ip_info *next;
} ip_info, *LNode;

static ip_info *ip_list = NULL;

static  void print()
{
    ip_info *p = ip_list;
    while(p->next !=NULL)
    {
        printf("list ip is %ld\n",p->acc_ip);
        p=p->next;
    }

}
static int packet_anylise(const char* packetData)
{

    struct ether_header *etherHeader = NULL;
    struct ip *ipHeader = NULL;

    etherHeader = (struct ether_header*)_recvBuf;

    // after the header there is IP info, will anylise the data here.
    ipHeader = (struct ip*)(etherHeader + 1);
    if (ipHeader == NULL)
    {
        printf("ip packet incorrect \n");
        return err;
    }

    struct protoent *ipProto = getprotobynumber(ipHeader->ip_p);
    struct in_addr ad1, ad2;
    ulong l1, l2;
    l1 = ntohl(ipHeader->ip_src.s_addr);
    l2 = ntohl(ipHeader->ip_dst.s_addr);
    memcpy(&ad1, &l1, 4);
    memcpy(&ad2, &l2, 4);
    printf(" 1 you are attacked by ip %s, server ip is %s\n", inet_ntoa(ad1),inet_ntoa(ad2));
    printf(" 2 you are attacked by ip %s\n", inet_ntoa(ad1));


    ip_info *q = ip_list;
    int found = 0;
    //printf("befofe while\n");
    while(q!=NULL)
    {
        //  printf("list ip have %d\n", q->acc_ip);
        if (ipHeader->ip_src.s_addr== q->acc_ip)
        {
            found = 1;
            q->acc_times++;

            if (q->acc_times > 20 )
            {
                struct timeval tmp;
                gettimeofday(&tmp,NULL);
                if (tmp.tv_sec - q->acc_1st_time.tv_sec < 5)
                {
                    struct in_addr ad1;
                    ulong l1;
                    l1 = ntohl(ipHeader->ip_src.s_addr);
                    memcpy(&ad1, &l1, 4);
                    printf("3 %ld : you are attacked by ip %s\n",_index++, inet_ntoa(ad1));
                    q->next=NULL;
                }
                else
                {
                    struct in_addr ad1;
                    ulong l1;
                    l1 = ntohl(ipHeader->ip_src.s_addr);
                    memcpy(&ad1, &l1, 4);
                    printf("4 %ld you are ok to access with ip %s\n",_index++,inet_ntoa(ad1));
                }
            }
            q = q->next;
        }
    }
    //	printf(" before dound\n");
    if (found == 0 ) // printf("fdound\n");
    {
        struct timeval tmp;
        ip_info *p = (ip_info*)malloc(sizeof(ip_info));
        p->acc_ip = ipHeader->ip_src.s_addr;
        p->acc_times = 0;
        gettimeofday(&tmp,NULL);
        p->acc_1st_time = tmp;
        p->next = NULL;

        if (ip_list == NULL) ip_list = p;
        else
        {
            p->next = ip_list->next;
            ip_list->next = p;
            print();
        }
    }
    return 0;
}

// mainly loop function, dead loop for recv data by capture the netcard
static void server_listen(const int socketID)
{

    int ret;
    socklen_t stsocketLen = 0;
    while (1)
    {
        memset(_recvBuf, 0, BUF_SIZE);
        ret = recvfrom(socketID, _recvBuf, _recvBufSize, 0, NULL, &stsocketLen);
        if (ret < 0) continue;
        packet_anylise(_recvBuf);
    }
}

// initalise the socket and set value if need
static int init_socket()
{

    int retvalue = -1;

    retvalue = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (retvalue < 0 )
    {
        printf("socket create error\n");
        return err;
    }

    if ((setsockopt(retvalue,SOL_SOCKET, SO_RCVBUF, &_recvBufSize, sizeof(int)))< 0)
    {
        printf("set socket error\n");
        close(retvalue);
        return err;
    }

    return retvalue;
}

int main(int argc, char*argv[])
{

  p->acc_1st_time = tmp;
    p->next = NULL;
    ip_list->next = p;
    */
    ip_list = NULL;
    if (socketID < 0 )
    {
        printf("init socket error\n");
        return err;
    }

    printf("server listing......\n");

    server_listen(socketID);
    close(socketID);
    return 0;
}





























