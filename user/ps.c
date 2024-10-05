#include "kernel/types.h"
#include "user/user.h"

/** 
 * Desc: Prints current system processes.
 * Opts:
 *  -h    Show help dialog.
 *  -r    Only show running processes.
 *
 * Author: Josiah Brough
 * SID: 22160417
 */
int main(int argc, char *argv[]) {
  if (argc > 2) { // handle incorrect number of args
    printf("Error: Incorrect number of arguments. Use -h for help.\n");
    exit(1);
  }

  switch (argc) {
  case 1: // for printing all processes
    ps();
    break;
  case 2:                             // for when a single argument is given
    if (strcmp(argv[1], "-r") == 0) { // print running only
      psrunning();
    } else if (strcmp(argv[1], "-h") == 0) { // print help string
      printf("Usage: ps [OPT]\n\n");
      printf("Options:\n");
      printf("\t-h\tShow this help dialog.\n");
      printf("\t-r\tOnly show running processes.\n\n");
    } else { // handle incorrect argument
      printf("Error: Unknown input argument '%s'. Use -h for help.\n", argv[1]);
      exit(1);
    }
    break;
  }

  exit(0);
}
