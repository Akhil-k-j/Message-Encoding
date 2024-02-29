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
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];  // 1*8

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];  // 4
    char secret_data[MAX_SECRET_BUF_SIZE];   //1
    long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;

typedef struct _DecodeInfo
{
    /* Secret File Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;
    char image_data[MAX_IMAGE_BUF_SIZE];
    int xsize;

    /*Output.txt */
    char *output_txt_fname;
    FILE *fptr_output_txt;

} DecodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);
/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);
/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_encode(EncodeInfo *encInfo);
/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);
/* decoding Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);
/* Decode secret file extenstion */
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo);

/* Encode secret file size */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo);
/* Decode secret file size */
Status decode_secret_file_size(int file_size, DecodeInfo *decInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);
/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image,EncodeInfo*encInfo);
/* Decode function, which does the real Decoding */
Status decode_image_to_data(int size, FILE *fptr_stego_image, FILE *fptr_output_txt,DecodeInfo*decInfo);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);
/* Decode a lsb into BYte of text data*/
Status decode_lsb_to_byte(char *image_buffer,char *data);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

/*To encode the secret file extension size*/
Status encode_size(int size, FILE* fptr_src_image, FILE *fptr_dest_image);
/*To decode the secret file extension size*/
Status decode_size(FILE* fptr_stego_image,DecodeInfo*decInfo);

/*Encode size to LSB -- int*/
Status encode_size_to_lsb(char *buffer,int size);
/*decode size to LSB -- int*/
Status decode_lsb_to_size(char *imagebuffer,int *data);

#endif