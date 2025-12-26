#include "RPN.hpp"

RPN::RPN()
{
}

RPN::RPN(const RPN& other)
{
    _stack = other._stack;
}

RPN& RPN::operator=(const RPN& other)
{
    if (this != &other)
    {
        _stack = other._stack;
    }
    return *this;
}

RPN::~RPN()
{
}

/**
 * Check if token is an operator
 * Valid operators: +, -, *, /
 */
bool RPN::isOperator(const std::string& token)
{
    return (token == "+" || token == "-" || token == "*" || token == "/");
}

/**
 * Check if token is a valid number
 * Rules:
 * - Single digit numbers (0-9)
 * - Can have leading negative sign
 */
bool RPN::isNumber(const std::string& token)
{
    if (token.empty())
        return false;
    
    // Check for negative sign
    size_t start = 0;
    if (token[0] == '-' || token[0] == '+')
    {
        if (token.length() < 2)
            return false;
        start = 1;
    }
    
    // Check that rest are digits
    for (size_t i = start; i < token.length(); i++)
    {
        if (!std::isdigit(token[i]))
            return false;
    }
    
    return true;
}

/**
 * Convert string to integer
 * Simple wrapper around atoi
 */
int RPN::stringToInt(const std::string& str)
{
    return std::atoi(str.c_str());
}

/**
 * Apply an operation to the top two stack elements
 * 
 * Algorithm:
 * 1. Check if stack has at least 2 elements
 * 2. Pop first element (this is the second operand)
 * 3. Pop second element (this is the first operand)
 * 4. Apply operation: first_op operation second_op
 * 5. Push result back
 * 
 * Important: Order matters!
 * Stack: [5, 3] with "-" means 5 - 3 = 2, NOT 3 - 5
 */
bool RPN::applyOperation(char op)
{
    // Check if we have enough operands
    if (_stack.size() < 2)
    {
        return false;
    }
    
    // Pop operands (note the order!)
    int second = _stack.top();
    _stack.pop();
    int first = _stack.top();
    _stack.pop();
    
    int result;
    
    // Apply operation
    switch (op)
    {
        case '+':
            result = first + second;
            break;
        case '-':
            result = first - second;
            break;
        case '*':
            result = first * second;
            break;
        case '/':
            // Division by zero check
            if (second == 0)
            {
                return false;
            }
            result = first / second;
            break;
        default:
            return false;
    }
    
    // Push result back
    _stack.push(result);
    return true;
}

/**
 * Evaluate an RPN expression
 * 
 * Main Algorithm:
 * 1. Split expression by spaces into tokens
 * 2. For each token:
 *    a. If it's a number: push onto stack
 *    b. If it's an operator: apply operation (pop 2, push result)
 * 3. After processing all tokens, stack should have exactly 1 element
 * 4. That element is the result
 * 
 * Error cases:
 * - Invalid token (not a number or operator)
 * - Not enough operands for an operator
 * - More than one element left in stack (malformed expression)
 * - Empty stack result
 */
bool RPN::evaluate(const std::string& expression)
{
    // Clear any previous state
    while (!_stack.empty())
        _stack.pop();
    
    // Split expression into tokens
    std::stringstream ss(expression);
    std::string token;
    
    while (ss >> token)
    {
        // ===== PROCESS TOKEN =====
        
        if (isNumber(token))
        {
            // It's a number: push onto stack
            int num = stringToInt(token);
            _stack.push(num);
        }
        else if (isOperator(token))
        {
            // It's an operator: apply operation
            if (!applyOperation(token[0]))
            {
                std::cerr << "Error: invalid operation or not enough operands" << std::endl;
                return false;
            }
        }
        else
        {
            // Invalid token
            std::cerr << "Error: invalid token: " << token << std::endl;
            return false;
        }
    }
    
    // ===== VALIDATE RESULT =====
    
    // Stack should have exactly 1 element
    if (_stack.size() != 1)
    {
        std::cerr << "Error: invalid expression (too many numbers)" << std::endl;
        return false;
    }
    
    return true;
}

/**
 * Get the result of the last evaluation
 */
int RPN::getResult()
{
    if (_stack.empty())
        return 0;
    
    return _stack.top();
}