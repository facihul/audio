/************************** Start of compresion  *************************
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include "bitstream.h"
#include "dct.h"
#include "myfunction.h"



#define N               8


/*
 * Global data used at various places in the program.
 */




double Quan_Lum[ N ][ N ] = { {16, 11, 10, 16, 24, 40, 51, 61},
                             {12, 12, 14, 19, 26, 58, 60, 55},
                             {14, 13, 16, 24, 40, 57, 69, 56},
                             {14, 17, 22, 29, 51, 87, 80, 62},
                             {18, 22, 37, 56, 68, 109,103,77},
                             {24, 35, 55, 64, 81, 104,113,92},
                             {49, 64, 78, 87, 103,121,120,101},
                             {72, 92, 95, 98, 112,100,103,99},
                             };


double Quan_Chrm[ N ][ N ] = {{17, 18, 24, 47, 99, 99, 99, 99},
                              {18, 21, 26, 66, 99, 99, 99, 99},
                              {24, 26, 56, 99, 99, 99, 99, 99},
                              {47, 66, 99, 99, 99, 99, 99, 99},
                              {99, 99, 99, 99, 99, 99, 99, 99},
                              {99, 99, 99, 99, 99, 99, 99, 99},
                              {99, 99, 99, 99, 99, 99, 99, 99},
                              {99, 99, 99, 99, 99, 99, 99, 99},
                              };



struct zigzag {
    int row;
    int col;
} ZigZag[ N * N ] =
{
    {0, 0},
    {0, 1}, {1, 0},
    {2, 0}, {1, 1}, {0, 2},
    {0, 3}, {1, 2}, {2, 1}, {3, 0},
    {4, 0}, {3, 1}, {2, 2}, {1, 3}, {0, 4},
    {0, 5}, {1, 4}, {2, 3}, {3, 2}, {4, 1}, {5, 0},
    {6, 0}, {5, 1}, {4, 2}, {3, 3}, {2, 4}, {1, 5}, {0, 6},
    {0, 7}, {1, 6}, {2, 5}, {3, 4}, {4, 3}, {5, 2}, {6, 1}, {7, 0},
    {7, 1}, {6, 2}, {5, 3}, {4, 4}, {3, 5}, {2, 6}, {1, 7},
    {2, 7}, {3, 6}, {4, 5}, {5, 4}, {6, 3}, {7, 2},
    {7, 3}, {6, 4}, {5, 5}, {4, 6}, {3, 7},
    {4, 7}, {5, 6}, {6, 5}, {7, 4},
    {7, 5}, {6, 6}, {5, 7},
    {6, 7}, {7, 6},
    {7, 7}
};


 



/*
 * This is the main compression routine.  By the time it gets called,
 * the input and output files have been properly opened, so all it has to
 * do is the compression.  
 *
 */

