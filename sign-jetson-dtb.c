#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <openssl/cmac.h>

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("usage: sign infile outfile\n");
        return -1;
    }
    char *in_file_name = argv[1];
    FILE *in_fp = fopen(in_file_name, "rb");
    if (!in_fp) {
        printf("can't open %s: for read %s\n", in_file_name, strerror(errno));
        return -1;
    }
    fseek(in_fp, 0, SEEK_END);
    unsigned padded_len = (ftell(in_fp) + 15) / 16 * 16;
    rewind(in_fp);
    char *out_file_name = argv[2];
    FILE *out_fp = fopen(out_file_name, "wb");
    if (!out_fp) {
        printf("can't open %s for write: %s\n", out_file_name, strerror(errno));
        return -1;
    }
    fseek(out_fp, 384, SEEK_SET);

    CMAC_CTX *ctx = CMAC_CTX_new();
    char key[16];
    memset(key, 0, 16); //key for empty device
    CMAC_Init(ctx, key, 16, EVP_aes_128_cbc(), NULL);
    struct signed_hdr { // little endian!
        uint32_t f0;
        uint32_t f1;
        uint64_t f2;
    }__attribute__((packed)) hdr1;
    hdr1.f0 = 0;
    hdr1.f1 = 5;
    hdr1.f2 = padded_len;
    CMAC_Update(ctx, &hdr1, sizeof(hdr1));
    fwrite(&hdr1, sizeof(hdr1), 1, out_fp);
    while (!feof(in_fp)) {
        char buf[16];
        int ret = fread(buf, 1, 16, in_fp);
        if (ret <= 0)
            break;
        if (ret < 16) {
            memset(buf + ret, 0, 16 - ret);
            buf[ret] = 0x80; // padding start
        }
        CMAC_Update(ctx, buf, 16); // padded to 16 bytes
        fwrite(buf, 16, 1, out_fp);
    }
    fclose(in_fp);
    size_t siglen;
    unsigned char sig[16];
    CMAC_Final(ctx, sig, &siglen); // siglen=16
    CMAC_CTX_free(ctx);

    rewind(out_fp);
    char unsigned_hdr[8] = {'G', 'S', 'H', 'V', 0, 0, 0, 0};
    fwrite(unsigned_hdr, 1, 8, out_fp);
    fwrite(sig, siglen, 1, out_fp);
    char zeros[384 - 8 - siglen];
    memset(zeros, 0, sizeof(zeros));
    fwrite(zeros, sizeof(zeros), 1, out_fp);
    fclose(out_fp);

    return 0;
}
