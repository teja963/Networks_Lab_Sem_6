#include<stdio.h>
#include<netdb.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/nameser.h>
#include<resolv.h>
#include<errno.h>
#include<arpa/inet.h>
#include<time.h>
#include<pthread.h>

extern const char *_res_opcodes[];
char dname[50];
char authoritative_server[100];
pthread_t tid[2];
pthread_mutex_t lock;

#define MY_GET16(s, cp) do { \
        register const u_char *t_cp = (const u_char *)(cp); \
        (s) = ((u_int16_t)t_cp[0] << 8) \
            | ((u_int16_t)t_cp[1]) \
            ; \
        (cp) += NS_INT16SZ; \
} while (0)

#define MY_GET32(l, cp) do { \
        register const u_char *t_cp = (const u_char *)(cp); \
        (l) = ((u_int32_t)t_cp[0] << 24) \
            | ((u_int32_t)t_cp[1] << 16) \
            | ((u_int32_t)t_cp[2] << 8) \
            | ((u_int32_t)t_cp[3]) \
            ; \
        (cp) += NS_INT32SZ; \
} while (0)

void* manage_ttl(void* arg)
{
	char line[200],temp[200];
	int i,count;
	time_t begin,end;
	while(1)
	{
		begin = time(NULL);
		bzero(line,sizeof(line));
		
		pthread_mutex_lock(&lock);
		FILE *ttlfp = fopen("dns_cache.txt","a+");
		FILE *tempfp = fopen("temp.txt","a+");
		while(fgets(line,200,ttlfp)!=NULL)
		{
			bzero(temp,sizeof(temp));
			//printf("%s",line);
			long int offset = -1*(strlen(line));
			//fseek(ttlfp, offset, SEEK_CUR);
			//printf("%s",line);
			//continue;
			i=0;
			count = 0;
			while(1)
			{
				if(count==4)
					break;
				if(line[i++]==' ')
					count++;
			}
			line[strlen(line)-1] = '\0';
			//printf("%s  ",line+i);	
			long int ttl = atoi(line+i);
			ttl-=5;
			if(ttl<=0)
				continue;
			//printf("%ld\n",ttl);
			strncpy(temp,line,i);
			sprintf(temp+i,"%ld",ttl);
			//printf("%s\n",temp);
			fprintf(tempfp,"%s\n",temp);
			//fseek(ttlfp, -1*offset, SEEK_CUR);
		}
		fclose(ttlfp);
		remove("dns_cache.txt");
		fclose(tempfp);
		rename("temp.txt","dns_cache.txt");
		pthread_mutex_unlock(&lock);
		while(((end=time(NULL))-begin)<5);
		//printf("elapsed time : %lds\n",end-begin);
	}
}
		

