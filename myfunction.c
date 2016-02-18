/************************** Start of DCT.C *************************
 *
 * This is the DCT module, which implements a graphics compression
 * program based on the Discrete Cosine Transform.  It needs to be
 * linked with the standard support routines.
 *
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
 * This macro is used to ensure correct rounding of integer values.
 */
#define ROUND( a )      ( ( (a) < 0 ) ? (int) ( (a) - 0.5 ) : (int) ( (a) + 0.5 ) )



/*
 * Global data used at various places in the program.
 */



int InputRunLength;
int OutputRunLength;
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
 This routine finds the category value of the input data
 
 */
 
 
int GetCategory(signed int value_in) 
{
   int value = abs(value_in);
   //printf("%d ",value);
   if (value == 1) {
       return 1;
   } else if(value >= 2 && value <= 3) {
       return 2;
   } else if(value >= 4 && value <= 7) {
       return 3;
   } else if(value >= 8 && value <= 15) {
       return 4;
   } else if(value >= 16 && value <= 31) {
       return 5;
   } else if(value >= 32 && value <= 63) {
       return 6;
   } else if(value >= 64 && value <= 127) {
       return 7;
   } else if(value >= 128 && value <= 255) {
       return 8;
   } else if(value >= 256 && value <= 511) {
       return 9;
   } else if(value >= 512 && value <= 1028) {
       return 10;
   }
}


/*
 *
 * A bit count of zero is followed by a four bit number telling how many
 * zeros are in the encoded run.  A value of 1 through ten indicates a
 * code value follows, which takes up that many bits.  The encoding of values
 * into this system has the following characteristics:
 *
 *         Bit Count               Amplitudes
 *         ---------       --------------------------
 *             0			0
 * 	       1                      -1, 1
 *             2                -3 to -2, 2 to 3
 *             3                -7 to -4, 4 to 7
 *             4               -15 to -8, 8 to 15
 *             5              -31 to -16, 16 to 31
 *             6              -63 to -32, 32 to 64
 *             7             -127 to -64, 64 to 127
 *             8            -255 to -128, 128 to 255
 *             9            -511 to -256, 256 to 511
 *            10           -1023 to -512, 512 to 1023
 *
 */
 
 
int InputCode( input_file )
bitstream *input_file; 
{
    int bit_count;
    int result;

    if ( InputRunLength > 0 ) {
        InputRunLength--;
        return( 0 );
    }
    bit_count = (int) getbits( input_file, 2 );
 
    if ( bit_count == 0 ) {
        InputRunLength = (int) getbits( input_file, 4 );
        return( 0 );
    }
    if ( bit_count == 1 )
        bit_count = (int) getbits( input_file, 1 ) + 1; 
    else{
        bit_count = (int) getbits( input_file, 2 ) + ( bit_count << 2 ) - 5;
	}
    
    result = (int) getbits( input_file, bit_count );
    if ( result & ( 1 << ( bit_count - 1 ) ) )
        return( result );
    return( result - ( 1 << bit_count ) + 1 );
}


/*
 * This routine writes out a strip of pixel data to a GS format file.
 */

