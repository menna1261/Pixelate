#ifndef DRAWINGWINDOW_H
#define DRAWINGWINDOW_H

#include <gtkmm.h>
#include <vector>

struct Point {
    double x, y, pressure;
    bool new_stroke;
    Gdk::RGBA color; 
    bool is_eraser = false;
    double PointStroke;
    double PointOpacity;
    

};

class DrawingWindow : public Gtk::Window {
public:
    DrawingWindow(int width, int height, Gdk::RGBA* current_color);
    void FillBackGround( Gdk::RGBA* color);
    void fill_with_color(const Gdk::RGBA& color);
    sigc::signal<void(double, double, guint)> signal_mouse_clicked();
    double Stroke{};
    double StrokeSize;
    double Opacity;
    double OpacityVal;

protected:
    // Signal handlers
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    bool on_button_press_event(GdkEventButton* event) override;
    bool on_motion_notify_event(GdkEventMotion* event) override;
    bool should_add_point(double x, double y);
    

private:
    sigc::signal<void(double, double, guint)> m_signal_mouse_clicked;
    bool fill_background = false; 
    Gdk::RGBA fill_color;       
    bool brushClicked;
    Gtk::DrawingArea drawing_area;
    std::vector<Point> points;
    Gdk::RGBA* currentColor;  // Reference to MainWindow's current color
    Gdk::RGBA current_stroke_color;  // Color of the current stroke being drawn
    Cairo::RefPtr<Cairo::Context> crr;
};

#endif // DRAWINGWINDOW_H