#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"


int main(int argc, char *argv[])
{ 
    if(argc == 1)
    {
      printf("Can't execute the program!!\nUsage:\nFor encoding : ./a.out -e beautiful.bmp secret.txt\nFor decoding : ./a.out -d beautiful.bmp secret.txt\n");
      return 0;
    }  
    if (check_operation_type(argv) == e_encode)
    {
        printf("Selected encode\n");
        EncodeInfo encInfo;
        if( read_and_validate_encode_args(argv, &encInfo)==e_success)
        {
               printf("Read and Validation succesfull\n");
               if(do_encoding(&encInfo)==e_success)
               {
               printf("Encodign completed\n");
               return 0;
               }
               else
               {
               printf("Failed encoding\n");
               return 0;
               }
        }
        else
        {
        printf("Failed Validation\nUsage:\nFor encoding : ./a.out -e beautiful.bmp secret.txt\nFor decoding : ./a.out -d beautiful.bmp secret.txt\n");
        return 0;
        } 
    }
    else if (check_operation_type(argv) == e_decode)
    {
            printf("selected decode\n");
            DecodeInfo decInfo;
        if( read_and_validate_decode_args(argv, &decInfo)==e_success)
        {
               printf("Read and Validation succesfull\n");
               if(do_decoding(&decInfo)==e_success)
               {
               printf("Decoding completed\n");
               return 0;
               }
               else
               {
               printf("Failed decoding\n");
               return 0;
               }
        }
        else
        {
        printf("Failed Validation\nUsage:\nFor encoding : ./a.out -e beautiful.bmp secret.txt\nFor decoding : ./a.out -d beautiful.bmp secret.txt\n");
        return 0;
        } 
    }
    else
    {
    printf("Invalid Option\nUsage:\nFor encoding : ./a.out -e beautiful.bmp secret.txt\nFor decoding : ./a.out -d beautiful.bmp secret.txt\n");
    return 0;
    }
}
OperationType check_operation_type(char* argv[])
{
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2]!=NULL && strcmp(strstr(argv[2],"."),".bmp")==0)
        encInfo->src_image_fname=argv[2];    
    else
    return e_failure;   
    if(argv[3]!=NULL && strcmp(strstr(argv[3],"."),".txt")==0)
           encInfo->secret_fname=argv[3];
    else
    return e_failure;
    if(argv[4]!=NULL)
        {
        encInfo->stego_image_fname=argv[4];
        return e_success;
        }
    else
        {
        encInfo->stego_image_fname="stego.bmp";
        return e_success;
        }
    return e_failure;    
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[2]!=NULL && strcmp(strstr(argv[2],"."),".bmp")==0 )
        decInfo-> stego_image_fname=argv[2];    
    else
    return e_failure;   
    if(argv[3]!=NULL && strcmp(strstr(argv[3],"."),".txt")==0)
    {
          decInfo->output_txt_fname=argv[3];
          return e_success;
    }
    else
    {
    decInfo->output_txt_fname="output.txt";
    return e_success;
    }
    return e_failure;
}