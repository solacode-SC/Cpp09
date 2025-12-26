#include "PmergeMe.hpp"
#include <iostream>

/**
 * PmergeMe: Ford-Johnson (Merge-Insertion) Sort
 * 
 * Usage: ./PmergeMe [positive_integers...]
 * 
 * Examples:
 * ./PmergeMe 3 5 9 7 4
 * ./PmergeMe `shuf -i 1-100000 -n 3000 | tr "\n" " "`
 * 
 * Requirements:
 * - All numbers must be positive
 * - No duplicates validation (left to discretion)
 * - Must handle at least 3000 integers
 * - Uses Ford-Johnson algorithm
 * - Compares vector and deque performance
 */
int main(int argc, char** argv)
{
    // Check argument count
    if (argc < 2)
    {
        std::cerr << "Error" << std::endl;
        return 1;
    }
    
    // Create PmergeMe object
    PmergeMe sorter;
    
    // Parse and validate arguments
    if (!sorter.parseArguments(argc, argv))
    {
        std::cerr << "Error" << std::endl;
        return 1;
    }
    
    // Sort using both containers
    sorter.sort();
    
    // Display results
    sorter.displayResults();
    
    return 0;
}