#include "nodes.hpp"

#include <iostream>
#include <cmath>

void Node::priorityPrint()
{
    if (parent != nullptr && parent->priority < this->priority) {
        std::cout << '(';
        this->print();
        std::cout << ')';
        return;
    }
    this->print();
}

// Number
void Number::print() { std::cout << number; }
Node *Number::differentiate(const std::string &x) { return new Number(0); }
Node *Number::copy() { return new Number(number); }
Node *Number::fold() { return this->copy(); }

// Variable
void Variable::print() { std::cout << str; }
Node *Variable::differentiate(const std::string &x)
{
    if (x == str)
    return new Number(1);
    return new Number(0);
}

Node *Variable::copy() { return new Variable(str); }
Node *Variable::fold() { return this->copy(); }

// Binary operation
void BinaryOperation::print()
{
    left->priorityPrint();
    std::cout << operation;
    right->priorityPrint();
}

Node *BinaryOperation::fold() { return nullptr; }

// Addition
Node *Addition::differentiate(const std::string &x)
{
    return new Addition(left->differentiate(x), right->differentiate(x));
}

Node *Addition::fold()
{
    Node *new_left = left->fold();
    Node *new_right = right->fold();

    if (typeid(*new_left) == typeid(Number) && typeid(*new_right) == typeid(Number)) {
        Node *res = new Number(dynamic_cast<Number*>(new_left)->number + dynamic_cast<Number*>(new_right)->number);
        delete new_left;
        delete new_right;
        return res;
    }

    if (typeid(*new_left) == typeid(Number) && dynamic_cast<Number*>(new_left)->number == 0) {
        delete new_left;
        return new_right;
    }

    if (typeid(*new_right) == typeid(Number) && dynamic_cast<Number*>(new_right)->number == 0) {
        delete new_right;
        return new_left;
    }

    return new Addition(new_left, new_right);
}

Node *Addition::copy() { return new Addition(left->copy(), right->copy()); }

// Subtraction
Node *Subtraction::differentiate(const std::string &x)
{
    return new Subtraction(left->differentiate(x), right->differentiate(x));
}

Node *Subtraction::fold()
{
    Node *new_left = left->fold();
    Node *new_right = right->fold();

    if (typeid(*new_left) == typeid(Number) && typeid(*new_right) == typeid(Number)) {
        Node *res = new Number(dynamic_cast<Number*>(new_left)->number - dynamic_cast<Number*>(new_right)->number);
        delete new_left;
        delete new_right;
        return res;
    }

    if (typeid(*new_left) == typeid(Number) && dynamic_cast<Number*>(new_left)->number == 0) {
        delete new_left;
        Node *mul = new Multiplication(new Number(-1), new_right);
        Node *folded_mul = mul->fold();
        delete mul;
        return folded_mul;
    }

    if (typeid(*new_right) == typeid(Number) && dynamic_cast<Number*>(new_right)->number == 0) {
        delete new_right;
        return new_left;
    }

    return new Subtraction(new_left, new_right);
}

Node *Subtraction::copy() { return new Subtraction(left->copy(), right->copy()); }

// Multiplication
Node *Multiplication::differentiate(const std::string &x)
{
    return new Addition(new Multiplication(left->copy(), right->differentiate(x)),
                        new Multiplication(left->differentiate(x), right->copy()));
}

Node *Multiplication::fold()
{
    Node *new_left = left->fold();
    Node *new_right = right->fold();

    if (typeid(*new_left) == typeid(Number) && typeid(*new_right) == typeid(Number)) {
        Node *res = new Number(dynamic_cast<Number*>(new_left)->number * dynamic_cast<Number*>(new_right)->number);
        delete new_left;
        delete new_right;
        return res;
    }

    if (typeid(*new_left) == typeid(Number)) {
        double number = dynamic_cast<Number*>(new_left)->number;
        if (number == 0) {
            delete new_right;
            return new_left;
        }
        else if (number == 1) {
            delete new_left;
            return new_right;
        }
    }

    if (typeid(*new_right) == typeid(Number)) {
        double number = dynamic_cast<Number*>(new_right)->number;
        if (number == 0) {
            delete new_left;
            return new_right;
        }
        else if (number == 1) {
            delete new_right;
            return new_left;
        }
    }

    return new Multiplication(new_left, new_right);
}

Node *Multiplication::copy() { return new Multiplication(left->copy(), right->copy()); }


