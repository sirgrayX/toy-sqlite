#ifndef TOKENIZER_H
#define TOKENIZER_H

#include<stdbool.h>

typedef enum
{
    TOKEN_SELECT,
    TOKEN_FROM,
    TOKEN_WHERE,
    TOKEN_INSERT,
    TOKEN_INTO,
    TOKEN_VALUES,
    TOKEN_CREATE,
    TOKEN_TABLE,
    TOKEN_INT,
    TOKEN_TEXT,

    TOKEN_STAR,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,

    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,

    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct 
{
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

typedef struct  
{
    const char* start;
    const char* current;
    int line;
} Tokenizer;

void tokenizer_init(Tokenizer* tokenizer, const char* src);
Token tokenizer_next(Tokenizer* tokenizer);
const char* token_type_to_string(TokenType token_type);
void token_print(const Token* token);

#endif