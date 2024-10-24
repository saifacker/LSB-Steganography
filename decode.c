#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

uint extn_size = 0;
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decoInfo)
{
    /*
    1. Check whether argv[2] is .bmp file or not
    2. If Yes GOTO step 3, If No print error message and return e_failure
    3. check if argv[3] is present or not, if yes check does it contain only name without any extension, if yes store it in sturcture member then return e_success
    if no print error message for two conditions then return e_failure.

    */
    if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        decoInfo->src_image_fname = argv[2];
    }
    else
    {
        printf("STATUS -> ERROR in 2nd Argument!!!\n");
        return d_failure;
    }
    if (argv[3] != NULL)
    {
        if (isalpha(argv[3]))
        {
            decoInfo->output_image_fname = argv[3];
        }
        else
        {
            printf("STATUS -> Please Enter Valid Command!!!\n");
            return d_failure;
        }
    }
    else
    {
        decoInfo->output_image_fname = "output";

        printf("STATUS -> Third File is Creating by Default!!!\n");
    }
    return d_success;
}

Status Open_files(DecodeInfo *decoInfo)
{
    /*
    1. open all the files if opened successfully return e_success
    else print error message
    */

    // Src Image file
    decoInfo->fptr_src_image = fopen(decoInfo->src_image_fname, "r");
    // Do Error handling
    if (decoInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "STATUS -> ERROR: Unable to open file %s\n", decoInfo->src_image_fname);
        return d_failure;
    }

    // No failure return d_success
    return d_success;
}

Status decode_magic_string(char *magic_string, DecodeInfo *decoInfo)
{
    fseek(decoInfo->fptr_src_image, 54, SEEK_SET);
    decode_data_from_image(strlen(magic_string), decoInfo);
    if ((strcmp(MAGIC_STRING, decoInfo->image_data)) == 0)
    {
        return d_success;
    }
    else
    {
        return d_failure;
    }
}

Status decode_data_from_image(int size, DecodeInfo *decoInfo)
{

    int i = 0;
    char data[size];
    char buffer[8];

    while (i < size)
    {
        data[i] = 0;
        fread(buffer, 8, 1, decoInfo->fptr_src_image);
        decode_byte_from_lsb(&data[i], buffer);
        i++;
    }
    data[size] = '\0';
    printf("%s\n", data);
    strcpy(decoInfo->image_data, data);
}

Status decode_byte_from_lsb(char *image_data, char *buffer)
{
    char decode[8];
    char ch = 0;
    int k = 7;
    for (int i = 0; i < 8; i++)
    {
        decode[k] = buffer[i] & 1;
        k--;
    }

    int val = 0;
    int base = 1;
    for (int i = 7; i >= 0; i--)
    {
        val = val + decode[i] * base;
        base = base * 2;
    }
    *image_data = (char)val;
}

Status decode_secret_file_extn_size(int size, FILE *fptr_src_image)
{
    char buffer[32];
    fread(buffer, 1, 32, fptr_src_image);
    decode_size_from_lsb(buffer, &extn_size);
    if (extn_size == strlen(".txt"))
    {
        return d_success;
    }
    else
    {
        return d_failure;
    }
}

Status decode_size_from_lsb(char *buffer, uint *length)
{
    char decode[32];
    int k = 0;
    for (int i = 0; i < 32; i++)
    {
        decode[k] = buffer[i] & 1;
        k++;
    }

    int ascii = 0;
    for (int i = 0; i < 32; i++)
    {
        ascii = ascii << 1;
        ascii = ascii | decode[i];
    }
    *length = ascii;
}

Status decode_secret_file_extn(char *file_extn, DecodeInfo *decoInfo)
{
    char buffer[8];
    char data[extn_size];
    for (int i = 0; i < extn_size; i++)
    {
        fread(buffer, 8, 1, decoInfo->fptr_src_image);
        decode_byte_from_lsb(&data[i], buffer);
    }
    data[extn_size] = '\0';
    printf("%s\n", data);

    if ((strcmp(".txt", data)) == 0)
    {
        strcpy(decoInfo->extn_data, data);
        return d_success;
    }
    else
    {

        return d_failure;
    }
}

