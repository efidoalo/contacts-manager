/*=============================================;
 *
 * File: contact.c
 * Content: Uses remote connection to PostgreSql
 * database to store and retrieve information of
 * different personal contacts. The library
 * libpq is used.
 * Date: 01/02/2017
 *
 ***********************************************/

/*   commands 
   l (list all contacts)
   sf search by first name
   sl search by last name 
   sfl search by first and last name
   df delete by first name 
   dl delete by last name
   dfl delete by first and last name
   i insert a contact in to the database 
   h list of commands and associated descriptions (TO DO/YET TO BE IMPLEMENTED)
   q quit program.

   Example commands
   run the program [ must be run using postgres user ]
   bob@local-machine:~/Documents$ sudo -u postgres ./contacts
   sf bob          -- search for contacts with first name bob
   l               -- list all contacts
   dfl Teresa Hill -- delete the contact called Teresa Hill
   i               -- insert contact 
   sl Stephenson   -- search for all contacts with a last name of Stephenson
   q               -- quit the program
   bob@local-machine:~/Documents$
*/ 

#include <stdio.h>
#include <postgresql/libpq-fe.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int max( int a, int b)
{
  if (a>b) {
   return a;
  }
  else return b;
}

// print table 
void print_table(PGresult *res, int NoOfTuples, int FieldsPerTuple)
{
  int fieldwidths[FieldsPerTuple];
  int fullFieldWidths[FieldsPerTuple];

  // print header
  for (int i=0; i<FieldsPerTuple; ++i) {
    int fieldwidth = 0, length = 0;
    for (int j=0; j<NoOfTuples; ++j) {
      if ( (length = strlen(PQgetvalue(res,j,i)))>fieldwidth)
        fieldwidth = length;
    }
    fieldwidths[i]=fieldwidth;
  } 
  for (int i=0; i<FieldsPerTuple; ++i) {
    int fieldNameWidth = strlen(PQfname(res,i));
    int width = max(fieldwidths[i], fieldNameWidth)+2;
    fullFieldWidths[i]=width;
    int left_padding = (width-fieldNameWidth)/2;
    for (int j=0; j<left_padding; ++j) 
      printf(" ");
    printf("%-*s",width-left_padding,PQfname(res,i));
    if (i<FieldsPerTuple-1)
     printf("|");
  }
  printf("\n");
  for (int i=0; i<FieldsPerTuple; ++i) {
    for (int j=0; j<fullFieldWidths[i]; ++j)
      printf("-");
    if (i<FieldsPerTuple-1) 
    printf("+");
  }
  printf("\n");

  // print table data
  for (int i=0; i<NoOfTuples; ++i) {
    for (int j=0; j<FieldsPerTuple; ++j) {
      printf(" ");
      printf("%-*s",fullFieldWidths[j]-1,PQgetvalue(res,i,j));
      if (j<FieldsPerTuple-1)
        printf("|");
    }
    printf("\n");
  }
  printf("\n");
}


