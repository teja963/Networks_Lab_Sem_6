#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <inttypes.h>

#define MAXLEN 512

typedef struct QUERY {
	char dname[10][50];
	int type;
}query;

typedef struct DNSheader {
	char ID[2];
	char FLAGS[2];
	char QDCOUNT[2];
	char ANCOUNT[2];
	char NSCOUNT[2];
	char ARCOUNT[2];
}header;

int idx, SIZE;
query Q;
char fetchedIP[500];

struct sockaddr_in client_addr;
socklen_t addrlen;

void strcopy(char *buf, int i, int n) {
	int j;
	for(j=0;j<n;j++) {
		Q.dname[idx][j] = *(buf + i);
		i++;
	}
	Q.dname[idx][j] = '\0';
	idx++;
}

int searchCache(char domain[500], int QType)
{
        int flag=0;
        char buff[1024];
        FILE *fp;
        fp = fopen("cache.txt", "a+");

        while(fgets(buff, 1024, fp) != NULL)
        {
                if(strcmp(buff, "")==0)
                        break;

                char *name, *querytype, *ip;
                const char s[3] = "\t";
                name = strtok(buff, s);
		querytype = strtok(NULL, s);
		int qtype = atoi(querytype);
                ip = strtok(NULL, s);
                char domainName[500];
                sprintf(domainName, "%s", name);

                if((strcmp(domain, domainName)==0) && (qtype==QType))
                {
                        flag=1;
                        printf("\nCache Hit\n");
                        printf("Name:\t%s\n", name);
                        printf("Address: %s\n", ip);
			strcpy(fetchedIP, ip);
                        break;
                }
        }
	fclose(fp);
        if(flag==0)
        {
                printf("Cache Miss\n");
                return 0;
        }
        else
                return 1;
}

void addToCache(char domain[500], int QType)
{

        FILE *fp1, *fp2;
        fp1 = fopen("result", "r");
        fp2 = fopen("cache.txt", "a");
        char buff[500];

        while(fgets(buff, 500, fp1) != NULL)
        {
                if(strncmp(buff, "Address: ", 9)==0)
                {
                        char *token1, *token2;
                        const char s[2] = " ";
                        token1 = strtok(buff, s);
                        token2 = strtok(NULL, s);
                        char line[1024];
                        sprintf(line, "%s\t%d\t%s", domain, QType, token2);
                        // printf("Adding %s to cache\n", line);
                        fwrite(line, 1, strlen(line), fp2);
                        break;
                }
        }
	fclose(fp1);
	fclose(fp2);
}

void getIP(char *ip) {
	char str[512];
	int pos, i;
	FILE *fp = fopen("result", "r");

	while(fgets(str, 512, fp) != NULL) {
		char *p;
		if(strstr(str, "Address: ") != 0) {
			bzero(ip, 50);
			p = strstr(str, "Address: ");
			pos = p - str;
			pos = pos + 9;
			i = 0;
			while(str[pos] != '\n') {
				ip[i++] = str[pos++];
			}
			ip[i] = '\0';
		}
	}
}

char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

void check_4(char final[4], char *octet)
{
    strcpy(final, "");
    if(strlen(octet) == 1)
    {
        strcat(final,"000");
        strcat(final, octet);
    }
    else if(strlen(octet) == 2)
    {
        strcat(final,"00");
        strcat(final, octet);
    }
    else if(strlen(octet) == 3)
    {
        strcat(final,"0");
        strcat(final, octet);
    }
    else
    strcat(final, octet);
}

char* substr(const char *src, int m, int n)
{
    int len = n - m;
 
    char *dest = (char*)malloc(sizeof(char) * (len + 1));
    for (int i = m; i < n && (*(src + i) != '\0'); i++)
    {
        *dest = *(src + i);
        dest++;
    }
    *dest = '\0';
    return dest - len;
}

void parseIPv6(char *RDATA, char *ip2)
{
    char *ip,*octect;
    char final[4], format[20];
    
    if(strstr(ip2,"::"))
    {
        ip = str_replace(ip2,"::",":0000:0000:");
        octect = strtok(ip, ":");
        //sprintf(format, "%%0%dd", 4);
        //sprintf(final, format, octect);
        check_4(final, octect);

    }
    else
    {
        octect = strtok(ip2, ":");
        check_4(final, octect);

        
    }
    char byte[2];
    char *ptr;
    RDATA[0] = strtoumax(substr(final,0,2), &ptr, 16); //atoi(octect);
    RDATA[1] = strtoumax(substr(final,2,4), &ptr, 16); //atoi(octect);

    short i = 1;
    for (short i = 1; i < 6; ++i)
    {
        octect = strtok(NULL, ":");
        check_4(final, octect);
        RDATA[i*2] = strtoumax(substr(final,0,2), &ptr, 16); //atoi(octect);
        RDATA[i*2+1] = strtoumax(substr(final,2,4), &ptr, 16); //atoi(octect);

     //   printf("%s %x %x %i-\n", final,RDATA[i*2], RDATA[i*2+1],i);
        
    }

    octect = strtok(NULL, "\0");
    check_4(final, octect);
    
    printf("%s\n", octect);
    RDATA[14] = strtoumax(substr(final,0,2), &ptr, 16); //atoi(octect);
    RDATA[15] = strtoumax(substr(final,2,4), &ptr, 16); //atoi(octect);

}

