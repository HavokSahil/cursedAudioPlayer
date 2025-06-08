#pragma once

#include <curses.h>
#include <memory>
#include <vector>

/// @enum WidgetState
/// @brief States the current state of the widget.
enum WidgetState {
    /// @brief The window is ready to be mounted, but not mounted.
    READY = 0,
    /// @brief The window is mounted, and placed on the screen.
    MOUNTED = 1,
};

static int id_glob = 0;

/// @class Widget
/// @brief The base class for all the widgets.
class Widget {
public:
    Widget(): _id(id_glob+=4) {}
    virtual ~Widget() = default;
    /// @brief Set the absolute height of the widget.
    /// @returns The pointer of the object
    Widget* height(int h);
    /// @brief Set the relative height of the widget.
    /// @returns The pointer of the object
    Widget* heightRel(double);
    /// @brief Set the absolute width of the object.
    /// @returns The pointer of the object
    Widget* width(int w);
    /// @brief Set the relative width of the object.
    /// @returns The pointer of the object
    Widget* widthRel(double);
    /// @brief Set the parent of the object.
    /// @returns The pointer of the object
    Widget* parent(Widget* p);
    /// @brief Set the absolute top margin of the object.
    /// @returns The pointer of the object
    Widget* marginTop(int m);
    /// @brief Set the relative top margin of the object.
    /// @returns The pointer of the object
    Widget* marginTopRel(double m);
    /// @brief Set the absolute left margin of the object.
    /// @returns The pointer of the object
    Widget* marginLeft(int m);
    /// @brief Set the relative left margin of the object.
    /// @returns The pointer of the object
    Widget* marginLeftRel(double m);
    /// @brief Build the final object from the current configuration.
    /// @returns The pointer of the object
    Widget* build();
    /// @brief Sets the state of the window
    void setState(WidgetState);
    /// @brief Clears the window
    void clear();
    /// @brief Updates the window size based on updated terminal size
    void resize();
    /// @brief Updates the content of the widget
    virtual void update() = 0;
    /// @brief Handle the IO event. Each widget implements its own event handler
    virtual void handleEvent(int ch, MEVENT &event) = 0;
    /// @brief Add the child widget
    virtual void add(std::shared_ptr<Widget> child) {
        _children.push_back(std::move(child));
    }

    /// @brief Returns the height of the widget
    int getHeight();
    /// @brief Returns the width of the widget
    int getWidth();
    /// @brief Returns the raw pointer to the parent of the widget
    Widget* getParent();
    /// @brief Returns the top-margin of the widget
    int getMarginTop();
    /// @brief Returns the left-margin of the widget
    int getMarginLeft();
    /// @brief Returns the vertical position of the top-left corner of the widget
    int getTopLeftY();
    /// @brief Returns the horizontal position of the top-left corner of the widget
    int getTopLeftX();
    /// @brief Returns the current state of the widget
    WidgetState getState();

protected:
    /// @brief The unique id of the widget
    int _id;
    /// @brief The NCurses window.
    WINDOW* _window{nullptr};
    /// @brief Pointer of the parent widget.
    Widget* _parent{nullptr};
    /// @brief Array of shared owned children; For widget with no child; the array is empty.
    std::vector<std::shared_ptr<Widget>> _children;
    /// @brief If absolute height is set, it is preferred over the relative height.
    int _height{-1};
    /// @brief If absolute width is set, it is preferred over the relative width.
    int _width{-1};
    /// @brief Relative height in fraction of the parent's height.
    double _heightRel{0.0};
    /// @brief Relative width in fraction of the parent's width.
    double _widthRel{0.0};
    /// @brief Top margin in absolute. It is preferred over the relative margin.
    int _marginTop{-1};
    /// @brief Top margin in relative.
    double _marginTopRel{0.0};
    /// @brief Top margin in absolute. It is preferred over the top margin.
    int _marginLeft{-1};
    /// @brief Left margin in relative.
    double _marginLeftRel{0.0};
    /// @brief Widget state enumerator.
    /// @brief Flag for the changes in state of the window
    bool _commit{true};
    WidgetState _state{READY};
};