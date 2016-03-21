#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitstream.h"
#include "myfunction.h"
   
int main(int argc, char *argv[])
{

   
    FILE  *image;
    bitstream *bs;
     
    image = fopen(argv[2], "rb");
    
    char *output = argv[3];
    
    int rows = atoi(argv[4]);
    int cols = atoi(argv[5]);
    int fileLen = rows*cols;
    int counter=0;
    double buffer[fileLen],temp; 
    unsigned char ch_read;
     
    printf("  width:  %d \n", cols);
    printf("  height:  %d \n",rows);
 
     printf("  Length:  %d \n",fileLen);
     
      
    if (image == NULL) {
       fprintf(stderr, "Can't open input file!\n");
       exit(1);
    }
    
  else{
  // Read the data in Buffer 
    while(!feof(image))   
     {
     
         ch_read = fgetc(image);
         temp = (double)ch_read;
         buffer[counter] = temp - 128.0;
         counter++;	
     
    
      }
     
     } 
 
     
             if((strcmp(argv[1],"encode"))==0) {
	      printf("Encoding Gray Image .....\n");
              bs= open_output_bitstream(argv[3]);
              CompressFile(buffer,bs,rows,cols,argc, argv );
              }
              if ((strcmp(argv[1],"decode"))==0){ 
              printf("Decoding Gray Image.....\n");   
              bs= open_input_bitstream(argv[2]); 
              ExpandFile(bs,output, rows, cols,argc, argv);
             }
 
  close_bitstream(bs);

    fclose(image);
return 0;	
}







