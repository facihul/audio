#ifndef _MYFUNCTION_H_
#define _MYFUNCTION_H_
#define N               8
int ROWS ;
int COLS ;


extern void ReadPixelStrip( double *input, double strip[ N ][ COLS ] );
extern int InputCode( bitstream *input ); 
extern void ReadDCTData( bitstream *input, int input_data[ N ][ N ] ); 
extern void OutputCode(bitstream *output_file, int code ); 
extern void WritePixelStrip(FILE *output, int strip[ N ][ N ] );
extern void CompressFile( double *input, bitstream *output, int rows, int cols, int argc, char *argv[]); 
extern void ExpandFile( bitstream *input, FILE *output,int rows, int cols, int argc, char *argv[] ); 
extern void zigzagcode( double zigzag_out[N*N], double output_data[N][N] );
extern int GetCategory( signed int value);


#else


void ReadPixelStrip();
int InputCode();
void ReadDCTData();
void OutputCode();
void WritePixelStrip();
void CompressFile();
void ExpandFile();
void zigzagcode();
int GetCategory();




/*
 * Global data used at various places in the program.
 */


int InputRunLength;
int OutputRunLength;



#endif




