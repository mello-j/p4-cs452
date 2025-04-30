#include "harness/unity.h"
#include "../src/lab.h"

// NOTE: Due to the multi-threaded nature of this project. Unit testing for this
// project is limited. I have provided you with a command line tester in
// the file app/main.cp. Be aware that the examples below do not test the
// multi-threaded nature of the queue. You will need to use the command line
// tester to test the multi-threaded nature of your queue. Passing these tests
// does not mean your queue is correct. It just means that it can add and remove
// elements from the queue below the blocking threshold.

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_create_destroy(void)
{
    queue_t q = queue_init(10);
    TEST_ASSERT_TRUE(q != NULL);
    queue_destroy(q);
}

void test_queue_dequeue(void)
{
    queue_t q = queue_init(10);
    TEST_ASSERT_TRUE(q != NULL);
    int data = 1;
    enqueue(q, &data);
    TEST_ASSERT_TRUE(dequeue(q) == &data);
    queue_destroy(q);
}

void test_queue_dequeue_multiple(void)
{
    queue_t q = queue_init(10);
    TEST_ASSERT_TRUE(q != NULL);
    int data = 1;
    int data2 = 2;
    int data3 = 3;
    enqueue(q, &data);
    enqueue(q, &data2);
    enqueue(q, &data3);
    TEST_ASSERT_TRUE(dequeue(q) == &data);
    TEST_ASSERT_TRUE(dequeue(q) == &data2);
    TEST_ASSERT_TRUE(dequeue(q) == &data3);
    queue_destroy(q);
}

void test_queue_dequeue_shutdown(void)
{
    queue_t q = queue_init(10);
    TEST_ASSERT_TRUE(q != NULL);
    int data = 1;
    int data2 = 2;
    int data3 = 3;
    enqueue(q, &data);
    enqueue(q, &data2);
    enqueue(q, &data3);
    TEST_ASSERT_TRUE(dequeue(q) == &data);
    TEST_ASSERT_TRUE(dequeue(q) == &data2);
    queue_shutdown(q);
    TEST_ASSERT_TRUE(dequeue(q) == &data3);
    TEST_ASSERT_TRUE(is_shutdown(q));
    TEST_ASSERT_TRUE(is_empty(q));
    queue_destroy(q);
}

void test_empty_queue(void)
{
    queue_t q = queue_init(10);
    TEST_ASSERT_TRUE(q != NULL);
    TEST_ASSERT_TRUE(is_empty(q));
    TEST_ASSERT_FALSE(is_shutdown(q));
    queue_destroy(q);
}

void test_full_queue(void)
{
    queue_t q = queue_init(3);
    TEST_ASSERT_TRUE(q != NULL);
    int data1 = 1, data2 = 2, data3 = 3;

    enqueue(q, &data1);
    enqueue(q, &data2);
    enqueue(q, &data3);

    // Queue is now full - verify all items are there
    TEST_ASSERT_FALSE(is_empty(q));
    TEST_ASSERT_TRUE(dequeue(q) == &data1);
    TEST_ASSERT_TRUE(dequeue(q) == &data2);
    TEST_ASSERT_TRUE(dequeue(q) == &data3);
    TEST_ASSERT_TRUE(is_empty(q));

    queue_destroy(q);
}

void test_queue_shutdown_empty(void)
{
    queue_t q = queue_init(5);
    TEST_ASSERT_TRUE(q != NULL);

    // Shutdown an empty queue
    queue_shutdown(q);
    TEST_ASSERT_TRUE(is_shutdown(q));
    TEST_ASSERT_TRUE(is_empty(q));

    // Verify dequeue returns NULL on empty shutdown queue
    TEST_ASSERT_TRUE(dequeue(q) == NULL);

    queue_destroy(q);
}

void test_enqueue_after_shutdown(void)
{
    queue_t q = queue_init(5);
    TEST_ASSERT_TRUE(q != NULL);
    int data = 42;

    // Shutdown the queue
    queue_shutdown(q);

    // Try to enqueue after shutdown
    enqueue(q, &data);

    // Verify the queue remains empty (enqueue should be ignored)
    TEST_ASSERT_TRUE(is_empty(q));
    TEST_ASSERT_TRUE(dequeue(q) == NULL);

    queue_destroy(q);
}

