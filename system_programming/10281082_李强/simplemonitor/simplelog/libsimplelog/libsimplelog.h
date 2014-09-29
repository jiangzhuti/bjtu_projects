#ifndef LIBSIMPLELOG_H
#define LIBSIMPLELOG_H

//unit type defines
#define INT_TYPE    (unsigned char)1
#define UINT_TYPE   (unsigned char)2
#define LONG_TYPE   (unsigned char)3
#define ULONG_TYPE  (unsigned char)4
#define CHAR_TYPE   (unsigned char)5
#define FLOAT_TYPE  (unsigned char)6
#define STRING_TYPE (unsigned char)7

//unit struct
struct log_unit {
    unsigned char unit_type;
    unsigned char unit_len;
    union {
        int int_unit;
        unsigned int uint_unit;
        long long_unit;
        unsigned long ulong_unit;
        char char_unit;
        float float_unit;
        char string_unit[128];
    } unit;
};

void log_add_unit(unsigned char utype, unsigned char ulen, void *ubuf);
void log_write(const char *logfile);
void log_view(const char *begin_time, const char *end_time, const char *logfile);

#endif // LIBSIMPLELOG_H
