/*********************************************************************
   Program  : miniShell                   Version    : 1.3
 --------------------------------------------------------------------
   skeleton code for linix/unix/minix command line interpreter
 --------------------------------------------------------------------
   File			: minishell.c
   Compiler/System	: gcc/linux

********************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define NV 20			/* max number of command tokens */
#define NL 100			/* input buffer size */
char            line[NL];	/* command input buffer */


/*
	shell prompt
 */

int prompt(void)
{
  fflush(stdout);
  return 0;

}

void child_handler(int signo){
    (void) signo;
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {
        printf ("Child process completed. \n");
    }
}

int main(int argk, char *argv[], char *envp[])
/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */

{
  int             frkRtnVal;	/* value returned by fork sys call */
  int             wpid = 1;		/* value returned by wait */
  char           *v[NV];	/* array of pointers to command line tokens */
  char           *sep = " \t\n";/* command line token separators    */
  int             i;		/* parse index */

    struct sigaction sa;
    sa.sa_handler = child_handler;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

  /* prompt for and process one command line at a time  */

  while (1) {			/* do Forever */
    prompt();
    fgets(line, NL, stdin);
    fflush(stdin);

    if (feof(stdin)) {		/* non-zero on EOF  */
      exit(0);
    }

    if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
      continue;			/* to prompt */

    v[0] = strtok(line, sep);
    for (i = 1; i < NV; i++) {
      v[i] = strtok(NULL, sep);
      if (v[i] == NULL)
	        break;
    }

    int background = 0;
    if (strcmp(v[i-1], "&") == 0) {
        i--;
        v[i] = NULL;
        background = 1;
    }

    if (strcasecmp(v[0], "cd") == 0) {
        if (chdir(v[1]) == -1) {
            perror("chdir");
        }
    } else {
        switch (frkRtnVal = fork()) {
            case -1:
            perror("fork");
            break;
            case 0:
            if(execvp(v[0], v) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
            break;
            default:
            if (background == 1) {
                printf("[%d] %d\n", wpid, frkRtnVal);
                wpid++;
            } else {
                wait(0);
            }
            break;
        }
    }
  }
    return 0;
    
}				/* main */