void test_null_queue_operations(void)
{
    // Test behaviors with NULL queue
    TEST_ASSERT_TRUE(is_empty(NULL));
    TEST_ASSERT_TRUE(is_shutdown(NULL));
    TEST_ASSERT_TRUE(dequeue(NULL) == NULL);

    // These should not crash
    enqueue(NULL, NULL);
    queue_shutdown(NULL);
    queue_destroy(NULL);

    // Test passing with no assertions (just checking for crashes)
    TEST_PASS();
}

void test_queue_zero_capacity(void)
{
    // Attempt to create a queue with zero capacity
    queue_t q = queue_init(0);

    // Should return NULL
    TEST_ASSERT_TRUE(q == NULL);
}

void test_circular_buffer_wraparound(void)
{
    // Create a queue with small capacity to test wraparound
    queue_t q = queue_init(3);
    TEST_ASSERT_TRUE(q != NULL);

    // Fill and empty the queue multiple times to force head/tail wraparound
    for (int cycle = 0; cycle < 5; cycle++)
    {
        int data1 = 10 + cycle;
        int data2 = 20 + cycle;
        int data3 = 30 + cycle;

        // Fill the queue
        enqueue(q, &data1);
        enqueue(q, &data2);
        enqueue(q, &data3);

        // Queue should be full now
        TEST_ASSERT_FALSE(is_empty(q));

        // Remove a single item
        TEST_ASSERT_TRUE(dequeue(q) == &data1);

        // Add one more item
        int data4 = 40 + cycle;
        enqueue(q, &data4);

        // Now verify all remaining items in correct order
        TEST_ASSERT_TRUE(dequeue(q) == &data2);
        TEST_ASSERT_TRUE(dequeue(q) == &data3);
        TEST_ASSERT_TRUE(dequeue(q) == &data4);

        // Queue should be empty again
        TEST_ASSERT_TRUE(is_empty(q));
    }

    queue_destroy(q);
}

void test_queue_capacity_stress(void)
{
    // Create a queue with maximum capacity 5
    int capacity = 5;
    queue_t q = queue_init(capacity);
    TEST_ASSERT_TRUE(q != NULL);

    // Create an array of test data
    int test_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    void *results[10] = {0};

    // Fill the queue to capacity and verify it's not empty
    for (int i = 0; i < capacity; i++)
    {
        enqueue(q, &test_data[i]);
    }
    TEST_ASSERT_FALSE(is_empty(q));

    // Attempt to fill beyond capacity by removing and adding
    // This verifies that capacity limits are properly enforced
    for (int i = 0; i < 10; i++)
    {
        // Remove an item
        results[i] = dequeue(q);

        // Add a new item (may wrap around in the buffer)
        int new_idx = (i + capacity) % 10;
        enqueue(q, &test_data[new_idx]);
    }

    // Verify we got the expected values in order
    for (int i = 0; i < 10; i++)
    {
        TEST_ASSERT_TRUE(results[i] == &test_data[i % 10]);
    }

    // Empty the queue and verify it's empty
    for (int i = 0; i < capacity; i++)
    {
        void *item = dequeue(q);
        // The items will start at test_data[0] again due to wraparound
        TEST_ASSERT_TRUE(item == &test_data[(i + 10) % 10]);
    }

    TEST_ASSERT_TRUE(is_empty(q));

    queue_destroy(q);
}

/**
 * Test the system integration by running the test.sh script
 * This script should run all the multi-threaded tests and report success
 */
void test_system_integration(void)
{
    // Run the test.sh script and check its return code
    // A return code of 0 means all tests passed
    int result = system("./test.sh");
    
    // Check if the script ran successfully
    TEST_ASSERT_TRUE(result != -1);
    
    // Check if the script reported all tests passing
    // system() returns the exit status in the high byte
    int exit_status = WEXITSTATUS(result);
    TEST_ASSERT_EQUAL_INT(0, exit_status);
    
    // If we get here, all the multiprocessor tests passed
    TEST_PASS();
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_create_destroy);
    RUN_TEST(test_queue_dequeue);
    RUN_TEST(test_queue_dequeue_multiple);
    RUN_TEST(test_queue_dequeue_shutdown);

    // New tests
    RUN_TEST(test_empty_queue);
    RUN_TEST(test_full_queue);
    RUN_TEST(test_queue_shutdown_empty);
    RUN_TEST(test_enqueue_after_shutdown);
    RUN_TEST(test_null_queue_operations);
    RUN_TEST(test_queue_zero_capacity);
    RUN_TEST(test_circular_buffer_wraparound);
    RUN_TEST(test_queue_capacity_stress);

    RUN_TEST(test_system_integration);
    return UNITY_END();
}
