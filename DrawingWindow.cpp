#include "DrawingWindow.h"
#include<MainWindow.h>
#include<iostream>
#include <gdk/gdk.h>


DrawingWindow::DrawingWindow(int width, int height, Gdk::RGBA* current_color) {
    set_default_size(width, height);
    set_title("Drawing Window");
    add(drawing_area);
    
    drawing_area.set_events(
        Gdk::BUTTON_PRESS_MASK |
        Gdk::BUTTON_RELEASE_MASK |
        Gdk::POINTER_MOTION_MASK
    );
    
    // Enable extended input devices (tablets, stylus) - GTK3 way
    // Instead of set_support_multidevice(true), try this:
drawing_area.add_events(Gdk::ALL_EVENTS_MASK);
    
    drawing_area.signal_draw().connect(sigc::mem_fun(*this, &DrawingWindow::on_draw), false);
    
    add_events(
        Gdk::BUTTON_PRESS_MASK |
        Gdk::POINTER_MOTION_MASK
    );
    
    show_all_children();
    currentColor = current_color;
    
    if (currentColor) {
        current_stroke_color = *currentColor;
    }
}

bool DrawingWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    // Clear background
    cr->set_source_rgb(1, 1, 1);
    cr->paint();
    
    // Set line properties for smoother appearance
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);
    cr->set_line_join(Cairo::LINE_JOIN_ROUND);
    
    Gdk::RGBA last_color;
    bool color_set = false;
    
    // Draw smooth curves
    for (size_t i = 0; i < points.size(); ++i) {
        if (points[i].new_stroke) {
            // Finish previous stroke if any
            if (color_set) {
                cr->stroke();
            }
            
            // Start new stroke with new color and pressure
            const Gdk::RGBA& point_color = points[i].color;
            double pressure = points[i].pressure;
            
            // Apply pressure to line width (1-5 pixels based on pressure)
            cr->set_line_width(1.0+ pressure * 20.0);
            
            // Apply pressure to opacity (30%-100% based on pressure)
            cr->set_source_rgba(point_color.get_red(), point_color.get_green(), 
                               point_color.get_blue(), 0.5 + pressure * 0.5);
            
            cr->move_to(points[i].x, points[i].y);
            last_color = point_color;
            color_set = true;
            
        } else if (i > 0 && !points[i-1].new_stroke) {
            // Check if color changed (shouldn't happen mid-stroke, but just in case)
            if (color_set && (points[i].color.get_red() != last_color.get_red() ||
                             points[i].color.get_green() != last_color.get_green() ||
                             points[i].color.get_blue() != last_color.get_blue())) {
                // Finish current stroke and start new one with new color
                cr->stroke();
                const Gdk::RGBA& point_color = points[i].color;
                double pressure = points[i].pressure;
                
                cr->set_line_width(1.0 + pressure * 20.0);
                cr->set_source_rgba(point_color.get_red(), point_color.get_green(), 
                                   point_color.get_blue(), 0.6 + pressure * 0.4);
                cr->move_to(points[i-1].x, points[i-1].y);
                last_color = point_color;
            }
            
            if (i == 1 || points[i-1].new_stroke) {
                // First line of stroke
                cr->line_to(points[i].x, points[i].y);
            } else if (i < points.size() - 1) {
                // Smooth curve using cubic Bézier
                double x1 = points[i-1].x;
                double y1 = points[i-1].y;
                double x2 = points[i].x;
                double y2 = points[i].y;
                double x3 = points[i+1].x;
                double y3 = points[i+1].y;
                
                // Control points for smoothness
                double cp1x = x1 + (x2 - points[i-2].x) * 0.1;
                double cp1y = y1 + (y2 - points[i-2].y) * 0.1;
                double cp2x = x2 - (x3 - x1) * 0.1;
                double cp2y = y2 - (y3 - y1) * 0.1;
                
                cr->curve_to(cp1x, cp1y, cp2x, cp2y, x2, y2);
            } else {
                // Last point
                cr->line_to(points[i].x, points[i].y);
            }
        }
    }
    
    // Finish the last stroke
    if (color_set) {
        cr->stroke();
    }
    return true;
}
// Add to private section of DrawingWindow.h:

// Add to DrawingWindow.cpp:
bool DrawingWindow::should_add_point(double x, double y) {
    if (points.empty()) return true;
    
    double dx = x - points.back().x;
    double dy = y - points.back().y;
    double distance = sqrt(dx*dx + dy*dy);
    
    return distance > 2.0; // Only add points if they're far enough apart
}

bool DrawingWindow::on_button_press_event(GdkEventButton* event) {
    if (event->button == 1) {
        double pressure = 0.5; // default for mouse
        
        // GTK3 way to get pressure
        if (event->device) {
            gdouble value;
            GdkAxisUse axis_use = GDK_AXIS_PRESSURE;
            
            if (gdk_device_get_axis(event->device, event->axes, axis_use, &value)) {
                pressure = value;
                std::cout << "Got pressure: " << pressure << std::endl;
            } else {
                std::cout << "No pressure axis found" << std::endl;
            }
        }
        
        if (currentColor) {
            current_stroke_color = *currentColor;
        }
        
        points.push_back({event->x, event->y, pressure, true, current_stroke_color});
        queue_draw();
    }
    return true;
}

bool DrawingWindow::on_motion_notify_event(GdkEventMotion* event) {
    if (event->state & GDK_BUTTON1_MASK) {
        double pressure = 0.5; // default
        
        // GTK3 way to get pressure
        if (event->device) {
            gdouble value;
            GdkAxisUse axis_use = GDK_AXIS_PRESSURE;
            
            if (gdk_device_get_axis(event->device, event->axes, axis_use, &value)) {
                pressure = value;
            }
        }
        
        if (should_add_point(event->x, event->y)) {
            points.push_back({event->x, event->y, pressure, false, current_stroke_color});
            queue_draw();
        }
    }
    return true;
}