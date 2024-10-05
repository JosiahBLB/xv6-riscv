#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // Declare vars
    char *path;
    int offset;
    int fd;

    // Ensure correct number of user args
    if (argc != 3) {
        fprintf(2, "Usage: file offset\n");
        exit(1);
    }

    // Get user args
    path = argv[1];
    offset = atoi(argv[2]);

    // // Write a random string to file
    fd = open(path, O_WRONLY | O_CREATE);
    const char *data = "Hello, World!\n";
    write(fd, data, strlen(data) * sizeof(char));
    close(fd);

    // Open same file at offset
    fd = open(path, O_WRONLY);
    lseek(fd, offset);
    const char *marker = "^";
    write(fd, marker, strlen(marker) * sizeof(char));
    close(fd);

    // Run cat on file to view it's contents
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed");
        exit(1);
    }
    if (pid == 0) { // If child proc
        char *args[2] = {"cat", path};
        exec("/cat", args);
        exit(0);
    } else { // If parent proc
        wait(0); // wait for cat to finish
    }

    exit(0);
}