void execute_command(char *cmd, 
                     char *first_name, 
                     char *last_name,
                     PGresult **resPtr_addr, PGconn *conn)
{
  
  if (strcmp(cmd,"l")==0) {
    PGresult *res = *resPtr_addr = PQexec(conn, "SELECT * FROM contact_data");
    if ( PQresultStatus(res)!=PGRES_TUPLES_OK ) {
        printf("\nerror inserting data in to table.\nerr message:  %s",
        PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return;
    }
    else {
      int NoOfTuples = PQntuples(res),
          FieldsperTuple = PQnfields(res),
          binary = PQbinaryTuples(res);

      print_table(res, NoOfTuples, FieldsperTuple);
      return;
    } 
  } 
  else if ( (strcmp(cmd,"sf")==0) || (strcmp(cmd,"df")==0) ) {
    const char *base_sql;
    ExecStatusType resultType;
    if ( strcmp(cmd,"sf")==0 ) {
      base_sql = "SELECT * FROM contact_data WHERE \"first name\"=\'";
      resultType = PGRES_TUPLES_OK;
    }
    else {
      base_sql = "DELETE FROM contact_data WHERE \"first name\"=\'";
      resultType = PGRES_COMMAND_OK;
    }
    int query_len = strlen(base_sql) + strlen(first_name) + 2; // includes null terminator
    char *query = malloc(query_len);
    memcpy(query, base_sql, strlen(base_sql));
    memcpy(&(query[strlen(base_sql)]), first_name, strlen(first_name));    
    memcpy(&(query[strlen(base_sql)+strlen(first_name)]), "\'", 2);
    PGresult *res = *resPtr_addr = PQexec(conn, query);
    free(query);
    if ( PQresultStatus(res)!=resultType )  {
        printf("\nerror making query to contact_data table.\nerr message:  %s",
                                                  PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return;
    }
    else {
      int NoOfTuples = PQntuples(res),
          FieldsperTuple = PQnfields(res),
          binary = PQbinaryTuples(res);
      if (strcmp(cmd, "sf")==0)
        print_table(res, NoOfTuples, FieldsperTuple);
      else
        printf("\nData deleted successfully from database\n\n");
      return;
    }
  }
  else if ( (strcmp(cmd, "sl")==0) || (strcmp(cmd,"dl")==0) ) {
    const char *base_sql;
    ExecStatusType returnType;
    if (strcmp(cmd, "sl")==0) {
      base_sql = "SELECT * FROM contact_data WHERE \"last name\"=\'";
      returnType = PGRES_TUPLES_OK;
    }
    else {
      base_sql = "DELETE FROM contact_data WHERE \"last name\"=\'";
      returnType = PGRES_COMMAND_OK;
    }

    int query_len = strlen(base_sql) + strlen(last_name) + 2; // includes null terminator
    char *query = malloc(query_len);
    memcpy(query, base_sql, strlen(base_sql));
    memcpy(&(query[strlen(base_sql)]), last_name, strlen(last_name));
    memcpy(&(query[strlen(base_sql)+strlen(last_name)]), "\'", 2);
    PGresult *res = *resPtr_addr = PQexec(conn, query);
    free(query);
    if ( PQresultStatus(res)!=returnType ) {
        printf("\nerror making query to table.\nerr message:  %s",
        PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return;
    }
    else {
      int NoOfTuples = PQntuples(res),
          FieldsperTuple = PQnfields(res),
          binary = PQbinaryTuples(res);
      if (strcmp(cmd, "sl")==0) 
        print_table(res, NoOfTuples, FieldsperTuple);
      else
        printf("\nData deleted successfully from database\n\n");
      return;
    }
  }
  else if ( (strcmp(cmd, "sfl")==0) || (strcmp(cmd, "dfl")==0) ) {
    const char *base_sql1;
    ExecStatusType returnType;
    if  (strcmp(cmd, "sfl")==0) {
      base_sql1 = "SELECT * FROM contact_data WHERE \"first name\"=\'";
      returnType = PGRES_TUPLES_OK;
    }
    else { 
      base_sql1 = "DELETE FROM contact_data WHERE \"first name\"=\'";
      returnType = PGRES_COMMAND_OK;
    }
    const char *base_sql2 = "\' AND \"last name\"=\'";
    int query_len = strlen(base_sql1)+strlen(first_name)+
                         strlen(base_sql2)+strlen(last_name)+2;
    char *query = malloc(query_len);
    memcpy(query, base_sql1, strlen(base_sql1));
    memcpy(&(query[strlen(base_sql1)]), first_name, strlen(first_name));
    memcpy(&(query[strlen(base_sql1) + strlen(first_name)]), base_sql2, strlen(base_sql2));
    memcpy(&(query[strlen(base_sql1)+strlen(first_name)+strlen(base_sql2)]),last_name,strlen(last_name));
    memcpy(&(query[query_len-2]), "\'",2);
    PGresult *res = *resPtr_addr = PQexec(conn, query);
    free(query);
    if ( PQresultStatus(res)!=returnType ) {
      printf("\nerror making query to table.\nerr message:  %s",
      PQerrorMessage(conn));
      PQclear(res);
      PQfinish(conn);
      return;
    }
    else {
      int NoOfTuples = PQntuples(res),
          FieldsperTuple = PQnfields(res),
          binary = PQbinaryTuples(res);
      if (strcmp(cmd, "sfl")==0)
        print_table(res, NoOfTuples, FieldsperTuple);
      else
        printf("\nData deleted successfully from database\n\n");
      return;
    }
  }
  else if ( strcmp(cmd,"i")==0) {
    char first_name_buff[30]={0}, last_name_buff[30] = {0},
         h_phone_buff[30]={0},  m_phone_buff[40]={0}, 
         e_mail_buff[70]={0}, h_addr_buff[200] = {0},
         birthday_buff[100]={0};
    printf("\nEnter the information of the individual who you wish"
           " to be placed in to the contacts database.\n\n");

    printf("first name (max 29 chars): ");
    int index = 0;
    while ( (first_name_buff[index++]=getc(stdin))!='\n') { }
    first_name_buff[--index]=0;
    
    printf("last name (max 29 chars): ");
    index = 0;
    while ( (last_name_buff[index++]=getc(stdin))!='\n') { }
    last_name_buff[--index]=0;

    printf("home phone number (max 29 chars): ");
    index = 0;
    while ( (h_phone_buff[index++]=getc(stdin))!='\n') { }
    h_phone_buff[--index]=0;

    printf("mobile phone number (max 39 chars): ");
    index = 0;
    while ( (m_phone_buff[index++]=getc(stdin))!='\n') { }
    m_phone_buff[--index]=0;

    printf("email address (max 69 chars): ");
    index = 0;
    while ( (e_mail_buff[index++]=getc(stdin))!='\n') { }
    e_mail_buff[--index]=0;

    printf("home address (max 199 chars): ");
    index = 0;
    while ( (h_addr_buff[index++]=getc(stdin))!='\n') { }
    h_addr_buff[--index]=0;     

    printf("birthday (max 99 chars): ");
    index = 0;
    while ( (birthday_buff[index++]=getc(stdin))!='\n') { }
    birthday_buff[--index]=0;

    
    const char *base_sql = "INSERT INTO contact_data VALUES (\'";
    int base_len = strlen(base_sql), fn_len = strlen(&(first_name_buff[0])),
        ln_len = strlen(&(last_name_buff[0])), hp_len = strlen(&(h_phone_buff[0])),
        mp_len = strlen(&(m_phone_buff[0])), em_len = strlen(&(e_mail_buff[0])),
        haddr_len = strlen(&(h_addr_buff[0])), bd_len = strlen(&(birthday_buff[0]));

    int query_len = base_len + ((4*6)+3) + fn_len +
                    ln_len + hp_len + mp_len + em_len + haddr_len + bd_len;

    char *query = malloc(query_len);
    for (int i=0; i<query_len; ++i)
      query[i]=0;

    strcat(query, base_sql); 
    strcat(query, &(first_name_buff[0]) );
    strcat(query, "\', \'");
    strcat(query, &(last_name_buff[0]) );
    strcat(query, "\', \'");
    strcat(query, &(h_phone_buff[0]) );
    strcat(query, "\', \'");
    strcat(query, &(m_phone_buff[0]) );
    strcat(query, "\', \'");
    strcat(query, &(e_mail_buff[0]) );
    strcat(query, "\', \'");
    strcat(query, &(h_addr_buff[0]) );
    strcat(query, "\', \'");
    strcat(query, &(birthday_buff[0]) );
    strcat(query, "\')");
    
    PGresult *res = *resPtr_addr = PQexec(conn, query);
    free(query);
    if ( PQresultStatus(res)!=PGRES_COMMAND_OK ) {
      printf("\nerror inserting data into table.\nerr message:  %s",
      PQerrorMessage(conn));
      PQclear(res);
      PQfinish(conn);
      return;
    }
    else {
      int NoOfTuples = PQntuples(res),
          FieldsperTuple = PQnfields(res),
          binary = PQbinaryTuples(res);
     
      printf("\nData successfully added to database");          
      return;
    }
  }// help menu - todo
  else if ( strcmp(cmd,"h")==0 ) {
    
  }
  else if ( strcmp(cmd,"q")==0) {
    free(resPtr_addr); 
    free(cmd);
    PQfinish(conn);
    exit(EXIT_SUCCESS);
  }
   
}

int main(int argc, char *argv[])
{ 
  // cmd_opt is a null terminated string that secifies commands
  char *cmd_opt = malloc(sizeof(char)*5), *first_name=0, *last_name=0; 
  cmd_opt[0]=0;
  if (argv[1])
    memcpy(cmd_opt, argv[1], strlen(argv[1])+1);

  PGconn *conn = PQconnectdb("user=postgres dbname=contacts"); 
  
  if (PQstatus(conn)==CONNECTION_BAD) {
    printf("\nError connecting to contacts database");
    PQfinish(conn);
    return 0;
  } 
  
  PGresult **resPtr_addr = malloc(sizeof(PGresult *));
  *resPtr_addr=0; 
  
  if ((cmd_opt[0])!=0) {      
    if ( strlen(cmd_opt)==2 ) {
      if (cmd_opt[1]=='l') {
        last_name = argv[2];
      }
      else first_name = argv[2];
    } 
    else if ( strlen(cmd_opt)==3) {
      first_name = argv[2];
      last_name = argv[3];
    }       
    execute_command(cmd_opt, first_name, last_name, resPtr_addr, conn);
  }
  while (1) {
    char currChar=0; 
    int index=0;
    while ( ( (currChar = getc(stdin))!='\n' ) && (currChar !=' ') ) {
      cmd_opt[index++] = currChar;
    }
    cmd_opt[index]=0; 
    if (currChar == '\n') 
      execute_command(cmd_opt, 0, 0, resPtr_addr, conn);
    else {
      if ( (strcmp(cmd_opt,"sf")==0) || (strcmp(cmd_opt,"df")==0) ) {
        first_name = malloc(50); 
        for (int i=0; i<50; ++i)
          first_name[i] =0;
        index=0;
        while ( (first_name[index++] = getc(stdin))!='\n' ) { }
        first_name[--index]=0;
        execute_command(cmd_opt, first_name, 0, resPtr_addr, conn);
        free(first_name);
      }    
      else if ( (strcmp(cmd_opt,"sl")==0) || (strcmp(cmd_opt,"dl")==0) ) {
        last_name = malloc(50);
        for (int i=0; i<50; ++i)
          last_name[i] =0;
        index=0;
        while ( (last_name[index++] = getc(stdin))!='\n' ) { }
        last_name[--index]=0;
        execute_command(cmd_opt, 0, last_name, resPtr_addr, conn);
        free(last_name);
      } 
      else if ( (strcmp(cmd_opt,"sfl")==0) || (strcmp(cmd_opt, "dfl")==0) ) {
        first_name = malloc(50);
        last_name = malloc(50);
        index = 0;
        while ( (first_name[index++]=getc(stdin))!=' ') { }
        first_name[--index] = 0;
        index = 0;
        while ( (last_name[index++] = getc(stdin))!='\n') { }
        last_name[--index]=0;
        execute_command(cmd_opt, first_name, last_name, resPtr_addr, conn);
        free(first_name);
        free(last_name);
      }
      
    }
  }
  
}
