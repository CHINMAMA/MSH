#include "parser.h"
#include "memory.h"
#include "io.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "shell.h"

#define SPACE "\n\t\v\r\f "
#define OPERATOR_CHARS "|<>=&"
#define EXP_OPEN_CHARS "'\""
#define EXP_CLOSE_CHARS "'\""
#define ESCAPE "\\"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

static char *next_token_p = NULL;

const char *operators[] = {
    "&&", ">", "<", "|", "="
};

const char *expansion_open[] = {
    "'", "\"",
};

const char *expansion_close[] = {
    "'", "\"",
};

const char *keywords[] = {
    "if",
    "then",
    "else",
    "elif",
    "fi",
    "case",
    "esac",
    "for",
    "select",
    "while",
    "until",
    "do",
    "done",
    "in",
    "function",
    "time",
    "coproc",
    "[[",
    "]]"
};

MSH_Token MSH_GetNextToken(char *line);

MSH_Token *MSH_SplitLine(char *line)
{
    size_t buf_size = 16;
    MSH_Token *tokens = (MSH_Token *)MSH_Malloc(buf_size * sizeof(MSH_Token));

    size_t position = 0;

    for (MSH_Token token = MSH_GetNextToken(line); token.str; token = MSH_GetNextToken(NULL))
    {
        // printf("token str -> %s\n", token.str);
        // MSH_ExpandToken(&token);
        tokens[position++] = token;
        if (position >= buf_size)
        {
            buf_size = 1 + (buf_size * 3) >> 1;
            tokens = MSH_Realloc(tokens, buf_size * sizeof(*tokens));
        }
    }

    if (position && tokens[position - 1].type == MSH_NONE)
        return tokens;

    if (position >= buf_size)
    {
        buf_size++;
        tokens = MSH_Realloc(tokens, buf_size * sizeof(*tokens));
    }

    tokens[position] = (MSH_Token){ MSH_NONE, NULL, 0 };

    return tokens;
}

char **MSH_ParseTokens(MSH_Token *tokens) {
    size_t buf_size = 16;
    char **str_tokens = MSH_Malloc(buf_size * sizeof(*str_tokens));

    int position = 0;
    for (MSH_Token *tok = tokens; tok->type != MSH_NONE; ++tok) {
        if (tok->type != MSH_WORD) {
            MSH_perror("WIP\n");
            free(str_tokens);
            return NULL;
        }
        str_tokens[position++] = tok->str;
        if (position >= buf_size)
        {
            buf_size = 1 + (buf_size * 3) >> 1;
            str_tokens = MSH_Realloc(str_tokens, buf_size * sizeof(*str_tokens));
        }
    }

    str_tokens[position] = NULL;

    return str_tokens;
}


/*
** Returns pointer to string if str starts with operator to locate operator in operators[]
** Else returns NULL
*/ 
static const char **MSH_WhatOperator(const char* str, const char* operators[], size_t operators_num)
{
    for (size_t i = 0; i < operators_num; ++i)
    {
        if (strlen(str) >= strlen(operators[i]) && !strncmp(operators[i], str, strlen(operators[i])))
        {
            return &operators[i];
        }
    }

    return NULL;
}

// Returns pointer to the next occurance of an any delimiters char in str
static void MSH_SkipTill(const char* token_end, char **str)
{
    if (!token_end || !str || !*str) return;
    while (**str != '\0' && strchr(token_end, (unsigned char)**str) == NULL) {
        (*str)++;
    }
}

