/**************************************************************************************************
 * @file LRUCache.hpp
 *
 * @brief This file contains the LRUCache class and its related classes.
 **************************************************************************************************/

#include <iostream>
#include <string>
#include <functional>
#include <chrono>
#include <map>
#include <list>
#include <memory>
#include <mutex>
#include <future>
#include <vector>
#include <deque>
#include <set>
#include <sstream>
#include <random>
#include <chrono>
#include <cassert>
#include <iomanip>

#include "Utility.hpp"

/**
 * @class LRUCacheCleanable
 *
 * @brief An abstract class that provides an interface for cleaning up resources.
 */
class LRUCacheCleanable
{
public:
    virtual ~LRUCacheCleanable() {}

    /**
     * @brief A pure virtual function for cleaning up resources.
     */
    virtual void cleanup() = 0;
};

/**
 * @class LRUCacheElement
 * 
 * @brief A class that represents an element in the LRUCache.
 * 
 * @tparam ElementType The type of the element.
 * @tparam PrimaryKeyType The type of the primary key.
 */
template <typename ElementType, typename PrimaryKeyType>
class LRUCacheElement
{
private:
    int64_t mLastAccessTime = 0;
    int64_t mElementSize = 0;
    std::weak_ptr<ElementType> mWeakPointerElement;
    PrimaryKeyType mPrimaryKey;
    typename std::list<std::shared_ptr<LRUCacheElement<ElementType,PrimaryKeyType>>>::iterator mElementInListIterator;

public:
    /**
     * @brief Constructor for the LRUCacheElement class.
     * 
     * @param element The element to be stored.
     * @param primaryKey The primary key associated with the element.
     */
    LRUCacheElement(std::shared_ptr<ElementType> element, PrimaryKeyType primaryKey)
        : mWeakPointerElement(element)
        , mPrimaryKey(primaryKey)
    {
    }

    /**
     * @brief Updates the last access time of the element.
     */
    void updateAccessTime()
    {
        mLastAccessTime = std::time(nullptr);
    }

    /**
     * @brief Sets the iterator pointing to this element in the list.
     * @param elementInListIterator The iterator pointing to this element in the list.
     */
    void setElementInListIterator(const typename std::list<std::shared_ptr<LRUCacheElement<ElementType,PrimaryKeyType>>>::iterator &elementInListIterator)
    {
        mElementInListIterator = elementInListIterator;
    }

    /**
     * @brief Gets the iterator pointing to this element in the list.
     * @return The iterator pointing to this element in the list.
     */
    typename std::list<std::shared_ptr<LRUCacheElement<ElementType,PrimaryKeyType>>>::iterator getElementInListIterator() const
    {
        return mElementInListIterator;
    }

    /**
     * @brief Gets the last access time of the element.
     * @return The last access time of the element.
     */
    int64_t getLastAccessTime()
    {
        return mLastAccessTime;
    }

    /**
     * @brief Gets the size of the element.
     * @return The size of the element.
     */
    int64_t getSize() const
    {
        return mElementSize;
    }

    /**
     * @brief Sets the size of the element.
     * @param size The size of the element.
     */
    void setSize(const int64_t &size)
    {
        mElementSize = size;
    }

    /**
     * @brief Gets a weak pointer to the element.
     * @return A weak pointer to the element.
     */
    std::weak_ptr<ElementType> getWeakPointerElement() const
    {
        return mWeakPointerElement;
    }

    /**
     * @brief Gets the primary key of the element.
     * @return The primary key of the element.
     */
    PrimaryKeyType getPrimaryKey() const
    {
        return mPrimaryKey;
    }
};

/**
 * @class LRUCache
 * 
 * @brief A class that represents a Least Recently Used (LRU) cache.
 * 
 * @tparam ElementType The type of the elements in the cache.
 * @tparam PrimaryKeyType The type of the primary key of the elements.
 */
template <typename ElementType, typename PrimaryKeyType>
class LRUCache
{
    static_assert(std::is_base_of<LRUCacheCleanable, ElementType>::value, "ElementType must derive from LRUCacheCleanable");

private:
    std::list<std::shared_ptr<LRUCacheElement<ElementType,PrimaryKeyType>>> mElementList; // List to keep order of elements
    std::map<PrimaryKeyType,std::shared_ptr<LRUCacheElement<ElementType,PrimaryKeyType>>> mElementMap; // Map to ease the search of elements
    std::multimap<int64_t, PrimaryKeyType> mElementSizeMap;  // Data structure to store elements sorted by size
    int64_t mTotalSize = 0;
    int64_t mMaxSizeSoftLimit = 0; // Scheduled cleaner will act on this
    int64_t mMaxSizeHardLimit = 0; // Cache won't be allowed to exceed this
    int64_t mTimeThresholdSec;  // Member variable to store the time threshold
    std::mutex mCacheMutex;

