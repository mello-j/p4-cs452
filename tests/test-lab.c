#include "harness/unity.h"
#include "../src/lab.h"

// NOTE: Due to the multi-threaded nature of this project. Unit testing for this
// project is limited. I have provided you with a command line tester in
// the file app/main.cp. Be aware that the examples below do not test the
// multi-threaded nature of the queue. You will need to use the command line
// tester to test the multi-threaded nature of your queue. Passing these tests
// does not mean your queue is correct. It just means that it can add and remove
// elements from the queue below the blocking threshold.


void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
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
    TEST_ASSERT_FALSE(is_empty(NULL));
    TEST_ASSERT_FALSE(is_shutdown(NULL));
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

int main(void) {
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

  return UNITY_END();
}