void CompressFile(input, output,rows,cols, argc, argv )
double *input;
bitstream *output; 
int rows;
int cols;
int argc;
char *argv[];
{    
    
    ROWS = rows; 
    COLS = cols;
    unsigned int val;
    signed int diff=0,Curr_dc=0,code;
    int row,col,dc_cate,ac_cate,ac_sol;
    int i,j,run =0,Len = ROWS*COLS;
    double input_array[64];
    double output_array[64];
    double dctq[ N ][ N ],temp;
    double buffer_im[ROWS][COLS];
    double zigzag_out[N*N];
  
      int counter=0;
         for(row=0; row<ROWS ; row++) {
            for(col=0; col<COLS; col++){
             
               buffer_im[row][col] = input[counter];
               //printf("  %f",buffer_im[i][j]);
               
               counter++;
                
              }
             
           }

    init_huffman_tables(); // initializing huffman table
    int bN = 0;
         for(row=0; row<ROWS ; row+=N) { 
         
            for (col=0; col<COLS; col+=N){
            
                //printf("block num: %d , row %d  col %d \n ", bN,row,col);
                counter=0; 
                
               /* 8x8 block of data is stored in 1D array 
                   prepare for DCT , quantization and further compression to write into bitstream.*/
                     for(i=row; i < (row+N); i++){
                        for(j = col; j < (col+N); j++){ 
                            input_array [counter] =  buffer_im[i][j];
                                printf(" %2.1f ",input_array[counter]);
                                
                             
                           counter++;
                            }
                       }
                       bN++;
                       printf("\n"); 
                   /* fDCT is done here*/
                      fdct( input_array, output_array );
                      counter=0;
                
             /* for ( i = 0 ; i < ( N * N ) ; i++ ) {
                           //if (N==8*i) printf("\n");
                           printf(" %2.1f ", output_array[i]);
                            
                             //val = (unsigned int)output_array[i];
                             //putbits(output, val, 8);
                           }
                           printf("\n\n");
             /* DCT output data is converted into 2D array 
                Each dct valu is quantized and rounded */
                 
                    for(i=0;i<N;i++){
                        for(j=0;j<N; j++){    
                          //temp = output_array[counter]/Quan_Lum[ i ][ j ] +0.5;
                         // dctq[ i ][ j ]=floor(temp);
                          dctq[ i ][ j ]=floor(output_array[counter]/Quan_Lum[ i ][ j ]+0.5);
                          //printf(" %2.1f  ",dctq[ i ][ j ] );
                    
                      counter++;
                     }
                     
                   }  
                   
                  printf(" \n \n "); 
              /* zigzag order arrenged  here  */ 
              
               zigzagcode( zigzag_out, dctq );
                 /*   for ( i = 0 ; i < ( N * N ) ; i++ ) {
                           printf(" %2.1f ", zigzag_out[i]);
                           }
                     printf(" \n "); 
                   
                   
                     
              /* Find DC Differential value and write into file */       
                
                if (col==0 && row==0) diff=(signed int)zigzag_out[0];
                else diff=(signed int)zigzag_out[0]-Curr_dc; 
                Curr_dc= (signed int)zigzag_out[0];
                dc_cate=solve_category(diff);  
                
              
               /* find the vlc and vli 
               /* write it into bitstream */
               
              putvlcdc(output,dc_cate);  
              putvli(output,dc_cate,diff); 
             
              
          /*This for loop is finding ac category value and coreesponding runlength. 
          four conditions needs to take care: 
          1: run=0 & code != 0
          2: run=0 & code = 0 // run increments here
          3: run!= 0 & code = 0 // EOB
          4: run!= 0 & code != 0 
          */
          
            for ( i = 1; i < (N*N) ; i++ ) 
            {
               
                code=(signed int)zigzag_out[i];
               
	        ac_cate=solve_category(code);
	        
	       
	       
	       /* run = 0 & code = 0  
	           run increments here */
	        if ( code == 0 ) {
		    run++;
		 
		    /* run!= 0 & code = 0  
		       EOB writing here */
		    if (i == 63 && run != 0)  
		    {
		   
		    putvlcac(output,0,0);                     
		    putvli(output,0,0);
		    run=0;
		    }
                } 
                /*  run!= & code != 0  */
              else if (run != 0 && code != 0){
		    
		        while(run != 0) {     
			    //printf("vlcac writing \n");
			    if(run < 16) {
			    /* writes the encoded value with respect to run and category value  */
			      
			        putvlcac(output,run,ac_cate);  
			                                                                 
			        run = 0;
			    } else {
			   
			        putvlcac(output,15,0);
			        run -=16;
			        if(run == 0)
			        putvlcac(output,0,ac_cate);
			        
			    }		        	
		        }
		      // printf("vliac writing: \n");
		        putvli(output,ac_cate,code);
		  
	         }
	        /* run=0 & code != 0 */
	     else if(run == 0 && code != 0){
	         // printf("%d %d ",run,ac_cate);
	          //printf("run = %d category =%d \n", run,ac_cate);
		    putvlcac(output,0,ac_cate);
		    putvli(output,ac_cate,code);
		    } 
	        
             }    
                                 
          }   
        }    
            
      /* remove huffman tables from the memory */  
  
      delete_huffman_tables();
}
/*
 This routine reads the 2D DCT data and arrange it in zigzag order 

*/

