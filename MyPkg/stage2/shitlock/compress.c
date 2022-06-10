#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void read_file(char *name, uint8_t **buffer, long *buffer_length)
{
    FILE *fileptr;

    fileptr = fopen(name, "rb");
    fseek(fileptr, 0, SEEK_END);
    *buffer_length = ftell(fileptr);
    rewind(fileptr);

    *buffer = malloc(*buffer_length * sizeof(**buffer));
    fread(*buffer, *buffer_length, 1, fileptr);

    fclose(fileptr); // Close the file
    //free(buffer);
}

int main()
{
    uint8_t *buffer;
    long buffer_length;
    read_file("stage2.efi", &buffer, &buffer_length);
    printf("buffer_length: %d\n", buffer_length);
    int zeroes = 0;

    int nzeroes = 0;
    for (int i = 0 ; i < buffer_length ; ++i)
        if (buffer[i] == 0)
            nzeroes++;

    uint8_t *zmap = malloc((nzeroes * 2) * sizeof(uint16_t)); // zeroes from / to
    printf("zmap length: %d\n", (nzeroes * 2) * sizeof(uint16_t));
    int zmapi = 0;
    uint8_t *cfile = malloc(buffer_length - nzeroes); // file without zeroes
    printf("cfile length: %d\n", buffer_length - nzeroes);
    int cfilei = 0;


    for (uint16_t i = 0 ; i < buffer_length ; ++i)
    {
        //printf("0x%02x ", buffer[i]);
        if (buffer[i] == 0) {
            zeroes++;
        } else {
            cfile[cfilei++] = buffer[i]; // fill compressed files
            zmap[zmapi++] = i - zeroes;
            zmap[zmapi++] = i;
            //printf("[z=%02x][f=%02x][t=%02x]\n", zeroes, i - zeroes, i);
            zeroes = 0;
        }
    }

    //printf("[z=%02x][f=%02x][t=%02x]\n", zeroes, buffer_length - zeroes, buffer_length);
    zmap[zmapi++] = buffer_length - zeroes;
    zmap[zmapi++] = buffer_length;

    /*
    uint8_t *decompressed = malloc(buffer_length * sizeof(*buffer)); 
    for (int j = 0 ; j < buffer_length ; j++) // traverse all file ()
    {
        if (zmap[zmapi] == j)
        {
            for (int rz = 0 ; rz = zmap[zmapi])
            decompressed[j] = 0;
            zmapi++;
            continue;
        }
    }
    */

    free(zmap);
    free(cfile);
    free(buffer);
}