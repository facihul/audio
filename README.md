

his code is consist of four  different files icode.c , bitstream.c and dct.c myfunction.c with three header files bitstream.h and dct.h , myfunction.h. bitstream and dct files are downloaded from course webpage . Myfunction.c file is the whole implementation of the gray image (encoding and decoding)

Project work description: 
The purpose of the project is to compress a raw image file and decompress. Three different sets of test image was given two of them are gray image and one rgb image. This code can compress and decompress the gray images but rgb cannot be done by this code. 

How to run the code: 
to compile:
gcc icode.c bitstream.c dct.c myfunction.c -o icode -lm  (for lintula OS)
gcc icode.c bitstream.c dct.c myfunction.c -o icode  (for unix OS)
to encode:
./icode encode city.gray city.enc 400 490  
./icode encode rapids.gray rapids.enc 256 256   
to decode:
./icode decode city.enc citydec.gray 400 490
./icode decode rapids.enc rapidsdec.gray 256 256

Snapshot:
panna-MacBook-Pro:image mdfacihulazam$ ./icode encode rapids.gray rapids.enc 256 256
  width:  256 
  height:  256 
  Length:  65536 
Encoding Gray Image .....
panna-MacBook-Pro:image mdfacihulazam$ ./icode decode rapids.enc rapidsdec.gray 256 256
  width:  256 
  height:  256 
  Length:  65536 
Decoding Gray Image.....
panna-MacBook-Pro:image mdfacihulazam$ ./icode encode city.gray city.enc 400 496
  width:  496 
  height:  400 
  Length:  198400 
Encoding Gray Image .....
panna-MacBook-Pro:image mdfacihulazam$ ./icode decode city.enc citydec.gray 400 496
  width:  496 
  height:  400 
  Length:  198400 
Decoding Gray Image.....