void zigzagcode(zigzag_out, output_data )
double zigzag_out[N*N];
double output_data[ N ][ N ];
{
    int i;
    int row;
    
    int col;
    
        
    for ( i = 0 ; i < ( N * N ) ; i++ ) {
        row = ZigZag[ i ].row;
        col = ZigZag[ i ].col;
        zigzag_out[i] = output_data[ row ][ col ];
		     
     }
}
/*
 * This routine reads in a block of encoded data from a compressed file.
 * The routine reorders it in row major format, 
 */

void Make2D( input_data, output_data )
double input_data[N*N]; 
double output_data[ N * N ];
{

  int zigzag[64] = { 0,1,8,16,9,2,3,10,17,24,32,25,
       18,11,4,5,12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,
       35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,58,59,52,45,
       38,31,39,46,53,60,61,54,47,55,62,63 };

    int i;
   
    
    for ( i = 0 ; i < ( N * N ) ; i++ ) {
        
        output_data[ zigzag[i] ] = input_data[i];
       
       // printf(" %2.1f ", output_data[ row ][ col ]);
    }
  

}

/*
 
 * The expansion routine reads in the compressed data from the DCT file,
 * then writes out the decompressed grey scale file.
 */

void ExpandFile( input, output, rows, cols,argc, argv )
bitstream *input; 
char *output; 
int rows;
int cols;
int argc;
char *argv[];
{   
   
    ROWS = rows;
    COLS = cols;
    int size=0,j,block=0,prevVlc=0;
    double buffer_im[ROWS][COLS];
    int *run,*cat;
    int row,col,count,vliAc,runValue,dc_Value,i,categry,mRun,bufIndex;
        int counter=0; 
    signed int vli; 
    double input_array[ N*N];
    double output_array[N*N];
    double output_array1[ROWS*COLS];
    double idct_out[N*N];
      bitstream *bs1 =  open_output_bitstream(output);
      
        fseek(input->stream, 0L, SEEK_END);
	size = ftell(input->stream);
	fseek(input->stream, SEEK_SET, 0);
	
    //printf(" %d \n ",size);
    double data;
    init_huffman_tables();
    run = (int*)malloc(4*sizeof(int));
    cat = (int*)malloc(4*sizeof(int));
      
       int tempInd = 0;
       count = 0;
     int tempCount = 0;
     while (ftell(input->stream) != size)
	{
		bufIndex = 0;
		memset(input_array, 0, sizeof(input_array));
		/* Get the DC data */
		dc_Value = getvlcdc(input);
		vli = getvli(input, dc_Value);
		
		input_array[bufIndex++] = vli+prevVlc;
		prevVlc = vli + prevVlc;
             
		/* Handle AC data */
		count =0;
		while(bufIndex < 63)
		{
			getvlcac(input,run,cat);
			
			runValue = *(run);
			if(runValue > 0)
			{
				while (runValue >= 16)
				{
					count = count + runValue;
					for (mRun = 0; mRun < runValue; mRun++)
					{
						input_array[bufIndex++] = 0;
					}
					getvlcac(input, run, cat);
					runValue = *(run);
					
				}
				count = count + runValue;
				for (mRun = 0; mRun < runValue; mRun++)
				{
					input_array[bufIndex++] = 0;
				}
				vliAc = getvli(input, *cat);
				input_array[bufIndex++] = vliAc;
				
			 }
			
			else
			{
				 vliAc = getvli(input,*cat);
				 if(vliAc == 0)
				 {
					 while (bufIndex < 64)
					 {
						 input_array[bufIndex++] = 0;
					 }
					
				 }
				 else
				 {
					
					 input_array[bufIndex++] = vliAc;
					 count++;
				 }
			 }
       
             }// Loop ending AC Coeff
             
           // Inverse zigzag is done here.
          
              
              
      
           Make2D(input_array, output_array );
              counter = 0;
/*             for ( i = 0 ; i <N ; i++ ) {*/
/*                  for  ( j = 0 ; j < N ; j++ ) */
/*                          printf(" %2.1f   ", output_array[i][j]);*/
/*                          */
/*                }       */
/*                     printf(" \n "); */
                     

                          
                        
                     
            //inverse quantization is done here 
               
                    
             for(i=0;i<N;i++){
                        for(j=0;j<N; j++){    
                         
         
                         input_array[counter] = output_array[counter]*Quan_Lum[ i ][ j ];
                         
                       
/*                         if (tempCount >=3099 && tempCount < 3100)*/
/*                          printf(" %.1f  ", input_array[counter]);*/
                          counter++; 
                     }
/*                     if (tempCount >=3099 && tempCount < 3100)*/
/*                     printf(" \n");*/
                  }   
                   
             // Inverse DCT is done here 
             
                
                
               /* 8x8 block of data is stored in 1D array 
                   prepare for IDCT , quantization and further compression to write into bitstream.*/
/*                     for(i=0; i < N; i++){*/
/*                        for(j = 0; j < N; j++){ */
/*                            input_array [counter] =  output_array[i][j];*/
/*                                //printf(" %2.1f ",input_array[counter]);*/
/*                                */
/*                             */
/*                           counter++;*/
/*                            }*/
/*                       }*/
                      
                      // printf("\n"); 
                   /* fDCT is done here*/
                   
                      idct( input_array, idct_out );
                      
                  for (i = 0; i<N*N; i++ ){


                     if (tempCount >=3099 && tempCount < 3100)
                          printf(" %.3f  ", idct_out [i]);
                     //printf(" %2.1f ",idct_out[i]);
                     }
                   /* IDCT output data is converted into 2D array */
          counter = 0; 
               for (i = 0; i<N*N; i++ ){
                 
                // output_array1[tempInd= input_array[counter];
                  output_array1[tempInd] = idct_out[i];
                  tempInd++;
                  counter ++;
                
                }
/*               unsigned int data = 0; */
/*                    for(i=0;i<N;i++){*/
/*                        for(j=0;j<N; j++){    */
/*                         */
/*                         output_array1[ i ][ j ]=idct_out[counter];*/
/*                    output_array1[ i ][ j ]=((unsigned int)floor(output_array1[ i ][ j ]+0.5))+128;*/
/*                         //data = (unsigned int)output_array[ i ][ j ];*/
/*                          */
/*                          printf(" %d  ",data);*/
/*                          //putbits(bs1,data,8);*/
/*                          //printf(" %2.1f  ",ou)tput_array[ i ][ j ]);*/
/*                    */
/*                      counter++;*/
/*                     }*/
/*                     */
/*                   } */
/*                   */
             
      
 tempCount++;
             
          }  //end of the File 
          
          
          
  
          
     delete_huffman_tables();               // remove huffman tables from the memory  
  
        // Making 2d buffer array 
                 counter=0;       
                   for(row=0; row<ROWS ; row+=N) { 
         
                      for (col=0; col<COLS; col+=N){
            
                
                
                
               /* 8x8 block of data is stored in 1D array 
                   prepare for DCT , quantization and further compression to write into bitstream.*/
                     for(i=row; i < (row+N); i++){
                        for(j = col; j < (col+N); j++){ 
                             buffer_im[i][j] = output_array1[counter];
                                //printf(" %2.1f ",  buffer_im[i][j]);
                                
                             
                           counter++;
                            }
                       }
                   }
                   
               }
               unsigned int dataV;
               for(i=0; i < ROWS; i++){
                        for(j = 0; j < COLS; j++){ 
                            // buffer_im[i][j] = buffer_im[i][j];
                             
                            dataV =((unsigned int)floor( buffer_im[i][j]+0.5))+128 ; 
                            if (dataV > 255) 
                                dataV = 255;
                            else if (dataV < 0 ) 
                             dataV = 0;    
                                
                           putbits(bs1,dataV,8);  
                                //printf(" %2.1f ",  buffer_im[i][j]);
                                
                             
                           counter++;
                            }
                       }
         
                 

    
}





/************************** End of compression *************************/

