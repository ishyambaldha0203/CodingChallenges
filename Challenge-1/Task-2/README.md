# Least Recently Used (LRU) Cache Implementation

This project implements a Least Recently Used (LRU) Cache.

## Directory Structure

The Task-2 directory has the following structure:
```
Task-2
├── LRUCache.hpp => LRU cache implementation.
├── Makefile
├── README.md
├── TestLRUCache.cpp => Code to test LRUCache class functionalities
└── Utility.hpp => Some common static utility function like logging.
```
## Job-1: Compile the Program and Explain the Output Produced

### The existing code implements a Least Recently Used (LRU) Cache. Here’s a detailed and ordered explanation of the LRUCache class:

1. LRUCache Class: This class represents the LRU Cache. It uses weak pointers of elements (of LRUCleanable), assuming the ownership is elsewhere. It initiates a cleaning thread that calls cleanup methods of the elements once a soft limit is reached, so they can clean their resources. Also, when adding a new element, if a certain hard limit is reached, cleanup will also be carried out. Weak pointers failed to be locked will simply be removed from the cache upon cleanup.

2. Data Structures: It uses a list (mListOfElements) to keep the order of elements and a map (mMapOfElements) to ease the search of elements. The total size of the cache is stored in mTotalSize.

3. Cleaning Thread: It has a cleaning thread that gets triggered either when the cache size exceeds the soft limit (mMaxSizeSoft) or when a new element is added and the cache size exceeds the hard limit (mMaxSizeHard). The cleaning thread runs every mCleanScheduleMs milliseconds.

4. Update Element: The updateElement function is used to add a new element to the cache or update an existing one. If the element already exists, it is removed from the list and then added back (to the end of the list), thus maintaining the LRU order. If the total size of the cache exceeds the hard limit after the update, the cleanup function is called.

5. Remove Element: The removeElement function is used to remove an element from the cache. It removes the element from both the list and the map, and updates the total size of the cache.

6. Cleanup: The cleanup function is used to remove elements from the cache until the total size is below the soft limit. It starts removing from the front of the list, which are the least recently used elements. However, it does not remove the element that matches the keyToSaveFromPurge.

7. Destructor: The destructor (~LRUCache) ensures that the cleaning thread is properly terminated when the LRUCache object is destroyed.

8. Multithreading: The LRUCache class uses a separate thread (mCleanerThread) to periodically clean up the cache. This thread runs a loop (loopCleaner) that waits for a specified duration (mCleanScheduleMs) or until it’s notified to stop. When it runs, it calls the cleanup function to remove elements from the cache. This allows the cache to be cleaned up in the background without blocking the main thread.

9. Synchronization: The LRUCache class uses several synchronization mechanisms to ensure thread safety.
    - std::mutex: The elementsMutex and mCleanMutex are used to ensure that only one thread can access or modify shared resources at a time. Before accessing or modifying shared resources, a thread must lock the mutex. If another thread has already locked the mutex, the thread will block until the mutex is unlocked.
    - std::lock_guard: This is a mutex wrapper that provides a convenient RAII-style mechanism for owning a mutex for the duration of a scoped block. It’s used in the updateElement, removeElement, cleanup, and end methods to automatically lock the mutex when control enters the scope and unlock it when control leaves the scope. This ensures that the mutex is always properly unlocked, even if an exception is thrown.
    - std::condition_variable: The mCleancv is used to block the cleaning thread until it’s notified to stop or until a timeout occurs. This allows the cleaning thread to sleep when it’s not needed and to be woken up when it’s time to clean up the cache or when the LRUCache object is destroyed.

* The LRUCache implementation prioritizes the most recently used items, removing the least recently used ones when the cache limit is reached. This strategy is beneficial for limiting memory usage while ensuring quick access to relevant data in a thread-safe manner.

