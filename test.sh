#!/bin/bash
# This script runs a series of tests on the queue implementation

# Counter for tests and failures
TOTAL_TESTS=0
FAILED_TESTS=0

# Function to run a test with specific parameters
run_test() {
    local consumers=$1
    local producers=$2
    local items=$3
    local queue_size=$4
    local delay=$5
    local test_name="c${consumers}_p${producers}_i${items}_s${queue_size}${delay:+_d}"
    local command="./myprogram -c $consumers -p $producers -i $items -s $queue_size $delay"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # Capture both stdout and stderr
    output=$(eval "$command" 2>&1)
    
    # Check for errors or mismatches
    if echo "$output" | grep -q "ERROR"; then
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo "FAILED: $test_name"
        echo "Command: $command"
        echo "$output" | grep "ERROR"
        return 1
    fi
    
    # Extract produced and consumed counts
    produced=$(echo "$output" | grep "Total produced:" | awk '{print $3}')
    consumed=$(echo "$output" | grep "Total consumed:" | awk '{print $3}')
    
    # Check if counts match
    if [ "$produced" != "$consumed" ]; then
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo "FAILED: $test_name - Produced ($produced) != Consumed ($consumed)"
        echo "Command: $command"
        return 1
    fi
    
    # Check if queue is empty at the end
    if ! echo "$output" | grep -q "Queue is empty:true"; then
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo "FAILED: $test_name - Queue is not empty at the end"
        echo "Command: $command"
        return 1
    fi
    
    return 0
}

# Make sure we have a working executable
if [ ! -x ./myprogram ]; then
    echo "Error: ./myprogram not found or not executable"
    echo "Make sure you've compiled the program with 'make'"
    exit 1
fi

echo "Starting queue test suite..."

# Test series 1: Varying consumers (1-8) with fixed producers, items, and queue size
for c in 1 2 4 8; do
    run_test $c 1 100 10 ""
done

# Test series 2: Varying producers (1-8) with fixed consumers, items, and queue size
for p in 1 2 4 8; do
    run_test 4 $p 100 10 ""
done

# Test series 3: Varying items with fixed consumers, producers, and queue size
for i in 10 100 1000; do
    run_test 4 4 $i 10 ""
done

# Test series 4: Varying queue sizes with fixed consumers, producers, and items
for s in 1 5 20 50; do
    run_test 4 4 100 $s ""
done

# Test series 5: Tests with delays (more likely to reveal race conditions)
for c in 2 4 8; do
    for p in 2 4 8; do
        run_test $c $p 100 10 "-d"
    done
done

# Test series 6: Edge cases
run_test 8 8 1000 1 ""      # Minimum queue size
run_test 1 8 100 10 "-d"    # Many producers, one consumer
run_test 8 1 100 10 "-d"    # One producer, many consumers

# Print summary
echo ""
echo "Test summary:"
echo "Total tests: $TOTAL_TESTS"
echo "Failed tests: $FAILED_TESTS"

if [ $FAILED_TESTS -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed. Check output above for details."
    exit 1
fi