#ifndef RPN_HPP
#define RPN_HPP

#include <string>
#include <stack>
#include <iostream>
#include <sstream>
#include <cstdlib>

/**
 * RPN (Reverse Polish Notation) Calculator
 * 
 * Implements an RPN expression evaluator.
 * 
 * How RPN works:
 * - Numbers are pushed onto a stack
 * - Operators pop the last two numbers, apply operation, push result
 * - Final result is the last item on stack
 * 
 * Example: "3 4 +" becomes (3 + 4) = 7
 * Example: "8 9 * 9 - 9 - 9 - 4 - 1 +" becomes 42
 * 
 * Using std::stack because:
 * - LIFO (Last In First Out) is exactly what RPN needs
 * - push(), pop(), top() are efficient O(1) operations
 * - Natural fit for the algorithm
 */
class RPN
{
private:
    std::stack<int> _stack;
    
    // Helper methods
    bool isOperator(const std::string& token);
    bool isNumber(const std::string& token);
    bool applyOperation(char op);
    int stringToInt(const std::string& str);

public:
    RPN();
    RPN(const RPN& other);
    RPN& operator=(const RPN& other);
    ~RPN();
    
    /**
     * Evaluate an RPN expression
     * 
     * Input: string with space-separated tokens
     * Output: result of evaluation or error
     * 
     * Example:
     * evaluate("3 4 +") returns 7
     * evaluate("8 9 *") returns 72
     */
    bool evaluate(const std::string& expression);
    
    /**
     * Get the result of the last evaluation
     */
    int getResult();
};

#endif