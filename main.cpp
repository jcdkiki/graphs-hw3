#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "nodes.hpp"
#include "tokenization.hpp"

Node *getAddition();

// all functions are unary, sorry :((((
Node *getFunction(const std::string &name)
{
    getToken(); // '('
    Node *operand = getAddition();
    
    Token &rparen = getToken();
    if (rparen.id != Token::RPAREN) {
        std::cout << "Error: expected RPAREN\n";
        exit(1);
    }

    if (name == "sin") {
        return new Sine(operand);
    }
    if (name == "cos") {
        return new Cosine(operand);
    }
    // ...

    std::cerr << "Error: unknown function " << name << '\n';
    exit(1);
    return nullptr;
}

Node *getValue()
{
    Token &token = getToken();
    if (token.id == Token::SUB) {
        Node *operand = getValue();
        return new Multiplication(new Number(-1), operand);
    }
    if (token.id == Token::NUMBER)
        return new Number(std::stod(token.str));
    if (token.id == Token::IDENT) {
        if (nextToken().id == Token::LPAREN) {
            return getFunction(token.str);
        }
        
        return new Variable(token.str);
    }
    if (token.id == Token::LPAREN) {
        Node *res = getAddition();

        Token &rparen = getToken();
        if (rparen.id != Token::RPAREN) {
            std::cout << "Error: expected RPAREN\n";
            exit(1);
        }

        return res;
    }

    std::cout << "Error: expected value\n";
    exit(1);
}

Node *getExponentiation()
{
    Node *res = getValue();

    while (nextToken().id == Token::EXP) {
        Token &tok = getToken();
        Node *right = getValue();
        res = new Exponentiation(res, right);
    }

    return res;
}

Node *getMultiplication()
{
    Node *res = getExponentiation();

    while (nextToken().id == Token::TIMES || nextToken().id == Token::DIV) {
        Token &tok = getToken();
        Node *right = getExponentiation();

        if (tok.id == Token::TIMES)
            res = new Multiplication(res, right);
        else
            res = new Division(res, right);
    }

    return res;
}

Node *getAddition()
{
    Node *res = getMultiplication();

    while (nextToken().id == Token::ADD || nextToken().id == Token::SUB) {
        Token &tok = getToken();
        Node *right = getMultiplication();

        if (tok.id == Token::ADD)
            res = new Addition(res, right);
        else
            res = new Subtraction(res, right);
    }

    return res;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "specify variable name\n";
        return 1;
    }

    std::getline(std::cin, text);
    tokenize();

    Node *tree = getAddition();
    Node *diff = tree->differentiate(std::string(argv[1]));
    Node *folded = diff->fold();
    
    std::cout << text << '\n';

    diff->priorityPrint();
    std::cout << '\n';

    folded->priorityPrint();
    std::cout << '\n';
}
