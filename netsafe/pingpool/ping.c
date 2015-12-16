#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/param.h>
#include <rpc/types.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include "ping_ip.h"
#include <sys/stat.h>
#include <fcntl.h>
int born_ip(char *srt,int *ip);
void add_ip(int *ip);
int cmp_ip(int *s, int *e);
void usage();
int pup = 0;
int isw = 0;
int main(int argc, char **argv)
{
	//ping send datalen
	setbuf(stdout,NULL);
	datalen = DEFAULT_LEN;
	//pipe


	int opt=0;
	int i=0;
	int options_index=0;
	int ip_start[4];
	int ip_end[4];
	int pid_num = 10;
	int sec = 0;
	int usec = 0;
	n_t = 3;

	char ip_scan[100];
	char ip_scan3[100];
	char out[100];
	FILE *outf;


	sprintf(out,"ips-test");

	while((opt=getopt_long(argc,argv,"s:e:t:u:c:f:n:p:wh",NULL,&options_index))!=EOF )
	{
		switch(opt)
		{
			case  0 : break;
			case 's': 
					  //printf("%s\n",optarg);
					  if (born_ip(optarg,ip_start) < 0) 
					  {
						  usage();
						  return 0;
					  }
					  else
					  {
						  ip_end[0]=ip_start[0];
						  ip_end[1]=ip_start[1];
						  ip_end[2]=ip_start[2];
						  ip_end[3]=255;
						  break;
						  for (i=0;i<4;i++)
						  {
							  printf("%d ",ip_start[i]);
						      printf("\n");
						  }
					  }
					  break; 
			case 'e': 
					  //printf("%s\n",optarg);
					  if (born_ip(optarg,ip_end) < 0) 
					  {
						  usage();
						  return 0;
					  }
					  else
					  {
						  break;
						  for (i=0;i<4;i++)
						  {
							  printf("%d ",ip_end[i]);
						      printf("\n");
						  }
					  }
					  break; 

			case 'c': 
					  //printf("%s\n",optarg);
					  pid_num = atoi(optarg);
					  break; 
			case 't': 
					  sec = atoi(optarg)/1000;
					  usec = atoi(optarg)%1000;
					  break; 
			case 'n': 
					  n_t = atoi(optarg);
					  break; 
			case 'p': 
					  pup = atoi(optarg);
					  break; 
			case 'f':
					  sprintf(out,"%s",optarg);
				  break;
			case 'w': 
					  isw = 1;
					  break; 
			case 'h': 
			case ':': 
            default:
					  usage();
					  return 0;
					  break;
		}

	}


	if (optind < argc)
	{
		usage();
		return 0;
	}
    //需要加入参数正确性校验
	//ip 0-9
	if( isw == 1)
	{
	    outf = fopen(out,"a+");
	}
	else
	{
	    outf = fopen(out,"w");
	}
	
	if(!outf)
	{
		usage();
		return 0;
	}
	//单次扫描最大时间
	if (sec + usec ==0 )
	{
		sec = 1;
		usec =0;
	}
	val_alarm.it_interval.tv_sec = sec;
	val_alarm.it_interval.tv_usec=usec*1000;
	val_alarm.it_value.tv_sec=0;
	val_alarm.it_value.tv_usec=1;
	
	if(pipe(mypipe))
	{
		perror("pipe failed.");
		return 0;
	}
	f=fdopen(mypipe[0],"r");
	if(f==NULL) 
	{
		perror("open pipe for reading failed.");
		return ;
	}
	//info
	gettimeofday(&start_t,NULL);	
	if(pup==2)
	{
		printf("ping scan %d.%d.%d.%d--%d.%d.%d.%d: \n",
				ip_start[0], ip_start[1], ip_start[2], ip_start[3],
				ip_end[0], ip_end[1], ip_end[2], ip_end[3]
			  );

	}
	else
	{
		printf("ping scan %d.%d.%d.%d--%d.%d.%d.%d result to %s:\n",
				ip_start[0], ip_start[1], ip_start[2], ip_start[3],
				ip_end[0], ip_end[1], ip_end[2], ip_end[3],
				out
			  );
	
	}
	signal(SIGCHLD,SIG_IGN);
	setvbuf(f,NULL,_IONBF,0);
	//创建pid_num个进程
	i = 0;
	pid = 0;
	while (i<pid_num)
	{

		if (cmp_ip(ip_start,ip_end) > 0)
		{
			break;
		}
		//sprintf(ip_scan,"%.3d.%.3d.%.3d.%.3d",ip_start[0],ip_start[1],ip_start[2],ip_start[3]);
		sprintf(ip_scan,"%d.%d.%d.%d",ip_start[0],ip_start[1],ip_start[2],ip_start[3]);
		printf("%.3d.%.3d.%.3d.%.3d\r",ip_start[0],ip_start[1],ip_start[2],ip_start[3]);
		pid = fork();
		if (pid < (pid_t)0)
		{
			fprintf(stderr,"scan %s wrong!\n",ip_scan);
			break;
		}
		if (pid == (pid_t)0)
		{
		//	printf("%s\n",ip_scan);
			ping_ip(ip_scan);
		}
		i++;
		add_ip(ip_start);
	}
	//扫描的ip小于进程数
	pid_num = i;

	int  is_up;
	char ip_result[20];
	int pid_runnum;
	int readl;
	float utime;
	pid_runnum = pid_num;
	int id = 0;
	pid_t my_pid;
	char msg[100];

	//通过外部文件置exit为1，停止创建新的进程
	FILE *fexit;
	fexit = fopen("exit","w");
	
    //置exit为0
	fprintf(fexit,"%d\n",0);
	fflush(fexit);
	fclose(fexit);

	int reade;
	while(1)
	{
		//this can make better by only send up ip to dec io works
		readl=fscanf(f,"%s %d %f",&ip_result,&is_up,&utime);
		if(readl<2)
		{
			fprintf(stderr,"Some of our childrens died.\n");
		}
		else
		{
		   my_pid = getpid();  
	       //printf("pid:%d id:%d:%s %d\n",my_pid,id,ip_result,is_up);
		   //printf("%s %d\n",ip_result,is_up);
		   if (pup == 1)
		   { 
			   if (is_up)
			   {
				   sprintf(msg,"%-20s %d %.3f up\n",ip_result,is_up,utime);
				   fputs(msg,outf);
				   fflush(outf);
			   }
		   }
		   else if( pup == 2)
		   {
			   if (is_up)
			   {
				   printf("%-20s %d %.3f up\n",ip_result,is_up,utime);
			   }
		   
		   }
		   else
		   {
			   if (is_up)
			   {
				   sprintf(msg,"%-20s %d %.3f up\n",ip_result,is_up,utime);
			   }
			   else
			   {
				   sprintf(msg,"%-20s %d %.3f down\n",ip_result,is_up,utime);
			   }
			   fputs(msg,outf);
			   fflush(outf);
		   }
		   //id++;
		
		}

		pid_runnum--;
		//读取exit值
		fexit = fopen("exit","r");
		reade = fscanf(fexit,"%d\n",&is_exit);
		fclose(fexit);

		if(is_exit < 1)
		{
			if(cmp_ip(ip_start,ip_end) <= 0)
			{
				sprintf(ip_scan,"%d.%d.%d.%d",ip_start[0],ip_start[1],ip_start[2],ip_start[3]);

				printf("%.3d.%.3d.%.3d.%.3d\r",ip_start[0],ip_start[1],ip_start[2],ip_start[3]);

				pid_runnum++;
				pid = fork();
				if (pid < (pid_t)0)
				{
					fprintf(stderr,"scan %s wrong!\n",ip_scan);
					break;
				}
				if (pid == (pid_t)0)
				{
					ping_ip(ip_scan);
				}
				add_ip(ip_start);
			}
		}


		if (pid_runnum < 1)
		{
		    break;
		}
	}

	gettimeofday(&end_t,NULL);	
    float stime  = (end_t.tv_sec-start_t.tv_sec)+(end_t.tv_usec-start_t.tv_usec)/1000000.0;
	printf("end:%.3d.%.3d.%.3d.%.3d using time:%.4fs\n",ip_start[0],ip_start[1],ip_start[2],ip_start[3],stime);

	fclose(f);
	fclose(outf);
	return 0;
}
int born_ip(char *str,int *ip)
{
	int  len = strlen(str);
	int  i = 0;
	int  p[3];
	int  p_n = 0;
	char ip_tmp[5];
    for (i=0;i<len;i++)
	{
	    if (str[i]== '.')
		{
			p[p_n] = i;
			p_n++;
		}
		else
		{
			if (str[i] < '0' || str[i] > '9')
			{
				//printf("c\n");
				return -1;
			}
		}
	}
    if (p_n != 3)
	{
		//printf(".\n");
		return -1;
	}
    
	//get ip0
    if(p[0] == 0 || p[0] > 3 )
	{
		return -1;
	}
	
	for (i=0;i<p[0];i++)
	{
		ip_tmp[i] = str[i];
	}
	ip_tmp[i]='\0';
	ip[0] = atoi(ip_tmp);
	if (ip[0] > 255 )
	{
		return -1;
	}
    
    //get ip1
    if(p[1]-1 == p[0] || p[1] - p[0] > 4 )
	{
		return -1;
	}
	
	for (i=p[0]+1;i<p[1];i++)
	{
		ip_tmp[i-p[0]-1] = str[i];
	}
	ip_tmp[i-p[0]-1]='\0';
	ip[1] = atoi(ip_tmp);
	if (ip[1] > 255 )
	{
		return -1;
	}

    //get ip2 
    if(p[2]-1 == p[1] || p[2] - p[1] > 4 )
	{
		return -1;
	}
	
	for (i=p[1]+1;i<p[2];i++)
	{
		ip_tmp[i-p[1]-1] = str[i];
	}
	ip_tmp[i-p[1]-1]='\0';
	ip[2] = atoi(ip_tmp);
	if (ip[2] > 255 )
	{
		return -1;
	}
    
    //get ip3 
    if(p[2] == len-1 || len - p[2] > 4 )
	{
		return -1;
	}
	
	for (i=p[2]+1;i<len;i++)
	{
		ip_tmp[i-p[2]-1] = str[i];
	}
	ip_tmp[i-p[2]-1]='\0';
	ip[3] = atoi(ip_tmp);
	if (ip[3] > 255 )
	{
		return -1;
	}
	if (ip[3] == 0)
	{
		ip[3] = 1;
	}
    
	return 1;
}
void add_ip(int *ip)
{
	ip[3] = ip[3] + 1;
    int i = 0;
	for (i=3;i>0;i--)
	{
		if (ip[i] > 255)
		{
			ip[i] = 0;
			ip[i-1] = ip[i-1] + 1;
		}
	}
	if (ip[3] == 0)
	{
		ip[3] = 1;
	}
}
int cmp_ip(int *s, int *e)
{
	int i = 0;
	for (i=0;i<4;i++)
	{
		if (s[i]-e[i]==0)
		{
			continue;
		}

		if (s[i]-e[i] > 0)
		{
			return 1;
		}
		if (s[i]-e[i] < 0)
		{
			return -1;
		}

	}

	if (i==4)
	{
		return 0;
	}

}
void usage()
{
	fprintf(stderr,
	       "pingseg -s start_ip \n"
		   "-s start_ip\n"
		   "-e end_ip [=the end of the start ip seg x.x.x.255 ]\n"
		   "-t ping send alarm by ms  [1000]\n"
           "-c ping pid num define by your net speed [=10]\n"
		   "-n ping times [=3]\n"
           "-f result save file [=ipstest]\n"
           "-w result save file by add mode\n"
		   "-p 0:fprint all 1:only fprint up host 2:print up host [=0]\n"
		   "-h help\n"
		   "please make sure ip is right!\n"
		  );
}
