/*
 * gen-keymap.c -- generate binary keymap file from text file description.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char keymap[256];
char buffer[32];

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage %s: INFILE OUTFILE\n", argv[0]);
        return 1;
    }

    memset(keymap, 0, sizeof (keymap));
    FILE *infile = fopen(argv[1], "r");
    if (!infile) {
        perror("Can not open input file");
        return 1;
    }

    /* For each line */
    int no = 0;
    while (fgets(buffer, 32, infile)) {
        if (*buffer == '\n')
            continue;
        no++;
        int idx, val;
        char *end;
        idx = strtol(buffer, &end, 10);
        while (isspace(*end)) end++;
        switch (*end++) {
        case '=': /* Read character itself. */
            while (isspace(*end)) end++;
            val = *end;
            break;
        case ':': /* Read ascii value. */
            val = strtol(end, NULL, 10);
            break;
        default:
            fprintf(stderr, "Error on line %d: unrecognized operator\n", no);
            return 1;
        }
        keymap[idx] = val;
    }

    fclose(infile);

    FILE *outfile = fopen(argv[2], "w");
    if (!outfile) {
        perror("Can not open output file");
        return 1;
    }
    fwrite(keymap, 1, 256, outfile);
    fclose(outfile);

    return 0;
}
