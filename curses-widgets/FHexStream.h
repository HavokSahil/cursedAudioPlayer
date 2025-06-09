/// @file FHexStream.h
/// @brief Definition of the derived widget HexStream

#pragma once
#include <deque>
#include <functional>

#include "Row.h"
#include "TextBox.h"
#include "Widget.h"

class Column;

class FHexStream final: public Widget {

    using DataCallback = std::function<std::deque<std::pair<size_t, uint32_t>>()>;

public:
    FHexStream() = default;
    ~FHexStream() override;

    FHexStream* nBytes(size_t n) { _nBytes = n; return this; }
    FHexStream* nLines(size_t n) { _nLines = n; return this; }
    FHexStream* bytesCb(DataCallback&& cb) { _dataCallback = std::move(cb); return this; }
    FHexStream* mount();

    void update() override;
    void handleEvent(int ch, MEVENT &event) override;

private:
    std::mutex _mutex;
    /// @brief Number of bytes per line
    /// @remark Must be multiple of 4
    size_t _nBytes{8};
    /// @brief Number of lines of bytes
    size_t _nLines{4};
    DataCallback _dataCallback;

    std::unique_ptr<std::pair<size_t, uint32_t>[]> _data{nullptr};
    std::vector<std::shared_ptr<Row>> _lines;
    std::vector<std::vector<std::shared_ptr<TextBox>>> _bytes;
    std::shared_ptr<Column> _column;
};
