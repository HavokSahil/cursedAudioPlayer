/// @file FHexStream.cpp
/// @brief Implementation of the derived widget HexStream

#include "FHexStream.h"
#include <iomanip>
#include <sstream>
#include "Column.h"

FHexStream::~FHexStream() {
    if (_window) delwin(_window);
}

static std::string toHex(uint32_t value) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << value;
    return ss.str();
}

FHexStream* FHexStream::mount() {
    std::lock_guard lock(_mutex);
    _data = std::make_unique<std::pair<size_t, uint32_t>[]>(_nBytes * _nLines / 4);
    _column = std::make_shared<Column>();
    _column->parent(this)->widthRel(1.0)->heightRel(1.0);
    _column->mainAxisAlignment(MX_CENTER);
    this->add(_column);

    _bytes.resize(_nLines);
    _lines.resize(_nLines);

    for (size_t i = 0; i < _nLines; i++) {
        auto row = std::make_shared<Row>();
        row->parent(this)->widthRel(1.0)->height(1);
        row->mainAxisAlignment(MX_CENTER);

        for (size_t j = 0; j < _nBytes / 4; j++) {
            size_t index = (i * _nBytes / 4) + j;
            auto address = std::make_shared<TextBox>();
            address->parent(row.get())->width(7)->height(1);
            address->padding(0);
            address->color(COLOR_GREEN);
            address->bgColor(COLOR_BLACK);
            address->text("000000");
            address->getTextCb([index, this]() -> std::string {
                std::ostringstream out;
                out << std::hex << std::uppercase << std::setfill('0') << std::setw(6) << _data[index].first;
                return out.str();
            });

            row->add(address);
            _bytes[i].push_back(std::move(address));

            auto textbox = std::make_shared<TextBox>();
            textbox->parent(row.get())->width(10)->height(1);
            textbox->color(COLOR_WHITE);
            textbox->bgColor(COLOR_BLACK);
            textbox->text("00000000");
            textbox->getTextCb([index, this]() -> std::string {
                return toHex(_data[index].second);
            });

            row->add(textbox);
            _bytes[i].push_back(std::move(textbox));
        }
        _column->add(row);
        _lines[i] = std::move(row);
    }
    return this;
}


void FHexStream::update() {
    const auto now = std::chrono::steady_clock::now();
    if (now >= _nextUpdate) {
        std::deque<std::pair<size_t, uint32_t>> update = _dataCallback();
        size_t total = _nBytes * _nLines / 4;

        size_t i = 0;
        for (; i < update.size() && i < total; ++i) {
            auto [cnt, bytes4] = update[i];
            _data[i] = {cnt, bytes4};
        }

        for (; i < total; ++i) {
            _data[i] = {0, 0};
        }
        _nextUpdate = now + std::chrono::milliseconds(100);
    }

    for (auto &child: _children) {
        child->update();
    }
}


void FHexStream::handleEvent(int ch, MEVENT &event) {
    for (auto &child: _children) {
        child->handleEvent(ch, event);
    }
}

