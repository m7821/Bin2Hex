#define  TIME_ESTIMATION  1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if TIME_ESTIMATION
#include <sys/time.h>
#endif

#define INPUT_CODE_TYPE "c", \
                        "asm"

enum CODE_TYPE {
  TypeC,
  TypeAsm,
  TypeUnknown
};