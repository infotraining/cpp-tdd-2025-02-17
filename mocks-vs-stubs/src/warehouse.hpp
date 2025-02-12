#ifndef WAREHOUSE_HPP
#define WAREHOUSE_HPP

#include <string>

class Warehouse
{
public:
    virtual bool has_inventory(const std::string& product, size_t quantity) const = 0;
    virtual void add(const std::string& name, size_t quantity) = 0;
    virtual void remove(const std::string& product, size_t quantity) = 0;
    virtual size_t get_inventory(const std::string& name) const = 0;
    virtual ~Warehouse() = default;
};


#endif

