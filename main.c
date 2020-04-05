#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long utf32be_to_unicode(unsigned char *symbols) {
    return (symbols[0] << 24) + (symbols[1] << 16) + (symbols[2] << 8) + symbols[3];
}

unsigned long utf32le_to_unicode(unsigned char *symbols) {
    return (symbols[3] << 24) + (symbols[2] << 16) + (symbols[1] << 8) + symbols[0];
}

unsigned long utf16be_to_unicode(unsigned char *symbols, int size) {
    switch (size) {
        case 2:
            return (symbols[0] << 8) + symbols[1];
        case 4:
            return ((((symbols[0] << 8) + symbols[1]) - 0xD800) * 0x0400) +
                   (((symbols[2] << 8) + symbols[3]) - 0xDC00) + 0x10000;
    }
}

unsigned long utf16le_to_unicode(unsigned char *symbols, int size) {
    switch (size) {
        case 2:
            return (symbols[1] << 8) + symbols[0];
        case 4:
            return ((((symbols[1] << 8) + symbols[0]) - 0xD800) * 0x0400) +
                   (((symbols[3] << 8) + symbols[2]) - 0xDC00) + 0x10000;
    }
}

unsigned long utf8_to_unicode(unsigned char *symbols, int size) {
    switch (size) {
        case 1:
            return symbols[0];
        case 2:
            return ((symbols[1] - 0xC0) << 6) + (symbols[0] - 0x80);
        case 3:
            return ((symbols[2] - 0xE0) << 12) + ((symbols[1] - 0x80) << 6) + (symbols[0] - 0x80);
        case 4:
            return ((symbols[3] - 0xF0) << 18) + ((symbols[2] - 0x80) << 12) + ((symbols[1] - 0x80) << 6) +
                   (symbols[0] - 0x80);
    }
}

void unicode_to_utf32be(unsigned char *symbols, unsigned long unicode) {
    symbols[0] = unicode >> 24;
    symbols[1] = (unicode << 8) >> 24;
    symbols[2] = (unicode << 16) >> 24;
    symbols[3] = (unicode << 24) >> 24;
}

void unicode_to_utf32le(unsigned char *symbols, unsigned long unicode) {
    symbols[3] = unicode >> 24;
    symbols[2] = (unicode << 8) >> 24;
    symbols[1] = (unicode << 16) >> 24;
    symbols[0] = (unicode << 24) >> 24;
}

int unicode_to_utf16be(unsigned char *symbols, unsigned long unicode) {
    if (unicode <= 0xFFFF) {
        symbols[0] = unicode >> 8;
        symbols[1] = (unicode << 24) >> 24;
        return 2;
    }
    if (unicode <= 0x10FFFF) {
        symbols[0] = ((unicode - 0x10000) >> 10) + 0xD800 >> 8;
        symbols[1] = ((((unicode - 0x10000) >> 10) + 0xD800) << 24) >> 24;
        symbols[2] = ((((unicode - 0x10000) << 22) >> 22) + 0xDC00) >> 8;
        symbols[3] = (((((unicode - 0x10000) << 22) >> 22) + 0xDC00) << 24) >> 24;
        return 4;
    }
    return -1;
}

int unicode_to_utf16le(unsigned char *symbols, unsigned long unicode) {
    if (unicode <= 0xFFFF) {
        symbols[1] = unicode >> 8;
        symbols[0] = (unicode << 24) >> 24;
        return 2;
    }
    if (unicode <= 0x10FFFF) {
        symbols[3] = ((unicode - 0x10000) >> 10) + 0xD800 >> 8;
        symbols[2] = ((((unicode - 0x10000) >> 10) + 0xD800) << 24) >> 24;
        symbols[1] = ((((unicode - 0x10000) << 22) >> 22) + 0xDC00) >> 8;
        symbols[0] = (((((unicode - 0x10000) << 22) >> 22) + 0xDC00) << 24) >> 24;
        return 4;
    }
    return -1;
}

