#include "PmergeMe.hpp"
#include <cmath>
#include <iomanip>

PmergeMe::PmergeMe() : _timeVector(0), _timeDeque(0)
{
}

PmergeMe::PmergeMe(const PmergeMe& other)
{
    _originalVector = other._originalVector;
    _sortedVector = other._sortedVector;
    _timeVector = other._timeVector;
    _originalDeque = other._originalDeque;
    _sortedDeque = other._sortedDeque;
    _timeDeque = other._timeDeque;
}

PmergeMe& PmergeMe::operator=(const PmergeMe& other)
{
    if (this != &other)
    {
        _originalVector = other._originalVector;
        _sortedVector = other._sortedVector;
        _timeVector = other._timeVector;
        _originalDeque = other._originalDeque;
        _sortedDeque = other._sortedDeque;
        _timeDeque = other._timeDeque;
    }
    return *this;
}

PmergeMe::~PmergeMe()
{
}

// ============================================================================
// JACOBSTHAL NUMBER GENERATION
// ============================================================================

/**
 * Generate Jacobsthal numbers up to max_value
 * 
 * Formula: J(n) = J(n-1) + 2 * J(n-2)
 * With J(0) = 0, J(1) = 1
 * 
 * Sequence: [0, 1, 1, 3, 5, 11, 21, 43, 85, 171, 341, 683, ...]
 * 
 * These numbers determine the optimal insertion order in Ford-Johnson
 */
std::vector<int> PmergeMe::generateJacobsthal(int max_value)
{
    std::vector<int> jacobsthal;
    jacobsthal.push_back(0);
    jacobsthal.push_back(1);
    
    while (jacobsthal.back() < max_value)
    {
        int size = jacobsthal.size();
        int next = jacobsthal[size - 1] + 2 * jacobsthal[size - 2];
        jacobsthal.push_back(next);
    }
    
    return jacobsthal;
}

/**
 * Generate insertion order based on Jacobsthal numbers
 * 
 * Why this works:
 * - We insert elements in gaps defined by Jacobsthal numbers
 * - This maximizes binary search efficiency
 * - Fewer comparisons needed than sequential insertion
 * 
 * Algorithm:
 * - Start with Jacobsthal number J[k]
 * - Insert elements from J[k]-1 down to J[k-1]
 * - Move to next J[k] and repeat
 * - Handle remaining elements
 * 
 * Example for 4 elements:
 * J = [0, 1, 1, 3, 5, ...]
 * Order: [0, 2, 1, 3] (not [0, 1, 2, 3])
 */
std::vector<int> PmergeMe::getInsertionOrder(int n_elements)
{
    std::vector<int> order;
    std::vector<bool> used(n_elements, false);
    
    // Generate Jacobsthal numbers
    std::vector<int> J = generateJacobsthal(n_elements);
    
    // Create insertion order using Jacobsthal gaps
    for (int k = 1; k < (int)J.size() && J[k] <= n_elements; k++)
    {
        // Process from J[k]-1 down to J[k-1]
        int start = (J[k] <= n_elements) ? J[k] - 1 : n_elements - 1;
        int end = (k > 1) ? J[k - 1] : 0;
        
        for (int j = start; j >= end; j--)
        {
            if (!used[j])
            {
                order.push_back(j);
                used[j] = true;
            }
        }
    }
    
    // Handle any remaining unused elements
    for (int i = 0; i < n_elements; i++)
    {
        if (!used[i])
        {
            order.push_back(i);
        }
    }
    
    return order;
}

// ============================================================================
// BINARY SEARCH AND INSERTION
// ============================================================================

/**
 * Binary search for insertion position
 * 
 * Why binary search?
 * - O(log n) instead of O(n) linear search
 * - Ford-Johnson specifically uses this for efficiency
 * 
 * Algorithm:
 * 1. Start with left=0, right=size
 * 2. Find middle point
 * 3. If middle element < value: search right half
 * 4. Else: search left half
 * 5. Return position where value should be inserted
 * 
 * Returns: index where value should be inserted to maintain sorted order
 */
template<typename Container>
int PmergeMe::binarySearchPosition(const Container& sorted_container, int value)
{
    int left = 0;
    int right = sorted_container.size();
    
    while (left < right)
    {
        int mid = left + (right - left) / 2;
        
        if (sorted_container[mid] < value)
        {
            left = mid + 1;
        }
        else
        {
            right = mid;
        }
    }
    
    return left;
}

/**
 * Insert value at correct position using binary search
 * 
 * Steps:
 * 1. Use binary search to find correct position
 * 2. Insert at that position (maintaining sorted order)
 * 
 * Note: Different efficiency for vector vs deque
 * - Vector: insert() is slow (O(n)) at arbitrary positions
 * - Deque: insert() is better for beginning/middle positions
 */