/*
** Divides line into MSH_Token's.
** Firstly, pass the line as argument. Function will return pointer to the first MSH_Token.
** Then pass NULL as argument to get next MSH_Token.
** Function can modify the line.
** If possible it will not allocate additional memory for MSH_Token.str
*/
MSH_Token MSH_GetNextToken(char *line)
{
    if (line != NULL)
        { next_token_p = line; }

    if (next_token_p == NULL || *next_token_p == '\0')
        { return (MSH_Token){ MSH_NONE, NULL, 0 }; }

    // Skip space symbols
    while (*next_token_p != '\0' && strchr(SPACE, *next_token_p))
        { next_token_p++; }
    
    // If reached the end of line
    if (*next_token_p == '\0')    
        { return (MSH_Token){ MSH_NONE, NULL, 0 }; }

    char *token = next_token_p;
    const char **op;
    unsigned char owns = 0;
    MSH_TokenType type = MSH_NONE;

    if (op = MSH_WhatOperator(token, operators, ARRAY_SIZE(operators))) {
        *token = '\0'; // null terminating previous token
        next_token_p += strlen(*op);
        return (MSH_Token) { MSH_OPERATOR, *op, .owns=0, MSH_NOT_EXP };
    }

    if (op = MSH_WhatOperator(token, expansion_open, ARRAY_SIZE(expansion_open))) {
        int exp_open_i = op - expansion_open; 
        *next_token_p = '\0'; // null terminating previous token
        next_token_p += strlen(*op);
        token = next_token_p;
        char **exp_close;      
        while (
            !(exp_close = MSH_WhatOperator(next_token_p, expansion_close + exp_open_i, 1)) 
            && *next_token_p != '\0'
        ) {
            ++next_token_p;
        }

        *next_token_p = '\0';
        
        if (!exp_close) { MSH_perror("Invalid expansion token\n"); return (MSH_Token){ MSH_NONE, NULL, 0 }; }
        next_token_p += strlen(*exp_close);
        MSH_ExpansionType exp_type;
        return (MSH_Token) { MSH_EXPANSION, token, .owns=0, MSH_NOT_EXP };
    }

    // Iterate next_token_p untill reached token end
    ++next_token_p;
    MSH_SkipTill(SPACE EXP_OPEN_CHARS OPERATOR_CHARS, &next_token_p);
    if (isspace(*next_token_p)) { *next_token_p = '\0'; ++next_token_p; }

    // Check if token is keyword
    {
        size_t i = 0;
        for (const char *keyword = keywords[0]; i < ARRAY_SIZE(keywords); keyword = keywords[++i])
        {
            if (!strncmp(token, keyword, strlen(keyword)))
            {
                return (MSH_Token) { MSH_KEYWORD, token, .owns=0 };
            }
        }
    }

    return (MSH_Token) { MSH_WORD, token, .owns=0, MSH_NOT_EXP };



    // // If there is not space to fit null terminating then 
    // // allocate new memory for token and set owns to true
    // if (*next_token_p != '\0' && !isspace(*next_token_p)) {
    //     owns = 1;
    //     int token_len = next_token_p - token;
    //     char *new_token = MSH_Malloc((token_len + 1) * sizeof(*new_token));
    //     memcpy(new_token, token, token_len * sizeof(*new_token));
    //     new_token[token_len] = '\0';
    //     token = new_token;
    // }
    // // If there is space for null terminating then token will be stored directly in line
    // else if (isspace(*next_token_p)) {
    //     *next_token_p = '\0';
    //     ++next_token_p;
    // }

}

void MSH_expand_braces(char *token);
void MSH_expand_tilde(char *token);
void MSH_expand_parameters(char *token);
void MSH_expand_commands(char *token);
void MSH_expand_arithmetic(char *token);

unsigned char MSH_IsValidVarName(const char *var_name) {
    if (!var_name) { return 0; }
    if (!isalpha(*var_name) && *var_name != '_') { return 0; }

    for (const char *c = var_name + 1; *c != '\0'; ++c) {
        if (!isalnum(*c) && *c != '_') { return 0; }
    }

    {
        size_t i = 0;
        for (const char *keyword = keywords[0]; i < ARRAY_SIZE(keywords); keyword = keywords[++i])
        {
            if (!strncmp(var_name, keyword, strlen(keyword)))
            {
                return 0;
            }
        }
    }

    return 1;
}

