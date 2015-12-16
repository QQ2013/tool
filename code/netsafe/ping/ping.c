#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>	
#include<unistd.h>	
#include<string.h>
#include<sys/socket.h>	
#include<sys/types.h>
#include<netdb.h>	
#include<errno.h>	
#include<arpa/inet.h>	
#include<signal.h>	
#include<sys/wait.h>
#include<netinet/in.h>	

#include"ping.h"
#define IP_HSIZE sizeof(struct iphdr)   
#define IPVERSION  4   

#define PING_NUM 3
struct itimerval val_alarm={.it_interval.tv_sec = 1,	
                               .it_interval.tv_usec=0,
                               .it_value.tv_sec=0,
                               .it_value.tv_usec=1
                                };
struct itimerval val_alarm1={.it_interval.tv_sec = 6,	
                               .it_interval.tv_usec=0,
                               .it_value.tv_sec=0,
                               .it_value.tv_usec=1
                                };
int alarmnum;
int ping_id;
int mypipe[2];
FILE *f;
int *data;
struct timeval start_t;
struct timeval end_t;

void result();
char *ip_seg;
#define IP_END 254
int main(int argc,char **argv)	
{
    gettimeofday(&start_t,NULL);	
	if(argc<2){		
		printf("Usage: %s hostname\n",argv[0]);
		exit(1);
	}

	if(pipe(mypipe))
	{
		perror("pipe failed.");
		return 0;
	}
	signal(SIGCHLD,SIG_IGN);
	int i=0;
	char ip[100];
	pid_t pid = 0;
	for (i=1;i<IP_END;i++)
	{
		ping_id = i;
		pid = fork();
		if (pid < (pid_t)0)
		{
			pid_exit();
		}

		if (pid == (pid_t)0)
		{
			sprintf(ip,"%s.%d",argv[1],i);
			ping_ip(ip);
		}
	}

	f=fdopen(mypipe[0],"r");
	if(f==NULL) 
	{
		perror("open pipe for reading failed.");
		return ;
	}
	setvbuf(f,NULL,_IONBF,0);

	int hostup_num = 0;
	int is_up;
	int readl;
    data =(int *) malloc(sizeof(int)*300);

	for (i=0;i<300;i++)
	{
		data[i] = 0;
	}
	ip_seg = argv[1];
    
	act_alarm.sa_handler=alarm_handler1;

    if(sigaction(SIGALRM,&act_alarm,NULL)==-1)	
       bail("SIGALRM handler setting fails.");
	
	if((setitimer(ITIMER_REAL,&val_alarm1,NULL))==-1)	
		bail("setitimer fails.");

	i = 0;
	while(i<IP_END-1)
	{
		readl=fscanf(f,"%d %d",&ping_id,&is_up);
		i++;
	//	printf("%d,%d\n",ping_id,is_up);
		if(readl<2)
		{
			fprintf(stderr,"Some of our childrens died.\n");
			continue;
		}

		data[ping_id] = is_up;

	}

	result();
	return 0;
}
void result()
{
    gettimeofday(&end_t,NULL);	

	int i;
    float utime  = (end_t.tv_sec-start_t.tv_sec)*1000+(end_t.tv_usec-start_t.tv_usec)/1000.0;
//	printf("%s:%.3fms\n",ip_seg,utime);
	for (i=0;i<IP_END;i++)
    {
		if (data[i] > 0 )
		{
            //printf("%s.%d up (%d)\n",ip_seg,i,data[i]);
			printf("%s.%d\n",ip_seg,i);
		}
	}
	fclose(f);

	free(data);
	
}
int ping_ip(char *ip)	
{
	struct hostent *host; 
	int on =1;

	alarmnum=0;

	memset(&dest,0,sizeof(dest));	
	dest.sin_family=PF_INET;	
	dest.sin_port=ntohs(0);	
	dest.sin_addr.s_addr=inet_addr(ip);


	if((sockfd = socket(PF_INET,SOCK_RAW,IPPROTO_ICMP))<0){ 
		perror("raw socket created error");
		exit(1);
	}

	setsockopt(sockfd,IPPROTO_IP,IP_HDRINCL,&on,sizeof(on));  

	setuid(getuid());
	pid=getpid(); 

	set_sighandler();

	if((setitimer(ITIMER_REAL,&val_alarm,NULL))==-1)	
		bail("setitimer fails.");

	recv_reply();

	return 0;
}
void send_ping(void)
{
    struct iphdr *ip_hdr;	
    struct icmphdr *icmp_hdr;	
    int len;
    int len1;
		
    ip_hdr=(struct iphdr *)sendbuf;	
    ip_hdr->hlen=sizeof(struct iphdr)>>2;	
    ip_hdr->ver=IPVERSION;	
    ip_hdr->tos=0;	
    ip_hdr->tot_len=IP_HSIZE+ICMP_HSIZE+datalen;	
    ip_hdr->id=0;	
    ip_hdr->frag_off=0;	
    ip_hdr->protocol=IPPROTO_ICMP;
    ip_hdr->ttl=255;	
    ip_hdr->daddr=dest.sin_addr.s_addr;	
    len1=ip_hdr->hlen<<2;	
	
    icmp_hdr=(struct icmphdr *)(sendbuf+len1);	
    icmp_hdr->type=8;  	
    icmp_hdr->code=0;	
    icmp_hdr->icmp_id=pid;	
    icmp_hdr->icmp_seq=nsent++;	
    memset(icmp_hdr->data,0xff,datalen);	 

    gettimeofday((struct timeval *)icmp_hdr->data,NULL);	

    len=ip_hdr->tot_len;	
    icmp_hdr->checksum=0;	
    icmp_hdr->checksum=checksum((u8 *)icmp_hdr,len);  

    sendto(sockfd,sendbuf,len,0,(struct sockaddr *)&dest,sizeof (dest)); 
}

