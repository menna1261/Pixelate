#include "DrawingWindow.h"
#include<MainWindow.h>
#include<iostream>
#include <gdk/gdk.h>
#include "Singleton.cpp"


DrawingWindow::DrawingWindow(int width, int height, Gdk::RGBA* current_color) {
    set_default_size(width, height);
    set_title("Drawing Window");
    add(drawing_area);
    
    Singleton::getInstance(); 
    drawing_area.set_events(
        Gdk::BUTTON_PRESS_MASK |
        Gdk::BUTTON_RELEASE_MASK |
        Gdk::POINTER_MOTION_MASK
    );
    
    drawing_area.add_events(Gdk::ALL_EVENTS_MASK);  
    drawing_area.signal_draw().connect(sigc::mem_fun(*this, &DrawingWindow::on_draw), false);
  //  drawing_area.signal_draw().connect(sigc::mem_fun(*this, &DrawingWindow::FillBackGround), false);
    
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

sigc::signal<void(double, double, guint)> DrawingWindow::signal_mouse_clicked() {
    return m_signal_mouse_clicked;
}

void DrawingWindow::fill_with_color(const Gdk::RGBA& color) {
    fill_background = true;
    fill_color = color;
    queue_draw();  // Trigger redraw
}

bool DrawingWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    // Draw background
    if (fill_background) {
        cr->set_source_rgb(fill_color.get_red(), fill_color.get_green(), fill_color.get_blue());
        cr->paint();
    } else {
        cr->set_source_rgb(1, 1, 1);
        cr->paint();
    }
    
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);
    cr->set_line_join(Cairo::LINE_JOIN_ROUND);
    
    Gdk::RGBA last_color;
    bool color_set = false;
    bool last_was_eraser = false;
    
    for (size_t i = 0; i < points.size(); ++i) {
        if (points[i].new_stroke) {
            // Finish previous stroke
            if (color_set) {
                cr->stroke();
            }
            
            const Gdk::RGBA& point_color = points[i].color;
            double pressure = points[i].pressure;
            bool is_eraser = points[i].is_eraser;
            
            // Set line width
            cr->set_line_width(1.0 + pressure * points[i].PointStroke);
            
            if (is_eraser) {
                // Eraser: paint with background color
                if (fill_background) {
                    cr->set_source_rgba(fill_color.get_red(), fill_color.get_green(), 
                                       fill_color.get_blue(), 1.0);
                } else {
                    cr->set_source_rgba(1.0, 1.0, 1.0, 1.0); // White background
                }
            } else {
                // Regular brush: use stroke color
                cr->set_source_rgba(point_color.get_red(), point_color.get_green(), 
                                   point_color.get_blue(), 0.5 + pressure * (points[i].PointOpacity)/100);
            }
            
            cr->move_to(points[i].x, points[i].y);
            last_color = point_color;
            last_was_eraser = is_eraser;
            color_set = true;
            
        } else if (i > 0 && !points[i-1].new_stroke) {
            // Check if we switched between brush and eraser mid-stroke (shouldn't happen, but just in case)
            if (points[i].is_eraser != last_was_eraser) {
                cr->stroke(); // Finish current stroke
                
                // Start new stroke with new mode
                const Gdk::RGBA& point_color = points[i].color;
                double pressure = points[i].pressure;
                bool is_eraser = points[i].is_eraser;
                
                cr->set_line_width(1.0 + pressure * points[i].PointStroke);
                
                if (is_eraser) {
                    if (fill_background) {
                        cr->set_source_rgba(fill_color.get_red(), fill_color.get_green(), 
                                           fill_color.get_blue(), 1.0);
                    } else {
                        cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
                    }
                } else {
                    cr->set_source_rgba(point_color.get_red(), point_color.get_green(), 
                                       point_color.get_blue(), 0.5 + pressure * 0.5);
                }
                
                cr->move_to(points[i-1].x, points[i-1].y);
                last_was_eraser = is_eraser;
            }
            
            // Continue drawing the stroke
            if (i == 1 || points[i-1].new_stroke) {
                cr->line_to(points[i].x, points[i].y);
            } else if (i < points.size() - 1) {
                // Smooth curve using cubic Bézier
                double x1 = points[i-1].x;
                double y1 = points[i-1].y;
                double x2 = points[i].x;
                double y2 = points[i].y;
                double x3 = points[i+1].x;
                double y3 = points[i+1].y;
                
                double cp1x = x1 + (x2 - points[i-2].x) * 0.1;
                double cp1y = y1 + (y2 - points[i-2].y) * 0.1;
                double cp2x = x2 - (x3 - x1) * 0.1;
                double cp2y = y2 - (y3 - y1) * 0.1;
                
                cr->curve_to(cp1x, cp1y, cp2x, cp2y, x2, y2);
            } else {
                cr->line_to(points[i].x, points[i].y);
            }
        }
    }
    
    if (color_set) {
        cr->stroke();
    }
    
    return true;
}
bool DrawingWindow::should_add_point(double x, double y) {
    if (points.empty()) return true;
    
    double dx = x - points.back().x;
    double dy = y - points.back().y;
    double distance = sqrt(dx*dx + dy*dy);
    
    return distance > 2.0; // Only add points if they're far enough apart
}

bool DrawingWindow::on_button_press_event(GdkEventButton* event) {


    m_signal_mouse_clicked.emit(event->x , event->y , event->button);
    if (!Singleton::getInstance().BrushClicked && !Singleton::getInstance().EraserClicked) {
      
        return false; // Ignore if brush is not clicked
    }
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

        if(Stroke){
            StrokeSize = Stroke;
        }

        if(Opacity){
            OpacityVal = Opacity;
        }
        
        bool is_eraser = Singleton::getInstance().EraserClicked;
        points.push_back({event->x, event->y, pressure, true, current_stroke_color, is_eraser, StrokeSize, OpacityVal});
        queue_draw();
    }
    return true;
}

bool DrawingWindow::on_motion_notify_event(GdkEventMotion* event) {

if(!Singleton::getInstance().BrushClicked && !Singleton::getInstance().EraserClicked) {
        return false; // Ignore if brush is not clicked
       
    }

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
            bool is_eraser = Singleton::getInstance().EraserClicked;
            points.push_back({event->x, event->y, pressure, false, current_stroke_color, is_eraser,StrokeSize, OpacityVal});
            queue_draw();
        }
    }
    return true;
}