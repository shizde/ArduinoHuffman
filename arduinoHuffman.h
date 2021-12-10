/*
 * ArduinoHuffman 
 * Authors: Rafael Perez
 */

#ifndef arduinoHuffman_H_
#define arduinoHuffman_H_

// Compression Dictionary Constant
#define NUM_CHAR 256

// Compression Functions
void compress_file(char* dataset, char* filename);
void compress_string(char *dataset);

// Decompression Functions
//void decompress_file(char *filename);
//void decompmress_string(char *dataset);

#endif /* arduinoHuffman_H_ */