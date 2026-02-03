#include "tokenizer.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================s
static bool is_at_end(const Tokenizer* t) 
{
    return *t->current == '\0';
}

static char advance(Tokenizer* t)
{
    t->current++;
    return t->current[-1];
}

static char peek(const Tokenizer* t)
{
    return *t->current;
}

static char peek_next(const Tokenizer* t) 
{
    if (is_at_end(t)) return '\0';
    return t->current[1];
}

static bool match(Tokenizer* t, char expected) 
{
    if (is_at_end(t) || *t->current != expected) return false;
    t->current++;
    return true;
}

static void skip_whitespace(Tokenizer* t) 
{
    while (true) {
        char c = peek(t);
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
                advance(t);
                break;
            case '\n':
                t->line++;
                t->line_start = t->current + 1;
                advance(t);
                break;
            default:
                return;
        }
    }
}

// ==================== ОБРАБОТКА КОММЕНТАРИЕВ ====================
static void skip_single_line_comment(Tokenizer* t) {
    while (!is_at_end(t) && peek(t) != '\n') {
        advance(t);
    }
}

static void skip_multiline_comment(Tokenizer* t)
{
    advance(t);
    advance(t);

    int depth = 1;
    while(depth > 0 && !is_at_end(t))
    {
        if (peek(t) == '/' && peek_next(t) == '*')
        {
            depth++;
            advance(t);
            advance(t);
        }
        else if (peek(t) == '*' && peek_next(t) == '/')
        {
            depth--;
            advance(t);
            advance(t);
        }
        else 
        {
            if (peek(t) == '\n') t->line++;
            advance(t);
        }
    }
}

// ==================== ЧТЕНИЕ РАЗНЫХ ТИПОВ ТОКЕНОВ ====================

static Token identifier(Tokenizer* t)
{
    Token token;
    token.start = t->current;
    token.line = t->line;
    token.column = (int)(t->current - t->line_start) + 1;

    while(isalnum(peek(t)) || peek(t) == '_'|| peek(t) == '$') { advance(t); }
    token.length = (int)(t->current - token.start);

    static const struct {
        const char* keyword;
        int length;
        TokenType type;
    } keywords[] = {
        {"SELECT", 6, TOKEN_SELECT}, {"FROM", 4, TOKEN_FROM},
        {"WHERE", 5, TOKEN_WHERE}, {"INSERT", 6, TOKEN_INSERT},
        {"INTO", 4, TOKEN_INTO}, {"VALUES", 6, TOKEN_VALUES},
        {"CREATE", 6, TOKEN_CREATE}, {"TABLE", 5, TOKEN_TABLE},
        {"DROP", 4, TOKEN_DROP}, {"DELETE", 6, TOKEN_DELETE},
        {"UPDATE", 6, TOKEN_UPDATE}, {"SET", 3, TOKEN_SET},
        {"INT", 3, TOKEN_INT}, {"TEXT", 4, TOKEN_TEXT},
        {"AND", 3, TOKEN_AND}, {"OR", 2, TOKEN_OR},
        {"NOT", 3, TOKEN_NOT}, {"NULL", 4, TOKEN_NULL},
        {"BETWEEN", 7, TOKEN_BETWEEN}, {"LIKE", 4, TOKEN_LIKE}, 
        {"IN", 2, TOKEN_IN}, {"LIMIT", 5, TOKEN_LIMIT},
        {"ORDER BY", 8, TOKEN_ORDER_BY}
    };

    token.type = TOKEN_IDENTIFIER;
    for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (token.length == keywords[i].length &&
            memcmp(token.start, keywords[i].keyword, token.length) == 0) {
            token.type = keywords[i].type;
            break;
        }
    }

    return token;
}

static Token number_literal(Tokenizer* t)
{
    Token token;
    token.start = t->current;
    token.line = t->line;
    token.column = (int)(t->current - t->line_start) + 1;
    token.type = TOKEN_NUMBER;

    while(isdigit(peek(t)))
    {
        advance(t);
    }

    if (peek(t) == '.' && isdigit(peek_next(t)))
    {
        advance(t);
        while(isdigit(peek(t))) { advance(t); }
    }

    if (peek(t) == 'e' || peek(t) == 'E') 
    {
        advance(t);
        if (peek(t) == '+' || peek(t) == '-') { advance(t); }
        if (!isdigit(peek(t))) 
        {
            token.type = TOKEN_ERROR;
            token.length = (int)(t->current - token.start);
            return token;
        }
        while (isdigit(peek(t))) { advance(t); }
    }

    token.length = (int)(t->current - token.start);
    return token;
}


