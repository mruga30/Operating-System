/**
 * CS543 Homework 2 Additiona Functionality question 1
 * Mruga Shah
 * hw2-additional-1.c
 * Operating System Concepts
 * 
 */
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* 80 chars per line, per command */

int main(void)
{
  char *args[MAX_LINE/2 + 1];
  char str[MAX_LINE];
  char strg[MAX_LINE];
  int should_run = 1;
  int i = 0;
  int j,l,m,n,p,s,u,ctr,k;
  int b = 0;
  char history[10][80];
  char index = 1;
  char ind_al = 0;
  char a[20];
  char c[20];
  char d[20];
  char alias[20][30];
  char cmnd[20][30];
  char split[10][10];

//This function runs until should_run is set to 0 which happens when user types exit
 while(should_run)
  {
        printf("\nosh> ");
        fflush(stdout);

        scanf ("%[^\n]%*c", str); //read in the input from user
      	//getline(&str,&nbytes,stdin); 
	
        i = 0;
        
	s = 0; ctr = 0;

	for(m=0; m <= (strlen(str));m++)
	{
		if (str[m] == ' '|| str[m] == '\0')
		{
		split[ctr][s] = '\0';
		ctr++; 
		s=0;
		}
		else
		{
		split[ctr][s] = str[m];
		s++; 
		}
	}
	
	strcpy(strg,str);
	
	//divide the input string into words	
        args[i] = strtok(str," ");
	//split[i] = strtok(args,"");
        
	while (args[i] != NULL) {
          i++;
          args[i] = strtok(NULL, " ");
        }

        if(strcmp(args[0], "exit") == 0) //check for user input "exit"
          break;	
	
	for(m=0; m <= ind_al;m++)
	{	
		if (strcmp(args[0],alias[m]) == 0)
		{
			//printf("%s,%s",args[0],cmnd[m]);
			strcpy(a,cmnd[m]);
			strcpy(d,alias[m]);
			b = 1;
			//printf("%s",a);
		}
	}

	if(strcmp(args[0], "history") == 0) //run when history command is entered by the user
	{
	  if (index>10)
	  {
	    n = index-10;
	    for(m=index-1;m>n-1;m--)
	    {
		printf("\n%d %s",m,history[m]);
	    }
	  }
	  else
	  {
	    for (m=index-1;m>0;m--)
	    {
		printf("\n%d %s",m,history[m]);
	    }
	  }
	}
	
	else if(strcmp(args[0], "!!") == 0) //execute most recent command
	{
	  if(index == 2) //check if there are commands in history
	  {
	  printf("No commands in history");
	  }
	  else{
	  strcpy(str,history[index-1]);
	  //printf("%s\n",str);
	  
	  strcpy(history[index],history[index-1]);
	  index++;

	  i = 0;
          args[i] = strtok(str," ");
          while (args[i] != NULL) {
            i++;
            args[i] = strtok(NULL, " ");
          }
          if(strcmp(args[i-1], "&") != 0) //if "&" at the end, parent invokes "wait()"
		{
		  pid_t pid;    
		  pid = fork(); //create the child process
		  if(pid < 0)
		  {
		        fprintf(stderr,"Fork has failed\n"); //give error if pid is negative
		        return 1;
		  }
		  else if (pid == 0) 
		  {
		        execvp(args[0],args); //execute commands entered by user
		        for(j=0;j<i;j++)
		          args[j] = NULL;
		  }
		  else
		  {
		        wait(NULL);
		  }
		}
		else
		{
		  pid_t pid;
		  pid = fork();
		  if(pid < 0){
		        fprintf(stderr,"Fork has failed\n");
		        return 1;
			//kill(pid,SIGSTOP);
		  }
		  else if (pid == 0){
		        args[i-1] = NULL;
		        execvp(args[0],args); //the child process invokes execvp()
		  }
		  else
		  {
		        printf("\n\n");
		  }
		}}
	}
	
	
	else if((args[0][0]-'!') == 0) //execute the Nth command
	{
		l = args[0][1]-'0';
		//printf("command no.:%d",l);
		if((l == 0) || (l > (index-2)))
		{
		  printf("No such command in history.\n N out of range\n");
		}
		else
		{
		  strcpy(str,history[l]);
		  strcpy(history[index],history[l]);
		  index++;
		  
		  i = 0;
		  args[i] = strtok(str," ");
		  while (args[i] != NULL) {
		    i++;
		    args[i] = strtok(NULL, " ");
		  }
		  if(strcmp(args[i-1], "&") != 0) //if "&" at the end, parent invokes "wait()"
			{
			  pid_t pid;    
			  pid = fork(); //create the child process
			  if(pid < 0)
			  {
				fprintf(stderr,"Fork has failed\n"); //give error if pid is negative
				return 1;
			  }
			  else if (pid == 0) 
			  {
				execvp(args[0],args); //execute commands entered by user
				for(j=0;j<i;j++)
				  args[j] = NULL;
			  }
			  else
			  {
				wait(NULL);
			  }
			}
			else
			{
			  pid_t pid;
			  pid = fork();
			  if(pid < 0){
				fprintf(stderr,"Fork has failed\n");
				return 1;
				//kill(pid,SIGSTOP);
			  }
			  else if (pid == 0){
				args[i-1] = NULL;
				execvp(args[0],args); //the child process invokes execvp()
			  }
			  else
			  {
				printf("\n\n");
			  }
			}
			}
	}
	
	//execute aliases and unaliases
	//execute aliases and unaliases
	else if(strcmp(args[0],"alias")==0)
		{
			if(i<2)
			{	
				if(ind_al==0)
				{printf("No aliases defined");}
				else{
				printf("%s\n","list of aliases(alias: command)");	  
				for(m=0;m<ind_al;m++)
				{			
				printf("%s: %s\n",alias[m],cmnd[m]);	
				}}
			}
			else if(i<3)
			{
				printf("%s\n","number of arguments invalid");
			}			
			else
			{							
				for(m=3;m<ctr;m++)
				{
				strcat(split[2]," ");
				strcat(split[2],split[m]);
				}
				strcpy(alias[ind_al],args[1]);
				strcpy(cmnd[ind_al],split[2]);
				ind_al++;			
			}
		}
	else if(strcmp(args[0],"unalias")==0)
		{
			if(args[1]=='\0'||args[2]!='\0')
			{
			printf("%s\n","Error: Wrong no of arg");
			}
			else{			
				for(m=0;m<ind_al;m++)
				{
				if (strcmp(args[1],alias[m]) == 0)
				{ u = m;}	
				}
				memset(alias[u],0,strlen(alias[u]));
				memset(cmnd[u],0,strlen(cmnd[u]));
			}
		}
			
	
	//run the commands when its alias is entered	
	else if(b == 1)
	{
		s = 0; ctr = 0;
		
		k = strlen(a)-2;
		memset(c,0,strlen(c));		
		
		for(m=0; m < k;m++) //remove the inverted commas from the command
		{
			c[m]=a[m+1];
			
		}
		//printf("%s",c);
		
		strcpy(history[index],d);
		index++;		

		i = 0;
		args[i] = strtok(c," ");
		while (args[i] != NULL) 
		{
		i++;
		args[i] = strtok(NULL, " ");
		}
		
		  if(strcmp(args[i-1], "&") != 0) //if "&" at the end, parent invokes "wait()"
			{
			  pid_t pid;    
			  pid = fork(); //create the child process
			  if(pid < 0)
			  {
				fprintf(stderr,"Fork has failed\n"); //give error if pid is negative
				return 1;
			  }
			  else if (pid == 0) 
			  {
				execvp(args[0],args); //execute commands entered by user
				for(j=0;j<i;j++)
				  args[j] = NULL;
			  }
			  else
			  {
				wait(NULL);
			  }
			}
			else
			{
			  pid_t pid;
			  pid = fork();
			  if(pid < 0){
				fprintf(stderr,"Fork has failed\n");
				return 1;
				//kill(pid,SIGSTOP);
			  }
			  else if (pid == 0){
				args[i-1] = NULL;
				execvp(args[0],args); //the child process invokes execvp()
			  }
			  else
			  {
				printf("\n\n");
			  }
			}
	}
	
	else  //execute the command entered by the user
	{
		
		strcpy(history[index],strg);
		index++;
		
		if(strcmp(args[i-1], "&") != 0) //if "&" at the end, parent invokes "wait()"
		{
		  pid_t pid;    
		  pid = fork(); //create the child process
		  if(pid < 0)
		  {
		        fprintf(stderr,"Fork has failed\n"); //give error if pid is negative
		        return 1;
		  }
		  else if (pid == 0) 
		  {
		        execvp(args[0],args); //execute commands entered by user
		        for(j=0;j<i;j++)
		          args[j] = NULL;
		  }
		  else
		  {
		        wait(NULL);
		  }
		}
		else
		{
		  pid_t pid;
		  pid = fork();
		  if(pid < 0){
		        fprintf(stderr,"Fork has failed\n");
		        return 1;
			//kill(pid,SIGSTOP);
		  }
		  else if (pid == 0){
		        args[i-1] = NULL;
		        execvp(args[0],args); //the child process invokes execvp()
		  }
		  else
		  {
		        printf("\n\n");
		  }
		}
	}

  }
	
return 0;
}