void getNameserver(char *nameserver,char *fileName){
	bzero(nameserver,MAXLEN);
	char string[MAXLEN];
	FILE *in_file = fopen(fileName, "r");
	int i,j,position;

	while ( fgets(string,MAXLEN,in_file)!=NULL) {
		if(strstr(string,"nameserver = ")!=0) {//if match found
			char *result = strstr(string, "nameserver = ");
			position = result - string;
			i=position+13;
			j=0;
			while(string[i]!='\n') {
				nameserver[j]=string[i];
				i++;
				j++;
			}
			nameserver[j]='\0';
	    	break;
	    }
	    else if(strstr(string,"canonical name = ")!=0) {//if match found
			char *result = strstr(string, "canonical name = ");
			position = result - string;
			i=position+17;
			j=0;
			while(string[i]!='\n') {
				nameserver[j]=string[i];
				i++;
				j++;
			}
			nameserver[j]='\0';
	    	break;
	    }
	    bzero(string,MAXLEN);
    }
    fclose(in_file);
}

void send_response(char *buf, int server_socket, struct sockaddr_in client_addr, socklen_t addrlen, int flag) {
	char resp[512], ip[50], rdata[16];
	bzero(resp, 512);
	for(int j=0;j<512;j++) {
		resp[j] = *(buf + j);
	}
	resp[2] = resp[2] | 128; 
	resp[3] = 128;
	resp[7] = 1;
	resp[SIZE + 0] = 192;
	resp[SIZE + 1] = 12;
	resp[SIZE + 3] = Q.type;
	resp[SIZE + 5] = 1;
	resp[SIZE + 8] = 1;
	resp[SIZE + 9] = 8;
	if(Q.type == 1) {
		resp[SIZE + 11] = 4;

		if(flag == 0) {
			bzero(ip, 50);
			getIP(ip);
			if(strcmp(ip, "") == 0) {
				resp[7] = 0;
			}
			else {
				char *ip1, *ip2, *ip3, *ip4;
				const char s[2] = ".";
				ip1 = strtok(ip, s);
				ip2 = strtok(NULL, s);
				ip3 = strtok(NULL, s);
				ip4 = strtok(NULL, s);
				resp[SIZE + 12] = atoi(ip1);
				resp[SIZE + 13] = atoi(ip2);
				resp[SIZE + 14] = atoi(ip3);
				resp[SIZE + 15] = atoi(ip4);
				SIZE += 16;
			}
		}
		else {
			char *ip1, *ip2, *ip3, *ip4;
			const char s[2] = ".";
			ip1 = strtok(fetchedIP, s);
			ip2 = strtok(NULL, s);
			ip3 = strtok(NULL, s);
			ip4 = strtok(NULL, s);
			
			resp[SIZE + 12] = atoi(ip1);
	        resp[SIZE + 13] = atoi(ip2);
	        resp[SIZE + 14] = atoi(ip3);
	        resp[SIZE + 15] = atoi(ip4);
	        SIZE += 16;
		}
	}
	else if(Q.type == 28) {
		resp[SIZE + 11] = 16;

		if(flag == 0) {
			bzero(ip, 50);
			bzero(rdata, 16);
			getIP(ip);
			if(strcmp(ip, "") == 0) {
				resp[7] = 0;
			}
			else {
				parseIPv6(rdata, ip);
				for(int j=0;j<16;j++) {
					resp[SIZE + 12 + j] = rdata[j];
				}
				SIZE += 28;
			}
		}
		else {
			parseIPv6(rdata, fetchedIP);
			for(int j=0;j<16;j++) {
				resp[SIZE + 12 + j] = rdata[j];
			}
			SIZE += 28;
		}
		
	}
	else if(Q.type == 2 || Q.type == 5) {
		int i;
		char fileName[512], nameserver[512];
		bzero(fileName, 512);
		bzero(nameserver, 512);
		strcpy(fileName, "result");
		getNameserver(nameserver, fileName);
		if(strcmp(nameserver, "") == 0) {
			resp[7] = 0;
		}
		else {
			char *token[10];
			const char s[2] = ".";
			token[0] = strtok(nameserver, s);
			i=1;
			while((token[i++] = strtok(NULL, s)) != NULL);
			int k = 12;
			for(int j=0;j<i-1;j++) {
				int t = strlen(token[j]);
				resp[SIZE + k++] = t;
				for(int m=0;m<t;m++) {
					resp[SIZE + k++] = *(token[j] + m);
				}
			}
			resp[SIZE + k++] = 0;
			resp[SIZE + 11] = k - 12;
			SIZE += k;
		}
	}
	
	if (sendto(server_socket, &resp, SIZE, 0, (struct sockaddr *)&client_addr, addrlen) < 0)
		perror("sendTo");
}