* **For testing, The LRUCache is created with a soft limit of 20, a hard limit of 40, and a cache clean schedule time of 1000ms (1 second).**
    - The output demonstrates the LRU Cache behavior. Here's a step-by-step explanation of the output:
        1. **First, Second, Third Elements**: These elements (with primary keys 1, 2, 3 and size 10) are added to the cache and printed out.
        2. After a delay (`sleep(2);`), the first element is cleaned from the cache due to exceeding the limit, resulting in **First Element Cleaned, Second Element, Third Element**.
        3. **Fourth and Fifth Elements** are added, and the second element is updated, triggering another cleanup.
    - Finally, the output is **First Element Cleaned, Second Element, Third Element Cleaned, Fourth Element Cleaned, Fifth Element**, showing that the third and fourth elements have been cleaned, and the second and fifth elements are still in the cache because they were the most recently used.

## Job-2: Extend This Class to Have a Cache That Takes Into Account the Size of an Element and Time

* There are two approaches I considered. Both approaches have their own trade-offs in terms of time and memory complexity:
    1. **Maintaining a Separate Sorted Data Structure**: This approach would likely have better time complexity because operations like insertion, deletion, and finding the maximum element can be done in logarithmic time if we use a balanced binary search tree or a heap-based priority queue. However, it would have higher memory complexity because we’re maintaining an additional data structure that stores pointers to all the elements.
    2. **Using a Tiered System**: This approach would likely have better memory complexity because we’re not maintaining any additional data structures; we’re just grouping the existing elements into different tiers. However, it could have worse time complexity because we might need to scan through multiple tiers to find the elements to remove.
* Given that the note mentions “The cache only stores pointers and a couple of variables, so, memory is not an issue”, it suggests that using additional memory to improve time efficiency is acceptable in this case. Therefore, the first approach (maintaining a separate sorted data structure) might be more suitable here.

## Notes for Job-2 Implementation
1. I enhanced the code readability while preserving the business logic.
2. Significant changes were made in the **cleanup** and **updateElement** functions, where I introduced 'mElementSizeMap' to implement the time threshold feature.
3. Key changes in the LRUCache class include:
    - Addition of `mTimeThresholdSec` Member Variable: Stores the time threshold.
    - Addition of `mElementSizeMap` Member Variable: A multimap that maps each element's size to its primary key, enabling element sorting by size.
    - Modification of Constructor: Now accepts an additional parameter `timeThresholdSec` to set the time threshold during cache creation.
    - Modification of `updateElement` Method: Adds the element to `mElementSizeMap` when added to the cache and removes it when updated.
    - Modification of `cleanup` Method: Removes elements based on their size if they haven’t been accessed for more than the time threshold. Reverts to the LRU strategy if all elements in the cache have been accessed within the time threshold.
    - Addition of `getLastAccessTime` Method to `LRUCacheElement`: Returns the last access time of an element. Used in the `cleanup` method to check if an element’s last access time exceeds the time threshold.
    - Renaming of local variables, member variables, and method names for improved readability.

* These modifications enable the LRUCache class to consider an element's size when cleaning the cache, prioritizing the removal of larger elements that haven’t been accessed for more than a certain time threshold.


## Executing program
* A simple make file is added to build and run the test code, use the provided `Makefile`.

```bash
make
./TestLRUCache
```

## Help
* If any part of the changes needs further clarification, please feel free to ask.

## Important
**If you prefer a version of the code that only includes changes related to size and time consideration for easier understanding, please let me know. I have a separate version where I've made minimal changes, focusing only on the necessary modifications for implementation on top of provided code.**

## Suggestions For Improving and Optimizing
**TODO**

## Valgrind Report
```
$ valgrind --tool=memcheck --leak-check=yes ./TestLRUCache

==84878== HEAP SUMMARY:
==84878==     in use at exit: 0 bytes in 0 blocks
==84878==   total heap usage: 428 allocs, 428 frees, 40,190 bytes allocated
==84878== 
==84878== All heap blocks were freed -- no leaks are possible
==84878== 
==84878== For lists of detected and suppressed errors, rerun with: -s
==84878== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
