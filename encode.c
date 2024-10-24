#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <unistd.h>

/*
--> MAGIC STRING IS USED AS PASSWORD. FIRST YOU NEED TO ENCODE MAGIC STRING, BECAUSE YOU ARE GOING TO SEND N NUMBER OF FILES. ONCE IF YOU FIND MAGIC
STRING FROM ANY OF THE FILE START ENCODING OTHER FILES. NOTE: FIRST 54 BYTES ARE FILLED WITH HEADER BYTES, FROM THERE SOME BYTES ARE FILLED WITH RGB
DATA. (IT IS KIND OF PASSWORD, BECAUSE WE SEND MULTIPLE FILES TO FIND THAT FILE WE ARE SENDING PASSWORD)

--> 1. source file size
    2. secret file size * 8
    3. Extension size

    check capacity (.bmp,stego.bmp)

    " (src_file_size) >  16+32+(extn size * 8) + 32 + (data size * 8) + 54 + 1"


    first add 54 bytes of header of .bmp file
    encode_magic_string (#*) 16
    encode_extn_size 32
    encode_extn size * 8
    encode_secret_data_size 32
    encode_secret_data (text file data size) size * 8
    copy remaining data
    EOF


*/
int extension_size;
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // STEP 1: argv[2] is .bmp file or not
    // STEP 2: IF YES -> Store agrv[2] to structre member GOTO STEP 3, if NO print error message and then return e_failure
    if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        sleep(1);
        printf("STATUS -> ERROR: In 2nd Argument .bmp File Not Exists!!!\n\n");
        return e_failure;
    }

    // STPE 3: check argv[3] is .txt file or not
    // STEP 4: IF YES -> Store argv[3] to structre member GOTO STEP 5, if NO print error message and then return e_failure
    if (strcmp(strstr(argv[3], "."), ".txt") == 0)
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, ".txt");
    }
    else
    {
        sleep(1);
        printf("STATUS -> ERROR: In 3rd Argument .txt File Not Exists!!!\n\n");
        return e_failure;
    }
    // STEP 5: Check argv[4] is passed or not
    // STEP 6: If passed -> check .bmp or not
    // STEP 7: IF YES -> Store agrv[4] to structre member GOTO STEP 8, if NO Default name into strucutre member

    if (argv[4] != NULL)
    {
        // STEP 8: Check .bmp or not
        // STEP 9: Yes-> storing into structure member and return e_success, if  NO print ERROR message and then return e_failure
        if (strcmp(strstr(argv[4], "."), ".bmp") == 0)
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            sleep(1);
            printf("STATUS -> ERROR: In 4th Argument .bmp File Not Exists!!!\n\n");
            return e_failure;
        }
    }
    else
    {
        encInfo->stego_image_fname = "stegno.bmp";
        sleep(1);
        printf("STATUS -> Third File is Creating by Default File\n\n");
    }

    return e_success;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        sleep(1);
        fprintf(stderr, "STATUS -> ERROR: Unable to open file %s\n\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        sleep(1);
        fprintf(stderr, "STATUS -> ERROR: Unable to open file %s\n\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        sleep(1);
        fprintf(stderr, "STATUS -> ERROR: Unable to open file %s\n\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    // write check capacity function

    // STEP 1: Find the size of source file
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // STEP 2: Find secret file size
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    // STEP 3: Extension file size
    char *extension = strstr(encInfo->secret_fname, ".");

    if (extension != NULL)
    {
        extension_size = strlen(extension);
    }

    // Checking condition for the source file size is greater than below conditions
    if ((encInfo->image_capacity) > 54 + strlen(MAGIC_STRING) + (extension_size) + (extension_size * 8) + (encInfo->size_secret_file) + (encInfo->size_secret_file) * 8)
    {
        return e_success;
    }

    else
    {
        return e_failure;
    }
}

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("WIDTH = %u\n\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("HEIGHT = %u\n\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr_file_size)
{
    fseek(fptr_file_size, 0, SEEK_END);
    int size = ftell(fptr_file_size);

    return size;
}

// Function for copying header file of 54 bytes
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    char header[54];

    if (fread(header, 1, 54, fptr_src_image))
    {
        if (fwrite(header, 1, 54, fptr_dest_image))
        {
            return e_success;
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        return e_failure;
    }
}

// Function for Magic string Encoding
Status encode_magic_string(const char *magic_string, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    encode_data_to_image(MAGIC_STRING, fptr_src_image, fptr_stego_image);
    return e_success;
}

// Encoding data to image
Status encode_data_to_image(char *data, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    int data_length = strlen(data);
    // STEP 1: read 8 bytes of data from src file and store into a arr
    for (int i = 0; i < data_length; i++)
    {
        fread(buffer, 1, 8, fptr_src_image);
        // STEP 2: Call function encode_byte_to_lsb(buffer,data[0])
        encode_byte_to_lsb(data[i], buffer);
        // STEP 3: Write 8 bytes of data to destination file
        fwrite(buffer, 1, 8, fptr_stego_image);
        // STEP 4: Repeat the operation data_length times
    }
    return e_success;
}

// Replacing LSB for Encoding
Status encode_byte_to_lsb(char ch, char *buffer)
{
    char res1[8];
    char res2[8];

    // STEP 1: Clear a bit in the arr[0];
    for (int i = 0; i < 8; i++)
    {
        res1[i] = buffer[i] & (~1);
    }

    // STEP 2: Get a bit from ch
    for (int i = 0; i < 8; i++)
    {
        res2[i] = (ch >> i) & 1;
    }

    // STEP 3: Replace the got bit into arr[0]th lsb position
    for (int i = 0; i < 8; i++)
    {
        buffer[i] = res1[i] | res2[i];
    }
    // STEP 4: Repeat the operation 8 times*/
}

// Function for Encoding Secret File Extension Size
Status encode_secret_file_extn_size(int extn_size, FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[32];
    // STEP 1: Read 32 bytes from source file and store into an array

    fread(buffer, 1, 32, fptr_src_image);
    // STEP 2: CALL encode_size_to_lsb(buffer, extn_size);
    encode_size_to_lsb(buffer, extension_size);
    fwrite(buffer, 1, 32, fptr_dest_image);

    return e_success;
}

Status encode_size_to_lsb(char *buffer, int size)
{
    // STEP 1: CLEAR A BIT IN THE ARR[0]
    // STEP 2: CALL ENCODE_BYTE_TO_LSB
    // STEP 3: WRITE 8 BYTES OF DATA TO DEST FILE
    // STEP 4: REPEAT THE OPERATION DATA_LENGTH TIME
    for (int i = 0; i < 32; i++)
    {
        buffer[i] = /*clearing last bit*/ (buffer[i] & 0xfffffffe) | /*getting last bit */ ((size & (1 << (31 - i))) >> (31 - i));
    }
}

// Function for Encoding Secret File Extension
Status encode_secret_file_extn(char *extn, FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // STEP 1: call encode_data_to_image(extn, file pointers)
    encode_data_to_image(extn, fptr_src_image, fptr_dest_image);
    return e_success;
}

// Function for Encoding Secret Data Size
Status encode_secret_file_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // printf("%d\n", size);
    char buffer[32];
    fread(buffer, 1, 32, fptr_src_image);
    encode_size_to_lsb(buffer, size);

    fwrite(buffer, 32, 1, fptr_stego_image);
    return e_success;
}

// Function for Encoding Secret Data
Status encode_secret_file_data(FILE *fptr_src_image, FILE *fptr_stego_image, FILE *fptr_secret, int size)
{
    rewind(fptr_secret);
    char ch;
    char buffer[8];
    char res1[8];
    char res2[8];
    for (int i = 0; i < size; i++)
    {
        fread(&ch, 1, 1, fptr_secret);
        fread(buffer,1,8,fptr_src_image);

        for(int i=0; i<8; i++)
        {
            res1[i] = buffer[i] & (~1);
            res2[i] = (ch >> i) & 1;
            buffer[i] = res1[i] | res2[i];
        }


        fwrite(buffer,1,8,fptr_stego_image);
    }

    return e_success;
}
// Function for copying remaining data
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src_image) == 1)
    {
        fwrite(&ch, 1, 1, fptr_stego_image);
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    // STEP 1: Call Open file function(encInfo)
    // STEP 2: Check e_success or e_failure
    // STEP 3: e_success -> GOTO step 4, e_failure -> print Error message and return e_failure
    if (open_files(encInfo) == e_success)
    {
        sleep(1);
        printf("STATUS -> Source Image Opened Successfully!!!\n\n");
        sleep(1);
        printf("STATUS -> Secret File Opened Successfully!!!\n\n");
        sleep(1);
        printf("STATUS -> Destination File Opened Successfully!!!\n\n");
        sleep(1);
    }
    else
    {
        sleep(1);
        printf("STATUS -> ERROR: FILE CANNOT BE OPENED!!!\n\n");
        return e_failure;
    }

    // STEP 4: call function check_capacity(encInfo)
    // STEP 5: Comparison -> true -> GOTO STEP 6:return e_success, False -> return e_failure
    // STEP 6: e_success -> print msg, GOTO STEP 7, e_failure -> print error msg and return e_failure.
    if (check_capacity(encInfo) == e_success)
    {
        sleep(1);
        printf("STATUS -> File Capacity has been calculated Successfully!!!\n\n");
    }
    else
    {
        sleep(1);
        printf("STATUS -> ERROR: In Checking File Capacity!!!\n\n");
        return e_failure;
    }
    // STEP 7: COPY bmp header
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        sleep(1);
        printf("STATUS -> Successfully Copied Header File!!!\n\n");
    }
    else
    {
        sleep(1);
        printf("STATUS -> ERROR: In Copying .bmp header file!!!\n\n");
        return e_failure;
    }

    // STEP 8: ENCODING MAGIC STRING
    if (encode_magic_string(MAGIC_STRING, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        sleep(1);
        printf("STATUS -> Successfully Encoded Magic String!!!\n\n");
    }
    else
    {
        sleep(1);
        printf("STATUS -> Encoding Magic String Failed!!!\n\n");
        return e_failure;
    }

    // STEP 9: ENCODING EXTENSION SIZE
    if (encode_secret_file_extn_size(extension_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        sleep(1);
        printf("STATUS -> Extension of File Size is Successfully Encoded!!!\n\n");
    }
    else
    {
        sleep(1);
        printf("STATUS -> ERROR: In Encoding Extension Size!!!\n\n");
        return e_failure;
    }

    // STEP 10: ENCODING EXTENSION
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        sleep(1);
        printf("STATUS -> Secret File Extension Successfully Encoded!!!\n\n");
    }
    else
    {
        sleep(1);
        printf("STATUS -> ERROR: In Encoding Extension!!!\n\n");
        return e_failure;
    }

    // STEP 11: ENCODING SECRET DATA SIZE
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        sleep(1);
        // CALL same encoding extension
        printf("STATUS -> Secret File Size is Encoded SuccessFully!!!\n\n");
    }
    else
    {
        sleep(1);
        printf("STATUS -> ERROR: In Encoding Secret file size!!!\n\n");
        return e_failure;
    }

    // STEP 12: ENCODING SECRET DATA
    if (encode_secret_file_data(encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo->fptr_secret, encInfo->size_secret_file) == e_success)
    {
        sleep(1);
        printf("STATUS -> Secret Data Is Encoded Successfully!!!\n\n");
    }
    else
    {
        sleep(1);
        printf("STATUS -> ERROR: In Encoding Secret file data!!!\n\n");
        return e_failure;
    }

    // STEP 13: COPYING REMAINING DATA
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        sleep(1);
        printf("STATUS -> Remaining Data Copied Successfuly!!!\n\n");
    }
    else
    {
        sleep(1);
        printf("STATUS -> ERROR: In Copying Remaining Data!!!\n\n");
        return e_failure;
    }
}
