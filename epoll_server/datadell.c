
void printfhelp(void)
{
	printf("-p,assign port\n");
	printf("-h,ask for help\n");
}

int datadell(int  arg_fd)
{
        char buff[1024];
        int rv=-1;

        if((rv=read(arg_fd,buff,sizeof(buff))) < 0)
        {   
                printf("read failure:%s\n",strerror(errno));
		return -1;
        }   

        else if(rv==0)
        {   
                printf("the socket has disconnect\n");
		return -2;
        }   

        else if(rv>0)
        {   
                printf("read data from client :%s\n",buff);
        }   

        char p_device[20];
        char p_time[20];
        char p_tem[20];
        char *ptemp=NULL;
        char *ptemp2=NULL;
        ptemp=strstr(buff,"/");
        ptemp++;
        strncpy(p_device,buff,strlen(buff)-strlen(ptemp)-1);
        ptemp2=strstr(ptemp,"/");
        ptemp2++;
        strncpy(p_time,ptemp,strlen(ptemp)-strlen(ptemp2)-1);
        strncpy(p_tem,ptemp2,strlen(ptemp2));
        printf("get the temprature is %s\n",ptemp2);

        sqlite3 *db;
        char *zErrMsg = 0;
int rc;
        char * sql;
        char buf[1024];
        rc = sqlite3_open("wujinlong.db", &db);
        if( rc )
        {
                fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                return -3;
        }
        else
        {
                fprintf(stderr, "Opened database successfully\n");
        }

        /* Create SQL statement */
        sql = "CREATE TABLE if not exists RPI_temdata("
         "ID INT PRIMARY KEY,"
         "DEVICE           CHAR(30)    NOT NULL,"
         "TIME        CHAR(30),"
         "TEMPRATURE         REAL );";

        /* Execute SQL statement */
        rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
        if( rc != SQLITE_OK )
        {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
        }

        else
        {
                fprintf(stdout, "Table created successfully\n");
        }
        snprintf(buf,sizeof(buf),"INSERT INTO RPI_temdata (DEVICE,TIME,TEMPRATURE) VALUES('%s','%s',%f);",p_device,p_time,atof(p_tem));
        sql = buf;
        /* Execute SQL statement */

        printf("char *sql=%s\n",sql);
        rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
        if( rc != SQLITE_OK )
        {
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
        }
        else
        {
                fprintf(stdout, "Records created successfully\n");
        }

        sqlite3_close(db);
	return rv;
}

