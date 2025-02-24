#include <algorithm>
#include <string>
#include <memory>
#include <filesystem>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <source.hpp>

using namespace std;
using namespace std::literals;

void save_file_content(const std::string& file_name, const std::vector<std::string>& content)
{
    std::ofstream fout(file_name);
    if (!fout)
        throw std::runtime_error("File not opened!!!");

    for (const auto& line : content)
    {
        fout << line << std::endl;
    }
}

std::vector<std::string> load_file_content(const std::string& file_name)
{
    std::vector<std::string> content;
    std::ifstream fin(file_name);
    if (!fin)
        throw std::runtime_error("File not opened!!!");

    std::string line;
    while (std::getline(fin, line))
    {
        content.push_back(line);
    }

    return content;
}

TEST(Example, ExampleTest)
{
    save_file_content("test.dat", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"});
    DataAnalyzer data_analyzer(avg);
    data_analyzer.load_data("test.dat");
    data_analyzer.calculate();
    data_analyzer.save_results("results.txt");

    auto content = load_file_content("results.txt");
    ASSERT_EQ(content, std::vector<std::string>{"Avg = 5.5"s});
}