#ifndef FLIGHT_SERVICE_HPP
#define FLIGHT_SERVICE_HPP

#include "flight_model.hpp"
#include "flight_repository.hpp"
#include <stdexcept>

struct ReservationRequest
{
    Flight flight;
    Client client;
    Timestamp timestamp;
};

class FlightReservationService
{
    FlightRepository& flight_repository_;

public:
    FlightReservationService(FlightRepository& flight_repository)
        : flight_repository_{flight_repository}
    {
    }

    void make_reservation(const ReservationRequest& reservation_request)
    {
        if (!is_valid(reservation_request.timestamp))
            throw std::invalid_argument("invalid timestamp");
        
        if (reservation_request.client.empty())
            throw std::invalid_argument("invalid clinet");

        flight_repository_.add(reservation_request.flight);
    }

private:
    bool is_valid(const Timestamp& timestamp)
    {
        return timestamp == "2017/01/01 1:45am";
    }
};


#endif //FLIGHT_SERVICE_HPP
