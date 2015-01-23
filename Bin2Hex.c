#include "Bin2Hex.h"

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
PrintValuToFile (
  FILE           *OutFile,
  long           FileSize,
  unsigned char  *InfileBuffer,
  char           *Prefix
  )
{
  unsigned long  Index = 0;
  
  for (Index = 0; Index < FileSize; Index++) {
    if ((Index % 16 == 0) && (Prefix != NULL)) {
      fprintf (OutFile, "%s ", Prefix);
    }

    if ((((Index + 1) % 16) == 0) || (Index == (FileSize - 1))) {
      fprintf (OutFile, "0x%02x\n", InfileBuffer[Index]);
    } else {
      fprintf (OutFile, "0x%02x, ", InfileBuffer[Index]);
    }
  }
}

void
inline
WriteBufToFile (
  FILE           *OutFile,
  long           FileSize,
  unsigned char  *InfileBuffer,
  enum CODE_TYPE CodeType
  )
{
  unsigned long  Index = 0;

  //
  // Head
  //
  switch (CodeType) {
  case TypeC :
    fprintf (OutFile, "unsigned char OutVariable[] = {\n");
    break;

  case TypeAsm :
  default :
    break;
  }

  //
  // Body
  //
  switch (CodeType) {
  case TypeC :
    PrintValuToFile (OutFile, FileSize, InfileBuffer, NULL);
    break;
    
  case TypeAsm :
    PrintValuToFile (OutFile, FileSize, InfileBuffer, "db");
    break;
    
  default :
    break;
  }

  //
  // Bottom
  //
  switch (CodeType) {
  case TypeC :
    fprintf (OutFile,"};\n");
    break;

  case TypeAsm :
  default :
    break;
  }
}

enum CODE_TYPE
FindInputCodeType (
  char          *CodeType
  )
{
  char          *SupportType[] = {INPUT_CODE_TYPE};
  int           SupportNum = 0;
  int           Index;

  SupportNum = sizeof (SupportType) / sizeof (SupportType[0]);

  for (Index = 0; Index < SupportNum; Index++)
    if (!strcasecmp (CodeType, SupportType[Index])) 
      return Index;

  return TypeUnknown;
}

void
main (
  int  Argc, 
  char *Argv[]
  )
{
  int            Status = 0;
  long           FileSize;
  unsigned char  *InfileBuffer = NULL;
  
  unsigned char  OutTmp[10] = {0};
  FILE           *InFile;
  FILE           *OutFile;

  enum CODE_TYPE  CodeType;

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
  // Check out put assembly or c
  //
  CodeType = FindInputCodeType (Argv[3]);
  if (CodeType == TypeUnknown) {
    printf ("ERROR : The code type \"%s\" is not supported in this version. ", Argv[3]);
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
  // Write buffer to ouput file
  //
  WriteBufToFile (
    OutFile,
    FileSize,
    InfileBuffer,
    CodeType
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
