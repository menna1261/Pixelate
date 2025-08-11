#pragma once
#include <gtkmm.h>
#include <vector>

class DrawingWindow : public Gtk::Window {
public:
    DrawingWindow(int width, int height);

protected:
    // Struct for storing each point
    struct Point {
        double x, y;
        double pressure;
        bool new_stroke; // true if this point starts a new stroke
        
    };

    // Drawing area widget
    Gtk::DrawingArea drawing_area;

    // Store all drawn points
    std::vector<Point> points;

    // Event handlers
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    bool on_button_press_event(GdkEventButton* event) override;
    bool on_motion_notify_event(GdkEventMotion* event) override;
};
