#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <vector>

#include "spm.h"

using namespace std;

#ifdef _WIN32
#define PRIx64 "llx"
#define PRIx8 "hhx"
#endif

#define ERROR(s, ...) fprintf(stderr, s, __VA_ARGS__)

int getfilesize(FILE *f) {
    int savepos = ftell(f);
    int ret;
    fseek(f, 0, SEEK_END);
    ret = ftell(f);
    fseek(f, savepos, SEEK_SET);
    return ret;
}

void loadfile(FILE *f, void *out) {
    int len = getfilesize(f);
    fread(out, 1, len, f);
    return;
}

void printhelp() {
    printf("gtex0 [spmfile] [outfile]\n");
    printf("   Extracts tex0 from an SPM model.\n");
}

bool streq(const char *s1, const char *s2) {
    if (0 == strcasecmp(s1, s2)) {
        return true;
    }
    return false;
}

tex0pack_t tex0listfromspm(const void *spm, int spmlen) {
    int npoly = spm::getpolygoncount(spm, spmlen);  // get the plygons from the spm

    spm::polygonheader_t *poly;          // get array of polygons
    vector<tex0_data_t> tex0list;        // vector of informations from tex0 from spm
    vector<u8> formats;

    for (int i = 0; i < npoly; i += 1) {  // for each poly
        if (spm::getpolygonbyindex(spm, spmlen, i, &poly)) {  // if the poly is sucessfully output
            u8 fmt = poly->format >> 24;  // get format but bitshift right 24 bits
            if (fmt != 0x20) {            // TODO: research format 0x20
                tex0_data_t tex0v = spm::tex0frompolygon(poly);

                // Check duplicate
                bool is_duplicate = false;
                for (const auto& existing : tex0list) {
                    if (existing.identifier == tex0v.identifier) {
                        // Check if CLAMP mode is the same
                        bool existing_regW = ((existing.clamp >> 0) & 0x3) >= 2;
                        bool existing_regH = ((existing.clamp >> 2) & 0x3) >= 2;

                        bool current_regW = ((tex0v.clamp >> 0) & 0x3) >= 2;
                        bool current_regH = ((tex0v.clamp >> 2) & 0x3) >= 2;

                        if (existing_regW == current_regW && existing_regH == current_regH) {
                            is_duplicate = true; // can consider as a duplicate
                            break;
                        }
                    }
                }

                if (!is_duplicate) {
                    tex0list.push_back(tex0v);
                    formats.push_back(fmt);

                    printf("TEX0: %016" PRIx64 "\n", tex0v);
                    printf("FORMAT: %02" PRIx8 "\n\n", fmt);
                }
            }
        }
    }

    tex0pack_t pack;
    pack.format = formats;
    pack.texdata = tex0list;
    return pack;
}

void gtex0(FILE *spmfile, FILE *outfile, FILE *fmtfile) {
    int spmlen = getfilesize(spmfile);
    void *spm = malloc(spmlen);
    fread(spm, 1, spmlen, spmfile);

    tex0pack_t pack = tex0listfromspm(spm, spmlen);

    for (size_t i = 0; i < pack.texdata.size(); i++) {
        const auto& d = pack.texdata[i];
        const auto& dfmt = pack.format[i];

        fprintf(outfile, "--- Entry [%zu] ---\n", i);
        fprintf(outfile, "ID:    %016" PRIx64 "\n", d.identifier);
        fprintf(outfile, "UNK:   %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n",
                d.unk[0], d.unk[1], d.unk[2]);
        fprintf(outfile, "CLAMP: %016" PRIx64 "\n", d.clamp);
        fprintf(outfile, "UNK2:  %016" PRIx64 "\n", d.reserved);
        fprintf(outfile, "FMT:   %02" PRIx8 "\n\n", dfmt);
    }

    printf("Found %d tex0\n", int(pack.texdata.size()));

    return;
}

#define CMD(a) if (true == streq(#a, args[1]))

int main(int argc, char *args[]) {
    if (argc <= 1) {
        printhelp();
    } else {
        CMD(gtex0) {
            if (argc <= 3) {
                printf("gtex0 <spmfile> <outfile>\n");
                return 1;
            } else {
                FILE *spmfile = fopen(args[2], "rb");
                if (NULL == spmfile) {
                    ERROR("Couldn't open SPM file %s\n", args[2]);
                    return 1;
                }
                FILE *outfile = fopen(args[3], "w");
                if (NULL == outfile) {
                    ERROR("Couldn't open output file %s\n", args[3]);
                    return 1;
                }
                char f[strlen(args[3]) + 3];
                sprintf(f, "%s.fmt", args[3]);
                FILE *fmtfile = fopen(f, "w");
                if (NULL == fmtfile) {
                    ERROR("Couldn't open format output file %s\n", f);
                    return 1;
                }                                //*/
                gtex0(spmfile, outfile, fmtfile  //*/
                      );
                fclose(spmfile);
                fclose(outfile);
                fclose(fmtfile);
            }
        }
        else {
            ERROR("pwf2spm:  Unknown command %s\n", args[1]);
            return 1;
        }
    }
    return 0;
}
