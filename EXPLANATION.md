# C++ Module 09 - Detailed Explanations

## Overview
This module focuses on the **Standard Template Library (STL)** and advanced algorithms. Each exercise demonstrates different STL containers and their practical applications.

---

## Exercise 00: Bitcoin Exchange 📊

### Concept
A program that calculates Bitcoin values based on historical exchange rates. Given a date and BTC amount, it finds the corresponding exchange rate and calculates the value.

### How It Works

#### 1. **Container Choice: `std::map<std::string, float>`**
```cpp
std::map<std::string, float> _database;
// Key: "2011-01-03" (date string)
// Value: 0.30 (exchange rate)
```

**Why `std::map`?**
- **Automatic sorting**: Dates are stored in chronological order
- **Efficient search**: O(log n) lookup using binary search tree
- **`lower_bound()`**: Perfect for finding "closest lower date" requirement

#### 2. **Algorithm Flow**

```
1. Load Database (data.csv)
   ├─ Parse each line: "date,rate"
   ├─ Validate date format (YYYY-MM-DD)
   ├─ Store in map: date → rate
   └─ Map auto-sorts by date

2. Process Input File
   ├─ Parse each line: "date | value"
   ├─ Validate date format
   ├─ Validate value (0 ≤ value ≤ 1000)
   ├─ Find exchange rate:
   │  ├─ Exact match? Use it
   │  └─ No match? Use lower_bound() to find closest LOWER date
   └─ Calculate: value × exchange_rate
```

#### 3. **Key Feature: Finding Closest Lower Date**

```cpp
std::map<std::string, float>::iterator it = _database.lower_bound(date);

// If exact match found
if (it != _database.end() && it->first == date)
    return it->second;

// If no exact match, go to previous (lower) date
if (it != _database.begin())
{
    --it;
    return it->second;
}
```

**Example:**
```
Database: 2011-01-01 → 0.25
          2011-01-05 → 0.30
          2011-01-10 → 0.35

Query: 2011-01-07
lower_bound() finds: 2011-01-10
Go back one: 2011-01-05 ✓
Use rate: 0.30
```

---

## Exercise 01: Reverse Polish Notation (RPN) 🧮

### Concept
An RPN calculator that evaluates postfix expressions. In RPN, operators come after operands.

**Normal notation**: `(3 + 4) × 5`  
**RPN notation**: `3 4 + 5 ×`

### How It Works

#### 1. **Container Choice: `std::stack<int>`**
```cpp
std::stack<int> _stack;
```

**Why `std::stack`?**
- **LIFO** (Last In, First Out) is the core of RPN
- **Perfect fit**: Push numbers, pop for operations
- **O(1)** push/pop operations

#### 2. **RPN Algorithm**

```
For each token in expression:
    
    IF token is a number:
        ├─ Push onto stack
        └─ Continue
    
    IF token is an operator (+, -, *, /):
        ├─ Pop top two numbers
        ├─ Apply operation
        └─ Push result back
    
    AFTER all tokens:
        └─ Result is the only item left on stack
```

#### 3. **Example: "8 9 * 9 - 9 - 9 - 4 - 1 +"**

```
Token  | Action           | Stack State
-------|------------------|------------------
8      | Push 8           | [8]
9      | Push 9           | [8, 9]
*      | Pop 9,8 → 8×9=72 | [72]
9      | Push 9           | [72, 9]
-      | Pop 9,72 → 72-9  | [63]
9      | Push 9           | [63, 9]
-      | Pop 9,63 → 63-9  | [54]
9      | Push 9           | [54, 9]
-      | Pop 9,54 → 54-9  | [45]
4      | Push 4           | [45, 4]
-      | Pop 4,45 → 45-4  | [41]
1      | Push 1           | [41, 1]
+      | Pop 1,41 → 41+1  | [42]

Result: 42 ✓
```

#### 4. **Operator Order Matters!**

```cpp
int second = _stack.top(); _stack.pop();  // Pop second operand
int first = _stack.top(); _stack.pop();   // Pop first operand
result = first - second;  // NOT second - first!
```

**Why?** Stack is LIFO:
- Push 5, then 3
- Stack: [5, 3] (3 on top)
- For "5 3 -" we want 5 - 3 = 2, not 3 - 5

---

## Exercise 02: PmergeMe (Ford-Johnson Algorithm) 🚀

