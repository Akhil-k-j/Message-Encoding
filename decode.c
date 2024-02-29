#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo-> fptr_stego_image=fopen(decInfo->stego_image_fname,"r");
    if(decInfo->fptr_stego_image ==NULL)   //Error handling.
    {  
        fprintf(stderr,"Error:unable to open the file %s", decInfo->stego_image_fname);
        return e_failure;
    }
    decInfo-> fptr_output_txt=fopen(decInfo->output_txt_fname,"w");
    if(decInfo->fptr_output_txt==NULL)
    {
        fprintf(stderr,"Error:unable to open the file %s\n",decInfo->output_txt_fname);
        return e_failure;
    }
    fseek(decInfo->fptr_output_txt,0,SEEK_SET);
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);    //File pointer always start from zero. (0-53 bytes => header with Total 54 Bytes);
    return e_success;                                            //Since our Magic string is stored at 54th byte we are seeking to 54. Not 53. 
}
Status decode_lsb_to_byte(char *image_buffer,char *data)
{
    //getting LSB 
    uint mask=1<<7; 
    for(int i=0;i<8;i++)
    {        
        *data=(*data&(~mask))|((image_buffer[i]&1)<<(7-i));
        mask=mask>>1;
    }
    return e_success;
}
Status decode_image_to_data(int size,FILE *fptr_stego_image, FILE *fptr_output_txt,DecodeInfo*decInfo)
{
    int i;
    char magicstring[size];
   // printf("main size:%d\n", size);
    for(i=0;i<size;i++)    
    {   
        fread(decInfo->image_data,sizeof(char),8,decInfo->fptr_stego_image);
        decode_lsb_to_byte(decInfo->image_data,&magicstring[i]);
    }
    magicstring[i]=0;
    if(strcmp(MAGIC_STRING,magicstring)==0)
    {
    printf("Magic string %s matched\n", magicstring);
    return e_success;
    }
    else
    return e_failure;
}
Status decode_magic_string(const char*magic_string,DecodeInfo*decInfo)
{
    //for every decoding we will call decode data to image function.
    if(decode_image_to_data(strlen(MAGIC_STRING),decInfo->fptr_stego_image,decInfo->fptr_output_txt,decInfo)==e_success)
    return e_success;
    else
    return e_failure;
}

Status decode_lsb_to_size(char *imagebuffer,int *data)
{
    unsigned int mask=1<<31;
    for(int i=0;i<32;i++)
    {
    *data=(*data&(~mask))| ((imagebuffer[i]&1) << (31-i));
    mask=mask>>1;
    }
    return e_success ;
}

Status decode_size(FILE* fptr_stego_image,DecodeInfo*decInfo)
{
    //decoding size which is int type 
    char str[32];
    int xsize=0;
    fread(str,sizeof(char),32,decInfo->fptr_stego_image);
    decode_lsb_to_size(str,&xsize);
    decInfo->xsize=xsize;
    printf("size of secret file extension is: %d\n",xsize);
    return e_success;
}

Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo)
{  
    int i;
    char xtension[decInfo->xsize];
    for(i=0;i<decInfo->xsize;i++)
    {
    fread(decInfo->image_data,sizeof(char),8,decInfo->fptr_stego_image);
    decode_lsb_to_byte(decInfo->image_data,&xtension[i]);
    }
    xtension[i]=0;
    printf("Extension is %s\n",xtension);  
    return e_success; 
}

Status decode_secret_file_size(int file_size, DecodeInfo *decInfo)
{
    char str[32];
    int xsize=0;
    fread(str,sizeof(char),32,decInfo->fptr_stego_image);
    decode_lsb_to_size(str,&xsize);
    decInfo->size_secret_file=xsize;
    printf("size of secret file is : %d\n",xsize);
    return e_success;
}


Status decode_secret_file_data(DecodeInfo *decInfo)
{
    int i;
    char charactor=0;
    for(i=0;i<decInfo->size_secret_file;i++)
    {
        fread(decInfo->image_data,sizeof(char),8,decInfo->fptr_stego_image);
        decode_lsb_to_byte(decInfo->image_data,&charactor);
        fwrite(&charactor,sizeof(char),1,decInfo->fptr_output_txt);
    }
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo) 
{
    if(open_files_decode(decInfo)==e_success)
    {
            printf("opened files successfully\n");
            //fseek(decInfo->fptr_stego_image,54,SEEK_END);
            if(decode_magic_string(MAGIC_STRING, decInfo)==e_success)
            {
                printf("Magic string success\n");
                if(decode_size(decInfo->fptr_stego_image,decInfo) == e_success)
                {
                    printf("Succefully decoded secret file extension size\n");
                    if(decode_secret_file_extn(decInfo->extn_secret_file ,decInfo)==e_success)
                    {
                        printf("Successfully encoded the secret file extension\n");
                        if(decode_secret_file_size(decInfo->size_secret_file, decInfo)==e_success)
                        {
                            printf("Successfully encoded secret file size\n");
                            if(decode_secret_file_data(decInfo)==e_success)
                            {
                                printf("Successfully encoded secret file data\n"); 
                                fclose(decInfo->fptr_output_txt);
                                fclose(decInfo->fptr_stego_image);
                                return  e_success;
                            } 
                            else
                            {
                                printf("failed encoding secret file data\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Failed decoding secret file extension\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to decode secret file extension\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Failed decoding secret file extension size\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Failed copying magic srtring\n");
                return e_failure;
            }
    }
    else
    {
        printf("Failed opening files\n");
        return e_failure;
    }
}