    // Cleaning thread variables
    std::unique_ptr<std::thread> mCleanerThread;
    bool mIsFinished = false;
    int64_t mCleanScheduleIntervalMs;
    std::condition_variable mCleanerCV;
    std::mutex mCleanerMutex;

    /**
     * @brief Ends the cleaning thread.
     */
    void endCleaningThread()
    {
        {
            std::lock_guard<std::mutex> lockGuard(mCleanerMutex);
            mIsFinished = true;
        }
        mCleanerCV.notify_all();
    }

    /**
     * @brief The loop for the cleaning thread.
     */
    void runCleanerThreadLoop()
    {
        while(true)
        {
            std::unique_lock<std::mutex> uniqueLock(mCleanerMutex);
            if (mCleanerCV.wait_for(uniqueLock,std::chrono::milliseconds(mCleanScheduleIntervalMs)) == std::cv_status::timeout)
            {
                cleanup();
            }
            if (mIsFinished) break;
        }
    }

    /**
     * @brief Gets the current time as a string.
     *
     * @return The current time as a string.
     */
    std::string getCurrentTime()
    {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%F %T");
        return ss.str();
    }

public:
    // #region Construction/Destruction

    /**
     * @brief Constructor for the LRUCache class.
     * 
     * @param softSizeLimit The soft maximum size of the cache.
     * @param hardSizeLimit The hard maximum size of the cache.
     * @param timeThresholdSec The time threshold for the cache.
     * @param cleaningIntervalMs The cleaning schedule in milliseconds.
     */
    LRUCache(int64_t softSizeLimit, int64_t hardSizeLimit, int64_t timeThresholdSec, int64_t cleaningIntervalMs = 0)
        : mMaxSizeSoftLimit(softSizeLimit)
        , mMaxSizeHardLimit(hardSizeLimit)
        , mTimeThresholdSec(timeThresholdSec)
        , mCleanScheduleIntervalMs(cleaningIntervalMs)
    {
        if (cleaningIntervalMs)
        {
            mCleanerThread.reset(new std::thread([this]()
            {
                this->runCleanerThreadLoop();
            }
            ));
        }
    }

    /**
     * @brief Destructor for the LRUCache class.
     */
    ~LRUCache()
    {
        if (mCleanerThread)
        {
            endCleaningThread();
            mCleanerThread->join();
        }
    }

    // #endregion

    // #region Public Functions

    /**
     * @brief Updates an element in the cache.
     *
     * @param element The element to be updated.
     * @param key The key associated with the element.
     * @param size The size of the element.
     */
    void updateElement(std::shared_ptr<ElementType> element, const PrimaryKeyType &key, int64_t size)
    {
        {
            std::lock_guard<std::mutex> lockGuard(mCacheMutex);

            std::shared_ptr<LRUCacheElement<ElementType,PrimaryKeyType>> cacheElement;

            auto mapIterator = mElementMap.find(key);
            if (mapIterator == mElementMap.end())
            {
                cacheElement = std::make_shared<LRUCacheElement<ElementType,PrimaryKeyType>>(element, key);
                mElementMap.insert(std::pair<PrimaryKeyType,std::shared_ptr<LRUCacheElement<ElementType,PrimaryKeyType>>>(key, cacheElement));
            }
            else //remove from list to reorder when inserting
            {
                cacheElement = mapIterator->second;
                mElementList.erase(cacheElement->getElementInListIterator());
                mTotalSize -= cacheElement->getSize();

                // Remove the element from sizeMap
                auto range = mElementSizeMap.equal_range(cacheElement->getSize());
                for (auto it = range.first; it != range.second; ++it)
                {
                    if (it->second == key) {
                        mElementSizeMap.erase(it);
                        break;
                    }
                }
            }

            cacheElement->setSize(size);
            mTotalSize += size;

            cacheElement->updateAccessTime();

            mElementList.push_back(cacheElement);// Insert at the back, and save the iterator in the element.
            cacheElement->setElementInListIterator(std::prev(mElementList.end()));

            // Add the element to element size map
            mElementSizeMap.insert({size, key});

            LOG("Updated element with key: " + std::to_string(key));
        }
        if (mTotalSize > mMaxSizeHardLimit)
        {
            cleanup(&key);
        }
    }

