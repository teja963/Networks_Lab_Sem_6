#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main()
{
	char command[512], Qtype[10], DomainName[20];
	printf("Enter name of the domain to be resolved\n");
	scanf("%s", DomainName);
	printf("Enter type of query\n");
	scanf("%s", Qtype);
	if((strcmp(Qtype, "a")==0) || (strcmp(Qtype, "aaaa")==0) || (strcmp(Qtype, "cname")==0) || (strcmp(Qtype, "ns")==0))
	{
		bzero(command, sizeof(command));

		sprintf(command, "nslookup -port=1234 -type=%s %s -retry=2 127.0.0.1 > outputfile", Qtype, DomainName);
		system(command);

		bzero(command, sizeof(command));
		sprintf(command, "cat outputfile");
		system(command);
	}
	else
	{
		printf("UNAUTHORISED REQUEST\n");
	}
	return 0;
}
