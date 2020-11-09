#include <stdio.h>
#define FIRE 2

int main()
{
	char num[12]={0};
	printf("please input your number:\n");
	scanf("%s",num);
	gprs_sendMessage(FIRE,num);
	return 0;
}
