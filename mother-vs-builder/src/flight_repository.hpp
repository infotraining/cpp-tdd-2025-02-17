#ifndef FLIGHT_REPOSITORY_HPP
#define FLIGHT_REPOSITORY_HPP

#include "flight_model.hpp"

class FlightRepository
{
public:
    virtual ~FlightRepository() = default;
    virtual void add(const Flight& flight) = 0;
};

#endif // FLIGHT_REPOSITORY_HPP
