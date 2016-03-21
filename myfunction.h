#ifndef _MYFUNCTION_H_
#define _MYFUNCTION_H_
#define N               8
int ROWS ;
int COLS ;

extern void inv_zigzag( double input_data[N*N], double output_data[ N * N ] ); 
extern void CompressFile( double *input, bitstream *output, int rows, int cols, int argc, char *argv[]); 
extern void ExpandFile( bitstream *input, char *output,int rows, int cols, int argc, char *argv[] ); 
extern void zigzagcode( double zigzag_out[N*N], double output_data[N][N] );



#else


void inv_zigzag();
void CompressFile();
void ExpandFile();
void zigzagcode();


#endif