void recv_reply()
{
    int n,len;
    int errno;

    n=nrecv=0;
    len=sizeof(from);	
    while(nrecv<PING_NUM){
      if((n=recvfrom(sockfd,recvbuf,sizeof recvbuf,0,(struct sockaddr *)&from,&len))<0){	
              if(errno==EINTR)	
              continue;
              bail("recvfrom error");
            }

            gettimeofday(&recvtime,NULL);	

            if(handle_pkt())	
            continue;
            nrecv++;
    }
    
    pid_exit();
}
 
u16 checksum(u8 *buf,int len)
{
    u32 sum=0;
    u16 *cbuf;

    cbuf=(u16 *)buf;

    while(len>1){
    sum+=*cbuf++;
    len-=2;
    }

    if(len)
        sum+=*(u8 *)cbuf;

        sum=(sum>>16)+(sum & 0xffff);
        sum+=(sum>>16);

        return ~sum;
}
 
int handle_pkt()
{
    struct iphdr *ip;
    struct icmphdr *icmp;

    int ip_hlen;
    u16 ip_datalen;	
    double rtt;	
    struct timeval *sendtime;

    ip=(struct iphdr *)recvbuf;

    ip_hlen=ip->hlen << 2;
    ip_datalen=ntohs(ip->tot_len)-ip_hlen;

    icmp=(struct icmphdr *)(recvbuf+ip_hlen);

    if(checksum((u8 *)icmp,ip_datalen))	
       return -1;


       if(icmp->icmp_id!=pid)
        return -1;

        sendtime=(struct timeval *)icmp->data; 
        rtt=((&recvtime)->tv_sec-sendtime->tv_sec)*1000+((&recvtime)->tv_usec-sendtime->tv_usec)/1000.0;
		
//        printf("%d bytes from %s:icmp_seq=%u ttl=%d rtt=%.3f ms\n",
//               ip_datalen, 
//               inet_ntoa(from.sin_addr),	
//               icmp->icmp_seq, 
//               ip->ttl,	
//               rtt);	

          return 0;
}

void set_sighandler()
{
    act_alarm.sa_handler=alarm_handler;
    if(sigaction(SIGALRM,&act_alarm,NULL)==-1)	
       bail("SIGALRM handler setting fails.");

       act_int.sa_handler=int_handler;
       if(sigaction(SIGINT,&act_int,NULL)==-1)
         bail("SIGALRM handler setting fails.");
}
 
void get_statistics(int nsent,int nrecv)
{
    printf("--- %s ping statistics ---\n",inet_ntoa(dest.sin_addr)); 
    printf("%d packets transmitted, %d received, %0.0f%% ""packet loss\n",
           nsent,nrecv,1.0*(nsent-nrecv)/nsent*100);
}
 
void bail(const char * on_what)
{
    fputs(strerror(errno),stderr);	
    fputs(":",stderr);
    fputs(on_what,stderr);
    fputc('\n',stderr);	
    exit(1);
}

 
void int_handler(int sig)
{
//  get_statistics(nsent,nrecv);	
    close(sockfd);	
    exit(1);
}
void alarm_handler1(int signo)
{
	if (alarmnum > 0)
	{
		result();
        pid_exit();
	}	
	alarmnum++;
}
void alarm_handler(int signo)
{
	if (alarmnum > 3)
	{
        pid_exit();
	}
    send_ping();	
	alarmnum++;
}
void pid_exit()
{
	close(sockfd);	
	f=fdopen(mypipe[1],"w");
	if(f==NULL)
	{
		perror("open pipe for writing failed.");
        exit(1);		
	}
	fprintf(f,"%d %d\n",ping_id,nrecv);
	fclose(f);

	exit(1);
}
