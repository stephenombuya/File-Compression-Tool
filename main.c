#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define CHUNK 16384

// Function prototypes
void compressFile(const char *inputFile, const char *outputFile);
void decompressFile(const char *inputFile, const char *outputFile);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s [compress|decompress] <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "compress") == 0) {
        compressFile(argv[2], argv[3]);
    } else if (strcmp(argv[1], "decompress") == 0) {
        decompressFile(argv[2], argv[3]);
    } else {
        printf("Invalid operation: %s. Use 'compress' or 'decompress'.\n", argv[1]);
        return 1;
    }

    return 0;
}

// Compress a file using zlib
void compressFile(const char *inputFile, const char *outputFile) {
    FILE *in = fopen(inputFile, "rb");
    if (in == NULL) {
        perror("Error opening input file");
        return;
    }

    FILE *out = fopen(outputFile, "wb");
    if (out == NULL) {
        perror("Error opening output file");
        fclose(in);
        return;
    }

    z_stream strm = {0};
    if (deflateInit(&strm, Z_BEST_COMPRESSION) != Z_OK) {
        fprintf(stderr, "Error initializing zlib compression.\n");
        fclose(in);
        fclose(out);
        return;
    }

    unsigned char inBuf[CHUNK];
    unsigned char outBuf[CHUNK];
    int flush;

    do {
        strm.avail_in = fread(inBuf, 1, CHUNK, in);
        if (ferror(in)) {
            fprintf(stderr, "Error reading input file.\n");
            deflateEnd(&strm);
            fclose(in);
            fclose(out);
            return;
        }
        flush = feof(in) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = inBuf;

        do {
            strm.avail_out = CHUNK;
            strm.next_out = outBuf;
            deflate(&strm, flush);
            fwrite(outBuf, 1, CHUNK - strm.avail_out, out);
        } while (strm.avail_out == 0);

    } while (flush != Z_FINISH);

    deflateEnd(&strm);
    fclose(in);
    fclose(out);
    printf("File compressed successfully.\n");
}

// Decompress a file using zlib
void decompressFile(const char *inputFile, const char *outputFile) {
    FILE *in = fopen(inputFile, "rb");
    if (in == NULL) {
        perror("Error opening input file");
        return;
    }

    FILE *out = fopen(outputFile, "wb");
    if (out == NULL) {
        perror("Error opening output file");
        fclose(in);
        return;
    }

    z_stream strm = {0};
    if (inflateInit(&strm) != Z_OK) {
        fprintf(stderr, "Error initializing zlib decompression.\n");
        fclose(in);
        fclose(out);
        return;
    }

    unsigned char inBuf[CHUNK];
    unsigned char outBuf[CHUNK];

    do {
        strm.avail_in = fread(inBuf, 1, CHUNK, in);
        if (ferror(in)) {
            fprintf(stderr, "Error reading input file.\n");
            inflateEnd(&strm);
            fclose(in);
            fclose(out);
            return;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = inBuf;

        do {
            strm.avail_out = CHUNK;
            strm.next_out = outBuf;
            inflate(&strm, Z_NO_FLUSH);
            fwrite(outBuf, 1, CHUNK - strm.avail_out, out);
        } while (strm.avail_out == 0);

    } while (!feof(in));

    inflateEnd(&strm);
    fclose(in);
    fclose(out);
    printf("File decompressed successfully.\n");
}
