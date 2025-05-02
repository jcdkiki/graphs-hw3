#include "tokenization.hpp"

std::string text;
std::vector<Token> tokens;
int next_token_index;

Token &nextToken() { return tokens[next_token_index]; }

Token &getToken() { return tokens[next_token_index++]; }

Token str2token(const std::string &str)
{
    if (isdigit(str[0]))
        return Token{Token::NUMBER, str};
    return Token{Token::IDENT, str};
}

void tokenize()
{
    std::string buf;
    for (int i = 0; i <= text.size(); i++) {
        char c = text[i];

        Token tok = {Token::ERROR};

        switch (c) {
        case '+':
            tok.id = Token::ADD;
            break;
        case '-':
            tok.id = Token::SUB;
            break;
        case '*':
            tok.id = Token::TIMES;
            break;
        case '/':
            tok.id = Token::DIV;
            break;
        case '^':
            tok.id = Token::EXP;
            break;
        case '(':
            tok.id = Token::LPAREN;
            break;
        case ')':
            tok.id = Token::RPAREN;
            break;
        }

        if (tok.id != Token::ERROR) {
            if (!buf.empty()) {
                tokens.push_back(str2token(buf));
                buf.clear();
            }

            tokens.push_back(tok);
            continue;
        }

        if (isspace(c) || c == '\0') {
            if (!buf.empty()) {
                tokens.push_back(str2token(buf));
                buf.clear();
            }
        }
        else {
            buf += c;
        }
    }

    tokens.push_back(Token{Token::END});
}