void WritePixelStrip( output, strip )
FILE *output;
int strip[ N ][ N ];
{
    int row;
    int col;

    for ( row = 0 ; row < N ; row++ )
        for ( col = 0 ; col < N ; col++ )
           putc( strip[ row ][ col ], output );
}






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
    double quant;
    signed int diff=0,Curr_dc,code;
    int row,col,dc_cate,ac_cate;
    int i,j,rowLim,colLim, Len = ROWS*COLS;
    double input_array[64];
    double output_array[64];
    double dctq[ N ][ N ];
    double buffer_im[ROWS][ROWS];
    double zigzag_out[N*N];
    
    
      int counter=0;
         for(i=0; i<rows ;i++) {
          for(j=0; j<cols ;j++){
             
             buffer_im[i][j] = input[counter];
             //printf("  %f",buffer_im[i][j]);
             counter++;
             }
        }
    
    init_huffman_tables(); // initializing huffman table
    
      printf("%d \n",N);
         for(row=0; row<ROWS ; row+=N) {
            for (col=0; col<COLS; col+=N){
            
             counter=0; rowLim = row+N; colLim = col+N;
             
               /* 8x8 block of data stored in 1D array */
                     for(i=row; i < rowLim; i++){
                        for(j = col; j < colLim; j++){ 
                            input_array [counter] =  buffer_im[i][j];
                                //printf("data-  %f",input_array[counter]);
               
                           counter++;
                            }
                       }
             /* fDCT is done here*/
                fdct( input_array, output_array );
                counter=0;
             /* DCT output data is converted into 2D array 
                Each dct valu is quantized and rounded */

                 for(i=0;i<N;i++){
                   for(j=0;j<N; j++){    
                     quant = output_array[counter];
                      dctq[ i ][ j ] =  ROUND(quant/Quan_Lum[ i ][ j ]);
                     // printf(" %2f ", dctq[ i ][ j ]);
                      counter++;
                     }
                   }  
              /* zigzag order arrenged  here  */ 
               zigzagcode( zigzag_out, dctq );
                    /* for ( i = 0 ; i < ( N * N ) ; i++ ) {
                           printf(" %3.2f ", zigzag_out[i]);
                           }
                     printf(" \n\n "); */
                     
              /* Find DC Differential value and write into file */       
                
                if (col==0) diff=(signed int)zigzag_out[0];
                else diff=(signed int)zigzag_out[0]-Curr_dc; 
                Curr_dc= (signed int)zigzag_out[0];
                dc_cate=GetCategory(diff);  
               // printf("temp  and diff value %d  %d\n",Curr_dc, dc_cate);
           
               // Problem here 
               /* find the vlc and vli 
               /* write it into bitstream */
               
              putvlcdc(output,dc_cate);  
              putvli(output,dc_cate,Curr_dc); 
              int run =0;
              
          /*This for loop is finding ac category value and coreesponding runlength. */
          
            for ( i = 1; i < (N*N) ; i++ ) 
            {
                code=(signed int)zigzag_out[i];
                //printf("code: %d \n",code);
	        ac_cate=GetCategory(code);
	        if ( code == 0 ) {
		    run++;
                } else {
		    if(run != 0) {
		        while(run > 0) {
			    if(run <= 16) {
			    /* writes the encoded value with respect to run and category value  */
			        putvlcac(output,run -1,ac_cate);  
			                                                                 
			        run = 0;
			    } else {
			        putvlcac(output,15L,ac_cate);
			        run -= 16;
			    }		        	
		        }
		      
		        putvli(output,ac_cate,code);
		     }
		    if(run == 0) {
		    putvlcac(output,run,ac_cate);
		    putvli(output,ac_cate,code);
		    } 
		     
	         }
             }      
                     
       
             
         
          
                     
          }   
        }    
            
      /* remove huffman tables from the memory */  
  
      delete_huffman_tables();
}
/*
 This routine reads the 2D DCT data and arrange it in zigzag order and than quantized the data afterwords
 After quantization rounded value is returned.

*/

void zigzagcode(zigzag_out, output_data )
double zigzag_out[N*N];
double output_data[ N ][ N ];
{
    int i;
    int row;
    
    int col;
    double result;
        
    for ( i = 0 ; i < ( N * N ) ; i++ ) {
        row = ZigZag[ i ].row;
        col = ZigZag[ i ].col;
        zigzag_out[i] = output_data[ row ][ col ];
		     
     }
}
/*
 * This routine reads in a block of encoded DCT data from a compressed file.
 * The routine reorders it in row major format, and dequantizes it using
 * the quantization matrix.
 */
/*
void ReadDCTData( input_file, input_data )
bitstream *input_file; 
int input_data[ N ][ N ];
{
    int i;
    int row;
    int col;
    
    for ( i = 0 ; i < ( N * N ) ; i++ ) {
        row = ZigZag[ i ].row;
        col = ZigZag[ i ].col;
        input_data[ row ][ col ] = InputCode( input_file )  * Quantum[ row ][ col ];
    }
  

}

/*
 
 * The expansion routine reads in the compressed data from the DCT file,
 * then writes out the decompressed grey scale file.
 */

void ExpandFile( input, output, rows, cols,argc, argv )
bitstream *input; 
FILE *output; 
int rows;
int cols;
int argc;
char *argv[];
{   
    ROWS = rows;
    COLS = cols;
    int row,dc_cat;
    
    int col,runlength,category;
    int i;
    signed int vli; 
    int input_array[ N ][ N ];
    int output_array[ N ][ N ];
 
    //init_huffman_tables();
                      // initializing huffman table
       for ( row = 0 ; row < ROWS ; row += N ) {
          for ( col = 0 ; col < COLS ; col += N ) {
              input_array[row][col] = getbit(input);
          }
       }
       
      
      // delete_huffman_tables();               // remove huffman tables from the memory  
    
}


   


/************************** End of DCT.C *************************/

