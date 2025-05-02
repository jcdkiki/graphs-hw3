#ifndef TOKENIZATION_HPP
#define TOKENIZATION_HPP

#include <string>
#include <vector>
#include <ostream>

struct Token {
    int id;
    std::string str;

    enum {
        ERROR,
        NUMBER,
        LPAREN,
        RPAREN,
        ADD,
        SUB,
        TIMES,
        DIV,
        EXP,
        IDENT,
        END,
    };
};

extern std::string text;
extern std::vector<Token> tokens;


std::ostream &operator<<(std::ostream &os, const Token &tok);
Token &nextToken();
Token &getToken();
Token str2token(const std::string &str);
void tokenize();

#endif