    /**
     * @brief Cleans up the cache.
     * 
     * @param keyToSaveFromPurge The key of the element to be saved from purging.
     */
    void cleanup(const PrimaryKeyType *keyToSaveFromPurge = nullptr)
    {
        std::vector<std::shared_ptr<LRUCacheCleanable>> elementsToClean;
        {
            std::lock_guard<std::mutex> lockGuard(mCacheMutex);

            // Print the total size of the cache before cleaning
            LOG("Total size before cleanup: " + std::to_string(mTotalSize));

            while (mElementList.size() &&  mTotalSize > mMaxSizeSoftLimit)
            {
                auto leastRecentlyUsedElement = mElementList.front();

                // Check if the last access time of the element is more than the time threshold
                if (std::time(nullptr) - leastRecentlyUsedElement->getLastAccessTime() > mTimeThresholdSec)
                {
                    auto sizeRange = mElementSizeMap.equal_range(mElementSizeMap.rbegin()->first);

                    // If the range is not empty
                    if (sizeRange.first != sizeRange.second) {
                        // The first element in the sizeRange is the first inserted element in mElementSizeMap.
                        auto firstElementInSizeMap = sizeRange.first;

                        auto cacheElement = mElementMap[firstElementInSizeMap->second];
                        
                        mElementSizeMap.erase(firstElementInSizeMap);

                        mElementList.erase(cacheElement->getElementInListIterator());  // Remove the element from mElementList

                        leastRecentlyUsedElement = mElementMap[cacheElement->getPrimaryKey()];

                        LOG("Element with key (" + std::to_string(leastRecentlyUsedElement->getPrimaryKey()) + ") removed based on time threshold and max size.");
                    }
                }
                else
                {
                    // If not, remove the least recently used element
                    mElementList.pop_front();

                    // Find the element with key
                    auto it = mElementSizeMap.find(leastRecentlyUsedElement->getSize());

                    // If the element is found, erase it
                    if (it != mElementSizeMap.end())
                    {
                        mElementSizeMap.erase(it);
                    }

                    LOG("Element with key (" + std::to_string(leastRecentlyUsedElement->getPrimaryKey()) + ") removed based on LRU policy");
                }

                mElementMap.erase(leastRecentlyUsedElement->getPrimaryKey());

                if (!keyToSaveFromPurge || *keyToSaveFromPurge != leastRecentlyUsedElement->getPrimaryKey())
                {
                    auto weakPointerElement = leastRecentlyUsedElement->getWeakPointerElement();
                    auto sharedPointerElement = weakPointerElement.lock();
                    if (sharedPointerElement)
                    {
                        elementsToClean.push_back(sharedPointerElement);
                    }

                    mTotalSize -= leastRecentlyUsedElement->getSize();
                }
            }
        } // Unlock the mutex here

        // Perform the actual cleanup outside the critical section
        for (auto &elementToClean : elementsToClean)
        {
            elementToClean->cleanup();
        }
    }

    // #endregion

    // #region Extra Function Added by Shyam For Testing

    /**
     * @brief Retrieves an element from the cache.
     *
     * @param key The key of the element to be retrieved.
     *
     * @return A shared pointer to the element if it exists in the cache, or nullptr if it does not.
     */
    std::shared_ptr<ElementType> getElement(const PrimaryKeyType& key)
    {
        std::lock_guard<std::mutex> lockGuard(mCacheMutex);

        auto mapIterator = mElementMap.find(key);
        if (mapIterator != mElementMap.end())
        {
            // The element is in the cache. Update its last access time and move it to the back of the list.
            auto cacheElement = mapIterator->second;
            cacheElement->updateAccessTime();
            mElementList.splice(mElementList.end(), mElementList, cacheElement->getElementInListIterator());

            // Return a shared pointer to the element.
            return cacheElement->getWeakPointerElement().lock();
        }
        else
        {
            // The element is not in the cache.
            return nullptr;
        }
    }

    /**
     * @brief Gets the current number of elements in the cache.
     *
     * @return The current number of elements in the cache.
     */
    size_t getNumberOfElements() const
    {
        return mElementList.size();
    }

    /**
     * @brief Gets the soft maximum size of the cache.
     *
     * @return The soft maximum size of the cache.
     */
    int64_t getSoftMaxSize() const
    {
        return mMaxSizeSoftLimit;
    }

    /**
     * @brief Gets the maximum size of the cache.
     *
     * @return The maximum size of the cache.
     */
    int64_t getMaxSize() const
    {
        return mMaxSizeHardLimit;
    }

    /**
     * @brief Gets the time threshold of the cache.
     *
     * @return The time threshold of the cache.
     */
    int64_t getTimeThreshold() const
    {
        return mTimeThresholdSec;
    }

    /**
     * @brief Gets the cleaning interval of the cache.
     *
     * @return The cleaning interval of the cache.
     */
    int64_t getCleaningInterval() const
    {
        return mCleanScheduleIntervalMs;
    }

    /**
     * @brief Dumps the current state of the cache.
     */
    void dumpCache()
    {
        std::lock_guard<std::mutex> lockGuard(mCacheMutex);
        std::cout << "Cache state:" << std::endl;
        for (const auto& element : mElementList)
        {
            std::cout << "Key: " << element->getPrimaryKey() << ", Size: " << element->getSize() << ", Last Access Time: " << element->getLastAccessTime() << std::endl;
        }
    }

    // #endregion
};