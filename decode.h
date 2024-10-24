#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
/* 
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    char image_data[10];
    char extn_data[10];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char *extn_secret_file;
    int size_secret_file;

    /* out Image Info */
    char *output_image_fname;
    FILE *fptr_output_image;

} DecodeInfo;

/* Decoding function prototype */

/* Perform the encoding */
Status do_decoding(DecodeInfo *decoInfo);

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decoInfo);

/* Get File pointers for i/p and o/p files */
Status Open_files(DecodeInfo *decoInfo);

/* Store Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decoInfo);

/* Decode secret file extenstion size */
Status decode_secret_file_extn_size(int size, FILE *fptr_src_image);

/* Decode secret file extenstion */
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decoInfo);

/* Decode secret file size */
Status decode_secret_file_size(int file_size, DecodeInfo *decoInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decoInfo);

/* Decode function, which does the real encoding */
Status decode_data_from_image(int size,DecodeInfo *decoInfo);

Status decode_extn_data_from_image(int size, FILE *fptr_src_image, DecodeInfo *decoInfo);

/* Decode a byte into LSB of image data array */
Status decode_byte_from_lsb(char *data, char *buffer);

/* Decode a size into LSB of image data array */
Status decode_size_from_lsb(char *buffer, uint *length);

#endif
