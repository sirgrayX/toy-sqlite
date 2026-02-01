#include "tokenizer.h"
#include <string.h>
#include <stdio.h>

void tokenizer_init(Tokenizer* tokenizer, const char* src)
{
    tokenizer->start = src;
    tokenizer->current = src;
    tokenizer->line = 1;
}

static char peek(const Tokenizer* t)
{
    return *t->current;
}

static char advance(Tokenizer* t)
{
    t->current++;
    return t->current[-1];
}

static bool is_at_end(Tokenizer* t)
{
    return *t->current == '\0';
}

static void skip_witespaces(Tokenizer* t)
{
    while (true)
    {
        char c = peek(t);
        switch(c)
        {
            case ' ':
            case '\t':
            case '\r':
                advance(t);
                break;
            case '\n':
                t->line++;
                advance(t);
                break;
            default:
                return;
        }
    }
}

Token tokenizer_next(Tokenizer* t)
{
    //char c;

    skip_witespaces(t);

    if (is_at_end(t))
    {
        printf("Reached if\n");
        Token token = {TOKEN_EOF, t->current, 0, t->line};
        return token;
    }
    Token token = {TOKEN_EOF, t->current, 0, t->line};
    return token;
    // c = peek(t);
    
}

const char* token_type_to_string(TokenType token_type)
{
    switch(token_type)
    {
        case TOKEN_SELECT: return "SELECT";
        case TOKEN_FROM: return "FROM";
        case TOKEN_WHERE: return "WHERE";
        case TOKEN_INSERT: return "INSERT";
        case TOKEN_INTO: return "INTO";
        case TOKEN_CREATE: return "CREATE";
        case TOKEN_TABLE: return "TABLE";
        case TOKEN_INT: return "INT";
        case TOKEN_TEXT: return "TEXT";
        case TOKEN_STAR: return "*";
        case TOKEN_COMMA: return ",";
        case TOKEN_SEMICOLON: return ";";
        case TOKEN_LPAREN: return "(";
        case TOKEN_RPAREN: return ")";
        case TOKEN_EQUAL: return "=";
        case TOKEN_NOT_EQUAL: return "<>";
        case TOKEN_LESS: return "<";
        case TOKEN_GREATER: return ">";
        case TOKEN_LESS_EQUAL: return "<=";
        case TOKEN_GREATER_EQUAL: return ">=";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void token_print(const Token* token)
{
    printf("[%s: '%.*s']",
            token_type_to_string(token->type),
            token->length,
            token->start);
}