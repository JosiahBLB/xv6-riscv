// Author: Josiah Brough
// SID: 22160417

#include "user/user.h"
#include "kernel/types.h"

#define FORK_ERROR -1
#define CHILD_PROC 0

void child_proc(int pipefd[2]);
void parent_proc(int pipefd[2], int *child_pid);

int main(int argc, char *argv[]) {
    int pipefd[2];
    int child_pid;

    // create the pipe
    if (pipe(pipefd) == -1) {
        printf("pipe failed\n");
        exit(1);
    }

    // create the fork
    child_pid = fork();
    switch (child_pid) {
    case FORK_ERROR:
        printf("fork failed\n");
        exit(1);
        break;
    case CHILD_PROC:
        child_proc(pipefd);
        break;
    default:
        parent_proc(pipefd, &child_pid);
    }
    exit(0);
}

// the process that the child will execute
void child_proc(int pipefd[2]) {
    // read the message
    char buf;
    read(pipefd[0], &buf, sizeof(char));
    printf("Child (pid %d) received integer: %d\n", (char)getpid(), buf);
    close(pipefd[0]); // close the read end of the pipe

    // send the reply
    buf *= 4;
    write(pipefd[1], &buf, sizeof(buf));
    close(pipefd[1]); // close the write end of the pipe
}

// the process the parent will execute
void parent_proc(int pipefd[2], int *child_pid) {
    // send an integer
    char msg = 3; // the integer to send
    write(pipefd[1], &msg, sizeof(msg));
    close(pipefd[1]); // close the write end of the pipe
    wait((int *)0);

    // read the reply
    char reply;
    read(pipefd[0], &reply, sizeof(reply));
    printf("Parent (pid %d) received integer: %d\n", (char)getpid(), reply);
    close(pipefd[0]); // close the read end of the pipe
}
