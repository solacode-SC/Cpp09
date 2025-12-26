#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>

/**
 * PmergeMe: Implements Ford-Johnson (Merge-Insertion) Sort
 * 
 * Required:
 * - Sort using two different containers (vector and deque)
 * - Use Ford-Johnson algorithm specifically
 * - Use Jacobsthal numbers for optimal insertion order
 * - Handle at least 3000 integers
 * - Display timing for each container
 * 
 * Why Ford-Johnson?
 * - Minimizes number of comparisons
 * - Theoretically optimal for comparison-based sorting
 * - Demonstrates advanced algorithmic knowledge
 * 
 * Why two containers?
 * - Compare performance characteristics
 * - Vector: better cache, slower insertions
 * - Deque: better insertions, slower random access
 */
class PmergeMe
{
private:
    std::vector<int> _originalVector;
    std::vector<int> _sortedVector;
    double _timeVector;
    
    std::deque<int> _originalDeque;
    std::deque<int> _sortedDeque;
    double _timeDeque;
    
    // ===== JACOBSTHAL FUNCTIONS =====
    std::vector<int> generateJacobsthal(int max_value);
    std::vector<int> getInsertionOrder(int n_elements);
    
    // ===== HELPER FUNCTIONS =====
    template<typename Container>
    int binarySearchPosition(const Container& sorted_container, int value);
    
    template<typename Container>
    void binaryInsert(Container& sorted_container, int value);
    
    // ===== FORD-JOHNSON FOR VECTOR =====
    std::vector<int> fordJohnsonVector(std::vector<int> array);
    
    // ===== FORD-JOHNSON FOR DEQUE =====
    std::deque<int> fordJohnsonDeque(std::deque<int> array);
    
    // ===== TIMING FUNCTIONS =====
    double getCurrentTime();

public:
    PmergeMe();
    PmergeMe(const PmergeMe& other);
    PmergeMe& operator=(const PmergeMe& other);
    ~PmergeMe();
    
    /**
     * Parse command line arguments into integer sequence
     * Validate that all are positive integers
     */
    bool parseArguments(int argc, char** argv);
    
    /**
     * Sort using both vector and deque with Ford-Johnson
     * Measure and store timing information
     */
    void sort();
    
    /**
     * Display results
     */
    void displayResults();
    
    /**
     * Get sorted vector
     */
    const std::vector<int>& getSortedVector() const;
    
    /**
     * Get sorted deque
     */
    const std::deque<int>& getSortedDeque() const;
};

#endif