static Token string_literal(Tokenizer* t, char quote_char) {
    Token token;
    token.start = t->current; 
    token.line = t->line;
    token.column = (int)(t->current - t->line_start) + 1;
    
    advance(t);
    
    while (!is_at_end(t) && peek(t) != quote_char) 
    {
        if (peek(t) == '\n') 
        {
            t->line++;
            t->line_start = t->current + 1;
        }
        
        // обработка escape-последовательностей
        if (peek(t) == '\\') 
        {
            advance(t);  
            if (is_at_end(t)) break;
        }
        
        advance(t);
    }
    
    if (is_at_end(t)) 
    {
        token.type = TOKEN_ERROR;
        token.length = (int)(t->current - token.start);
        return token;
    }
    
    advance(t); 
    
    token.type = TOKEN_STRING;
    token.length = (int)(t->current - token.start);
    return token;
}


// ==================== ОСНОВНАЯ ФУНКЦИЯ ТОКЕНИЗАЦИИ ====================

Token tokenizer_next(Tokenizer* t) 
{
    while (true) 
    {
        skip_whitespace(t);
        
        if (is_at_end(t)) 
        {
            return (Token){
                TOKEN_EOF, 
                t->current, 
                0, 
                t->line, 
                (int)(t->current - t->line_start) + 1};
        }
        
        if (peek(t) == '-' && peek_next(t) == '-') 
        {
            skip_single_line_comment(t);
            continue;
        }
        
        if (peek(t) == '/' && peek_next(t) == '*') 
        {
            skip_multiline_comment(t);
            continue;
        }
        
        break; 
    }

    int column = (int)(t->current - t->line_start) + 1;
    
    char c = peek(t);
    
    if (isalpha(c) || c == '_' || c == '$') 
    {
        Token token = identifier(t);
        token.column = column;
        return token;
    }
    
    if (isdigit(c)) 
    {
        Token token = number_literal(t);
        token.column = column;
        return token;
    }
    
    if (c == '"' || c == '\'')
    {
        Token token = string_literal(t, c);
        token.column = column;
        return token;
    }
    
    switch (c) 
    {
        case '*': advance(t); return (Token){TOKEN_STAR, t->current-1, 1, t->line, column};
        case ',': advance(t); return (Token){TOKEN_COMMA, t->current-1, 1, t->line, column};
        case ';': advance(t); return (Token){TOKEN_SEMICOLON, t->current-1, 1, t->line, column};
        case '.': advance(t); return (Token){TOKEN_DOT, t->current-1, 1, t->line, column};
        case '(': advance(t); return (Token){TOKEN_LPAREN, t->current-1, 1, t->line, column};
        case ')': advance(t); return (Token){TOKEN_RPAREN, t->current-1, 1, t->line, column};
        case '{': advance(t); return (Token){TOKEN_LBRACE, t->current-1, 1, t->line, column};
        case '}': advance(t); return (Token){TOKEN_RBRACE, t->current-1, 1, t->line, column};
        case '+': advance(t); return (Token){TOKEN_PLUS, t->current-1, 1, t->line, column};
        case '%': advance(t); return (Token){TOKEN_PERCENT, t->current-1, 1, t->line, column};
        
        case '=':
            advance(t);
            return (Token){TOKEN_EQUAL, t->current-1, 1, t->line, column};
            
        case '!':
            advance(t);
            if (match(t, '=')) 
            {
                return (Token){TOKEN_NOT_EQUAL, t->current-2, 2, t->line, column};
            }
            return (Token){TOKEN_ERROR, t->current-1, 1, t->line, column};
            
        case '<':
            advance(t);
            if (match(t, '=')) 
            {
                return (Token){TOKEN_LESS_EQUAL, t->current-2, 2, t->line, column};
            } 
            else if (match(t, '>')) 
            {
                return (Token){TOKEN_NOT_EQUAL, t->current-2, 2, t->line, column};
            }
            return (Token){TOKEN_LESS, t->current-1, 1, t->line, column};
            
        case '>':
            advance(t);
            if (match(t, '=')) 
            {
                return (Token){TOKEN_GREATER_EQUAL, t->current-2, 2, t->line, column};
            }
            return (Token){TOKEN_GREATER, t->current-1, 1, t->line, column};
            
        case '-':
            advance(t);
            return (Token){TOKEN_MINUS, t->current-1, 1, t->line, column};
            
        case '/':
            advance(t);
            return (Token){TOKEN_SLASH, t->current-1, 1, t->line, column};
    }
    
    advance(t);
    return (Token){TOKEN_UNKNOWN, t->current-1, 1, t->line, column};
}