### Concept
Implement the **Ford-Johnson merge-insertion sort** algorithm using two containers. This is one of the most efficient comparison-based sorting algorithms.

### How It Works

#### 1. **Container Choices**
```cpp
std::vector<int> _sortedVector;  // Contiguous memory, fast access
std::deque<int> _sortedDeque;    // Fast insertions, flexible
```

**Why both?**
- Compare performance characteristics
- Vector: Better cache locality
- Deque: Better insertion performance

---

## 🎯 FORD-JOHNSON ALGORITHM DEEP DIVE

### Overview
The Ford-Johnson algorithm (also called merge-insertion sort) minimizes the number of comparisons needed to sort a sequence. It's **theoretically optimal** for small sequences.

### Phase 1: Pair and Compare

```
Input: [5, 2, 9, 1, 7, 3, 8, 4]

Step 1: Create pairs
        (5,2) (9,1) (7,3) (8,4)

Step 2: Compare within pairs (1 comparison per pair)
        Larger │ Smaller
        ───────┼────────
           5   │   2
           9   │   1
           7   │   3
           8   │   4

Winners = [5, 9, 7, 8]  // The larger elements
Losers  = [2, 1, 3, 4]  // The smaller elements
```

### Phase 2: Recursively Sort Winners

```
Recursively sort winners using Ford-Johnson:
[5, 9, 7, 8] → [5, 7, 8, 9]

Initial sorted sequence: [5, 7, 8, 9]
```

### Phase 3: Insert Losers Using Jacobsthal Order

**This is the KEY optimization!**

#### What are Jacobsthal Numbers?

```
Formula: J(n) = J(n-1) + 2×J(n-2)
Starting: J(0) = 0, J(1) = 1

Sequence: 0, 1, 1, 3, 5, 11, 21, 43, 85, 171, 341, ...
```

#### Why Jacobsthal Numbers?

**Problem:** In what order should we insert the losers?

**Naive approach:** Insert sequentially: [2, 1, 3, 4]
- Binary search positions: [0-1], [0-2], [0-3], [0-4]
- Not optimal!

**Ford-Johnson approach:** Use Jacobsthal gaps
- Maximizes binary search efficiency
- Minimizes number of comparisons
- The gaps align perfectly with binary search tree structure

#### Insertion Order Algorithm

```
For our losers = [2, 1, 3, 4] (indices 0, 1, 2, 3):

1. Generate Jacobsthal: [0, 1, 1, 3, 5, ...]

2. Create insertion order:
   
   k=1: J[1]=1
        Insert from index (1-1)=0 down to J[0]=0
        → Insert index 0 (value 2)
   
   k=2: J[2]=1
        Insert from index (1-1)=0 down to J[1]=1
        → Already done
   
   k=3: J[3]=3
        Insert from index (3-1)=2 down to J[2]=1
        → Insert index 2 (value 3)
        → Insert index 1 (value 1)
   
   Remaining:
        → Insert index 3 (value 4)

Final insertion order: [0, 2, 1, 3]
Which means: Insert [2, 3, 1, 4] in that order
```

### Phase 4: Binary Search Insertion

For each loser in Jacobsthal order, use binary search to find insertion position:

```
Sorted: [5, 7, 8, 9]
Losers: [2, 1, 3, 4]
Order:  [2, 3, 1, 4]  // Jacobsthal-determined order

Step 1: Insert 2
        Binary search in [5, 7, 8, 9]
        → Insert at position 0
        Result: [2, 5, 7, 8, 9]

Step 2: Insert 3
        Binary search in [2, 5, 7, 8, 9]
        → Insert at position 1
        Result: [2, 3, 5, 7, 8, 9]

Step 3: Insert 1
        Binary search in [2, 3, 5, 7, 8, 9]
        → Insert at position 0
        Result: [1, 2, 3, 5, 7, 8, 9]

Step 4: Insert 4
        Binary search in [1, 2, 3, 5, 7, 8, 9]
        → Insert at position 3
        Result: [1, 2, 3, 4, 5, 7, 8, 9]
```

### Complete Example Trace

