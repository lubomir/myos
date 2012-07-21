#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct initrd_header {
    unsigned char magic;
    char name[64];
    unsigned int offset;
    unsigned int length;
};

int main(int argc, char *argv[])
{
    int nheaders = (argc-1) / 2;
    struct initrd_header headers[64];

    printf("size of header: %lu\n", sizeof(struct initrd_header));
    unsigned int off = sizeof(struct initrd_header) * 64 + sizeof(int);
    int i;

    for (i = 0; i < nheaders; ++i) {
        printf("Writing file %s -> %s at 0x%x\n",
                argv[i*2+1], argv[i*2+2], off);
        strcpy(headers[i].name, argv[i*2+2]);
        headers[i].offset = off;
        FILE *stream = fopen(argv[i*2+1], "r");
        if (!stream) {
            perror("Can not open file");
            return 1;
        }
        fseek(stream, 0, SEEK_END);
        headers[i].length = ftell(stream);
        off += headers[i].length;
        fclose(stream);
        headers[i].magic = 0xBF;
    }

    FILE *wstream = fopen("./initrd.img", "w");
    fwrite(&nheaders, sizeof(int), 1, wstream);
    fwrite(headers, sizeof(struct initrd_header), 64, wstream);

    for (i = 0; i < nheaders; i++) {
        FILE *stream = fopen(argv[i*2+1], "r");
        unsigned char *buf = malloc(headers[i].length);
        fread(buf, 1, headers[i].length, stream);
        fwrite(buf, 1, headers[i].length, wstream);
        fclose(stream);
        free(buf);
    }

    fclose(wstream);

    return 0;
}
