#ifndef SOURCE_HPP
#define SOURCE_HPP

#include <vector>
#include <string>
#include <algorithm>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

struct StatResult
{
    std::string description;
    double value;

    StatResult(const std::string& desc, double val)
        : description(desc)
        , value(val)
    {
    }
};

using Data = std::vector<double>;
using Results = std::vector<StatResult>;

enum StatisticsType
{
    avg,
    min_max,
    sum
};

class DataAnalyzer
{
    StatisticsType stat_type_;
    Data data_;
    Results results_;

public:
    DataAnalyzer(StatisticsType stat_type)
        : stat_type_{stat_type}
    {
    }

    void load_data(const std::string& file_name)
    {
        data_.clear();
        results_.clear();

        std::ifstream fin(file_name.c_str());
        if (!fin)
            throw std::runtime_error("File not opened");

        double d;
        while (fin >> d)
        {
            data_.push_back(d);
        }

        std::cout << "File " << file_name << " has been loaded...\n";
    }

    void set_statistics(StatisticsType stat_type)
    {
        stat_type_ = stat_type;
    }

    void calculate()
    {
        if (stat_type_ == avg)
        {
            double sum = std::accumulate(data_.begin(), data_.end(), 0.0);
            double avg = sum / data_.size();

            StatResult result("Avg", avg);
            results_.push_back(result);
        }
        else if (stat_type_ == min_max)
        {
            double min = *(std::min_element(data_.begin(), data_.end()));
            double max = *(std::max_element(data_.begin(), data_.end()));

            results_.push_back(StatResult("Min", min));
            results_.push_back(StatResult("Max", max));
        }
        else if (stat_type_ == sum)
        {
            double sum = std::accumulate(data_.begin(), data_.end(), 0.0);

            results_.push_back(StatResult("Sum", sum));
        }
    }

    const Results& results() const
    {
        return results_;
    }

    void save_results(const std::string& file_name)
    {
        std::ofstream out{file_name};

        if (!out)
            throw std::runtime_error("Unable to open the file!!!");

        for (const auto& rslt : results_)
            std::cout << rslt.description << " = " << rslt.value << std::endl;
    }
};

#endif