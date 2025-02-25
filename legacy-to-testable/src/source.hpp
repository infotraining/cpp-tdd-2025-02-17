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

class Logger
{
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
public:
    void log(const std::string& message) 
    {
        std::cout << "Log: " << message << std::endl;
    }

    static Logger& instance()
    {
        static Logger logger;
        return logger;
    }
};

namespace Legacy
{
    /*inline*/ namespace ver_0
    {
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

                Logger::instance().log("File " + file_name + " has been loaded...\n");
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
                    out << rslt.description << " = " << rslt.value << std::endl;

                Logger::instance().log("File " + file_name + " has been saved...\n");
            }
        };
    }

    inline namespace ver_1
    {
        struct DataLoader
        {
            Data load_data(const std::string& file_name) const
            {
                Data data;

                std::ifstream fin(file_name.c_str());
                if (!fin)
                    throw std::runtime_error("File not opened");

                double d;
                while (fin >> d)
                {
                    data.push_back(d);
                }                

                return data;
            }
        };

        template <typename TDataLoader = DataLoader, typename TLogger = Logger>
        class DataAnalyzer
        {
            StatisticsType stat_type_;
            TDataLoader data_loader_;
            TLogger& logger_;
            Data data_;
            Results results_;

        public:
            DataAnalyzer(StatisticsType stat_type, TDataLoader data_loader = TDataLoader{}, TLogger& logger = Logger::instance())
                : stat_type_{stat_type}, data_loader_{data_loader}, logger_{logger}
            {
            }

            void load_data(const std::string& file_name)
            {
                data_.clear();
                results_.clear();

                data_ = data_loader_.load_data(file_name);
            
                logger_.log("File " + file_name + " has been loaded...\n");
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
                do_save_results(file_name, results_);

                logger_.log("File " + file_name + " has been saved...\n");
            }
     
        protected:  
            virtual void do_save_results(const std::string& file_name, const Results& results)
            {
                std::ofstream out{file_name};

                    if (!out)
                        throw std::runtime_error("Unable to open the file!!!");

                    for (const auto& rslt : results)
                        out << rslt.description << " = " << rslt.value << std::endl;
            }
        };
    }    
}

#endif