/*
** Substitutes variables with their values
*/
void MSH_ExpandVariables(char **line)
{
    if (!line || !*line || **line == '\0') { return; }

    size_t bufsize = MSH_BUFSIZE;
    size_t varsize = 128;
    size_t new_line_len = 0;
    size_t new_line_size = bufsize;
    char *_line = *line;
    char *var_start = _line;
    char *new_line = NULL;
    char var_name[varsize];

    while (1) {
        MSH_SkipTill("$", &var_start);
        if (!var_start || *var_start == '\0' || *(var_start + 1) == '\0') break;
        
        if (!new_line) {
            new_line = MSH_Malloc(new_line_size);
            new_line[0] = '\0';
            new_line_len = 0;
        }

        // Check if escaped. ("\\$var")
        if (var_start != _line && *(var_start - 1) == '\\') {
            size_t prefix_len = (size_t)((var_start - _line) - 1);
            if (prefix_len) {
                if (MSH_Strcat(&new_line, _line, &new_line_size, &new_line_len, prefix_len) < 0) {
                    MSH_perror("Something went wrong in strcat. Unlucky!\n");
                    if (new_line) { free(new_line); }
                    return;
                }
            }

            new_line[new_line_len++] = '$';
            new_line[new_line_len] = '\0';

            _line = var_start + 1;
            var_start = _line;
            continue;
        }

        char *var_end = var_start + 1;
        MSH_SkipTill(SPACE EXP_OPEN_CHARS OPERATOR_CHARS, &var_end);

        size_t name_len = (size_t)(var_end - (var_start + 1));

        if (name_len == 0) {
            size_t prefix_len = (size_t)(var_start - _line);
            if (MSH_Strcat(&new_line, _line, &new_line_size, &new_line_len, prefix_len) < 0) {
                MSH_perror("Something went wrong in strcat. Unlucky!\n");
                if (new_line) { free(new_line); }
                return;
            }
            new_line[new_line_len++] = '$';
            new_line[new_line_len] = '\0';

            _line = var_start + 1;
            var_start = _line;
            continue;
        }

        if (name_len >= varsize) {
            MSH_perror("Invalid variable length\n");
            if (new_line) { free(new_line); }
            return;
        }

        memcpy(var_name, var_start + 1, name_len);
        var_name[name_len] = '\0';
        char *var_val;
        if (!MSH_IsValidVarName(var_name)) {
            if (MSH_Strcat(&new_line, "$", &new_line_size, &new_line_len, 1) < 0) {
                MSH_perror("Something went wrong in strcat. Unlucky!\n");
                if (new_line) { free(new_line); }
                return;
            }
            var_val = var_name;
        }
        else {
            var_val = MSH_GetVar(var_name);
        }

        if (!var_val) { var_val = ""; }

        size_t vlen = strlen(var_val);
        size_t prefix_len = (size_t)(var_start - _line);
        
        if (MSH_Strcat(&new_line, _line, &new_line_size, &new_line_len, prefix_len) < 0) {
            MSH_perror("Something went wrong in strcat. Unlucky!\n");
            if (new_line) { free(new_line); }
            return;
        }
        
        if (MSH_Strcat(&new_line, var_val, &new_line_size, &new_line_len, vlen) < 0) {
            MSH_perror("Something went wrong in strcat. Unlucky!\n");
            if (new_line) { free(new_line); }
            return;
        }

        _line = var_end;
        var_start = _line;
    }

    if (new_line != NULL) {
        
        size_t rem = strlen(_line);
        if (MSH_Strcat(&new_line, _line, &new_line_size, &new_line_len, rem) < 0) {
            MSH_perror("Something went wrong in strcat. Unlucky!\n");
            if (new_line) { free(new_line); }
            return;
        }

        free(*line);
        *line = new_line;
    }
}

/*
** Substitutes expansion tokens
*/
void MSH_ExpandToken(MSH_Token **token)
{
    if ((*token)->type != MSH_EXPANSION) { return; }
    if ((*token)->exp_type == MSH_QUOTE_EXP) {
        (*token)->type = MSH_WORD;
    }
} // TODO: add functionality to expand arithmetic, command etc.