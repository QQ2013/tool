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
#include"ping_ip.h"

int ping_ip(char *ip)	
{   
    gettimeofday(&start_t,NULL);	
	struct hostent *host; 
	int on =1;

	alarmnum=0;

	//printf("ip_ping start:%s\n",ip);
	memset(&dest,0,sizeof(dest));	
	dest.sin_family=PF_INET;	
	dest.sin_port=ntohs(0);	
	dest.sin_addr.s_addr=inet_addr(ip);

	//printf("ping_ip start :%s--%s\n",ip,inet_ntoa(dest.sin_addr));
	if((sockfd = socket(PF_INET,SOCK_RAW,IPPROTO_ICMP))<0){ 
		perror("raw socket created error");
		exit(1);
	}

	setsockopt(sockfd,IPPROTO_IP,IP_HDRINCL,&on,sizeof(on));  

	setuid(getuid());
	pid=getpid(); 

	set_sighandler();

	//printf("ping_ip%s\n",ip);
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
	//n_t send ping times
    while(nrecv<n_t){
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
    struct timeval *sendtime;
	double rtt;

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
void alarm_handler(int signo)
{
    //n_t send ping times
	if (alarmnum > n_t)
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
    gettimeofday(&end_t,NULL);	
    float utime  = (end_t.tv_sec-start_t.tv_sec)*1000+(end_t.tv_usec-start_t.tv_usec)/1000.0;
	fprintf(f,"%s %d %f\n",inet_ntoa(dest.sin_addr),nrecv,utime);
//	printf("exit:%d %s %d\n",pid,inet_ntoa(dest.sin_addr),nrecv);
	fclose(f);

	exit(1);
}
void int_handler_exit(int sig)
{
    is_exit = 1;
	printf("%d %d",getpid(),is_exit);
	return;
}
