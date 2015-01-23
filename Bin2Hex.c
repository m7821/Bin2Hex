#define  TIME_ESTIMATION  1

#include <stdio.h>
#include <stdlib.h>

#if TIME_ESTIMATION
#include <sys/time.h>
#endif

int
inline
ReadFileAndSize (
  char           *BiaryName,
  char           *OutFileName,
  FILE           **InFile,
  FILE           **OutFile,
  long           *FileSize,
  unsigned char  **InfileBuffer
  )
{
  int            Status = 0;
  
  *InFile = fopen (BiaryName, "rb");
  if (InFile == NULL) {
    printf ("ERROR : Biary \"%s\" does not exist!!\n", BiaryName);
    return -1;
  }

  *OutFile = fopen (OutFileName, "w");
  if (OutFile == NULL) {
    printf ("ERROR : Create \"%s\" fail!!\n", OutFileName);
    return -1;
  }

  Status = fseek (*InFile, 0, SEEK_END);
  if (Status < 0) {
    printf ("ERROR : Read \"%s\" fail!!\n", BiaryName);
    return Status;
  }

  *FileSize = ftell (*InFile);

  Status = fseek (*InFile, 0, SEEK_SET);
  if (Status < 0) {
    printf ("ERROR : Read \"%s\" fail!!\n", BiaryName);
    return Status;
  }

  *InfileBuffer = malloc (*FileSize);

  fread (*InfileBuffer, 1, *FileSize, *InFile);
  
}

void
inline
WriteBufToFile (
  FILE           *OutFile,
  long           FileSize,
  unsigned char  *InfileBuffer,
  unsigned char  IsC
  )
{
  unsigned long  Index = 0;

  if (IsC) fprintf (OutFile, "unsigned char OutVariable[] = {\n");

  for (Index = 0; Index < FileSize; Index++) {
    if (Index % 16 == 0 && IsC == 0) {
      fprintf (OutFile, "db ");
    }

    if ((((Index + 1) % 16) == 0) || (Index == (FileSize - 1))) {
      if (IsC) fprintf (OutFile, "0x%02x\n", InfileBuffer[Index]);
      else fprintf (OutFile, "0%xh\n", InfileBuffer[Index]);
    } else {
      if (IsC) fprintf (OutFile, "0x%02x, ", InfileBuffer[Index]);
      else fprintf (OutFile, "0%xh, ", InfileBuffer[Index]);
    }
  }
  
  if (IsC) fprintf (OutFile,"};\n");
}

void
main (
  int Argc, 
  char *Argv[]
  )
{
  int            Status = 0;
  long           FileSize;
  unsigned char  *InfileBuffer = NULL;
  
  unsigned char  OutTmp[10] = {0};
  unsigned char  IsC;
  FILE           *InFile;
  FILE           *OutFile;

#if TIME_ESTIMATION
  struct timeval        StartTime;
  struct timeval        EndTime;
  struct timezone       TimeZone;

  gettimeofday (&StartTime, &TimeZone);
#endif
  
  //
  // Check input parameters
  //
  if (Argc != 4) {
    printf ("\nPlease input parameters like follows :\n");
    printf ("    Bin2Hex.exe iii ooo asm|c\n");
    printf ("    iii : input biary\n");
    printf ("    ooo : output text\n");

    return;
  }

  //
  // Read input biary and its size then create a output file,  
  //
  Status = ReadFileAndSize (
             Argv[1],
             Argv[2],
             &InFile,
             &OutFile,
             &FileSize,
             &InfileBuffer
             );
  if (Status < 0) {
    return;
  }

  //
  // Check out put assembly or c
  //
  Status = strcmp (Argv[3], "asm");
  if (Status != 0) {
    IsC = 1;
  } else {
    IsC = 0;
  }

  WriteBufToFile (
    OutFile,
    FileSize,
    InfileBuffer,
    IsC
    );
  
#if TIME_ESTIMATION
  gettimeofday (&EndTime, &TimeZone);

  printf ("Used Time : %d us\n",
    (EndTime.tv_sec * 1000000 + EndTime.tv_usec) - (StartTime.tv_sec * 1000000 + StartTime.tv_usec)
    );
#endif

  printf ("\nThis program is written by Spencer Yeh.\n");
  printf ("For more information, please contact\n"); 
  printf ("== m782123@gmail.com ==\n");

  free (InfileBuffer);
  fclose (InFile);
  fclose (OutFile);
}
