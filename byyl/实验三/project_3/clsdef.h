#ifndef PROJECTDEF_H
#define PROJECTDEF_H

/* class-code defines */
#define BEGIN  0x01    /* begin           */
#define END    0x02    /* end             */
#define IF     0x03    /* if              */
#define THEN   0x04    /* then            */
#define ELSE   0x05    /* else            */
#define FOR    0x06    /* for             */
#define DO     0x07    /* do              */
#define WHILE  0x08    /* while           */
#define AND    0x09    /* and             */
#define OR     0x0A    /* or              */
#define NOT    0x0B    /* not             */
#define ID     0x0C    /* user-defined id */
#define UINT   0x0D    /* unsigned int    */
#define SSPL   0x0E    /* single splitter */
#define DSPL   0x0F    /* double splitter */
#define LT     0x10    /* lesser than     */
#define GT     0x11    /* greater than    */
#define MH     0x12    /* mao hao  (:)    */
#define XS     0x13    /* xie shu  (/)    */
#define EQ     0x14    /* deng hao (=)    */
#define EOI    0xff    /* end of input    */
/* 保留字 */
struct reserved_t {
    char name[8];//值
    int class_code;//类别码
};
#define RESERVED_COUNT 11
/* 保留字表 */
static const struct reserved_t reserved_table[RESERVED_COUNT] = {
    {"begin", BEGIN},
    {"end",   END},
    {"if",    IF},
    {"then",  THEN},
    {"else",  ELSE},
    {"for",   FOR},
    {"do",    DO},
    {"while", WHILE},
    {"and",   AND},
    {"or",    OR},
    {"not",   NOT}
};

#endif
