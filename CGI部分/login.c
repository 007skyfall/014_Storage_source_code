#include <stdio.h> 
#include "cgic.h" 
#include <string.h> 
#include <stdlib.h>  

#define USER_NAME_LEN	16
#define PASSWORD_LEN	16

struct _user_meg{
	char username[USER_NAME_LEN];
	char password[PASSWORD_LEN];
} user_meg;

int Search_DataBase(char *username, char *password);

int cgiMain() 
{ 
	int ret;
	bzero(user_meg.username, USER_NAME_LEN);
	bzero(user_meg.password, PASSWORD_LEN);

	//cgiFormStringNoNewlines("username", user_meg.username, 16); 
	//cgiFormStringNoNewlines("passkey", user_meg.password, 16);
	cgiFormString("username", user_meg.username, USER_NAME_LEN); 
	cgiFormString("password", user_meg.password, PASSWORD_LEN);

	ret = Search_DataBase(user_meg.username, user_meg.password);
	if(ret)
	{
		cgiHeaderContentType("text/html"); 
		fprintf(cgiOut, "<HTML><HEAD>\n"); 
		fprintf(cgiOut, "<TITLE>login success!</TITLE></HEAD>\n"); 
		fprintf(cgiOut, "<BODY>"); 
		fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"0; url=../main.html\">");
		fprintf(cgiOut, "</BODY>\n"); 
		fprintf(cgiOut, "</HTML>\n"); 
	}
	else
	{
		cgiHeaderContentType("text/html"); 
		fprintf(cgiOut, "<HTML><HEAD>\n"); 
		fprintf(cgiOut, "<TITLE>login failed!</TITLE></HEAD>\n"); 
		fprintf(cgiOut, "<BODY>"); 
		fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"3; url=../index.html\">");
		fprintf(cgiOut, "<label><H1>µÇÂ¼Ê§°Ü£¬3Ãëºó·µ»Ø£¡</H1></label>");
		fprintf(cgiOut, "</BODY>\n"); 
		fprintf(cgiOut, "</HTML>\n");
	}
	fflush(stdout);
	
	return 0; 
} 

int Search_DataBase(char *username, char *password)
{
	int ret = 0;
	if((strncmp(username, "user", 4) == 0) && (strncmp(password, "123456", 6) == 0))
	{
		ret = 1;	
	}
	return ret;
}
