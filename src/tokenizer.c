#include "tokenizer.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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

static bool is_at_end(Tokenizer* t)
{
    return *t->current == '\0';
}

static char peek_next(Tokenizer* t)
{
    if (is_at_end(t)) return '\0';
    return t->current[1];
}

static char advance(Tokenizer* t)
{
    t->current++;
    return t->current[-1];
}

static bool match(Tokenizer* t, char expected) {
    if (is_at_end(t)) return false;
    if (*t->current != expected) return false;
    t->current++;
    return true;
}

static void skip_single_comment(Tokenizer* t)
{
    while(peek(t) != '\n' && !is_at_end(t)) 
        {advance(t);}
}

static void skip_multiline_comment(Tokenizer* t)
{
    advance(t);
    advance(t);
    while(!(peek(t) == '*' && peek_next(t) == '/') && !is_at_end(t))
    {
        if (peek(t) == '\n') t->line++;
        advance(t);
    }

    if (peek(t) == '*' && peek_next(t) == '/')
    {
        advance(t);
        advance(t);
    }
}

static void skip_witespaces_and_comments(Tokenizer* t)
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
            case '-':
                if (peek_next(t) == '-')
                {
                    skip_single_comment(t);
                }
                else
                {
                    return;
                }
                break;
            case '/':
                if (peek_next(t) == '*')
                {
                    skip_multiline_comment(t);
                }
                else return;
                break;
            default:
                return;
        }
    }
}


static Token number_literal(Tokenizer* t)
{
    Token token;
    token.start = t->current;
    token.line = t->line;
    token.type = TOKEN_NUMBER;

    advance(t);
    while(isdigit(peek(t)))
    {
        advance(t);
    }

    if (peek(t) == '.' && isdigit(peek_next(t)))
    {
        advance(t);
        while(isdigit(peek(t)))
        {
            advance(t);
        }
    }

    token.length = (int)(t->current - token.start);
    return token;
}



static Token identifier(Tokenizer* t)
{
    Token token;
    token.start = t->current;
    token.line = t->line;

    while(isalnum(peek(t)) || peek(t) == '_') { advance(t); }
    token.length = (int)(t->current - token.start);

    const char* text = token.start;
    if (token.length == 6 && memcmp(text, "SELECT", 6) == 0) {
        token.type = TOKEN_SELECT;
    } else if (token.length == 4 && memcmp(text, "FROM", 4) == 0) {
        token.type = TOKEN_FROM;
    } else if (token.length == 5 && memcmp(text, "WHERE", 5) == 0) {
        token.type = TOKEN_WHERE;
    } else if (token.length == 6 && memcmp(text, "INSERT", 6) == 0) {
        token.type = TOKEN_INSERT;
    } else if (token.length == 4 && memcmp(text, "INTO", 4) == 0) {
        token.type = TOKEN_INTO;
    } else if (token.length == 6 && memcmp(text, "CREATE", 6) == 0) {
        token.type = TOKEN_CREATE;
    } else if (token.length == 5 && memcmp(text, "TABLE", 5) == 0) {
        token.type = TOKEN_TABLE;
    } else if (token.length == 3 && memcmp(text, "INT", 3) == 0) {
        token.type = TOKEN_INT;
    } else if (token.length == 4 && memcmp(text, "TEXT", 4) == 0) {
        token.type = TOKEN_TEXT;
    } else {
        token.type = TOKEN_IDENTIFIER;
    }

    return token;
}

static Token string_literal(Tokenizer* t)
{
    Token token;
    token.start = t->current;
    token.line = t->line;

    advance(t);

    while(peek(t) != '"' && !is_at_end(t))
    {
        if (peek(t) == '\n') t->line++;
        advance(t);
    }

    if (is_at_end(t))
    {
        token.type = TOKEN_ERROR;
        token.length = 0;
        return token;
    }

    advance(t);
    token.type = TOKEN_STRING;
    token.length = (int)(t->current - token.start);

    return token;
}



Token tokenizer_next(Tokenizer* t)
{
    char c;

    skip_witespaces_and_comments(t);

    if (is_at_end(t))
    {
        Token token = {TOKEN_EOF, t->current, 0, t->line};
        return token;
    }

    c = peek(t);

    if (isalpha(c) || c == '_') 
    {
        return identifier(t);
    }

    if (isdigit(c))
    {
        return number_literal(t);
    }

    switch(c)
    {
        case '*': advance(t); return (Token){TOKEN_STAR, t->current-1, 1, t->line};
        case ',': advance(t); return (Token){TOKEN_COMMA, t->current-1, 1, t->line};
        case ';': advance(t); return (Token){TOKEN_SEMICOLON, t->current-1, 1, t->line};
        case '(': advance(t); return (Token){TOKEN_LPAREN, t->current-1, 1, t->line};
        case ')': advance(t); return (Token){TOKEN_RPAREN, t->current-1, 1, t->line};
        case '=': advance(t); return (Token){TOKEN_EQUAL, t->current-1, 1, t->line};
        case '"': advance(t); return string_literal(t);
        case '>':
            advance(t);
            if (match(t, '='))
            {
                return (Token){
                    TOKEN_GREATER_EQUAL, 
                    t->current-2, 
                    2, 
                    t->line
                };
            }
            return (Token){
                TOKEN_GREATER,
                t->current-1,
                1,
                t->line
            };
        case '<':
            advance(t);
            if (match(t, '=')) {
                return (Token){
                    TOKEN_LESS_EQUAL,
                    t->current-2, 
                    2, 
                    t->line};
            } else if (match(t, '>')) {
                return (Token){
                    TOKEN_NOT_EQUAL, 
                    t->current-2, 
                    2, 
                    t->line
                };
            }
            return (Token){
                TOKEN_LESS, 
                t->current-1, 
                1, 
                t->line
            };
    }
    advance(t);
    return (Token){
        TOKEN_ERROR,
        t->current-1,
        1, t->line
    };
    
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