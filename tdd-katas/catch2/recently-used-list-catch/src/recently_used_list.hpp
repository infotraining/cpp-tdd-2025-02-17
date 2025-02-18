#ifndef RUL_HPP
#define RUL_HPP

#include <algorithm>
#include <deque>
#include <stdexcept>
#include <string>
#include <limits>
#include <vector>

namespace TDD
{
    class RecentlyUsedList {
    public:
        RecentlyUsedList() = default;
        RecentlyUsedList(size_t stackCapacity) : capacity{stackCapacity} {};
        std::string& operator[](size_t id) { return data[id]; }
        std::string operator[](size_t id) const { return data[id]; }

        size_t size() { return data.size(); }

        void push(const std::string& item) 
        {
            if(item.empty())
                throw std::invalid_argument("Empty string is not allowed");
                
            std::erase_if(data, [item](const auto &innerItem)
                          { return item == innerItem; });
            data.insert(data.begin(), item); 

            if (capacity > 0 && capacity < data.size())
                data.erase(data.end());
        }

        size_t GetCapacity() { return capacity; }
    private:
        std::vector<std::string> data {} ;
        size_t capacity{0};
    };
}

#endif