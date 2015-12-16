#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>	
#include <string.h>
#include <sys/socket.h>	
#include <netdb.h>	
#include <errno.h>	
#include <arpa/inet.h>	
#include <signal.h>	
#include <netinet/in.h>	
#include <linux/tcp.h>
#include <netinet/ip.h>
#include <time.h>
#define TCP_SIZE sizeof(struct tcphdr)
void send_data(int sockfd,struct sockaddr_in *addr, int soureport,char ip[30]);
unsigned short check_sum(unsigned short *addr, int len);
void *recv_packet();

int startport;
int endport;
int sockfd;
int sourceport = 33333;

char ip[30];

void main()
{
	freopen("./input", "r", stdin);
    struct sockaddr_in addr;
	struct hostent *host;
	char sourceip[30];
	int i;
	int on = 1;
	pthread_t tid;
	clock_t start;
	
	printf("Please enter the Startport\n");
	scanf("%d", &startport);
	printf("Please enter the Endport\n");
	scanf("%d", &endport);
	printf("Please enter the LocalIP\n");
	scanf("%s", sourceip);
	printf("Please enter the IP\n");
	scanf("%s", ip);

    if(startport < 1|| endport > 65535 || endport < startport)
	{
		fprintf(stderr,"PORT RANGE ERROR!\n");
		return;
	}

	start = clock();
	printf("insteresting ports %d-%d on %s\n", startport,endport,ip);
	
	memset(&addr,0,sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	printf("PORT STATE\n");

	if((sockfd = socket(AF_INET,SOCK_RAW,IPPROTO_TCP)) <= 0)
	{
		fprintf(stderr,"Socket Error:%s\n", strerror(h_errno));
		exit(EXIT_FAILURE);
	}

	setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
	if((errno = pthread_create(&tid,NULL,recv_packet,start))<0)
	{
		perror("pthread\n");
		return;
	}
	for(i = startport; i < endport+1; i++)
	{
		addr.sin_port = htons(i);
		send_data(sockfd, &addr, sourceport, sourceip);
		usleep(0);
	}
 
	pthread_join(tid, NULL);
}

void send_data(int sockfd, struct sockaddr_in *addr, int sourceport,char sourceip[30])
{
	char buffer[100];
	struct iphdr *ip;
	struct tcphdr *tcp;

	int head_len;
	int n,i;
	u_char *pPseudoHead;
	u_char pseudoHead[12 + sizeof(struct tcphdr)];
	u_short tcpHeadLen;

	tcpHeadLen = htons(sizeof(struct tcphdr));
	head_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
	bzero(buffer,100);
	
	//ip
	ip = (struct iphdr *)buffer;
	ip->version = IPVERSION;
	ip->ihl = sizeof(struct ip) >> 2;
	ip->tos = 0;
	ip->tot_len = htons(head_len);
	ip->id = 0;
	ip->frag_off = 0;
	ip->ttl = MAXTTL;
	ip->protocol = IPPROTO_TCP;
	ip->check = 0;
	ip->daddr = addr->sin_addr.s_addr;
	ip->saddr = inet_addr(sourceip);

	//tcp
	tcp = (struct tcphdr*)(buffer + sizeof(struct ip));
	tcp->source = htons(sourceport);
	tcp->dest = addr->sin_port;
	tcp->seq = htonl(30000);
	tcp->ack_seq = 0;
	tcp->doff = 5;
	tcp->syn = 1;
	tcp->urg_ptr = 0;
	tcp->window = htons(10052);

	//header
	pPseudoHead = pseudoHead;
	memset(pPseudoHead,0,12 +sizeof(struct tcphdr));
	memcpy(pPseudoHead,&ip->saddr,4);
	pPseudoHead+=4;
	memcpy(pPseudoHead,&ip->daddr,4);
	pPseudoHead+=4;
    memset(pPseudoHead,0,1);
	pPseudoHead++;
	memset(pPseudoHead,0x0006,1);
	pPseudoHead++;
	memcpy(pPseudoHead,&tcpHeadLen,2);
	pPseudoHead+=2;
	memcpy(pPseudoHead,tcp,sizeof(struct tcphdr));

	tcp->check = 0;
	tcp->check = check_sum((unsigned short*)pseudoHead,sizeof(struct tcphdr) + 12);
	if(sendto(sockfd, buffer,head_len,0,(struct sockaddr*)addr,(socklen_t)sizeof(struct sockaddr_in)) < 0)
	{
		perror("sendto\n");
	}
}

void *recv_packet(clock_t start)
{
	struct tcphdr *tcp;
	char *srcaddr;
	int loopend;
	int size;
	char readbuff[1600];
	struct sockaddr_in from;
	int from_len;
	int n;
	float costtime;
	clock_t end;

	tcp = (struct tcphdr*)(readbuff + 20);
	for(n=startport;n<endport;n++)
	{
		size = recv(sockfd, readbuff, 1600, 0);
		if(size <(20+20))
		{
			continue;
		}

		if(ntohs(tcp->dest)!= sourceport)
		{
			continue;
		}

		if(tcp->rst&&tcp->ack)
		{
			continue;
		}
		if(tcp->ack&&tcp->syn)
		{
			printf("%5u open\n",(ntohs(tcp->source)));
			fflush(stdout);
			continue;
		}
	}
	end = clock();
	costtime = (float)(end-start)/CLOCKS_PER_SEC;
	printf("\nSmart Nmap finished:%s scanned in %f seconds\n", ip, costtime);
}

unsigned short check_sum(unsigned short *buffer, int size)
{
	unsigned int cksum = 0;
	while(size>1)
	{
		cksum+=*buffer++;
		size-=sizeof(unsigned short);
	}

	if(size)
	{
		cksum += *(unsigned char*)buffer;
	}
	cksum = (cksum>>16) + (cksum& 0xffff);
	cksum += (cksum>>16);
	return (unsigned short)(~cksum);
}