int unicode_to_utf8(unsigned char *symbols, unsigned long unicode) {
    if (unicode <= 0x007F) {
        symbols[0] = unicode;
        return 1;
    }
    if (unicode <= 0x7FF) {
        symbols[0] = 0xC0 | (unicode >> 6);
        symbols[1] = 0x80 | (unicode & 0x3F);
        return 2;
    }
    if (unicode <= 0xFFFF) {
        symbols[0] = 0xE0 | (unicode >> 12);
        symbols[1] = 0x80 | ((unicode >> 6) & 0x3F);
        symbols[2] = 0x80 | (unicode & 0x3F);
        return 3;
    }
    if (unicode <= 0x10FFFF) {
        symbols[0] = 0xF0 | (unicode >> 18);
        symbols[1] = 0x80 | ((unicode >> 12) & 0x3F);
        symbols[2] = 0x80 | ((unicode >> 6) & 0x3F);
        symbols[3] = 0x80 | (unicode & 0x3F);
        return 4;
    }
    return -1;
}

int utf32be_to_utf8(unsigned char *utf32be, unsigned char *utf8) {
    int res = unicode_to_utf8(utf8, utf32be_to_unicode(utf32be));
    if (res != -1) {
        return res;
    } else {
        return -1;
    }
}

int utf32be_to_utf16be(unsigned char *utf32be, unsigned char *utf16be) {
    int res = unicode_to_utf16be(utf16be, utf32be_to_unicode(utf32be));
    if (res != -1) {
        return res;
    } else {
        return -1;
    }
}

int utf32be_to_utf16le(unsigned char *utf32be, unsigned char *utf16le) {
    int res = unicode_to_utf16le(utf16le, utf32be_to_unicode(utf32be));
    if (res != -1) {
        return res;
    } else {
        return -1;
    }
}

int utf32be_to_utf32le(unsigned char *utf32le, unsigned char *utf32be) {
    unicode_to_utf32le(utf32le, utf32be_to_unicode(utf32be));
    return 4;
}

int utf32le_to_utf32be(unsigned char *utf32le, unsigned char *utf32be) {
    unicode_to_utf32be(utf32be, utf32le_to_unicode(utf32le));
    return 4;
}

int utf32le_to_utf16be(unsigned char *utf32le, unsigned char *utf16be) {
    unsigned char utf32be[4];
    utf32le_to_utf32be(utf32le, utf32be);
    utf32be_to_utf16be(utf32be, utf16be);
}

int utf32le_to_utf16le(unsigned char *utf32le, unsigned char *utf16le) {
    unsigned char utf32be[4];
    utf32le_to_utf32be(utf32le, utf32be);
    utf32be_to_utf16le(utf32be, utf16le);
}

int utf32le_to_utf8(unsigned char *utf32le, unsigned char *utf8) {
    unsigned char utf32be[4];
    utf32le_to_utf32be(utf32le, utf32be);
    utf32be_to_utf8(utf32be, utf8);
}

int encoding_to_itself(unsigned char* in, unsigned char * out, int size){
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    out[3] = in[3];
    return size;
}

int utf16be_to_utf32be(unsigned char *utf32be, unsigned char *utf16be, int size) {
    unicode_to_utf32be(utf32be, utf16be_to_unicode(utf16be, size));
    return 4;
}

int utf16be_to_utf32le(unsigned char *utf32le, unsigned char *utf16be, int size) {
    unsigned char utf32be[4];
    utf16be_to_utf32be(utf32be, utf16be, size);
    utf32be_to_utf32le(utf32le, utf32be);
    return 4;
}

int utf16be_to_utf16le(unsigned char *utf16le, unsigned char *utf16be, int size) {
    unsigned char utf32be[4];
    utf16be_to_utf32be(utf32be, utf16be, size);
    return utf32be_to_utf16le(utf32be, utf16le);
}

int utf16be_to_utf8(unsigned char *utf8, unsigned char *utf16be, int size) {
    unsigned char utf32be[4];
    utf16be_to_utf32be(utf32be, utf16be, size);
    return utf32be_to_utf8(utf32be, utf8);
}

int utf16le_to_utf32be(unsigned char *utf32be, unsigned char *utf16le, int size) {
    unicode_to_utf32be(utf32be, utf16le_to_unicode(utf16le, size));
    return 4;
}

int utf16le_to_utf32le(unsigned char *utf32le, unsigned char *utf16le, int size) {
    unsigned char utf32be[4];
    utf16le_to_utf32be(utf32be, utf16le, size);
    utf32be_to_utf32le(utf32le, utf32be);
    return 4;
}

