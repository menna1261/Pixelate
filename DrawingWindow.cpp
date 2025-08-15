#include "DrawingWindow.h"
#include<MainWindow.h>
#include<iostream>



DrawingWindow::DrawingWindow(int width, int height, Gdk::RGBA* current_color) {
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
    currentColor = current_color;
    
    // Initialize current stroke color
    if (currentColor) {
        current_stroke_color = *currentColor;
    }
}

bool DrawingWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    // Clear background
    cr->set_source_rgb(1, 1, 1);
    cr->paint();
    
    // Draw all strokes with their original colors
    for (size_t i = 1; i < points.size(); ++i) {
        if (points[i].new_stroke) continue;
        
        // Use the color stored with each point
        const Gdk::RGBA& point_color = points[i].color;
        cr->set_source_rgb(point_color.get_red(), point_color.get_green(), point_color.get_blue());
        
        cr->move_to(points[i - 1].x, points[i - 1].y);
        cr->line_to(points[i].x, points[i].y);
        cr->stroke();
    }
    
    return true;
}

bool DrawingWindow::on_button_press_event(GdkEventButton* event) {
    if (event->button == 1) { // Left mouse or stylus press
        double pressure = 1.0; // default for mouse
        if (event->axes) {
            double val = event->axes[GDK_AXIS_PRESSURE];
            if (val >= 0.0 && val <= 1.0) pressure = val;
        }
        
        // Capture the current color when starting a new stroke
        if (currentColor) {
            current_stroke_color = *currentColor;
        }
        
        // Store the point with the current stroke color
        points.push_back({event->x, event->y, pressure, true, current_stroke_color});
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
        
        // Continue using the same color for this stroke (current_stroke_color)
        points.push_back({event->x, event->y, pressure, false, current_stroke_color});
        queue_draw();
    }
    return true;
}