#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{

    int op_type = check_operation_type(argv[1]);
    // STEP 1: Check op_type == e_encode or not
    // STEP 2: Yes -> Start Our Encoding part, No-> STEP 3
    if (op_type == e_encode)
    {
        if (argc > 3)
        {
            sleep(1);
            printf("STATUS -> You opted for Encoding!!!\n\n");
            sleep(1);
            // Creating structure variable
            EncodeInfo encInfo;
            if (read_and_validate_encode_args(argv, &encInfo) == e_success)
            {
                // Proceed to next
                // call function do_encoding
                usleep(1);
                printf("STATUS -> Read and Validated Successfully!!!\n\n");
                do_encoding(&encInfo);
            }
            else
            {
                usleep(1);
                printf("STATUS -> ERROR: Read and Validation Failed!!!\n\n");
                return 0;
            }
        }
        else
        {
            sleep(1);
            printf("INFO : Please Pass Valid Arguments.\n");
            sleep(1);
            printf("INFO : Encoding - minimum 4 Arguments\n");
            sleep(1);
            printf("INFO : Decoding - minimum 3 Arguments\n");
            sleep(1);
            return 0;
        }
    }

    // STEP 3: Check op_type is e_decode or not
    // STEP 4: Yes-> Start decoding part, NO->STEP 5
    else if (op_type == e_decode)
    {
        if (argc > 2)
        {
            usleep(1);
            printf("STATUS -> You opted for Decoding!!!\n\n");
            // Creating structures variable
            DecodeInfo decoInfo;
            if (read_and_validate_decode_args(argv, &decoInfo) == d_success)
            {
                // Poceed to next
                // call function do_decoding()
                usleep(1);
                printf("STATUS -> Read and Validated Successfully!!!\n\n");
                do_decoding(&decoInfo);
            }
            else
            {
                usleep(1);
                printf("STATUS -> ERROR: Read and Validation Failed!!!\n\n");
                return 0;
            }
        }
        else
        {
            sleep(1);
            printf("INFO : Please Pass Valid Arguments.\n");
            sleep(1);
            printf("INFO : Encoding - minimum 4 Arguments\n");
            sleep(1);
            printf("INFO : Decoding - minimum 3 Arguments\n");
            sleep(1);
            return 0;
        }
    }
    // STEP 5: Print the error message and terminate the program (return e_failure)
    else
    {
        usleep(1);
        printf("STATUS -> ERROR: ENTER VALID COMMAND\n\n");
        return 0;
    }
}

OperationType check_operation_type(char *argv)
{
    // STEP 1: Check argv is -e or not
    // STEP 2: Yes -> return e_encode, NO -> Goto STEP 3
    if (strcmp(argv, "-e") == 0)
    {
        return e_encode;
    }
    // STEP 3: Check argv is -d or not
    // STEP 4: Yes-> return e_decode, NO -> Goto STEP 5
    else if (strcmp(argv, "-d") == 0)
    {
        return e_decode;
    }
    // STEP 5: return e_unsupported

    else
    {
        return e_unsupported;
    }
}