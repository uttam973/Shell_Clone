#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>


#define MAX_INPUT_SIZE 1024
#define MAX_ARG_SIZE 64
#define MAX_ARGS 16

char initial_prompt=1;

void handle_sigint(int signo) {
	//CTRL+C is handled
	printf("\nCaught Ctrl+C\n");
}

void handle_sigtstp(int signo) {
	//CTRL+Z is handled
	printf("\nCaught Ctrl+Z\n");
}


void display_prompt() {
	char cwd[1024];
	if (getcwd(cwd,sizeof(cwd))!=NULL) {
		if (initial_prompt) {
			printf("=) ");
			initial_prompt=0;
		} else{
			printf("%s =)",cwd);
		}
	} else{
		perror("getcwd");
		printf("=) ");
	}
}

void parse_input(char *input,char **args){
	char *token;
	int arg_count=0;
	token=strtok(input," ");
	while(token!=NULL) {
		args[arg_count++]=token;
		token=strtok(NULL," ");
		if (arg_count >= MAX_ARGS) {
			fprintf(stderr, "Too many arguments\n");
			exit(EXIT_FAILURE);
		}
	}	
	args[arg_count]=NULL;
}

void execute_command(char **args) {
	if (args[0]==NULL) {
		return;
	}

	if (strcmp(args[0],"cd")==0) {
		if (args[1]==NULL) {
			return;
		} else {
			if(chdir(args[1])!=0) {
				perror("cd");
			}
		}
	} else if (strcmp(args[0],"exit")==0) {
		exit(EXIT_SUCCESS);
	} else if (strcmp(args[0], "cat") == 0) {
        	//Cat command->testing input redirection
		char c;
		FILE* file = fopen(args[1], "r");
		if (file == NULL) {
		    perror("fopen");
		    exit(EXIT_FAILURE);
		}
		while ((c = fgetc(file)) != EOF) {
		    putchar(c);
		}
		fclose(file);
		
	}else{
		int background=0;
		
		//checking if there is background execution
		for(int i=0;args[i]!=NULL;++i){
			if (strcmp(args[i],"&")==0){
				background=1;
				args[i]=NULL;
				break;
			}
		}
	
		pid_t pid=fork();
		if (pid==-1) {
			perror("fork");
			exit(EXIT_FAILURE);
		} else if (pid==0) {
			//input/output redirection
			for(int i=0;args[i]!=NULL;++i) {
				if(strcmp(args[i],">")==0) {
					//output redirection
					args[i]=NULL;
					int fd=open(args[i+1],O_WRONLY | O_CREAT | O_TRUNC,0644);
					if(fd==-1) {
						perror("open");
						exit(EXIT_FAILURE);
					}
					dup2(fd,STDOUT_FILENO);
					close(fd);
				} else if (strcmp(args[i], "<")==0){
					//input redirection
					args[i]=NULL;
					int fd=open(args[i+1],O_RDONLY);
					if(fd==-1) {
						perror("open");
						exit(EXIT_FAILURE);
					}
					dup2(fd,STDIN_FILENO);
					close(fd);
				} else if (strcmp(args[i],"|")==0) {
					//checking for pipes
					args[i]=NULL;
					int pipe_fd[2];
					if(pipe(pipe_fd)==-1) {
						perror("pipe");
						exit(EXIT_FAILURE);
					}
					pid_t sub_pid = fork();

					if (sub_pid==-1) {
						perror("fork");
						exit(EXIT_FAILURE);
					} else if(sub_pid ==0) {

						//Subprocess->right side of the pipe. need to redirect standard output to the write end of the pipe
						dup2(pipe_fd[1],STDOUT_FILENO);
						close(pipe_fd[0]);
						close(pipe_fd[1]);

						if(execvp(args[i+1],args+i+1)==-1) {
							perror("execvp");
							exit(EXIT_FAILURE);
						}
					} else {

						//Parent processs ->left side of the pipe
						//need to redirect standard input to the read end of the pipe
						dup2(pipe_fd[0], STDIN_FILENO);
		                		close(pipe_fd[0]);
			                     	close(pipe_fd[1]);
                     	
						if (execvp(args[0],args)==-1) {
							perror("execvp");
			                        	exit(EXIT_FAILURE);
                        			}

					}

				}

			}

			if (execvp(args[0],args)==-1) {
				perror("execvp");
				exit(EXIT_FAILURE);
			}
		}else {
			if(!background){
				wait(NULL);
			}
		}
	}
}

int main(){
	//SIgnal handlers need to be specified
	signal(SIGINT,handle_sigint);
	signal(SIGTSTP,handle_sigtstp);

	char input[MAX_INPUT_SIZE];
	char* args[MAX_ARGS];

	while(1) {
		display_prompt();

		if (fgets(input,sizeof(input),stdin)==NULL) {
			perror("Error reading input");
			exit(EXIT_FAILURE);
		}

		input[strcspn(input,"\n")]='\0';

		parse_input(input,args);

		execute_command(args);

	}

	return 0;
}
