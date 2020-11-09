#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "cgic.h"
#include "sqlite3.h"

#define STORAGE_NO	2 // 仓库号

#define SQLITE_OPEN		"/warehouse.db"

#define GOODS_ID_NUM	7
char goods_name[GOODS_ID_NUM][16] = {"apple", "banana", "pear", "kiwi", "mango", "pitaya", "orange"};

int goods_id_num = 0;

struct _goods_info{
	int storage_no;
	int goods_id;
	int goods_count;
	char goods_time[40];
};

struct _goods_info goods_info[GOODS_ID_NUM];

/*
回到函数
*/
int callback(void * para, int n_column, char ** column_value, char ** column_name )
{
	goods_info[goods_id_num].storage_no = (int)atoi(column_value[0]);
	goods_info[goods_id_num].goods_id = (int)atoi(column_value[1]);
	goods_info[goods_id_num].goods_count = (int)atoi(column_value[2]);
	strcpy(goods_info[goods_id_num].goods_time, column_value[3]);
	return 0;
}

/*
读取指定的货物种类和数量
返回值：0正确，1错误
*/
int get_goods_info(int storage_no, int goods_kinds)
{
	sqlite3 *db = NULL;
	char *err_msg = NULL;
	int recode = 0;
	char sql[1024] = "NULL";

	sprintf(sql, "select * from goods where dev_no = '%d' and goodsId = '%d';", storage_no, goods_kinds);
	
	recode = sqlite3_open(SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf("sqlite3_open:%s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	else
	{
		recode = sqlite3_exec(db, sql, callback, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf("sqlite3_exec:%s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return 1;
		}
	}
	sqlite3_close(db);
	return 0;
}

int cgiMain()
{
	int i = 0;
	int count = 0;
	int buf[GOODS_ID_NUM+1] = {0}; 
	goods_id_num = 0;

	for(i = 1; i <= GOODS_ID_NUM; i++)
	{
		if(get_goods_info(STORAGE_NO, i))
		{
			printf("get_goods_info:error");
			continue;  
		}
		goods_id_num++;
	}
	
	
	for(i = 0; i < GOODS_ID_NUM; i++)
	{
		if(goods_info[i].storage_no == STORAGE_NO)
		{	
			count++;
			buf[count] = i;
		}
	}
	
	cgiHeaderContentType("text/html"); 
	fprintf(cgiOut, "<HTML><HEAD>"); 
	fprintf(cgiOut, "<TITLE>当前实时数据</TITLE>");  
	fprintf(cgiOut, "<style type=\"text/css\"> body,td,th {font-size: 12px;margin-left: 0px;margin-top: 0px;}</style></HEAD>"); 
	fprintf(cgiOut, "<BODY BGCOLOR=\"#F8F9FA\">"); 
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"3\">");
	
	fprintf(cgiOut, "<table width=\"100%%\" border=\"0\" cellpadding=\"3\" cellspacing=\"0\">");
	// 货物信息表头
	fprintf(cgiOut, "<tr bgcolor=\"#DEE4E6\">");
	fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\" width=\"15%%\">仓库编号</td>");
	fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\" width=\"15%%\">货物名称</td>");
	fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\" width=\"15%%\">货物数量</td>");
	fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\" width=\"15%%\">更新时间</td>");
	fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\" width=\"15%%\">备注</td>");
	fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\" width=\"45%%\"></td>");
	fprintf(cgiOut, 	"</tr>");
	
	// 实时货物信息
	for(i = 0; i < count; i++)
	{
		if((i+2)%2 == 0)
			fprintf(cgiOut, 	"<tr bgcolor=\"#f2f2f2\">");
		else
			fprintf(cgiOut, "<tr>");
			
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\" width=\"15%%\">%d#</td>", goods_info[buf[i+1]].storage_no);
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\" width=\"15%%\">%s</td>", goods_name[goods_info[buf[i+1]].goods_id - 1]);
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\" width=\"15%%\">%d</td>", goods_info[buf[i+1]].goods_count);
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\" width=\"15%%\">%s</td>", goods_info[buf[i+1]].goods_time);
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\" width=\"15%%\">-</td>");
		fprintf(cgiOut, 		"<td height=\"30\" align=\"center\" valign=\"middle\" width=\"45%%\"></td>");
		fprintf(cgiOut, 	"</tr>");
	}
	
	fprintf(cgiOut, "</table>");

	fprintf(cgiOut, "<script type=\"text/jscript\">");
	fprintf(cgiOut, "setTimeout(\"self.close()\", 3000)");
	fprintf(cgiOut, "</script>");
	fprintf(cgiOut, "</BODY>"); 
	fprintf(cgiOut, "</HTML>");
	fflush(stdout);

	return 0;
}
