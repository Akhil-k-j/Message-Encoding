#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

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
    printf("width = %u\n", width);
    //at this point file pointer is moved to next 4 bytes.
    //seeked to 19th bit.

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity,  3 is used since its RGB color system.
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files_encode(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen"); //function prints an error message to stderr.
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
uint get_file_size(FILE *secret_file)
{
    fseek(secret_file,0,SEEK_END);
    return ftell(secret_file);
}
Status check_capacity(EncodeInfo* encInfo)
{
   encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
   encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
   if(encInfo->image_capacity>(54+(2+4+4+4+encInfo->size_secret_file)*8))
   return e_success;
   else 
   return e_failure;
}
Status copy_bmp_header(FILE *fptr_src_image,FILE*fptr_stego_image)
{
    fseek(fptr_src_image,0,SEEK_SET);
    char header[54];
    fread(header,sizeof(char),54,fptr_src_image);
    fwrite(header,sizeof(char),54,fptr_stego_image);
    return e_success;
}
Status encode_byte_to_lsb(char data,char *image_buffer)
{
    //getting MSB
    uint mask=1<<7;
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=(image_buffer[i]&0xfe)|((data&mask)>>7-i);
        //for gettting preceeding bits.
        mask=mask>>1;                 
    }
    return e_success;
}
Status encode_data_to_image(const char*data,int size,FILE*fptr_src_image,FILE*fptr_stego_image,EncodeInfo *encInfo)
{
    for(int i=0;i<size;i++)
    {
    fread(encInfo->image_data,8,sizeof(char),fptr_src_image);
    encode_byte_to_lsb(data[i],encInfo->image_data);
    fwrite(encInfo->image_data,8,sizeof(char),fptr_stego_image);
    }
    return e_success;
}
Status encode_magic_string(const char*magic_string,EncodeInfo*encInfo)
{
    //for every encoding we will call encode data to image function.
    encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo);
    return e_success;
}
Status encode_size_to_lsb(char *buffer,int size)
{
    unsigned int mask=1<<31;
    for(int i=0;i<32;i++)
    {
    buffer[i]=(buffer[i] & 0xFE) | ((size & mask) >> (31-i));
    mask=mask>>1;
    }
    return e_success ;  
}
Status encode_size(int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char str[32];
    fread(str,sizeof(char),32,fptr_src_image);
    encode_size_to_lsb(str,size);
    fwrite(str,sizeof(char),32,fptr_stego_image);
    return e_success;
}
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    file_extn=".txt";
    encode_data_to_image(file_extn,strlen(file_extn),encInfo-> fptr_src_image,encInfo->fptr_stego_image, encInfo);
    return e_success;
}
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str,sizeof(char),32,encInfo->fptr_src_image);
    encode_size_to_lsb(str,file_size);
    fwrite(str,sizeof(char),32,encInfo->fptr_stego_image);
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    //bringin the file pointer back to first charactor
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        fread(encInfo->image_data,sizeof(char),8,encInfo->fptr_src_image);
        fread(&ch,1,sizeof(char),encInfo->fptr_secret);
        encode_byte_to_lsb(ch,encInfo->image_data);
        fwrite(encInfo->image_data,sizeof(char),8,encInfo->fptr_stego_image);
    }
    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char ch;
    while(fread(&ch,1,1,fptr_src_image)>0)
    {
        fwrite(&ch,1,1,fptr_stego_image);
    }
    fclose(fptr_src_image);
    fclose(fptr_stego_image);
    return e_success;
}
Status do_encoding(EncodeInfo *encInfo) 
{
    if(open_files_encode(encInfo)==e_success)
    {
     printf("opened files successfully\n");
     if(check_capacity(encInfo)==e_success)
     {
        printf("Size okay, started encoding\n");
        if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
        {
            printf("Copied header\n");
            if(encode_magic_string(MAGIC_STRING, encInfo)==e_success)
            {
                printf("Magic string copied\n");
                if(encode_size(strlen(".txt"),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
                {
                    printf("Succefully encoded secret file extension size\n");
                    if(encode_secret_file_extn(encInfo->extn_secret_file ,encInfo)==e_success)
                    {
                        printf("Successfully encoded the secret file extension\n");
                        if(encode_secret_file_size(encInfo->size_secret_file, encInfo)==e_success)
                        {
                            printf("Successfully encoded secret file size\n");
                            if(encode_secret_file_data(encInfo)==e_success)
                            {
                                printf("Successfully encoded secret file data\n"); 
                                if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
                                {
                                    printf("Completed encoding\n");
                                    return e_success;
                                }
                                else
                                {
                                    printf("Failed encoding the remaining data\n");
                                    return e_failure;
                                }
                            } 
                            else
                                printf("failed encoding secret file data\n");
                                return e_failure;
                        }
                        else
                        {
                            printf("Failed encoding secret file extension\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to encode secret file extension\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Failed encoding secret file extension size\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Failed copying magic srtingn\n");
                return e_failure;
            }
        }
        else
        {
            printf("failed copying the header\n");
            return e_failure;
        }
    }
    else
    {
        printf("Can't do ecndoing due to Image size mismatch\n");
        return e_failure;
    }
}
else
{
    printf("Failed opening files\n");
    return e_failure;
}
}