# C++ Module 09 - Quick Reference Card

## Exercise 00: Bitcoin Exchange (btc)

### Container: `std::map<std::string, float>`
```cpp
// Key feature: Automatically sorted keys
std::map<std::string, float> database;
database["2011-01-03"] = 0.30;
database["2011-01-05"] = 0.32;

// Finding closest lower date
auto it = database.lower_bound("2011-01-04");
// Returns iterator to "2011-01-05"
--it;  // Move to previous (lower) date: "2011-01-03"
```

### Algorithm Flow
1. Load CSV database: `date,rate`
2. For each input line: `date | value`
3. Validate date (YYYY-MM-DD) and value (0-1000)
4. Find rate using `lower_bound()` for closest lower date
5. Calculate: `value × rate`

### Key Features
- **Map auto-sorts** by date string
- **lower_bound()** finds exact or next higher
- Move back one for "closest lower"
- O(log n) lookup time

---

## Exercise 01: Reverse Polish Notation (RPN)

### Container: `std::stack<int>`
```cpp
std::stack<int> stack;

// RPN: "3 4 +"
stack.push(3);           // Stack: [3]
stack.push(4);           // Stack: [3, 4]
int b = stack.top(); stack.pop();  // b=4, Stack: [3]
int a = stack.top(); stack.pop();  // a=3, Stack: []
stack.push(a + b);       // Stack: [7]
```

### Algorithm Flow
```
For each token:
    if NUMBER:  push(token)
    if OPERATOR:
        pop two numbers (order matters!)
        apply operation
        push result
Result: final stack top
```

### Key Features
- **LIFO** (Last In, First Out)
- Order matters: `second = pop(); first = pop(); result = first op second;`
- Example: "8 9 * 9 - 9 - 9 - 4 - 1 +" = 42

---

## Exercise 02: PmergeMe (Ford-Johnson Sort)

### Containers: `std::vector<int>` + `std::deque<int>`

### Ford-Johnson Algorithm (4 Phases)

#### Phase 1: Pair and Compare
```
[5, 2, 9, 1, 7, 3, 8, 4]
     ↓
Pairs: (5,2) (9,1) (7,3) (8,4)
     ↓
Winners: [5, 9, 7, 8]  (larger from each pair)
Losers:  [2, 1, 3, 4]  (smaller from each pair)
```

#### Phase 2: Recursively Sort Winners
```
[5, 9, 7, 8] → recursively sort → [5, 7, 8, 9]
```

#### Phase 3: Generate Jacobsthal Insertion Order
```
Jacobsthal: 0, 1, 1, 3, 5, 11, 21, 43, ...
Formula: J(n) = J(n-1) + 2×J(n-2)

For losers [2, 1, 3, 4] (indices 0,1,2,3):
Optimal order: [0, 2, 1, 3] → values [2, 3, 1, 4]
```

#### Phase 4: Binary Insert Each Loser
```
Start:     [5, 7, 8, 9]
Insert 2:  [2, 5, 7, 8, 9]
Insert 3:  [2, 3, 5, 7, 8, 9]
Insert 1:  [1, 2, 3, 5, 7, 8, 9]
Insert 4:  [1, 2, 3, 4, 5, 7, 8, 9] ✓
```

### Key Features
- **Minimizes comparisons** (theoretically optimal)
- **Jacobsthal order** ensures optimal binary search tree traversal
- **Two containers** for performance comparison
- Handles 3000+ elements

### Why Jacobsthal?
- Creates balanced binary search tree structure
- Each insertion has maximum information
- Minimum total comparisons
- Close to theoretical minimum: log₂(n!)

### Vector vs Deque
```
Vector:  [■][■][■][■][■]     Contiguous, great cache
Deque:   [■■] [■■] [■■]      Chunks, better insertions

Small data → Vector wins (cache locality)
Large data → Depends on pattern
```

---

## Container Choice Summary

| Exercise | Container | Why? | Unique Feature |
|----------|-----------|------|----------------|
| ex00 | `std::map` | Sorted keys | `lower_bound()` for closest lower |
| ex01 | `std::stack` | LIFO | Perfect for RPN evaluation |
| ex02 | `std::vector` + `std::deque` | Comparison | Performance benchmarking |

---

## Testing Commands

```bash
# Exercise 00
./btc input.txt

# Exercise 01
./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"    # Output: 42
./RPN "7 7 * 7 -"                     # Output: 42
./RPN "1 2 * 2 / 2 * 2 4 - +"         # Output: 0

# Exercise 02
./PmergeMe 3 5 9 7 4
./PmergeMe `shuf -i 1-100000 -n 3000 | tr "\n" " "`
```

---

## Common Patterns

### Binary Search for Insertion Position
```cpp
template<typename Container>
int binarySearchPosition(const Container& arr, int value)
{
    int left = 0;
    int right = arr.size();
    
    while (left < right)
    {
        int mid = left + (right - left) / 2;
        if (arr[mid] < value)
            left = mid + 1;
        else
            right = mid;
    }
    return left;
}
```

### Map Lower Bound for Closest Lower
```cpp
auto it = map.lower_bound(key);
if (it != map.end() && it->first == key)
    return it->second;  // Exact match

if (it != map.begin())
{
    --it;
    return it->second;  // Closest lower
}
```

### Stack-Based RPN
```cpp
for (each token)
{
    if (isNumber(token))
        stack.push(parseNumber(token));
    else if (isOperator(token))
    {
        int b = stack.top(); stack.pop();
        int a = stack.top(); stack.pop();
        stack.push(apply(a, operator, b));
    }
}
return stack.top();
```

---

## Complexity Analysis

| Operation | ex00 (map) | ex01 (stack) | ex02 (Ford-Johnson) |
|-----------|------------|--------------|---------------------|
| **Search** | O(log n) | O(1) | O(log n) binary search |
| **Insert** | O(log n) | O(1) | O(n) per insertion |
| **Overall** | O(n log n) | O(n) | O(n log n) optimized |
| **Comparisons** | Standard | None | ~Minimal (theoretical optimum) |

---

## Orthodox Canonical Form (OCF)

All classes implement OCF (C++98 requirement):

```cpp
class MyClass {
public:
    MyClass();                              // Default constructor
    MyClass(const MyClass& other);          // Copy constructor
    MyClass& operator=(const MyClass& rhs); // Assignment operator
    ~MyClass();                             // Destructor
};
```

---

## Compilation

```bash
# All exercises use same Makefile structure
make          # Compile
make clean    # Remove objects
make fclean   # Remove objects and binary
make re       # Recompile from scratch

# Flags: -Wall -Wextra -Werror -std=c++98
```
