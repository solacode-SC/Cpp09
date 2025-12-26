#ifndef BITCOIN_EXCHANGE_HPP
#define BITCOIN_EXCHANGE_HPP

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iomanip>

/**
 * BitcoinExchange class handles:
 * 1. Loading Bitcoin price database from CSV
 * 2. Parsing input file with date and value pairs
 * 3. Finding exchange rates for given dates (with fallback to lower date)
 * 4. Calculating Bitcoin values
 */
class BitcoinExchange
{
private:
    // Database: Date (YYYY-MM-DD) -> Exchange Rate (BTC value in some currency)
    // Using std::map because:
    // - Keys are automatically sorted
    // - lower_bound() allows efficient finding of closest lower date
    // - O(log n) lookup time
    std::map<std::string, float> _database;
    
    // Private helper methods
    bool isValidDate(const std::string& date);
    bool isValidValue(const std::string& value_str, float& value);
    float stringToFloat(const std::string& str);
    bool dateExists(const std::string& date);
    std::string getClosestLowerDate(const std::string& date);

public:
    BitcoinExchange();
    BitcoinExchange(const BitcoinExchange& other);
    BitcoinExchange& operator=(const BitcoinExchange& other);
    ~BitcoinExchange();
    
    /**
     * Load database from CSV file (data.csv provided with subject)
     * Format: date,exchange_rate
     * Example:
     *   2011-01-03,0.30
     *   2011-01-04,0.32
     */
    bool loadDatabase(const std::string& filename);
    
    /**
     * Process input file with transactions
     * Format: date | value
     * Example:
     *   2011-01-03 | 3
     *   2012-01-11 | 1
     */
    void processInputFile(const std::string& filename);
};

#endif