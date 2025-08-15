#ifndef DRAWINGWINDOW_H
#define DRAWINGWINDOW_H

#include <gtkmm.h>
#include <vector>

struct Point {
    double x, y, pressure;
    bool new_stroke;
    Gdk::RGBA color;  // Store color with each point
};

class DrawingWindow : public Gtk::Window {
public:
    DrawingWindow(int width, int height, Gdk::RGBA* current_color);

protected:
    // Signal handlers
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    bool on_button_press_event(GdkEventButton* event) override;
    bool on_motion_notify_event(GdkEventMotion* event) override;
    bool should_add_point(double x, double y);

private:
    Gtk::DrawingArea drawing_area;
    std::vector<Point> points;
    Gdk::RGBA* currentColor;  // Reference to MainWindow's current color
    Gdk::RGBA current_stroke_color;  // Color of the current stroke being drawn
};

#endif // DRAWINGWINDOW_H