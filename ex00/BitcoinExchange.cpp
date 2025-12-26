#include "BitcoinExchange.hpp"
#include <cmath>
#include <cerrno>

BitcoinExchange::BitcoinExchange()
{
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other)
{
    _database = other._database;
}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other)
{
    if (this != &other)
    {
        _database = other._database;
    }
    return *this;
}

BitcoinExchange::~BitcoinExchange()
{
}

/**
 * Helper: Convert string to float
 * Handles conversion errors gracefully
 */
float BitcoinExchange::stringToFloat(const std::string& str)
{
    char* endptr;
    errno = 0;
    float value = std::strtof(str.c_str(), &endptr);
    
    // Check for conversion errors
    if (errno == ERANGE || *endptr != '\0')
    {
        throw std::runtime_error("Invalid float conversion");
    }
    return value;
}

/**
 * Helper: Validate date format YYYY-MM-DD
 * Also checks if date is realistic (1900-2999, months 01-12, days 01-31)
 */
bool BitcoinExchange::isValidDate(const std::string& date)
{
    // Check format: YYYY-MM-DD
    if (date.length() != 10)
        return false;
    
    if (date[4] != '-' || date[7] != '-')
        return false;
    
    // Check all digits
    for (int i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7)
            continue;
        if (!std::isdigit(date[i]))
            return false;
    }
    
    // Parse year, month, day
    int year = std::atoi(date.substr(0, 4).c_str());
    int month = std::atoi(date.substr(5, 2).c_str());
    int day = std::atoi(date.substr(8, 2).c_str());
    
    // Validate ranges
    if (year < 1900 || year > 2999)
        return false;
    if (month < 1 || month > 12)
        return false;
    if (day < 1 || day > 31)
        return false;
    
    return true;
}

/**
 * Helper: Validate and parse value
 * Rules:
 * - Must be a valid number (int or float)
 * - Must be positive
 * - Must be between 0 and 1000
 */
bool BitcoinExchange::isValidValue(const std::string& value_str, float& value)
{
    try
    {
        value = stringToFloat(value_str);
    }
    catch (const std::exception&)
    {
        return false;
    }
    
    // Check if positive
    if (value < 0)
        return false;
    
    // Check if not too large
    if (value > 1000)
        return false;
    
    return true;
}

/**
 * Load the Bitcoin price database from CSV file
 * Format: date,exchange_rate
 */
bool BitcoinExchange::loadDatabase(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    
    if (!file.is_open())
    {
        std::cerr << "Error: could not open file." << std::endl;
        return false;
    }
    
    std::string line;
    int line_num = 0;
    
    while (std::getline(file, line))
    {
        line_num++;
        
        // Skip header line
        if (line_num == 1)
            continue;
        
        // Skip empty lines
        if (line.empty())
            continue;
        
        // Find comma separator
        size_t comma_pos = line.find(',');
        if (comma_pos == std::string::npos)
            continue;
        
        std::string date = line.substr(0, comma_pos);
        std::string rate_str = line.substr(comma_pos + 1);
        
        // Trim whitespace from date
        while (!date.empty() && std::isspace(date[date.length() - 1]))
            date.erase(date.length() - 1);
        while (!date.empty() && std::isspace(date[0]))
            date = date.substr(1);
        
        // Trim whitespace from rate
        while (!rate_str.empty() && std::isspace(rate_str[rate_str.length() - 1]))
            rate_str.erase(rate_str.length() - 1);
        while (!rate_str.empty() && std::isspace(rate_str[0]))
            rate_str = rate_str.substr(1);
        
        // Validate date format
        if (!isValidDate(date))
            continue;
        
        // Parse exchange rate
        float rate;
        try
        {
            rate = stringToFloat(rate_str);
        }
        catch (const std::exception&)
        {
            continue;
        }
        
        // Store in map
        _database[date] = rate;
    }
    
    file.close();
    return true;
}

/**
 * Check if exact date exists in database
 */
bool BitcoinExchange::dateExists(const std::string& date)
{
    return _database.find(date) != _database.end();
}

