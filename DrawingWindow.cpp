#include "DrawingWindow.h"

DrawingWindow::DrawingWindow(int width, int height) {
    set_default_size(width, height);
    set_title("Drawing Window");

    add(drawing_area);

    drawing_area.set_events(
        Gdk::BUTTON_PRESS_MASK |
        Gdk::BUTTON_RELEASE_MASK |
        Gdk::POINTER_MOTION_MASK
    );

    drawing_area.signal_draw().connect(sigc::mem_fun(*this, &DrawingWindow::on_draw), false);

    // Enable mouse/stylus events
    add_events(
        Gdk::BUTTON_PRESS_MASK |
        Gdk::POINTER_MOTION_MASK
    );

    show_all_children();
}

bool DrawingWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    cr->set_source_rgb(1, 1, 1);
    cr->paint();
    cr->set_source_rgb(0, 0, 0); // black color

    for (size_t i = 0; i < points.size(); ++i) {
        cr->set_line_width(1.0 + points[i].pressure * 4.0); // thickness 1–5px

        if (points[i].new_stroke) {
            cr->move_to(points[i].x, points[i].y);
        } else {
            cr->line_to(points[i].x, points[i].y);
        }
    }

    cr->stroke();
    return true;
}

bool DrawingWindow::on_button_press_event(GdkEventButton* event) {
    if (event->button == 1) { // Left mouse or stylus press
        double pressure = 1.0; // default for mouse
        if (event->axes) {
            double val = event->axes[GDK_AXIS_PRESSURE];
            if (val >= 0.0 && val <= 1.0) pressure = val;
        }

        points.push_back({event->x, event->y, pressure, true});
        queue_draw();
    }
    return true;
}

bool DrawingWindow::on_motion_notify_event(GdkEventMotion* event) {
    if (event->state & GDK_BUTTON1_MASK) { // Left mouse/stylus held
        double pressure = 1.0; // default
        if (event->axes) {
            double val = event->axes[GDK_AXIS_PRESSURE];
            if (val >= 0.0 && val <= 1.0) pressure = val;
        }

        points.push_back({event->x, event->y, pressure, false});
        queue_draw();
    }
    return true;
}