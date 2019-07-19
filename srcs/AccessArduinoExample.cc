#include <iostream>
#include <fstream>

int main()
{
  FILE *file;
  std::string GTPP = "G TPP 11 \r";
  file = fopen("/dev/ttyUSB0","w");  //Opening device file
  if(file != NULL){
    fprintf(file,"%s",GTPP.c_str()); //Writing to the file
    fclose(file);
  }
    return 0;
}
