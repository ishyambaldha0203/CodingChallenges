/**************************************************************************************************
 * @file TestLRUCache.cpp
 *
 * @brief This file contains tests for the LRUCache class.
 **************************************************************************************************/

#include <thread>
#include <chrono>

#include "LRUCache.hpp"
#include "Utility.hpp"

namespace 
{
    /**
     * @class TestElement
     * @brief A class that represents a test element in the LRUCache.
     * @details This class inherits from LRUCacheCleanable and overrides the cleanup method.
     */
    class TestElement : public LRUCacheCleanable 
    {
    private:
        std::string mName; ///< The name of the test element.
        int mId; ///< The ID of the test element.
        int64_t mSize; ///< The size of the test element.

    public:
        /**
         * @brief Constructor for the TestElement class.
         * @param name The name of the test element.
         * @param id The ID of the test element.
         * @param size The size of the test element.
         */
        TestElement(const std::string& name, int id, int64_t size) : mName(name), mId(id), mSize(size) {}

        /**
         * @brief Prints the name of the test element.
         */
        void print() const { std::cout << mName << std::endl; }

        /**
         * @brief Gets the ID of the test element.
         * @return The ID of the test element.
         */
        int getId() const { return mId; }

        /**
         * @brief Gets the size of the test element.
         * @return The size of the test element.
         */
        int64_t getSize() const { return mSize; }

        /**
         * @brief Cleans up the test element by setting its size to 0 and appending " cleaned" to its name.
         */
        void cleanup() override
        {
            mSize = 0;
            mName += " cleaned";
        }
    };

    /**
     * @brief Creates a test element and updates it in the cache.
     * @param name The name of the test element.
     * @param cache The cache in which to update the test element.
     * @param size The size of the test element.
     * @return A shared pointer to the created test element.
     */
    std::shared_ptr<TestElement> createElement(const std::string &name, LRUCache<TestElement, int>& cache, int64_t size)
    {
        static int id = 0;
        auto element = std::make_shared<TestElement>(name, ++id, size);

        LOG(name + "(" + std::to_string(id) + ") with size " + std::to_string(size));

        cache.updateElement(element, element->getId(), element->getSize());

        return element;
    }

    /**
     * @brief Prints the names of a vector of test elements.
     * @param elements The vector of test elements to print.
     */
    void printElements(const std::vector<std::shared_ptr<TestElement>>& elements)
    {
        LOG("Printing elements:");
        for (const auto& element : elements)
        {
            std::cout << "\t";
            element->print();
        }
        std::cout << std::endl;
    }
}

/**
 * @brief Main function to test the LRUCache.
 * 
 * @return int 
 */
int main() 
{
    std::vector<std::shared_ptr<TestElement>> elements;

    {
        LRUCache<TestElement, int> cache(60, 100, 5, 500);  // Set a time threshold of 5 seconds

        LOG("Cache created with soft max size: " + std::to_string(cache.getSoftMaxSize())
                        + ", hard max size: " + std::to_string(cache.getMaxSize())
                        + ", time threshold: " + std::to_string(cache.getTimeThreshold())
                        + ", and cleaning interval: " + std::to_string(cache.getCleaningInterval()));

        cache.dumpCache();

        elements.push_back(createElement("First element", cache, 20));
        auto secondElement = createElement("Second element", cache, 20);
        elements.push_back(secondElement);
        elements.push_back(createElement("Third element", cache, 25));
        
        cache.dumpCache();
        printElements(elements);
                
        LOG("Sleeping for 6 seconds");
        std::this_thread::sleep_for(std::chrono::seconds(6));

        cache.dumpCache();
        printElements(elements);
        
        elements.push_back(createElement("Fourth element", cache, 10));

        cache.updateElement(secondElement, secondElement->getId() , secondElement->getSize());
        
        elements.push_back(createElement("Fifth element", cache, 10));
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        elements.push_back(createElement("Sixth element", cache, 10));
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        elements.push_back(createElement("Seventh element", cache, 10));
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        elements.push_back(createElement("Eighth element", cache, 10));
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        elements.push_back(createElement("Ninth element", cache, 10));
        elements.push_back(createElement("Tenth element", cache, 10));
        elements.push_back(createElement("Eleventh element", cache, 10));

        cache.dumpCache();

        LOG("Sleeping for 1 second");
        std::this_thread::sleep_for(std::chrono::seconds(1));

        cache.dumpCache();
    }

    printElements(elements);

    return 0;
}
