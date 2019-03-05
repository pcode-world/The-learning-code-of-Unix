#include <stdio.h>
#include <sqlite3.h>

int main(int argc, char* argv[])
{
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;

   rc = sqlite3_open("test.db", &db);

   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }
   else
   {
      fprintf(stderr, "Opened database successfully\n");
   }
   /* Create SQL statement */
   sql = "CREATE TABLE RPI_temdata("  
         "ID INT PRIMARY KEY," 
         "DEVICE           TEXT    NOT NULL," 
         "TIME        CHAR(30)," 
         "TEMPRATURE         REAL );";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   else
   {
      fprintf(stdout, "Table created successfully\n");
   }
  sql = "INSERT INTO COMPANY (DEVICE,TIME,TEMPRATURE) "  
         "VALUES ('RPIWUJINLONG','2019/3/5 21-48-48',8.2); " ;
	/*
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  
         "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" 
         "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" 
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" 
         "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";
	*/

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Records created successfully\n");
   }