int utf16le_to_utf16be(unsigned char *utf16be, unsigned char *utf16le, int size) {
    unsigned char utf32be[4];
    utf16le_to_utf32be(utf32be, utf16le, size);
    return utf32be_to_utf16be(utf32be, utf16be);
}

int utf16le_to_utf8(unsigned char *utf8, unsigned char *utf16le, int size) {
    unsigned char utf32be[4];
    utf16le_to_utf32be(utf32be, utf16le, size);
    return utf32be_to_utf8(utf32be, utf8);
}

int utf8_to_utf32be(unsigned char *utf32be, unsigned char *utf8, int size) {
    unicode_to_utf32be(utf32be, utf8_to_unicode(utf8, size));
    return 4;
}

int utf8_to_utf32le(unsigned char *utf32le, unsigned char *utf8, int size) {
    unsigned char utf32be[4];
    utf8_to_utf32be(utf32be, utf8, size);
    utf32be_to_utf32le(utf32le, utf32be);
    return 4;
}

int utf8_to_utf16be(unsigned char *utf16be, unsigned char *utf8, int size) {
    unsigned char utf32be[4];
    utf8_to_utf32be(utf32be, utf8, size);
    return utf32be_to_utf16be(utf32be, utf16be);
}

int utf8_to_utf16le(unsigned char *utf16le, unsigned char *utf8, int size) {
    unsigned char utf32be[4];
    utf8_to_utf32be(utf32be, utf8, size);
    return utf32be_to_utf16le(utf32be, utf16le);
}

int encoding_decider(int mode, int encoding, unsigned char *symbols, unsigned char *result, int size) {
    if (encoding == 0 || encoding == 1) {
        switch (mode) {
            case 0:
                return encoding_to_itself(symbols, result, size);
            case 1:
                return encoding_to_itself(symbols, result, size);
            case 2:
                return utf8_to_utf16le(result, symbols, size);
            case 3:
                return utf8_to_utf16be(result, symbols, size);
            case 4:
                return utf8_to_utf32le(result, symbols, size);
            case 5:
                return utf8_to_utf32be(result, symbols, size);
        }
    }
    if (encoding == 2) {
        switch (mode) {
            case 0:
                return utf16le_to_utf8(result, symbols, size);
            case 1:
                return utf16le_to_utf8(result, symbols, size);
            case 2:
                return encoding_to_itself(symbols, result, size);
            case 3:
                return utf16le_to_utf16be(result, symbols, size);
            case 4:
                return utf16le_to_utf32le(result, symbols, size);
            case 5:
                return utf16le_to_utf32be(result, symbols, size);
        }
    }
    if (encoding == 3) {
        switch (mode) {
            case 0:
                return utf16be_to_utf8(result, symbols, size);
            case 1:
                return utf16be_to_utf8(result, symbols, size);
            case 2:
                return utf16be_to_utf16le(result, symbols, size);
            case 3:
                return encoding_to_itself(symbols, result, size);
            case 4:
                return utf16be_to_utf32le(result, symbols, size);
            case 5:
                return utf16be_to_utf32be(result, symbols, size);
        }
    }
    if (encoding == 4) {
        switch (mode) {
            case 0:
                return utf32le_to_utf8(symbols, result);
            case 1:
                return utf32le_to_utf8(symbols, result);
            case 2:
                return utf32le_to_utf16le(symbols, result);
            case 3:
                return utf32le_to_utf16be(symbols, result);
            case 4:
                return encoding_to_itself(symbols, result, size);
            case 5:
                return utf32le_to_utf32be(symbols, result);
        }
    }
    if (encoding == 5) {
        switch (mode) {
            case 0:
                return utf32be_to_utf8(symbols, result);
            case 1:
                return utf32be_to_utf8(symbols, result);
            case 2:
                return utf32be_to_utf16le(symbols, result);
            case 3:
                return utf32be_to_utf16be(symbols, result);
            case 4:
                return utf32be_to_utf32le(result, symbols);
            case 5:
                return encoding_to_itself(symbols, result, size);
        }
    }
}