template<typename Container>
void PmergeMe::binaryInsert(Container& sorted_container, int value)
{
    int pos = binarySearchPosition(sorted_container, value);
    sorted_container.insert(
        sorted_container.begin() + pos,
        value
    );
}

// ============================================================================
// FORD-JOHNSON ALGORITHM FOR VECTOR
// ============================================================================

/**
 * Ford-Johnson Algorithm for std::vector
 * 
 * Complete Algorithm:
 * 
 * STEP 1: PAIRING AND COMPARISON
 * - Pair consecutive elements
 * - Compare within each pair
 * - Separate into winners (larger) and losers (smaller)
 * - Count: n/2 comparisons
 * 
 * STEP 2: RECURSIVE SORT
 * - Recursively sort the winners
 * - Base case: single element or small array (insertion sort)
 * 
 * STEP 3: INSERTION OF LOSERS
 * - Insert losers in Jacobsthal-optimal order
 * - Use binary search for each insertion
 * - Minimize total comparisons
 * 
 * STEP 4: INSERT PENDING
 * - If original array was odd-sized, insert last element
 * 
 * Why it's efficient:
 * - Minimizes comparisons vs quicksort/mergesort
 * - Theoretically optimal for comparison-based sorting
 * - Trade-off: more complex than simple algorithms
 */
std::vector<int> PmergeMe::fordJohnsonVector(std::vector<int> array)
{
    // BASE CASE: Arrays of size 0 or 1 are already sorted
    if (array.size() <= 1)
        return array;
    
    // ===== STEP 1: PAIRING AND COMPARISON =====
    std::vector<int> winners;  // Larger elements from each pair
    std::vector<int> losers;   // Smaller elements from each pair
    int pending = -1;          // For odd-numbered arrays
    
    for (size_t i = 0; i + 1 < array.size(); i += 2)
    {
        if (array[i] > array[i + 1])
        {
            winners.push_back(array[i]);
            losers.push_back(array[i + 1]);
        }
        else
        {
            winners.push_back(array[i + 1]);
            losers.push_back(array[i]);
        }
    }
    
    // Handle odd element
    if (array.size() % 2 == 1)
    {
        pending = array.back();
    }
    
    // ===== STEP 2: RECURSIVELY SORT WINNERS =====
    std::vector<int> sorted_winners = fordJohnsonVector(winners);
    
    // ===== STEP 3: INSERT LOSERS IN JACOBSTHAL ORDER =====
    
    // Generate optimal insertion order
    std::vector<int> insertion_order = getInsertionOrder(losers.size());
    
    // Insert each loser in optimal order using binary search
    for (size_t i = 0; i < insertion_order.size(); i++)
    {
        int loser_index = insertion_order[i];
        binaryInsert(sorted_winners, losers[loser_index]);
    }
    
    // ===== STEP 4: INSERT PENDING ELEMENT =====
    if (pending != -1)
    {
        binaryInsert(sorted_winners, pending);
    }
    
    return sorted_winners;
}

// ============================================================================
// FORD-JOHNSON ALGORITHM FOR DEQUE
// ============================================================================

/**
 * Ford-Johnson Algorithm for std::deque
 * 
 * Same algorithm as vector version, but using deque operations
 * 
 * Differences from vector:
 * - Deque insert/erase is more efficient at beginning
 * - Random access is slightly slower
 * - Overall should be similar performance
 * 
 * Purpose: Compare performance between containers
 */
std::deque<int> PmergeMe::fordJohnsonDeque(std::deque<int> array)
{
    // BASE CASE
    if (array.size() <= 1)
        return array;
    
    // ===== STEP 1: PAIRING AND COMPARISON =====
    std::deque<int> winners;
    std::deque<int> losers;
    int pending = -1;
    
    for (size_t i = 0; i + 1 < array.size(); i += 2)
    {
        if (array[i] > array[i + 1])
        {
            winners.push_back(array[i]);
            losers.push_back(array[i + 1]);
        }
        else
        {
            winners.push_back(array[i + 1]);
            losers.push_back(array[i]);
        }
    }
    
    if (array.size() % 2 == 1)
    {
        pending = array.back();
    }
    
    // ===== STEP 2: RECURSIVELY SORT WINNERS =====
    std::deque<int> sorted_winners = fordJohnsonDeque(winners);
    
    // ===== STEP 3: INSERT LOSERS IN JACOBSTHAL ORDER =====
    std::vector<int> insertion_order = getInsertionOrder(losers.size());
    
    for (size_t i = 0; i < insertion_order.size(); i++)
    {
        int loser_index = insertion_order[i];
        binaryInsert(sorted_winners, losers[loser_index]);
    }
    
    // ===== STEP 4: INSERT PENDING ELEMENT =====
    if (pending != -1)
    {
        binaryInsert(sorted_winners, pending);
    }
    
    return sorted_winners;
}

