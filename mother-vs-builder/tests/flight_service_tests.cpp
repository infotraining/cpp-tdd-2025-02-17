#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <memory>

#include "flight_model.hpp"
#include "flight_repository.hpp"
#include "flight_service.hpp"

using namespace std;

struct Mother
{
    constexpr static const char* flight_no = "LOT101";
    constexpr static const char* client = "John Newman";
    constexpr static const char* timestamp = "2017/01/01 1:45am";

    static ReservationRequest create_reservation_request()
    {
        return ReservationRequest{Flight{flight_no, 100.0}, client, timestamp};
    }
};

class ReservationRequestBuilder
{
    constexpr static const char* flight_no = "LOT101";
    constexpr static const char* client = "John Newman";
    constexpr static const char* timestamp = "2017/01/01 1:45am";

    ReservationRequest reservation_request_{Flight{flight_no, 100.0}, client, timestamp};

public:
    ReservationRequestBuilder() = default;

    ReservationRequestBuilder& with_client(const string& client)
    {
        reservation_request_.client = client;

        return *this;
    }

    ReservationRequestBuilder& with_timestamp(const string& timestamp)
    {
        reservation_request_.timestamp = timestamp;

        return *this;
    }

    ReservationRequestBuilder& with_flight(const Flight& flight)
    {
        reservation_request_.flight = flight;

        return *this;
    }

    ReservationRequest get_reservation_request() const
    {
        return reservation_request_;
    }
};

class MockFlightRepository : public FlightRepository
{
public:
    MOCK_METHOD1(add, void(const Flight&));
};

class FlightServiceTests : public ::testing::Test
{
protected:
    MockFlightRepository flight_repository_;
    FlightReservationService sut_;

public:
    FlightServiceTests()
        : sut_{flight_repository_}
    {
    }
};

TEST_F(FlightServiceTests, CanAddReservationToRepository)
{
    auto reservation_request = Mother::create_reservation_request();

    EXPECT_CALL(flight_repository_, add(reservation_request.flight)).Times(1);

    sut_.make_reservation(reservation_request);
}

TEST_F(FlightServiceTests, ThrowsWhenTimestampInInvalidFormat)
{
    ReservationRequestBuilder reservation_request_builder;
    reservation_request_builder.with_timestamp("2017|01|01 1:45am");
    auto reservation_request = reservation_request_builder.get_reservation_request();

    EXPECT_THROW(sut_.make_reservation(reservation_request), std::invalid_argument);
}

TEST_F(FlightServiceTests, ThrowsWhenClientIsEmptyString)
{
    ReservationRequestBuilder reservation_request_builder;
    reservation_request_builder.with_client("");
    auto reservation_request = reservation_request_builder.get_reservation_request();

    EXPECT_THROW(sut_.make_reservation(reservation_request), std::invalid_argument);
}