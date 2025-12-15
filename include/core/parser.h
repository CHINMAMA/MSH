#ifndef PARSER_H
#define PARSER_H

typedef enum MSH_TokenType
{
    MSH_NONE,
    MSH_WORD,
    MSH_OPERATOR,
    MSH_KEYWORD,
    MSH_EXPANSION
} MSH_TokenType;

typedef enum MSH_ExpansionType
{
    MSH_NOT_EXP,
    MSH_ARITHMETIC_EXP,
    MSH_QUOTE_EXP
} MSH_ExpansionType;

typedef struct MSH_Token {
    MSH_TokenType type;
    char *str;
    unsigned char owns; // If true str should be freed separately
    MSH_ExpansionType exp_type;
} MSH_Token;

MSH_Token *MSH_SplitLine(char *line);
void MSH_ExpandVariables(char **line);
char **MSH_ParseTokens(MSH_Token *tokens);

#endif