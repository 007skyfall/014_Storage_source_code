#include "database_task.h"

static int LoadMyInfo(void * para, int n_column, char ** column_value, char ** column_name )
{
	int i;
	printf("number %d \n", n_column );
	for( i = 0 ; i < n_column; i ++ )
	{
		printf("name:%s,values:%s\n",  column_name[i], 	column_value[i] );
	}
	printf("------------------\n" );		
	return 0;
}

#if 1
int main()
{
	char sql[1024];
	char *itime;
	sqlite3 *db;
	char *err_msg=0;
	time_t t;
	t=time(NULL);
	itime = ctime(&t);
	itime[strlen(itime)-1] = 0;
	int recode;
#if 1
	sprintf(sql,"create table users(userid int identity(1,1) primary key, username varchar(10) not null);");
	recode = sqlite3_open("test.db",&db);
	if(recode!=SQLITE_OK)
	{
		printf("Can't Open Database:%s!\n",sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	else
	{
		printf("open OK!\n");
		recode=sqlite3_exec(db,sql,0,0,&err_msg);
		if(recode!=SQLITE_OK)
		{
			printf("Error:%s",err_msg);
			sqlite3_close(db);
		}
		else
		{
			printf("creat OK!\n");
			sqlite3_close(db);
		}
		
	}
#endif
	sprintf(sql,"insert into users values(3,'2');");
	recode = sqlite3_open("test.db",&db);
	if(recode!=SQLITE_OK)
	{
		printf("Can't Open Database:%s!\n",sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	else
	{
		printf("open OK!\n");
		recode=sqlite3_exec(db,sql,0,0,&err_msg);
		if(recode!=SQLITE_OK)
		{
			printf("Error:%s\n",err_msg);
			sqlite3_close(db);
		}
		else
		{
			printf("insert OK!\n");
			sqlite3_close(db);
		}
		
	}

	sprintf(sql,"select * from users;");
	recode = sqlite3_open("test.db",&db);
	if(recode!=SQLITE_OK)
	{
		printf("Can't Open Database:%s!\n",sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	else
	{
		printf("open OK!\n");
		recode=sqlite3_exec(db,sql,LoadMyInfo,0,&err_msg);
		if(recode!=SQLITE_OK)
		{
			printf("Error:%s\n",err_msg);
			sqlite3_close(db);
		}
		else
		{
			printf("select OK!\n");
			printf("select OK!\n");
			sqlite3_close(db);
		}
		
	}

	return 0;
}

#endif