void find_dname_addr(char *dnsserver)
{
	struct addrinfo hints, *res, *result;
	int err;
	char addrstr[100];
	void *ptr;
	
	memset(&hints,0,sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;
	
	err = getaddrinfo(dnsserver, NULL, &hints, &result);
	
	if(err!=0)
	{
		perror("getaddrinfo");
		return;
	}
	
	res = result;
	
	printf("Authoritative Server: %s, ",dnsserver);
	while(res)
	{
		inet_ntop(res->ai_family, res->ai_addr->sa_data,addrstr,100);
		switch(res->ai_family)
		{
			case AF_INET:
				ptr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
				inet_ntop(res->ai_family, ptr, addrstr,100);
				bzero(authoritative_server,sizeof(authoritative_server));
				strcpy(authoritative_server,addrstr);
				printf("IPv%d address: %s\n",res->ai_family == PF_INET6?6:4, addrstr);
				return;
				break;
			case AF_INET6:
				ptr = &((struct sockaddr_in6 *)res->ai_addr)->sin6_addr;
				break;	
		}
		inet_ntop(res->ai_family, ptr, addrstr,100);
		printf("IPv%d address: %s (%s)\n",res->ai_family == PF_INET6?6:4, addrstr, res->ai_canonname);
		res = res->ai_next;
	}
	return;
}		

char *getserver(char *a_str, char *a_delim,char *server_addr)
{

	char * token;
  	char * temp_token;

  
  	temp_token = strtok(a_str, a_delim);

  	while( temp_token != NULL )
  	{
    		token = temp_token;
    		temp_token = strtok(NULL, a_delim);
  	}

	//printf("%s\n",token);
  	strcpy(server_addr,token);
  	return token;
}


void check_cache(char host[100], char *type, char *l)
{
	FILE *fpcheck = fopen("dns_cache.txt","ab+");
	//printf("%d\n",fpcheck);
	int len = strlen(host);
	if(len!=1)
		len-=1;
	char line[200];
	//printf("%s %d\n",host,len);
	while(fgets(line,200,fpcheck)!=NULL)
	{
		if(strncmp(host,line,len)==0)
		{	
			if(strncmp(line+len+1,type,strlen(type))==0 && strncmp(line+len+strlen(type)+2,"IN",2)==0)
			{
				int position = len+strlen(type)+5;
				int i=0;
				while(line[position+i]!=' ')
				{
					l[i] = line[position+i];
					i++;
				}
				l[i]='\0';
				fclose(fpcheck);
				return;
			}
		}			
	}
	bzero(l,sizeof(line));
	strcpy(l,"Not Found");
	fclose(fpcheck);
	return;
}
	
void strtype(int type,char *t)
{
	if(type==1)
		strcpy(t,"A");
	else if(type==2)
		strcpy(t,"NS");
	else if(type==5)
		strcpy(t,"CNAME");
	else if(type==15)
		strcpy(t,"MX");
	else if(type==28)
		strcpy(t,"AAAA");
	return;
}
		
int main(int argc, char **argv)
{

	pthread_mutex_init(&lock,NULL);
	pthread_create(&(tid[0]), NULL, &manage_ttl, NULL);
	time_t begin,end;
	
	//while(((end=time(NULL))-begin)<1);
	
        int socketfd,confd;
        struct sockaddr_in server, client;

        //Create UDP socket
        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if((socketfd==-1)) {
                printf("Socket creation failed \n");
                exit(0);
        }
        else
                printf("Socket created successfully \n");

        bzero(&server,sizeof(server));
	bzero(&client,sizeof(client));

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(atoi(argv[1]));


        if((bind(socketfd, (struct sockaddr *)&server, sizeof(server))) !=0) {
                printf("Socket binding failed \n");
                exit(0);
        }
        else
                printf("Socket successfully binded \n");

	
	char buffer[512];
	int clen = sizeof(client);
	int n;
	
	ns_msg msg;
	ns_rr rr;
	char server_addr[50];
	
	while(1)
	{
	bzero(buffer,sizeof(buffer));
	
	n = recvfrom(socketfd, buffer, sizeof(buffer) - 1, MSG_WAITALL, (struct sockaddr *)&client,&clen);
	
	char url[50];
	
	int len=0, check_url=buffer[12], j=0;
	
	//printf("%d\n",check_url);
	int i;
	for(i=13;i<n;i++)
	{
		if(buffer[i]==0)
			break;
		if(j==check_url)
		{
			//printf("%d\n",j);
			url[len++] = '.';
			j=0;
			check_url = buffer[i];
			//printf("%d %d\n",check_url,i);
			
		}
		else
		{
			url[len++] = buffer[i];
			j++;
		}
		
	}
	
	int qtype = buffer[i+2];
		
	url[len] = '.';
	url[len+1] = '\0';
	//printf("%s\n",url);
	//printf("%d\n",qtype);
	
	int id1 = buffer[0],id2=buffer[1];
	//printf("%d %d\n",id1,id2);

	n=0;
	
	char host_to_lookup[200],temp_string[200];

	strcpy(dname,"8.8.8.8");
	
	int dot_count = 0;
	
	for (i=0;i<strlen(url);i++)
	{
		if(url[i]=='.')
			dot_count++;
	}
		
	char response[1024];
	char t[10],line[200];
	int p;
	u_char buf[PACKETSZ],nsbuf[4096];
	
	char dispbuf[4096];
	//setenv("LOCALDOMAIN",dname,1);
	//printf("%d\n",dot_count);
	int flag_check = 1;
	int check2 = 1;
	res_init();
	
	while(dot_count>=0)
	{
		check2 = 1;
		_res.nsaddr_list[0].sin_addr.s_addr = inet_addr(dname);
		
		flag_check = 2;
		bzero(host_to_lookup,sizeof(host_to_lookup));
		bzero(temp_string,sizeof(temp_string));
		bzero(buf,sizeof(buf));
		bzero(response,sizeof(response));
		j=strlen(url)-1;
		i=0;
		while(url[j]!='.' && j>=0)
		{
			if(url[j]=='*')
				{
					temp_string[i++]='.';
					j--;
				}
			else
				temp_string[i++]=url[j--];
			
		}
		url[j] = '*';
		if(j==strlen(url)-1)
		{
			temp_string[0] = '.';
		}
		
		int clss = C_IN;
		int type1 = T_NS;
		
		i=0;
		j=0;
		for(i=strlen(temp_string)-1;i>=0;i--)
		{
			host_to_lookup[j++]=temp_string[i];
		}
		host_to_lookup[j] = '\0';	
		
		if(strncmp("www.",host_to_lookup,4)==0)
			break;
			
		strtype(type1,t);
		
		pthread_mutex_lock(&lock);
		check_cache(host_to_lookup,t,line);
		pthread_mutex_unlock(&lock);
		
		//printf("%s\n",line);
	
		if(strncmp(line,"Not Found",9)!=0)
		{
			printf("%s type Record for <%s> found in cache: %s\n",t,host_to_lookup,line);
			strcpy(authoritative_server,line);
			dot_count--;
			continue;
		}

		printf("Sending to DNS Server for Domain <%s>\n",host_to_lookup);
		
		n = res_mkquery(QUERY,host_to_lookup,clss,type1,NULL,0,NULL,(u_char *)buf,PACKETSZ);
		
		p = res_send(buf,n,response,1024);
		//p = res_query(".", ns_c_in, ns_t_ns, response, sizeof(response));
		//printf("Response size: %d\n",p);
		//printf("%s\n",strerror(errno));

		ns_initparse(response,p,&msg);
		int l = ns_msg_count(msg,ns_s_an);
		char buf[NS_MAXDNAME];
		
		for(j=0;j<l;j++)
		{
		ns_parserr(&msg, ns_s_an, j, &rr);
		ns_sprintrr(&msg, &rr, NULL, NULL, dispbuf, sizeof(dispbuf));
		//printf("%s\n",dispbuf);
		u_int type;
		type = ns_rr_type(rr);
		const u_char *rd = ns_rr_rdata(rr);
		u_int32_t soa[5];
		switch (type) 
		{
		case ns_t_soa:
			n = ns_name_uncompress(ns_msg_base(msg), ns_msg_end(msg),rd, buf, sizeof buf);
			if (n < 0)
				goto error;
			//putc(',', trace);
			//fputs(buf, trace);
			rd += n;
			n = ns_name_uncompress(ns_msg_base(msg), ns_msg_end(msg),rd, buf, sizeof buf);
				       
			if (n < 0)
				goto error;
			//putc(',', trace);
			//fputs(buf, trace);
			rd += n;
			if (ns_msg_end(msg) - rd < 5*NS_INT32SZ)
				goto error;
			for (n = 0; n < 5; n++)
				MY_GET32(soa[n], rd);
			sprintf(buf, "%u,%u,%u,%u,%u",soa[0], soa[1], soa[2], soa[3], soa[4]);
			break;
		case ns_t_a:
			inet_ntop(AF_INET, rd, buf, sizeof buf);
			break;
		case ns_t_aaaa:
			inet_ntop(AF_INET6, rd, buf, sizeof buf);
			break;
		case ns_t_ns:
		case ns_t_ptr:
		case ns_t_cname:
			n = ns_name_uncompress(ns_msg_base(msg), ns_msg_end(msg),rd, buf, sizeof buf);
				 
			if (n < 0)
				goto error;
			if(check2==3)
			{
				bzero(dname,sizeof(dname));
				strcpy(dname,buf);
			}
			break;
		case ns_t_txt:
			snprintf(buf, (size_t)rd[0]+1, "%s", rd+1);
			break;
		default:
 			error:sprintf(buf, "[%u]", ns_rr_rdlen(rr));
		}	
		if(check2==3)
		{
			find_dname_addr(dname);
			check2=2;
		}
		pthread_mutex_lock(&lock);
		FILE *fp = fopen("dns_cache.txt","a+");
		fprintf(fp,"%s %s %s %s %lu\n",ns_rr_name(rr), p_type(type), p_class(ns_rr_class(rr)), buf, (u_long)ns_rr_ttl(rr));
		fclose(fp);
		pthread_mutex_unlock(&lock);
		}
		dot_count--;	
		strcpy(authoritative_server,buf);
	}
	
	find_dname_addr(authoritative_server);
	_res.nsaddr_list[0].sin_addr.s_addr = inet_addr(authoritative_server);
	
	strtype(qtype,t);
	check_cache(host_to_lookup,t,line);
	//printf("%s\n",host_to_lookup);
		
	
	if(strncmp(line,"Not Found",9)!=0)
	{
		printf("%s type Record for <%s> found in cache: %s\n",t,host_to_lookup,line);
		//strcpy(authoritative_server,line);
	}
	
	n = res_mkquery(QUERY,host_to_lookup,C_IN,qtype,NULL,0,NULL,(u_char *)buf,PACKETSZ);
	p = res_send(buf,n,response,1024);
	
	ns_initparse(response,p,&msg);
		int l = ns_msg_count(msg,ns_s_an);
		char buf2[NS_MAXDNAME];
		
		for(j=0;j<l;j++)
		{
		ns_parserr(&msg, ns_s_an, j, &rr);
		ns_sprintrr(&msg, &rr, NULL, NULL, dispbuf, sizeof(dispbuf));
		//printf("%s\n",dispbuf);
		u_int type;
		type = ns_rr_type(rr);
		const u_char *rd = ns_rr_rdata(rr);
		u_int32_t soa[5];
		switch (type) 
		{
		case ns_t_soa:
			n = ns_name_uncompress(ns_msg_base(msg), ns_msg_end(msg),rd, buf, sizeof buf);
			if (n < 0)
				goto error;
			//putc(',', trace);
			//fputs(buf, trace);
			rd += n;
			n = ns_name_uncompress(ns_msg_base(msg), ns_msg_end(msg),rd, buf, sizeof buf);
				       
			if (n < 0)
				goto error;
			//putc(',', trace);
			//fputs(buf, trace);
			rd += n;
			if (ns_msg_end(msg) - rd < 5*NS_INT32SZ)
				goto error;
			for (n = 0; n < 5; n++)
				MY_GET32(soa[n], rd);
			sprintf(buf2, "%u,%u,%u,%u,%u",soa[0], soa[1], soa[2], soa[3], soa[4]);
			break;
		case ns_t_a:
			inet_ntop(AF_INET, rd, buf2, sizeof buf2);
			break;
		case ns_t_aaaa:
			inet_ntop(AF_INET6, rd, buf2, sizeof buf2);
			break;
		case ns_t_ns:
		case ns_t_ptr:
		case ns_t_cname:
			n = ns_name_uncompress(ns_msg_base(msg), ns_msg_end(msg),rd, buf2, sizeof buf2);
				 
			if (n < 0)
				goto error;
			if(check2==3)
			{
				bzero(dname,sizeof(dname));
				strcpy(dname,buf2);
			}
			break;
		case ns_t_txt:
			snprintf(buf2, (size_t)rd[0]+1, "%s", rd+1);
			break;
		default:
 			error2:sprintf(buf2, "[%u]", ns_rr_rdlen(rr));
		}	
		if(check2==3)
		{
			find_dname_addr(dname);
			check2=2;
		}
		pthread_mutex_lock(&lock);
		FILE *fp = fopen("dns_cache.txt","a+");
		if(strncmp(line,"Not Found",9)==0)
			fprintf(fp,"%s %s %s %s %lu\n",ns_rr_name(rr), p_type(type), p_class(ns_rr_class(rr)), buf2, (u_long)ns_rr_ttl(rr));
		fclose(fp);
		pthread_mutex_unlock(&lock);
		}
	
	//HEADER *dnsheader;
	//dnsheader = (HEADER *)&response;
	//printf("QR= %d\n",dnsheader->qr);
	//printf("AA= %d\n",dnsheader->aa);
	
	
	response[0] = id1;
	response[1] = id2;
	sendto(socketfd, (char *)response, p, MSG_CONFIRM, (struct sockaddr *)&client,sizeof(client));
	printf("\n");
	}
	close(socketfd);
	pthread_mutex_destroy(&lock);
	return 0;

}
