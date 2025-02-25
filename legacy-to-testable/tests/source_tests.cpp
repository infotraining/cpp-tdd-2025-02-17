#include <algorithm>
#include <string>
#include <memory>
#include <filesystem>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <source.hpp>

using namespace std;
using namespace std::literals;

std::string get_file_contents(const std::string& file_name)
{
    std::ifstream in{file_name};
    if (!in)
        throw std::runtime_error("File not opened!!!");

    std::string contents;
    std::string line;
    while (std::getline(in, line))
    {
        contents += line + "\n";
    }

    return contents;
}

TEST(Acceptance_DataAnalyzer, End_2_End)
{
    using namespace Legacy;
    DataAnalyzer data_analyzer(StatisticsType::avg);
    data_analyzer.load_data("data.dat");
    data_analyzer.calculate();
    data_analyzer.set_statistics(StatisticsType::min_max);
    data_analyzer.calculate();
    data_analyzer.set_statistics(StatisticsType::sum);
    data_analyzer.calculate();
    data_analyzer.save_results("results.txt");

    std::string expected_results = "Avg = 47.15\nMin = 1\nMax = 99\nSum = 4715\n";

    ASSERT_EQ(get_file_contents("results.txt"), expected_results);
}

struct StubDataLoader
{
    Data load_data(const std::string& file_name) const
    {
        return {1, 2, 3, 4, 5};
    }
};

struct SpyLogger
{
    std::vector<std::string> messages;

    void log(const std::string& message)
    {
        messages.push_back(message);
    }
};

template <typename TDataLoader, typename TLogger>
struct TestDataAnalyzer : public Legacy::DataAnalyzer<TDataLoader, TLogger>
{
    using BaseClass = Legacy::DataAnalyzer<TDataLoader, TLogger>;
    using BaseClass::BaseClass;

    Results calculated_results;
protected:
    void do_save_results(const std::string& file_name, const Results& results) override
    {
        calculated_results = results;
    }
};

TEST(UnitTest_DataAnalyzer, ReadingFile)
{
    using namespace Legacy;

    SpyLogger logger;
 
    TestDataAnalyzer data_analyzer(StatisticsType::avg, StubDataLoader{}, logger);
    data_analyzer.load_data("data.dat");
    data_analyzer.calculate();
    data_analyzer.save_results("results.txt");

    ASSERT_EQ(logger.messages.size(), 2);
    ASSERT_EQ(data_analyzer.calculated_results.size(), 1);
}