void tokenizer_init(Tokenizer* t, const char* source) 
{
    t->start = source;
    t->current = source;
    t->line_start = source;
    t->line = 1;
}

Token tokenizer_peek(Tokenizer* t) 
{
    Tokenizer copy = *t;  
    return tokenizer_next(&copy);
}

const char* token_type_to_string(TokenType type) 
{
    switch (type) 
    {
        case TOKEN_SELECT: return "SELECT";
        case TOKEN_FROM: return "FROM";
        case TOKEN_WHERE: return "WHERE";
        case TOKEN_INSERT: return "INSERT";
        case TOKEN_INTO: return "INTO";
        case TOKEN_VALUES: return "VALUES";
        case TOKEN_CREATE: return "CREATE";
        case TOKEN_TABLE: return "TABLE";
        case TOKEN_INT: return "INT";
        case TOKEN_TEXT: return "TEXT";
        case TOKEN_DROP: return "DROP";
        case TOKEN_DELETE: return "DELETE";
        case TOKEN_UPDATE: return "UPDATE";
        case TOKEN_SET: return "SET";
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";
        case TOKEN_BETWEEN: return "BETWEEN";
        case TOKEN_IN: return "IN";
        case TOKEN_ORDER_BY: return "ORDER BY";
        case TOKEN_LIMIT: return "LIMIT";
        case TOKEN_LIKE: return "LIKE";
        case TOKEN_STAR: return "*";
        case TOKEN_COMMA: return ",";
        case TOKEN_SEMICOLON: return ";";
        case TOKEN_DOT: return ".";
        case TOKEN_LPAREN: return "(";
        case TOKEN_RPAREN: return ")";
        case TOKEN_LBRACE: return "{";
        case TOKEN_RBRACE: return "}";
        case TOKEN_EQUAL: return "=";
        case TOKEN_NOT_EQUAL: return "!=";
        case TOKEN_LESS: return "<";
        case TOKEN_GREATER: return ">";
        case TOKEN_LESS_EQUAL: return "<=";
        case TOKEN_GREATER_EQUAL: return ">=";
        case TOKEN_PLUS: return "+";
        case TOKEN_MINUS: return "-";
        case TOKEN_SLASH: return "/";
        case TOKEN_PERCENT: return "%";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_NULL: return "NULL";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        case TOKEN_UNKNOWN: return "UNKNOWN";
        default: return "???";
    }
}

void token_print(const Token* token) 
{
    if (token->type == TOKEN_EOF) 
    {
        printf("[EOF]");
    } else 
    {
        printf("[%s: '%.*s' at %d:%d]", 
               token_type_to_string(token->type),
               token->length, token->start,
               token->line, token->column);
    }
}

bool token_is_keyword(const Token* token) 
{
    return token->type >= TOKEN_SELECT && token->type <= TOKEN_NULL;
}

bool token_is_literal(const Token* token) 
{
    return token->type == TOKEN_IDENTIFIER || 
           token->type == TOKEN_STRING || 
           token->type == TOKEN_NUMBER ||
           token->type == TOKEN_NULL;
}

bool token_is_operator(const Token* token) 
{
    return (token->type >= TOKEN_STAR && token->type <= TOKEN_PERCENT) ||
           token->type == TOKEN_AND || token->type == TOKEN_OR || 
           token->type == TOKEN_NOT;
}

bool token_match(const Token* token, TokenType type) 
{
    return token->type == type;
}

bool token_text_equals(const Token* token, const char* text) 
{
    if (token->length != (int)strlen(text)) return false;
    return memcmp(token->start, text, token->length) == 0;
}