// Division
Node *Division::differentiate(const std::string &x)
{
    return new Division(new Subtraction(new Multiplication(left->differentiate(x), right->copy()),
                                        new Multiplication(left->copy(), right->differentiate(x))),
                        new Multiplication(right->copy(), right->copy()));
}

Node *Division::fold()
{
    Node *new_left = left->fold();
    Node *new_right = right->fold();

    if (typeid(*new_left) == typeid(Number) && typeid(*new_right) == typeid(Number)) {
        Node *res = new Number(dynamic_cast<Number*>(new_left)->number / dynamic_cast<Number*>(new_right)->number);
        delete new_left;
        delete new_right;
        return res;
    }

    if (typeid(*new_left) == typeid(Number) && dynamic_cast<Number*>(new_left)->number == 0) {
        delete new_right;
        return new_left;
    }

    return new Division(new_left, new_right);
}

Node *Division::copy() { return new Division(left->copy(), right->copy()); }


// Logarithm
void Logarithm::print()
{
    std::cout << "ln";
    operand->priorityPrint();
}

Node *Logarithm::differentiate(const std::string &x)
{
    return new Division(operand->differentiate(x), operand->copy());
}

Node *Logarithm::copy() { return new Logarithm(operand->copy()); }

Node *Logarithm::fold()
{
    Node *new_operand = operand->fold();
    
    if (typeid(*operand) == typeid(Variable) && dynamic_cast<Variable*>(new_operand)->str == "e") {
        delete new_operand;
        return new Number(1);
    }

    return new Logarithm(new_operand);
}

// Exponentiation
Node *Exponentiation::differentiate(const std::string &x)
{
    // e^f(x)
    if (typeid(*left) == typeid(Variable) && dynamic_cast<Variable*>(left)->str == "e") {
        return new Multiplication(this->copy(), right->differentiate(x));
    }

    // f(x)^alpha
    if (typeid(*right) == typeid(Number)) {
        double number = dynamic_cast<Number*>(right)->number;
        return new Multiplication(
            new Multiplication(
                new Number(number),
                new Exponentiation(
                    left->copy(),
                    new Number(number - 1)
                )
            ),
            left->differentiate(x)
        );
    }

    std::cerr << "Can't differentiate ";
    this->priorityPrint();
    std::cerr << '\n';
    exit(1);

    return nullptr;
}

Node *Exponentiation::fold()
{
    Node *new_left = left->fold();
    Node *new_right = right->fold();
    
    if (typeid(*new_left) == typeid(Number) && typeid(*new_right) == typeid(Number)) {
        Node *res = new Number(pow(dynamic_cast<Number*>(new_left)->number, dynamic_cast<Number*>(new_right)->number));
        delete new_left;
        delete new_right;
        return res;
    }

    if (typeid(*new_right) == typeid(Number) && dynamic_cast<Number*>(new_right)->number == 0) {
        delete new_left;
        delete new_right;
        return new Number(1);
    }

    if (typeid(*new_right) == typeid(Number) && dynamic_cast<Number*>(new_right)->number == 1) {
        delete new_right;
        return new_left;
    }

    return new Exponentiation(new_left, new_right);
}

Node *Exponentiation::copy() { return new Exponentiation(left->copy(), right->copy()); }


// Sine
void Sine::print()
{ 
    std::cout << "sin(";
    operand->print();
    std::cout << ")";
}

Node *Sine::differentiate(const std::string &x)
{
    return new Multiplication(new Cosine(operand->copy()), operand->differentiate(x));
}

Node *Sine::copy() { return new Sine(operand->copy()); }

Node *Sine::fold()
{
    Node *new_operand = operand->fold();

    if (typeid(*new_operand) == typeid(Number) && dynamic_cast<Number*>(new_operand)->number == 0) {
        delete new_operand;
        return new Number(0);
    }

    return new Sine(new_operand);
}

// Cosine
void Cosine::print()
{
    std::cout << "cos(";
    operand->print();
    std::cout << ")";
}

Node *Cosine::differentiate(const std::string &x)
{
    return new Multiplication(new Multiplication(new Number(-1), new Sine(operand->copy())), operand->differentiate(x));
}

Node *Cosine::copy() { return new Cosine(operand->copy()); }

Node *Cosine::fold()
{
    Node *new_operand = operand->fold();

    if (typeid(*new_operand) == typeid(Number) && dynamic_cast<Number*>(new_operand)->number == 0) {
        delete new_operand;
        return new Number(1);
    }

    return new Cosine(new_operand);
}

