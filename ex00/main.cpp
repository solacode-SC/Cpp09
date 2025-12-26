#include "BitcoinExchange.hpp"
#include <iostream>

/**
 * Bitcoin Exchange Program
 * 
 * Usage: ./btc [input_file]
 * 
 * The program:
 * 1. Loads Bitcoin price database (data.csv)
 * 2. Reads transactions from input file
 * 3. Calculates Bitcoin values for each date
 * 4. Uses closest lower date if exact date not in database
 */
int main(int argc, char** argv)
{
    // Check argument count
    if (argc != 2)
    {
        std::cerr << "Error: could not open file." << std::endl;
        return 1;
    }
    
    // Create exchange object
    BitcoinExchange btc;
    
    // Load the Bitcoin price database
    // This file should be in the same directory as the binary
    if (!btc.loadDatabase("data.csv"))
    {
        std::cerr << "Error: could not open file." << std::endl;
        return 1;
    }
    
    // Process the input file
    btc.processInputFile(argv[1]);
    
    return 0;
}