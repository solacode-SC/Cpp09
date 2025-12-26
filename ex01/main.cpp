#include "RPN.hpp"
#include <iostream>

/**
 * RPN Calculator Program
 * 
 * Usage: ./RPN "expression"
 * 
 * Examples:
 * ./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"
 * Output: 42
 * 
 * ./RPN "7 7 * 7 -"
 * Output: 42
 * 
 * ./RPN "1 2 * 2 / 2 * 2 4 - +"
 * Output: 0
 * 
 * Error cases:
 * ./RPN "(1 + 1)"
 * Output: Error
 */
int main(int argc, char** argv)
{
    // Check argument count
    if (argc != 2)
    {
        std::cerr << "Error" << std::endl;
        return 1;
    }
    
    // Create RPN calculator
    RPN rpn;
    
    // Evaluate the expression
    if (!rpn.evaluate(argv[1]))
    {
        std::cerr << "Error" << std::endl;
        return 1;
    }
    
    // Output the result
    std::cout << rpn.getResult() << std::endl;
    
    return 0;
}