/**
 * Get the closest lower date that exists in the database
 * Uses map's lower_bound for efficient search
 * 
 * Example:
 * Database has: 2011-01-03, 2011-01-04, 2011-01-05
 * Query: 2011-01-04 12:00 → Returns 2011-01-03 (closest lower)
 */
std::string BitcoinExchange::getClosestLowerDate(const std::string& date)
{
    // lower_bound returns iterator to first element >= date
    // So we need to go back one to get the closest lower date
    std::map<std::string, float>::iterator it = _database.lower_bound(date);
    
    // If exact match exists, use it
    if (it != _database.end() && it->first == date)
        return date;
    
    // Go back to get closest lower date
    if (it != _database.begin())
    {
        --it;
        return it->first;
    }
    
    // No lower date exists in database
    return "";
}

/**
 * Process input file and calculate Bitcoin values
 * 
 * Algorithm:
 * 1. Open file
 * 2. Parse each line: "date | value"
 * 3. Validate date and value
 * 4. Find exchange rate for date (use closest lower if exact not found)
 * 5. Calculate: result = value * exchange_rate
 * 6. Output result or error message
 */
void BitcoinExchange::processInputFile(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    
    if (!file.is_open())
    {
        std::cerr << "Error: could not open file." << std::endl;
        return;
    }
    
    std::string line;
    int line_num = 0;
    
    while (std::getline(file, line))
    {
        line_num++;
        
        // Skip header line
        if (line_num == 1 && line == "date | value")
            continue;
        
        // Skip empty lines
        if (line.empty())
            continue;
        
        // ===== PARSE LINE =====
        // Expected format: "2011-01-03 | 3"
        
        // Find pipe separator
        size_t pipe_pos = line.find('|');
        if (pipe_pos == std::string::npos)
        {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;
        }
        
        // Split by pipe
        std::string date_part = line.substr(0, pipe_pos);
        std::string value_part = line.substr(pipe_pos + 1);
        
        // Trim whitespace from both parts
        // Remove trailing spaces from date
        while (!date_part.empty() && std::isspace(date_part[date_part.length() - 1]))
            date_part.erase(date_part.length() - 1);
        // Remove leading spaces from date
        while (!date_part.empty() && std::isspace(date_part[0]))
            date_part = date_part.substr(1);
        
        // Remove trailing spaces from value
        while (!value_part.empty() && std::isspace(value_part[value_part.length() - 1]))
            value_part.erase(value_part.length() - 1);
        // Remove leading spaces from value
        while (!value_part.empty() && std::isspace(value_part[0]))
            value_part = value_part.substr(1);
        
        // ===== VALIDATE DATE =====
        if (!isValidDate(date_part))
        {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;
        }
        
        // ===== VALIDATE AND PARSE VALUE =====
        float value;
        if (!isValidValue(value_part, value))
        {
            // Determine which error to report
            if (value < 0)
            {
                std::cout << "Error: not a positive number." << std::endl;
            }
            else if (value > 1000)
            {
                std::cout << "Error: too large a number." << std::endl;
            }
            else
            {
                std::cout << "Error: bad input => " << line << std::endl;
            }
            continue;
        }
        
        // ===== FIND EXCHANGE RATE =====
        std::string lookup_date = getClosestLowerDate(date_part);
        
        if (lookup_date.empty())
        {
            std::cout << "Error: no exchange rate available for " << date_part << std::endl;
            continue;
        }
        
        float exchange_rate = _database[lookup_date];
        
        // ===== CALCULATE AND OUTPUT =====
        float result = value * exchange_rate;
        
        // Format output: "2011-01-03 => 3 = 0.9"
        // Use fixed precision for display
        std::cout << date_part << " => " << value;
        
        // Handle integer display (no decimal if whole number)
        if (value == static_cast<int>(value))
        {
            std::cout << " = ";
        }
        else
        {
            std::cout << " = ";
        }
        
        // Display result with appropriate precision
        std::cout << std::fixed << std::setprecision(2) << result << std::endl;
    }
    
    file.close();
}