void get_mode_BOM(int mode, FILE *out) {
    switch (mode) {
        case 0:
            return;
        case 1:
            fprintf(out, "%c%c%c", 0xEF, 0xBB, 0xBF);
            return;
        case 2:
            fprintf(out, "%c%c", 0xFF, 0xFE);
            return;
        case 3:
            fprintf(out, "%c%c", 0xFE, 0xFF);
            return;
        case 4:
            fprintf(out, "%c%c%c%c", 0xFF, 0xFE, 0x00, 0x00);
            return;
        case 5:
            fprintf(out, "%c%c%c%c", 0x00, 0x00, 0xFE, 0xFF);
            return;
    }
}

int main(int argc, char *argv[]) {
    unsigned char BOM[4];
    FILE *inp = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");
    char *mode = argv[3];
    fscanf(inp, "%c%c%c%c", &BOM[0], &BOM[1], &BOM[2], &BOM[3]);
    get_mode_BOM(*mode - '0', out);
    if (memcmp(BOM, "\x00\x00\xFE\xFF", 4) == 0) {
        // UTF-32 big-endian
        unsigned char ch[4];
        do {
            ch[0] = fgetc(inp);
            if (feof(inp)) {
                break;
            }
            unsigned char result[4];
            ch[1] = fgetc(inp);
            ch[2] = fgetc(inp);
            ch[3] = fgetc(inp);
            int index = encoding_decider(*mode - '0', 5, ch, result, 4);
            if (index == 1) {
                fprintf(out, "%c", result[0]);
            } else if (index == 2) {
                fprintf(out, "%c%c", result[0], result[1]);
            } else if (index == 3) {
                fprintf(out, "%c%c%c", result[0], result[1], result[2]);
            } else if (index == 4) {
                fprintf(out, "%c%c%c%c", result[0], result[1], result[2], result[3]);
            }
        } while (1);
    } else if (memcmp(BOM, "\xFF\xFE\x00\x00", 4) == 0) {
        // UTF-32 little-endian
        unsigned char ch[4];
        do {
            ch[0] = fgetc(inp);
            if (feof(inp)) {
                break;
            }
            unsigned char result[4];
            ch[1] = fgetc(inp);
            ch[2] = fgetc(inp);
            ch[3] = fgetc(inp);
            int index = encoding_decider(*mode - '0', 4, ch, result, 4);
            if (index == 1) {
                fprintf(out, "%c", result[0]);
            } else if (index == 2) {
                fprintf(out, "%c%c", result[0], result[1]);
            } else if (index == 3) {
                fprintf(out, "%c%c%c", result[0], result[1], result[2]);
            } else if (index == 4) {
                fprintf(out, "%c%c%c%c", result[0], result[1], result[2], result[3]);
            }
        } while (1);
    } else if (memcmp(BOM, "\xFE\xFF", 2) == 0) {
        // UTF-16 big-endian
        unsigned char ch[4];
        int index;
        ch[0] = BOM[2];
        ch[1] = BOM[3];
        while (1) {
            unsigned char result[4];
            if (utf16be_to_unicode(ch, 2) <= 0xD7FF) {
                index = encoding_decider(*mode - '0', 3, ch, result, 2);
            } else {
                fscanf(inp, "%c%c", &ch[2], &ch[3]);
                index = encoding_decider(*mode - '0', 3, ch, result, 4);
            }
            if (index == 1) {
                fprintf(out, "%c", result[0]);
            } else if (index == 2) {
                fprintf(out, "%c%c", result[0], result[1]);
            } else if (index == 3) {
                fprintf(out, "%c%c%c", result[0], result[1], result[2]);
            } else if (index == 4) {
                fprintf(out, "%c%c%c%c", result[0], result[1], result[2], result[3]);
            }
            fscanf(inp, "%c%c", &ch[0], &ch[1]);
            if (feof(inp)) {
                break;
            }
            ch[2] = '\0';
            ch[3] = '\0';

        }
    } else if (memcmp(BOM, "\xFF\xFE", 2) == 0) {
        // UTF-16 little-endian
        unsigned char ch[4];
        int index;
        ch[0] = BOM[2];
        ch[1] = BOM[3];
        while (1) {
            unsigned char result[4];
            if (utf16be_to_unicode(ch, 2) <= 0xD7FF) {
                index = encoding_decider(*mode - '0', 2, ch, result, 2);
            } else {
                fscanf(inp, "%c%c", &ch[2], &ch[3]);
                index = encoding_decider(*mode - '0', 2, ch, result, 4);
            }
            if (index == 1) {
                fprintf(out, "%c", result[0]);
            } else if (index == 2) {
                fprintf(out, "%c%c", result[0], result[1]);
            } else if (index == 3) {
                fprintf(out, "%c%c%c", result[0], result[1], result[2]);
            } else if (index == 4) {
                fprintf(out, "%c%c%c%c", result[0], result[1], result[2], result[3]);
            }
            fscanf(inp, "%c%c", &ch[0], &ch[1]);
            if (feof(inp)) {
                break;
            }
            ch[2] = '\0';
            ch[3] = '\0';

        }
    } else if (memcmp(BOM, "\xEF\xBB\xBF", 3) == 0) {
        // UTF-8
        unsigned char ch[4];
        ch[0] = BOM[3];
        int index;
        while (1) {
            unsigned char result[4];
            if (ch[0] <= 0x01111111) {
                index = encoding_decider(*mode - '0', 1, ch, result, 1);
            } else if (ch[0] <= 0x11011111) {
                fscanf(inp, "%c", &ch[1]);
                ch[0] &= 0x00111111;
                ch[1] &= 0x01111111;
                index = encoding_decider(*mode - '0', 1, ch, result, 2);
            } else if (ch[0] <= 0x11101111) {
                fscanf(inp, "%c%c", &ch[1], &ch[2]);
                ch[0] &= 0x00011111;
                ch[1] &= 0x01111111;
                ch[2] &= 0x01111111;
                index = encoding_decider(*mode - '0', 1, ch, result, 3);
            } else if (ch[0] <= 0x11110111) {
                fscanf(inp, "%c%c%c", &ch[1], &ch[2], &ch[3]);
                ch[0] &= 0x00001111;
                ch[1] &= 0x01111111;
                ch[2] &= 0x01111111;
                ch[3] &= 0x01111111;
                index = encoding_decider(*mode - '0', 1, ch, result, 4);
            }
            if (index == 1) {
                fprintf(out, "%c", result[0]);
            } else if (index == 2) {
                fprintf(out, "%c%c", result[0], result[1]);
            } else if (index == 3) {
                fprintf(out, "%c%c%c", result[0], result[1], result[2]);
            } else if (index == 4) {
                fprintf(out, "%c%c%c%c", result[0], result[1], result[2], result[3]);
            }
            fscanf(inp, "%c", &ch[0]);
            if (feof(inp)) {
                break;
            }
            ch[1] = '\0';
            ch[2] = '\0';
            ch[3] = '\0';
        }
    } else {
        rewind(inp);
        unsigned char ch[4];
        int index;
        fscanf(inp, "%c", &ch[0]);
        while (1) {
            unsigned char result[4];
            if (ch[0] <= 0x01111111) {
                index = encoding_decider(*mode - '0', 1, ch, result, 1);
            } else if (ch[0] <= 0x11011111) {
                fscanf(inp, "%c", &ch[1]);
                ch[0] &= 0x00111111;
                ch[1] &= 0x01111111;
                index = encoding_decider(*mode - '0', 1, ch, result, 2);
            } else if (ch[0] <= 0x11101111) {
                fscanf(inp, "%c%c", &ch[1], &ch[2]);
                ch[0] &= 0x00011111;
                ch[1] &= 0x01111111;
                ch[2] &= 0x01111111;
                index = encoding_decider(*mode - '0', 1, ch, result, 3);
            } else if (ch[0] <= 0x11110111) {
                fscanf(inp, "%c%c%c", &ch[1], &ch[2], &ch[3]);
                ch[0] &= 0x00001111;
                ch[1] &= 0x01111111;
                ch[2] &= 0x01111111;
                ch[3] &= 0x01111111;
                index = encoding_decider(*mode - '0', 1, ch, result, 3);
            }
            if (index == 1) {
                fprintf(out, "%c", result[0]);
            } else if (index == 2) {
                fprintf(out, "%c%c", result[0], result[1]);
            } else if (index == 3) {
                fprintf(out, "%c%c%c", result[0], result[1], result[2]);
            } else if (index == 4) {
                fprintf(out, "%c%c%c%c", result[0], result[1], result[2], result[3]);
            }
            fscanf(inp, "%c", &ch[0]);
            if (feof(inp)) {
                break;
            }
            ch[1] = '\0';
            ch[2] = '\0';
            ch[3] = '\0';
        }
    }
    return 0;
}