// ============================================================================
// TIMING FUNCTIONS
// ============================================================================

/**
 * Get current time in microseconds
 * Uses gettimeofday for precise timing
 */
double PmergeMe::getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000.0) + tv.tv_usec;
}
#include "PmergeMe.hpp"
#include <iomanip>

/**
 * Parse command line arguments
 * 
 * Rules (from subject):
 * - All arguments must be positive integers
 * - No negative numbers allowed
 * - Can handle at least 3000 different integers
 * 
 * Algorithm:
 * 1. Iterate through all arguments (skip program name at argv[0])
 * 2. For each argument:
 *    a. Check if it's a valid integer
 *    b. Check if it's positive
 *    c. Store in both vector and deque
 * 3. Handle errors gracefully
 */
bool PmergeMe::parseArguments(int argc, char** argv)
{
    // Need at least one argument
    if (argc < 2)
        return false;
    
    // Parse each argument
    for (int i = 1; i < argc; i++)
    {
        char* endptr;
        long num = std::strtol(argv[i], &endptr, 10);
        
        // Check if conversion was valid
        if (*endptr != '\0')
        {
            std::cerr << "Error: invalid argument \"" << argv[i] << "\"" << std::endl;
            return false;
        }
        
        // Check if positive
        if (num <= 0)
        {
            std::cerr << "Error" << std::endl;
            return false;
        }
        
        // Store in both containers
        int int_num = static_cast<int>(num);
        _originalVector.push_back(int_num);
        _originalDeque.push_back(int_num);
    }
    
    return true;
}

/**
 * Sort using both containers with Ford-Johnson algorithm
 * 
 * Process:
 * 1. Make copies of original data
 * 2. Time the vector sort
 * 3. Time the deque sort
 * 4. Store results
 * 
 * Why we measure both?
 * - Compare performance characteristics
 * - Vector vs Deque: different trade-offs
 * - Show understanding of containers
 */
void PmergeMe::sort()
{
    // ===== SORT WITH VECTOR =====
    double start_time = getCurrentTime();
    _sortedVector = fordJohnsonVector(_originalVector);
    double end_time = getCurrentTime();
    _timeVector = end_time - start_time;
    
    // ===== SORT WITH DEQUE =====
    start_time = getCurrentTime();
    _sortedDeque = fordJohnsonDeque(_originalDeque);
    end_time = getCurrentTime();
    _timeDeque = end_time - start_time;
}

/**
 * Display results in required format
 * 
 * Output format:
 * Before: [original sequence]
 * After: [sorted sequence]
 * Time to process a range of N elements with std::vector : X.XXXXX us
 * Time to process a range of N elements with std::deque : X.XXXXX us
 * 
 * Important notes:
 * - "us" means microseconds
 * - Show precision to see difference between containers
 * - Truncate sequence display if too large (> 20 elements)
 */
void PmergeMe::displayResults()
{
    int size = _originalVector.size();
    
    // ===== DISPLAY ORIGINAL SEQUENCE =====
    std::cout << "Before: ";
    for (int i = 0; i < size; i++)
    {
        if (i > 0)
            std::cout << " ";
        
        // Truncate display for large sequences
        if (i >= 20)
        {
            std::cout << "[...]";
            break;
        }
        
        std::cout << _originalVector[i];
    }
    std::cout << std::endl;
    
    // ===== DISPLAY SORTED SEQUENCE =====
    std::cout << "After: ";
    for (int i = 0; i < size; i++)
    {
        if (i > 0)
            std::cout << " ";
        
        // Truncate display for large sequences
        if (i >= 20)
        {
            std::cout << "[...]";
            break;
        }
        
        std::cout << _sortedVector[i];
    }
    std::cout << std::endl;
    
    // ===== DISPLAY TIMING FOR VECTOR =====
    std::cout << "Time to process a range of " << size 
              << " elements with std::vector : ";
    std::cout << std::fixed << std::setprecision(5) 
              << _timeVector << " us" << std::endl;
    
    // ===== DISPLAY TIMING FOR DEQUE =====
    std::cout << "Time to process a range of " << size 
              << " elements with std::deque : ";
    std::cout << std::fixed << std::setprecision(5) 
              << _timeDeque << " us" << std::endl;
}

/**
 * Getter: sorted vector
 */
const std::vector<int>& PmergeMe::getSortedVector() const
{
    return _sortedVector;
}

/**
 * Getter: sorted deque
 */
const std::deque<int>& PmergeMe::getSortedDeque() const
{
    return _sortedDeque;
}