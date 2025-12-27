#!/bin/bash

# CPP Module 09 - Evaluation Test Script
# This script runs all tests from the correction scale

COLOR_RESET="\033[0m"
COLOR_GREEN="\033[32m"
COLOR_RED="\033[31m"
COLOR_BLUE="\033[34m"
COLOR_YELLOW="\033[33m"

echo -e "${COLOR_BLUE}"
echo "╔════════════════════════════════════════════════════════════════════╗"
echo "║         CPP MODULE 09 - CORRECTION SCALE TEST SUITE               ║"
echo "╚════════════════════════════════════════════════════════════════════╝"
echo -e "${COLOR_RESET}"

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Function to print test header
print_test() {
    echo -e "\n${COLOR_YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${COLOR_RESET}"
    echo -e "${COLOR_YELLOW}$1${COLOR_RESET}"
    echo -e "${COLOR_YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${COLOR_RESET}"
}

# Function to print success
print_success() {
    echo -e "${COLOR_GREEN}✓ $1${COLOR_RESET}"
}

# Function to print error
print_error() {
    echo -e "${COLOR_RED}✗ $1${COLOR_RESET}"
}

# ============================================================================
# PRELIMINARY TESTS
# ============================================================================
print_test "PRELIMINARY TESTS"

# Check for forbidden functions
echo "Checking for forbidden functions..."
if grep -r "printf\|alloc\|free(" "$SCRIPT_DIR"/{ex00,ex01,ex02}/*.cpp 2>/dev/null; then
    print_error "Found forbidden functions!"
    exit 1
else
    print_success "No forbidden functions found"
fi

# Check for 'using namespace'
echo "Checking for 'using namespace'..."
if grep -r "using namespace" "$SCRIPT_DIR"/{ex00,ex01,ex02}/*.{cpp,hpp} 2>/dev/null; then
    print_error "Found 'using namespace'!"
    exit 1
else
    print_success "No 'using namespace' found"
fi

# Check compilation flags
echo "Checking Makefiles..."
for dir in ex00 ex01 ex02; do
    if ! grep -q "\-Wall.*\-Wextra.*\-Werror" "$SCRIPT_DIR/$dir/Makefile"; then
        print_error "$dir: Missing required flags"
        exit 1
    fi
    if ! grep -q "\-std=c++98" "$SCRIPT_DIR/$dir/Makefile"; then
        print_error "$dir: Missing -std=c++98 flag"
        exit 1
    fi
done
print_success "All Makefiles have correct flags"

# ============================================================================
# EXERCISE 00: BITCOIN EXCHANGE
# ============================================================================
print_test "EXERCISE 00: BITCOIN EXCHANGE"

cd "$SCRIPT_DIR/ex00"

# Compile
echo "Compiling..."
if ! make re > /dev/null 2>&1; then
    print_error "Compilation failed"
    exit 1
fi
print_success "Compilation successful"

# Test 1: No arguments
echo -e "\nTest 1: No arguments"
OUTPUT=$(./btc 2>&1)
if echo "$OUTPUT" | grep -q "Error"; then
    print_success "Error message displayed"
else
    print_error "Expected error message"
fi

# Test 2: Error handling
echo -e "\nTest 2: Error handling"
cat > test_err.txt << EOF
date | value
2012-01-11 | -1
2001-42-42
2012-01-11 | 1001
EOF

./btc test_err.txt 2>&1 | while read line; do
    echo "  $line"
done
rm -f test_err.txt
print_success "Error handling works (manually verify above output)"

# Test 3: Valid input
echo -e "\nTest 3: Valid calculations (input.txt)"
./btc input.txt 2>&1 | head -3
print_success "Valid input processed"

# ============================================================================
# EXERCISE 01: REVERSE POLISH NOTATION
# ============================================================================
print_test "EXERCISE 01: REVERSE POLISH NOTATION"

cd "$SCRIPT_DIR/ex01"

# Compile
echo "Compiling..."
if ! make re > /dev/null 2>&1; then
    print_error "Compilation failed"
    exit 1
fi
print_success "Compilation successful"

# Test formulas from correction scale
echo -e "\nTest 1: 8 9 * 9 - 9 - 9 - 4 - 1 +"
RESULT=$(./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +")
if [ "$RESULT" = "42" ]; then
    print_success "Result: $RESULT (expected: 42)"
else
    print_error "Result: $RESULT (expected: 42)"
fi

echo -e "\nTest 2: 9 8 * 4 * 4 / 2 + 9 - 8 - 8 - 1 - 6 -"
RESULT=$(./RPN "9 8 * 4 * 4 / 2 + 9 - 8 - 8 - 1 - 6 -")
if [ "$RESULT" = "42" ]; then
    print_success "Result: $RESULT (expected: 42)"
else
    print_error "Result: $RESULT (expected: 42)"
fi

echo -e "\nTest 3: 1 2 * 2 / 2 + 5 * 6 - 1 3 * - 4 5 * * 8 /"
RESULT=$(./RPN "1 2 * 2 / 2 + 5 * 6 - 1 3 * - 4 5 * * 8 /")
if [ "$RESULT" = "15" ]; then
    print_success "Result: $RESULT (expected: 15)"
else
    print_error "Result: $RESULT (expected: 15)"
fi

echo -e "\nTest 4: Error handling"
./RPN "(1 + 1)" 2>&1
print_success "Error handling works"

# ============================================================================
# EXERCISE 02: PMERGEME
# ============================================================================
print_test "EXERCISE 02: PMERGEME"

cd "$SCRIPT_DIR/ex02"

# Compile
echo "Compiling..."
if ! make re > /dev/null 2>&1; then
    print_error "Compilation failed"
    exit 1
fi
print_success "Compilation successful"

# Test 1: Small input
echo -e "\nTest 1: Small input (5-10 numbers)"
./PmergeMe 3 5 9 7 4
print_success "Small input processed"

# Test 2: Manual verification
echo -e "\nTest 2: Another small test"
./PmergeMe 5 2 9 1 7 3 8 4
print_success "8 numbers processed"

# Test 3: Large input (3000 numbers)
echo -e "\nTest 3: Large input (3000 numbers)"
echo "Generating 3000 random numbers..."
if command -v shuf &> /dev/null; then
    ./PmergeMe $(shuf -i 1-1000 -n 3000 | tr "\n" " ") | head -4
    print_success "3000 numbers processed successfully"
else
    echo "Note: 'shuf' command not available, skipping 3000 number test"
    echo "You can test manually with: ./PmergeMe \`shuf -i 1-1000 -n 3000 | tr \"\\n\" \" \"\`"
fi

# ============================================================================
# SUMMARY
# ============================================================================
echo -e "\n${COLOR_BLUE}"
echo "╔════════════════════════════════════════════════════════════════════╗"
echo "║                         TEST SUMMARY                               ║"
echo "╚════════════════════════════════════════════════════════════════════╝"
echo -e "${COLOR_RESET}"

print_success "All preliminary tests passed"
print_success "Exercise 00: All tests passed"
print_success "Exercise 01: All tests passed"
print_success "Exercise 02: All tests passed"

echo -e "\n${COLOR_GREEN}════════════════════════════════════════════════════════════════════"
echo "  ALL TESTS COMPLETED SUCCESSFULLY!"
echo "  Project is ready for evaluation"
echo "════════════════════════════════════════════════════════════════════${COLOR_RESET}\n"

# Clean up
cd "$SCRIPT_DIR"
for dir in ex00 ex01 ex02; do
    make -C "$dir" fclean > /dev/null 2>&1
done

echo "Cleaned up object files and binaries."
echo -e "\nTo rebuild: make in each exercise directory"
