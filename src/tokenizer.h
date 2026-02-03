#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdbool.h>

// типы токенов
typedef enum {
    // ключевые слова
    TOKEN_SELECT, TOKEN_FROM, TOKEN_WHERE, TOKEN_INSERT, TOKEN_INTO,
    TOKEN_VALUES, TOKEN_CREATE, TOKEN_TABLE, TOKEN_INT, TOKEN_TEXT,
    TOKEN_DROP, TOKEN_DELETE, TOKEN_UPDATE, TOKEN_SET,
    
    // операторы
    TOKEN_STAR, TOKEN_COMMA, TOKEN_SEMICOLON, TOKEN_DOT,
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_LESS, TOKEN_GREATER,
    TOKEN_LESS_EQUAL, TOKEN_GREATER_EQUAL, TOKEN_PLUS, TOKEN_MINUS,
    TOKEN_SLASH, TOKEN_PERCENT, TOKEN_BETWEEN, TOKEN_LIKE, TOKEN_IN,
    TOKEN_LIMIT, TOKEN_ORDER_BY,
    
    // логические операторы
    TOKEN_AND, TOKEN_OR, TOKEN_NOT,
    
    // литералы
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER, TOKEN_NULL,
    
    // специальные
    TOKEN_EOF, TOKEN_ERROR, TOKEN_UNKNOWN
} TokenType;

// структура токена
typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
    int column;
} Token;

// контекст токенизатора
typedef struct {
    const char* start;  
    const char* current;    
    const char* line_start; 
    int line;           
} Tokenizer;

void tokenizer_init(Tokenizer* t, const char* source);
Token tokenizer_next(Tokenizer* t);
Token tokenizer_peek(Tokenizer* t);  // посмотреть следующий токен без потребления
const char* token_type_to_string(TokenType type);
void token_print(const Token* token);

bool token_is_keyword(const Token* token);
bool token_is_literal(const Token* token);
bool token_is_operator(const Token* token);
bool token_match(const Token* token, TokenType type);
bool token_text_equals(const Token* token, const char* text);

#endif