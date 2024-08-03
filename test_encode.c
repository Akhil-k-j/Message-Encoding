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
        printf("Failed Validation\nUsage:\nFor encoding : ./a.out -e beautiful.bmp secret.txt optional.bmp\nFor decoding : ./a.out -d stego.bmp secret.txt\n");
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
        printf("Failed Validation\nUsage:\nFor encoding : ./a.out -e beautiful.bmp secret.txt optional.bmp\nFor decoding : ./a.out -d stego.bmp secret.txt\n");
        return 0;
        } 
    }
    else
    {
    printf("Failed Validation\nUsage:\nFor encoding : ./a.out -e beautiful.bmp secret.txt optional.bmp\nFor decoding : ./a.out -d stego.bmp secret.txt\n");
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
    char *p;
    if(argv[2]!=NULL && strstr(argv[2],"."))
        {
            p=strstr(argv[2],".");
            if(strcmp(p,".bmp")==0)
            {
                printf("%s\n",argv[2]);
                encInfo->src_image_fname=argv[2];
            }
            else
            {
            printf("!! %s extension should be .bmp !! \n",argv[2]);
            return e_failure;
            }
        }     
    else
    return e_failure;   
    if(argv[3]!=NULL && strstr(argv[3],"."))
        {
            p=strstr(argv[3],".");
            if(strcmp(p,".txt")==0)
            {
                printf("%s\n",argv[3]);
                encInfo->secret_fname=argv[3];
            }
            else
            {
            printf("!! %s extension should be .txt !! \n",argv[3]);
            return e_failure;
            }
        }
    else
    return e_failure;
    if(argv[4]!=NULL && strstr(argv[4],"."))
        {
            p=strstr(argv[4],".");
            if(strcmp(p,".bmp")==0)
            {
        encInfo->stego_image_fname=argv[4];
        return e_success;
            }
            else
            {
            printf("!! %s extension should be .bmp !! \n",argv[4]);
            return e_failure;
            }
        }
    else
        {
        printf("!!filename defaults to stego.bmp!!\n");
        encInfo->stego_image_fname="stego.bmp";
        return e_success;
        }
    return e_failure;    
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *p;
    if(argv[2]!=NULL && strstr(argv[2],"."))
        {
            p=strstr(argv[2],".");
            if(strcmp(p,".bmp")==0)
            {
                printf("%s\n",argv[2]);
                decInfo->stego_image_fname =argv[2];
            }
            else
            {
            printf("!! %s extension should be .bmp !! \n",argv[2]);
            return e_failure;
            }
        }       
    else
    return e_failure; 


    if(argv[3]!=NULL && strstr(argv[3],"."))
        {
            p=strstr(argv[3],".");
            if(strcmp(p,".txt")==0)
            {
                printf("%s\n",argv[3]);
                decInfo->output_txt_fname=argv[3];
                return e_success;
            }
            else
            {
             printf("!!filename defaults to output.txt!!\n");
             decInfo->output_txt_fname="output.txt";
             return e_success;
             }
        }
    else
    return e_failure;
}
