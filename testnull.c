#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
   int *p = 0;
   printf(1, "HERE\n");
   printf(1, "%x\n", *p);
   exit();
}