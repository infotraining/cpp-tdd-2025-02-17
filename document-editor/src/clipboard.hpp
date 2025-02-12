#ifndef CLIPBOARD_HPP
#define CLIPBOARD_HPP

#include <mutex>
#include <string>

class SharedClipboard
{
    std::string content_;
    mutable std::mutex content_mtx_;

public:
    SharedClipboard& instance()
    {
        static SharedClipboard unique_instance;

        return unique_instance;
    }

    std::string content() const
    {
        std::lock_guard<std::mutex> lk{content_mtx_};

        return content_;
    }

    void set_content(const std::string& content)
    {
        std::lock_guard<std::mutex> lk{content_mtx_};

        content_ = content;
    }
};

#endif // CLIPBOARD_HPP
