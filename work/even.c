#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>

// Signal handler for SIGHUP
void handle_sighnal(int signo) {
if (signo == SIGHUP) {
printf("Ouch!\n");
}
// Signal handler for SIGINT
if (signo == SIGINT) {
printf("Yeah!\n");
}
}

int main(int argc, char *argv[]) {
if (argc != 2) {
printf("Usage: %s <n>\n", argv[0]);
return 1;
}

int n = atoi(argv[1]);

// Setup signal handlers
signal(SIGINT, handle_sighnal);
signal(SIGHUP, handle_sighnal);

// Print the first n even numbers
for (int i = 0; i < n; i++) {
printf("%d\n", i * 2);
sleep(5);
}

return 0;
}
