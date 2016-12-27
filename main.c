#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>

static FT_Library library;
static char fontfile[100] = "./DejaVuSans.ttf";
static int fontsize = 12;

#define CHECK(S) \
{ \
    int ret = S;\
    if (ret != 0) \
    { \
        fprintf(stderr, "Failed at %s:%d with code %#x\n", __FUNCTION__, __LINE__, ret); \
        return -1; \
    } \
}


void show_entry(FT_Face face, uint16_t c)
{
    FT_UInt glyph_index = FT_Get_Char_Index(face, c);

    if (FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT ) ||
        FT_Render_Glyph(face->glyph,  FT_RENDER_MODE_MONO))
    {
        fprintf(stderr, "char '%c' font failed\n", c);
        exit(-1);
    }

    FT_GlyphSlot slot = face->glyph;
    FT_Bitmap *pb = &slot->bitmap;
    unsigned char *pbuf = pb->buffer;

//    for (int row = 0; row < pb->rows; row++)
//    {
//        for (int col = 0; col < pb->width; col++)
//        {
//            if (pbuf[col/8] & (0x80 >> (col % 8)))
//                fprintf(stdout, "*");
//            else
//                fprintf(stdout, " ");
//        }
//        pbuf += pb->pitch;
//        fprintf(stdout, "\n");
//    }
//    fprintf(stdout, "\n");

    fprintf(stdout, "\t{0x%x, %d, %d, %d, %d, %d, %d, %d, \"", c, (uint8_t) (slot->advance.x >> 6), (uint8_t)(slot->advance.y >> 6), slot->bitmap_left, slot->bitmap_top, pb->width, pb->rows, pb->pitch);
    for (int i=0;i<pb->rows * pb->pitch;i++)
    {
        fprintf(stdout, "\\x%x", pbuf[i]);
    }
    fprintf(stdout, "\"},\n");
}

void output_header()
{
    static char *header = "#ifndef _FONT_BITS_H_\n"
                          "#define _FONT_BITS_H_\n"
                          "typedef struct{\n"
                          " uint16_t c;\n"
                          " int8_t adv_x;\n"
                          " int8_t adv_y;\n"
                          " int8_t x;\n"
                          " int8_t y;\n"
                          " uint8_t w;\n"
                          " uint8_t h;\n"
                          " uint8_t pitch;\n"
                          " uint8_t *data;\n"
                          "}font_bits_t;\n"
                          "extern font_bits_t font_bits[];\n"
                          "#define FB_FONT_SIZE %d\n"
                          "/* font file name %s */\n"
                          "#endif\n";
    printf(header, fontsize, fontfile);
}

void usage()
{
    printf("font-bits - font data extraction\n"
           "Usage: font-bits <opts>\n"
           "Options:\n"
           "-h\tthis message\n"
           "-e\toutput header to stdout\n"
           "-f\tfont filename, default %s\n"
           "-s\tfont size, default %d\n"
           ,fontfile, fontsize);
}

int main(int argc, char *argv[])
{
    int c;
    bool header = false;
    bool normal = true;
    while ((c = getopt (argc, argv, "hes:f:")) != -1) {
        switch (c)
        {
        case 'h':
            usage();
            return 0;
        case 'e':
            header = true;
            break;
        case 's':
            fontsize = atoi(optarg);
            break;
        case 'f':
            strncpy(fontfile, optarg, sizeof(fontfile));
            break;
        }
    }

    if (header)
    {
        output_header();
        return 0;
    }

    CHECK(FT_Init_FreeType( &library ));

    FT_Face face; /* handle to face object */
    CHECK(FT_New_Face(library, fontfile, 0, &face));
    CHECK(FT_Set_Pixel_Sizes(face, 0, fontsize));

    printf("font_bits_t font_bits[] = {\n");
    if (normal)
    {
        for (uint16_t c=0x20; c<=0x7e; c++)
            show_entry(face, c);
    }
    else
    {
        //from stdin
        while ((c = getchar()) != EOF)
        {
            show_entry(face, c);
        }
    }
    printf("};\n");

     CHECK(FT_Done_Face(face));

    return 0;
}