Status decode_secret_file_size(int size, DecodeInfo *decoInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decoInfo->fptr_src_image);
    char decode[32];
    int k = 0;
    for (int i = 0; i < 32; i++)
    {
        decode[k] = buffer[i] & 1;
        k++;
    }
    int ascii = 0;
    for (int i = 0; i < 32; i++)
    {
        ascii = ascii << 1;
        ascii = ascii | decode[i];
    }
    decoInfo->size_secret_file = ascii;
    return d_success;
}

Status decode_secret_file_data(DecodeInfo *decoInfo)
{
    char ch;
    decoInfo->fptr_secret = fopen(decoInfo->output_image_fname, "w");
    if (decoInfo->fptr_secret == NULL)
    {
        fprintf(stderr, "can't open decode.txt file\n");
        return d_failure;
    }
    else
    {
        for (int i = 0; i < decoInfo->size_secret_file; i++)
        {
            fread(decoInfo->src_image_fname, 8, sizeof(char), decoInfo->fptr_src_image);
            decode_byte_from_lsb(&ch, decoInfo->src_image_fname);

            fputc(ch, decoInfo->fptr_secret);
        }
        return d_success;
    }
}

Status do_decoding(DecodeInfo *decoInfo)
{
    // OPENING SOURCE FILE
    if (Open_files(decoInfo) == d_success)
    {
        printf("STATUS -> Source File Opened Successfully!!!\n");
    }
    else
    {
        printf("STATUS -> Source File Cannot be Opened!!!\n");
        return d_failure;
    }

    //  DECODE MAGIC STRING
    if (decode_magic_string(MAGIC_STRING, decoInfo) == d_success)
    {
        printf("STATUS -> Magic String Decoded Successfully!!!\n");
    }
    else
    {
        printf("STATUS -> Magic String Decoding Failed!!!\n");
        return d_failure;
    }

    // DECODE EXTENSION SIZE
    if (decode_secret_file_extn_size(sizeof(int), decoInfo->fptr_src_image) == d_success)
    {
        printf("STATUS -> Secret File Extension Size Decoded Successfully!!!\n");
    }
    else
    {
        printf("STATUS -> Secret File Extension Size Decoding Failed!!!\n");
        return d_failure;
    }

    // DECODE EXTENSION
    if (decode_secret_file_extn(".txt", decoInfo) == d_success)
    {
        printf("STATUS -> Secret File Extension Decoded Successfully!!!\n");
    }
    else
    {
        printf("STATUS -> Secret File Extension Decoding Failed!!!\n");
        return d_failure;
    }

    // DECODE SECRET FILE SIZE
    if (decode_secret_file_size(decoInfo->size_secret_file, decoInfo) == d_success)
    {
        printf("STATUS -> Secret File Size is Succesfully Decoded!!!\n");
    }
    else
    {
        printf("STATUS -> Secret File Size is Failed At Decoding!!!\n");
        return d_failure;
    }

    // CREATE FILE .BMP
    char str1[10];
    char str2[10];
    strcpy(str1, decoInfo->output_image_fname);
    strcpy(str2, decoInfo->extn_data);
    strcat(str1, str2);
    decoInfo->output_image_fname = str1;
    printf("%s\n", decoInfo->output_image_fname);
    decoInfo->fptr_output_image = fopen(decoInfo->output_image_fname, "w");
    if (decoInfo->fptr_output_image == NULL)
    {
        printf("File Failed to Open\n");
    }
    else
    {
        printf("Successfully Opened\n");
    }
    // DECODE FILE DATA
    if (decode_secret_file_data(decoInfo) == d_success)
    {
        printf("STATUS -> Secret Data Successfully Stroed In Output File\n");
    }
    else
    {
        printf("STATUS -> Secret Data Cannot be Decoded in the Output File\n");
        return d_failure;
    }


}