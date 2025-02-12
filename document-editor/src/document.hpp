#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <sstream>
#include <algorithm>
#include <string>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>


class Document
{
    std::string text_;

public:
    class Memento
    {
    private:
        std::string snapshot_;

        friend class Document;
    };

    Document() : text_{}
    {
    }

    Document(const std::string& text) : text_{text}
    {
    }

    std::string text() const
    {
        return text_;
    }

    size_t length() const
    {
        return text_.size();
    }

    void add_text(const std::string& txt)
    {
        text_ += txt;
    }

    void to_upper()
    {
        std::transform(text_.begin(), text_.end(), text_.begin(), [](auto c) { return std::toupper(c); });        
    }

    void to_lower()
    {
        std::transform(text_.begin(), text_.end(), text_.begin(), [](auto c) { return std::tolower(c); });
    }

    void clear()
    {
        text_.clear();
    }

    template <typename TSerializer = cereal::BinaryOutputArchive>
    Memento create_memento() const
    {
        std::stringstream stream;
        TSerializer oarchive(stream);
        oarchive(text_);

        Memento memento;
        memento.snapshot_ = stream.str();

        return memento;
    }

    template <typename TDeserializer = cereal::BinaryInputArchive>
    void set_memento(Memento& memento)
    {
        std::stringstream stream{memento.snapshot_};
        TDeserializer iarchive(stream);
        iarchive(text_);
    }

    void replace(size_t start_pos, size_t count, const std::string& text)
    {
        text_.replace(start_pos, count, text);
    }
};

#endif
