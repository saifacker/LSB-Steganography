#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types
/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;  // Source Image File Name
    FILE *fptr_src_image;   // File Pointer for source Image
    uint image_capacity;    // Size of the source Image
    //uint bits_per_pixel;    
    //char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname; //Secret File name(source)
    FILE *fptr_secret;  //File Pointer for secret file
    char extn_secret_file[MAX_FILE_SUFFIX]; //Secret File Extension
    //char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;  //Size of the secret file

    /* Stego Image Info */
    char *stego_image_fname;    //Destination file name
    FILE *fptr_stego_image;     //File pointer for destination Image

} EncodeInfo;

/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv); //Done

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo); //Done

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo); //Process

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo); //Done

/* check capacity */
Status check_capacity(EncodeInfo *encInfo); //Done

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image); //Done

/* Get file size */
uint get_file_size(FILE *fptr); //Done

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image); //Done

/* Store Magic String */
Status encode_magic_string(const char *magic_string, FILE *fptr_src_image, FILE *fptr_stego_image); 

/* Encode secret file extenstion */
Status encode_secret_file_extn(char *extn, FILE *fptr_src_image, FILE *fptr_dest_image);

/* Encode secret file size */
Status encode_secret_file_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode secret file data*/
Status encode_secret_file_data(FILE *fptr_src_image, FILE *fptr_stego_image, FILE *fptr_secret, int size);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data,  FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode Secret file extension size*/
Status encode_secret_file_extn_size(int size, FILE *fptr_src_image, FILE *fptr_dest_image); //Done

Status encode_size_to_lsb(char *buffer, int size);

#endif
