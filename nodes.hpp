#ifndef NODES_HPP
#define NODES_HPP

#include <string>

enum Priority { PRIORITY_NUMBER, PRIORITY_VARIABLE, PRIORITY_EXP, PRIORITY_DIV, PRIORITY_MUL, PRIORITY_SUB, PRIORITY_ADD };

struct Node {
    Node *parent;
    int priority;

    Node(int priority, Node *parent = nullptr) : priority(priority), parent(parent) {}

    virtual void print() = 0;
    virtual Node *differentiate(const std::string &x) = 0;
    virtual Node *copy() = 0;
    virtual Node *fold() = 0;

    void priorityPrint();
};

struct Number : public Node {
    double number;

    Number(double number) : number(number), Node(PRIORITY_NUMBER) {}

    virtual void print();
    virtual Node *differentiate(const std::string &x);
    virtual Node *copy();
    virtual Node *fold();
};

struct Variable : public Node {
    std::string str;

    Variable(const std::string &str) : str(str), Node(PRIORITY_VARIABLE) {}

    virtual void print();
    virtual Node *differentiate(const std::string &x);
    virtual Node *copy();
    virtual Node *fold();
};

struct BinaryOperation : public Node {
    Node *left, *right;
    char operation;

    BinaryOperation(Node *left, Node *right, char operation, int priority)
        : left(left), right(right), operation(operation), Node(priority)
    {
        left->parent = this;
        right->parent = this;
    }

    virtual void print();
    virtual Node *fold();
};

struct Addition : public BinaryOperation {
    Addition(Node *left, Node *right) : BinaryOperation(left, right, '+', PRIORITY_ADD) {}

    virtual Node *differentiate(const std::string &x);
    virtual Node *fold();
    virtual Node *copy();
};

struct Subtraction : public BinaryOperation {
    Subtraction(Node *left, Node *right) : BinaryOperation(left, right, '-', PRIORITY_SUB) {}

    virtual Node *differentiate(const std::string &x);
    virtual Node *fold();
    virtual Node *copy();
};

struct Multiplication : public BinaryOperation {
    Multiplication(Node *left, Node *right) : BinaryOperation(left, right, '*', PRIORITY_MUL) {}

    virtual Node *differentiate(const std::string &x);
    virtual Node *fold();
    virtual Node *copy();
};

struct Division : public BinaryOperation {
    Division(Node *left, Node *right) : BinaryOperation(left, right, '/', PRIORITY_DIV) {}

    virtual Node *differentiate(const std::string &x);
    virtual Node *fold();
    virtual Node *copy();
};

struct Exponentiation : public BinaryOperation {
    Exponentiation(Node *left, Node *right) : BinaryOperation(left, right, '^', PRIORITY_EXP) {}

    virtual Node *differentiate(const std::string &x);
    virtual Node *fold();
    virtual Node *copy();
};

struct Logarithm : public Node {
    Node *operand;
    Logarithm(Node *operand) : operand(operand), Node(PRIORITY_EXP) { operand->parent = this; }

    virtual void print();
    virtual Node *differentiate(const std::string &x);
    virtual Node *copy();
    virtual Node *fold();
};

struct Sine : public Node {
    Node *operand;
    Sine(Node *operand) : operand(operand), Node(PRIORITY_VARIABLE) { operand->parent = this; }

    virtual void print();
    virtual Node *differentiate(const std::string &x);
    virtual Node *copy();
    virtual Node *fold();
};

struct Cosine : public Node {
    Node *operand;
    Cosine(Node *operand) : operand(operand), Node(PRIORITY_VARIABLE) { operand->parent = this; }

    virtual void print();
    virtual Node *differentiate(const std::string &x);
    virtual Node *copy();
    virtual Node *fold();
};

#endif
