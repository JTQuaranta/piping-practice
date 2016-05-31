/* 11336441 joey quaranta
360 lab 3
2/17/2016 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sys/types.h>
#include <fcntl.h>

char line[128];
char cmd[16];
char *args[20];
int main_index;
char *operand = NULL;
char *file = NULL;
int ioflag = 0;

int parseLine()
{
	int i = 1;
	char *token;
	char linecpy[128];
	if (!hasSpace(line))
	{
		strcpy(cmd, line);
		args[0] = (char *)malloc(strlen(line));
		strcpy(args[0], line);
		return 1;
	}
	
	strcpy(linecpy, line);

	token = strtok (linecpy, " ");
	
	strcpy(cmd, token);
	args[0] = (char *)malloc(strlen(line));
	strcpy(args[0], token);

	while (token = strtok(0, " "))
	{
		args[i] = (char *)malloc(strlen(line));
		strcpy(args[i], token);
		i++;
	}	
}

int hasSpace(char *string)
{
	int i = 0;
	while(string[i])
	{
		if (string[i] == ' ')
		{
			return 1;
		}
		i++;
	}
	return 0;
}

int reset_memory()
{
	int i = 0;

	memset(line, 0, 128);
	memset(cmd, 0, 16);
	
	while(args[i])
	{
		args[i] = NULL;
		i++;
	}

}

int hasPipe(char *string)
{
    int i = 0;

    while(string[i])
    {
        if (string[i] == '|')
        {
            return 1;
        }

        i++;
    }

    return 0;
}

int indexer(char *target)
{
	int i = 0;
	while(args[i])
	{
		if(!strcmp(target, args[i]))
		{
			return i;
		}
		i++;
	}
	return 0;
}

int system_call(char *env[])
{
	pid_t pid;
	int ioredirindex = 0;
	char bin_path[128] = "/bin/";
	int ret = 0;
	if (!(strcmp(cmd,"cd"))||!(strcmp(cmd,"exit")))
	{
		printf("it's either cd or exit\n");
		if(!strcmp(cmd,"exit"))
		{
			printf("it's exit\n");
			exit(1);
			return 1;
		}
		printf("it's cd\n");
		if (hasSpace(line))
		{
			if(chdir(args[1])==0) 
			{
				return 1;
			}
		}
		printf("no args, cding to home\n");
		chdir(getenv("HOME"));
	
	}
	
	if (ioredirindex = indexer("<"))
	{
		strcpy(operand, args[ioredirindex]);
		strcpy(file, args[ioredirindex + 1]);
		args[ioredirindex] = NULL;
		ioflag = 1;
		ioredirindex = 0;
	}

	if (ioredirindex = indexer(">"))
	{
		strcpy(operand, args[ioredirindex]);
		strcpy(file, args[ioredirindex + 1]);
		args[ioredirindex] = NULL;
		ioflag = 2;
		ioredirindex = 0;
	}

	if (ioredirindex = indexer(">>"))
	{
		strcpy(operand, args[ioredirindex]);
		strcpy(file, args[ioredirindex + 1]);
		args[ioredirindex] = NULL;
		ioflag = 3;
		ioredirindex = 0;
	}
	
	pid = fork();
	if (pid == -1) //if child and we failed
	{
		perror("fork()");
		return 0;
	}
	if (pid) //if pid != 0, ie parent
	{
		int status;
	
		if (wait(&status) == -1)
		{
			perror("wait()");
		}
		else
		{
			//did the child die naturally?
			if(WIFEXITED(status))
			{
				printf("%ld exited with return code %d\n",
	                                    (long)pid, WEXITSTATUS(status));
			}
			//was the child killed by a signal?
			else if(WIFSIGNALED(status))
			{
				printf("%ld terminated because it didn't    						catch signal number %d\n", 					(long)pid, WTERMSIG(status));
			}
	
		}
		return 1;
	}
	//if pid = 0, ie child and everything is going well
	if(ioflag == 1) //<
	{
		printf("file: %s \n",file);
		close(0);
		open(file, O_RDONLY);
		ioflag = 0;
	}
	else if(ioflag == 2) //>
	{
		printf("file: %s \n",file);
		close(1);
		open(file, O_WRONLY|O_TRUNC|O_CREAT, 0644);
		ioflag = 0;
	}
	if(ioflag == 3) //>>
	{
		printf("file: %s \n",file);
		close(1);
		open(file, O_WRONLY|O_CREAT|O_APPEND, 0644);
		ioflag = 0;
	}
	strcat(bin_path, cmd);
	ret = execve(bin_path, args, env);
	exit(1);
	return 1;
}

int removeNewLine(char *string)
{
	string[strlen(string)-1] = '\0';
	return 1;
}

main(int argc, char *argv[], char *env[])
{
	operand = (char *)malloc(strlen(line));
	file = (char *)malloc(strlen(line));

	while (1)
	{
		main_index = 1;
		printf("user@user-computer (pwd) $ ");
		fgets (line, 128, stdin);
		removeNewLine(line);

		parseLine();
		printf("you typed: %s\n", line);
		printf("cmd (args[0]) = %s\n", cmd);
		while (args[main_index])
		{
			printf("arg[%d] = %s\n",main_index,args[main_index]);
			main_index++;
		}
		system_call(env);
		reset_memory();
		printf("\n");
	}
	printf("goodbye\n");
	
}