```
Input: [5, 2, 9, 1, 7, 3]

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
PHASE 1: Pair and Compare
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Pairs: (5,2) (9,1) (7,3)

Compare each pair:
  5 > 2 ✓  →  Winners: 5, Losers: 2
  9 > 1 ✓  →  Winners: 9, Losers: 1
  7 > 3 ✓  →  Winners: 7, Losers: 3

Winners = [5, 9, 7]
Losers  = [2, 1, 3]

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
PHASE 2: Recursively Sort Winners
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Input to recursion: [5, 9, 7]

  Pairs: (5,9) (7,-)
  Winners: [9, 7]
  Losers:  [5]
  
  Sort winners: [7, 9]
  Insert loser 5: [5, 7, 9]

Result: [5, 7, 9]

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
PHASE 3: Generate Jacobsthal Order
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Losers: [2, 1, 3]  (indices: 0, 1, 2)
Jacobsthal: [0, 1, 1, 3, ...]

Insertion order: [0, 2, 1]
Values in order:  [2, 3, 1]

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
PHASE 4: Binary Insert Losers
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Start:    [5, 7, 9]

Insert 2:
  Binary search: 2 < 5
  Insert at 0:  [2, 5, 7, 9]

Insert 3:
  Binary search: 2 < 3 < 5
  Insert at 1:  [2, 3, 5, 7, 9]

Insert 1:
  Binary search: 1 < 2
  Insert at 0:  [1, 2, 3, 5, 7, 9]

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
FINAL RESULT: [1, 2, 3, 5, 7, 9] ✓
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

### Complexity Analysis

#### Comparisons:
- **Pairing phase**: n/2 comparisons
- **Sorting winners**: T(n/2) recursively
- **Binary insertions**: Each loser needs at most log₂(k) comparisons
- **Total**: Approximately **n log n** but with better constants than quicksort

#### Why It's Optimal:
1. **Minimal comparisons**: Close to information-theoretic lower bound
2. **Binary search**: Each insertion uses minimum comparisons
3. **Jacobsthal ordering**: Optimizes the search tree structure
4. **No wasted comparisons**: Every comparison provides maximum information

### Code Structure

```cpp
std::vector<int> fordJohnsonVector(std::vector<int> array)
{
    // Base case
    if (array.size() <= 1)
        return array;
    
    // PHASE 1: Pair and compare
    std::vector<int> winners, losers;
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
        losers.push_back(array.back());
    
    // PHASE 2: Recursively sort winners
    std::vector<int> sorted = fordJohnsonVector(winners);
    
    // PHASE 3: Get Jacobsthal insertion order
    std::vector<int> order = getInsertionOrder(losers.size());
    
    // PHASE 4: Binary insert losers in optimal order
    for (size_t i = 0; i < order.size(); i++)
    {
        int loser = losers[order[i]];
        binaryInsert(sorted, loser);
    }
    
    return sorted;
}
```

### Performance Comparison: Vector vs Deque

```
Time to process a range of 5 elements with std::vector : 8.00000 us
Time to process a range of 5 elements with std::deque  : 9.00000 us
```

**Why the difference?**

**Vector advantages:**
- Contiguous memory → better cache locality
- Fast random access: O(1)
- CPU prefetching works well

**Deque advantages:**
- Fast insertions at both ends: O(1)
- No reallocation needed
- Good for frequent insertions

**In this algorithm:**
- Many binary searches (favor vector)
- Insertions in the middle (favor deque)
- Small data sets (cache matters more)
- Result: Vector often wins for small-medium sizes

---

## Summary

| Exercise | Container(s) | Algorithm | Key Concept |
|----------|-------------|-----------|-------------|
| **ex00** | `std::map` | Binary search tree | Sorted key-value pairs |
| **ex01** | `std::stack` | RPN evaluation | LIFO structure |
| **ex02** | `std::vector` + `std::deque` | Ford-Johnson sort | Optimal comparison sorting |

### Key Takeaways:
1. **Choose containers wisely**: Each has specific strengths
2. **STL provides powerful abstractions**: Focus on algorithm, not implementation
3. **Complexity matters**: Ford-Johnson shows theoretical optimization
4. **Real-world performance**: Cache, memory layout, and operations matter

---

## Testing Commands

```bash
# Exercise 00
./btc input.txt

# Exercise 01
./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"

# Exercise 02
./PmergeMe 3 5 9 7 4
./PmergeMe `shuf -i 1-100000 -n 3000 | tr "\n" " "`
```