int main (int argc, char *argv[]) {

	int PORT = atoi(argv[1]);
	int server_socket, bindStat;
	struct sockaddr_in server_addr;
	char buffer[512];

	server_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(server_socket == -1) {
		perror("[-] Socket");
		exit(EXIT_FAILURE);
	}
	printf("[+] Socket created\n");

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	bindStat = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(bindStat == -1) {
		perror("[-] Bind");
		exit(EXIT_FAILURE);
	}
	printf("[+] Binding successfull\n");

	printf("[*]DNS Server Active\n");

	while(1) {
		printf("\n**** Waiting ****\n\n");
		bzero(buffer, 512);
		recvfrom(server_socket, &buffer, 512, 0, (struct sockaddr *)&client_addr, &addrlen);

		// dns header
		header *dns = (header *)calloc(1,sizeof(header));
		dns->ID[0] = buffer[0];
		dns->ID[1] = buffer[1];
		dns->FLAGS[0] = buffer[2];
		dns->FLAGS[1] = buffer[3];
		dns->QDCOUNT[0] = buffer[4];
		dns->QDCOUNT[1] = buffer[5];
		dns->ANCOUNT[0] = buffer[6];
		dns->ANCOUNT[1] = buffer[7];
		dns->NSCOUNT[0] = buffer[8];
		dns->NSCOUNT[1] = buffer[9];
		dns->ARCOUNT[0] = buffer[10];
		dns->ARCOUNT[1] = buffer[11];
		
		// copy domain names and type to Query struct
		idx = 0;
		int i = 12;
		while(buffer[i] != 0) {
			strcopy(buffer, i+1, buffer[i]);
			i = buffer[i] + i + 1;
		}
		Q.type = buffer[i+2];
		SIZE = i+5;

		char domName[500];
		
		strcpy(domName, Q.dname[0]);
                for(int k=1;k<idx;k++) {
                        strcat(domName, "."); 
			strcat(domName, Q.dname[k]);
                }

		int flag;
		if((flag=searchCache(domName, Q.type))==0)
		{
			char nameserver[512], fileName[512] = "root", command[512], domain[512];
			//lookup root servers
			system("nslookup -type=ns . > root");
			getNameserver(nameserver,fileName);
			// printf("%s\n",nameserver);

			// lookup name servers
			for(int j=idx-1;j>0;j--) {
				bzero(command, 512);
				bzero(domain, 512);
				strcpy(command, "nslookup -type=ns ");
				strcpy(domain, Q.dname[j]);
				for(int k=j+1;k<idx;k++) {
					strcat(domain, ".");
					strcat(domain, Q.dname[k]);
				}
				strcat(command, domain);
				strcat(command, " ");
				strcat(command, nameserver);
				strcat(command, " > ");
				bzero(fileName, 512);
				sprintf(fileName,"nameserver%d",idx-j);
				strcat(command, fileName);
				system(command);

				getNameserver(nameserver,fileName);
			}

			//lookup based on type
			bzero(command, 512);
			switch(Q.type) {
				case 1: strcpy(command, "nslookup -type=a ");
						break;
				case 2: strcpy(command, "nslookup -type=ns ");
						break;
				case 5: strcpy(command, "nslookup -type=cname ");
						break;
				case 28: strcpy(command, "nslookup -type=aaaa ");
						break;
				default: printf("[-] Invalid type\n");
			}
			bzero(domain, 512);
			strcpy(domain, Q.dname[0]);
			for(int k=1;k<idx;k++) {
				strcat(domain, ".");
				strcat(domain, Q.dname[k]);
			}
			strcat(command, domain);
			strcat(command, " ");
			strcat(command, nameserver);
			strcat(command, " > result");
			system(command);
			system("cat result");

			addToCache(domName, Q.type);
		}

		//send response
		send_response(buffer, server_socket, client_addr, addrlen, flag);
	}

	close(server_socket);

	return 0;
}

