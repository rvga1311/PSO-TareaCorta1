#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int glob_var;

main (int ac, char **av)
{
  int pid;

  glob_var = 1;

  if ((pid = fork()) == 0) {
    /* child */
    glob_var = 5;
  }
  else {
    /* Error */
    perror ("fork");
    exit (1);
  }

  int status;
  while (wait(&status) != pid) {
  }
   printf("%d\n",glob_var); // this will display 1 and not 5.
}