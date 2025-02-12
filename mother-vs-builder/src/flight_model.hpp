#ifndef FLIGHT_MODEL_HPP
#define FLIGHT_MODEL_HPP

#include <string>
#include <tuple>

struct Flight
{
    std::string no_of_flight;
    double price;

    bool operator==(const Flight& other) const
    {
        return std::tie(no_of_flight, price) == std::tie(other.no_of_flight, other.price);
    }
};

using Client = std::string;
using Timestamp = std::string;


#endif //FLIGHT_MODEL_HPP
