#ifndef __PING_IP_H
#define __PING_IP_H
#define ICMP_ECHOREPLY 0 
#define ICMP_ECHO	

#define BUFSIZE 1500	
#define DEFAULT_LEN 56  


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;


struct icmphdr {
    u8 type;     
    u8 code;	
    u16 checksum;	
    union{
    	struct{
	    u16 id;
	    u16 sequence;
	}echo;
	u32 gateway;
	struct{
 	    u16 unsed;
	    u16 mtu;
	}frag; 
    }un;
  
    u8 data[0];
#define icmp_id un.echo.id
#define icmp_seq un.echo.sequence
};
#define ICMP_HSIZE sizeof(struct icmphdr)

struct iphdr {
    u8 hlen:4, ver:4;	
    u8 tos;		
    u16 tot_len;	
    u16 id;			
    u16 frag_off;	
    u8 ttl;			
    u8 protocol;	
    u16 check;		
    u32 saddr;		
    u32 daddr;		
};

char *hostname;			
int datalen; 
char sendbuf[BUFSIZE];		
char recvbuf[BUFSIZE];		
int nsent;			
int nrecv;			
pid_t pid;			
struct timeval recvtime;	
int sockfd;			
struct sockaddr_in dest;	
struct sockaddr_in from;	
struct sigaction act_alarm;
struct sigaction act_int;


void alarm_handler(int);	
void int_handler(int);		
void pid_exit();
void set_sighandler();		
void send_ping();		
void recv_reply();		
u16 checksum(u8 *buf, int len); 
int handle_pkt();		
void get_statistics(int, int);	
void bail(const char *);	

#define IP_HSIZE sizeof(struct iphdr)   
#define IPVERSION  4   

#define PING_NUM 3
struct itimerval val_alarm;
int alarmnum;
int mypipe[2];
FILE *f;
void int_handler_exit(int sig);
int is_exit;    
int n_t;
struct timeval start_t;
struct timeval end_t;
#endif
