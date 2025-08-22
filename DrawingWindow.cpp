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
    signal_draw_cursor().connect(sigc::mem_fun(*this, &DrawingWindow::signal_cursor));

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

sigc::signal<void()>DrawingWindow::signal_draw_cursor(){
    return m_signal_draw_cursor;
}

void DrawingWindow::fill_with_color(const Gdk::RGBA& color) {
    fill_background = true;
    fill_color = color;
    queue_draw();  
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
    
    // Draw ALL layers
    for (const auto& layer : Layers) {
        const std::vector<Point>& layer_points = layer.first;
        draw_layer_points(cr, layer_points);
    }
    
    std::cout << "Drawing all layers, active: " << CurrentIndex << std::endl;
    return true;
}

void DrawingWindow::draw_layer_points(const Cairo::RefPtr<Cairo::Context>& cr, 
                                     const std::vector<Point>& layer_points) {
    Gdk::RGBA last_color;
    bool color_set = false;
    bool last_was_eraser = false;
    
    for (size_t i = 0; i < layer_points.size(); ++i) {
        if (layer_points[i].new_stroke) {
            // Finish previous stroke
            if (color_set) {
                cr->stroke();
            }
            
            const Gdk::RGBA& point_color = layer_points[i].color;
            double pressure = layer_points[i].pressure;
            bool is_eraser = layer_points[i].is_eraser;
         
            cr->set_line_width(1.0 + pressure * layer_points[i].PointStroke);
            
            if (is_eraser) {
                // Eraser: paint with background color
                if (fill_background) {
                    cr->set_source_rgba(fill_color.get_red(), fill_color.get_green(), 
                                       fill_color.get_blue(), 1.0);
                } else {
                    cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
                }
            } else {
                // Regular brush: use stroke color
                cr->set_source_rgba(point_color.get_red(), point_color.get_green(), 
                                   point_color.get_blue(), 0.5 + pressure * (layer_points[i].PointOpacity)/100);
            }
            
            cr->move_to(layer_points[i].x, layer_points[i].y);
            last_color = point_color;
            last_was_eraser = is_eraser;
            color_set = true;
            
        } else if (i > 0 && !layer_points[i-1].new_stroke) {

            if (i == 1 || layer_points[i-1].new_stroke) {
                cr->line_to(layer_points[i].x, layer_points[i].y);
            } else if (i < layer_points.size() - 1) {
                // Smooth curve using cubic Bézier
                double x1 = layer_points[i-1].x;
                double y1 = layer_points[i-1].y;
                double x2 = layer_points[i].x;
                double y2 = layer_points[i].y;
                double x3 = layer_points[i+1].x;
                double y3 = layer_points[i+1].y;
                
                double cp1x = x1 + (x2 - layer_points[i-2].x) * 0.1;
                double cp1y = y1 + (y2 - layer_points[i-2].y) * 0.1;
                double cp2x = x2 - (x3 - x1) * 0.1;
                double cp2y = y2 - (y3 - y1) * 0.1;
                
                cr->curve_to(cp1x, cp1y, cp2x, cp2y, x2, y2);
            } else {
                cr->line_to(layer_points[i].x, layer_points[i].y);
            }
        }
    }
    
    if (color_set) {
        cr->stroke();
    }
}



bool DrawingWindow::should_add_point(double x, double y) {
    if (CurrentIndex <= 0 || CurrentIndex > Layers.size()) return true;
    
    auto& current_layer_points = Layers[CurrentIndex-1].first;
    if (current_layer_points.empty()) return true;
    
    double dx = x - current_layer_points.back().x;
    double dy = y - current_layer_points.back().y;
    double distance = sqrt(dx*dx + dy*dy);
    
    return distance > 2.0;
}

bool DrawingWindow::on_button_press_event(GdkEventButton* event) {

    m_signal_draw_cursor.emit();
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
        Point new_point = {event->x, event->y, pressure, true, current_stroke_color, is_eraser, StrokeSize, OpacityVal};
        
        // Add point to the ACTIVE layer, not the global points vector
        if (CurrentIndex > 0 && CurrentIndex <= Layers.size()) {
            Layers[CurrentIndex-1].first.push_back(new_point);
        }
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
        Point new_point = {event->x, event->y, pressure, false, current_stroke_color, is_eraser, StrokeSize, OpacityVal};
        
        // Add point to the ACTIVE layer, not the global points vector
        if (CurrentIndex > 0 && CurrentIndex <= Layers.size()) {
            Layers[CurrentIndex-1].first.push_back(new_point);
        }
            queue_draw();
        }
    }
    return true;
}


//TBH Claude wrote this function
void DrawingWindow::signal_cursor() {
    // Calculate cursor size (clamp between reasonable bounds)
    int cursor_diameter = std::max(8, std::min(64, (int)Stroke));
    int cursor_size = cursor_diameter + 4; // Add padding
    
    // Create surface for drawing cursor
    auto surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, cursor_size, cursor_size);
    auto cr = Cairo::Context::create(surface);
    
    // Clear background (transparent)
    cr->set_operator(Cairo::OPERATOR_CLEAR);
    cr->paint();
    cr->set_operator(Cairo::OPERATOR_OVER);
    
    // Draw brush size circle
    cr->set_source_rgba(0.0, 0.0, 0.0, 0.8); // Semi-transparent black
    cr->set_line_width(1.0);
    cr->arc(cursor_size/2.0, cursor_size/2.0, cursor_diameter/2.0, 0, 2 * M_PI);
    cr->stroke();
    
    // Draw center crosshair
    cr->move_to(cursor_size/2.0 - 4, cursor_size/2.0);
    cr->line_to(cursor_size/2.0 + 4, cursor_size/2.0);
    cr->move_to(cursor_size/2.0, cursor_size/2.0 - 4);
    cr->line_to(cursor_size/2.0, cursor_size/2.0 + 4);
    cr->stroke();
    
    // Convert Cairo surface to GdkPixbuf
    auto pixbuf = Gdk::Pixbuf::create(surface, 0, 0, cursor_size, cursor_size);
    
    // Create and set cursor
    auto display = get_display();
    auto cursor = Gdk::Cursor::create(display, pixbuf, cursor_size/2, cursor_size/2);
    
    drawing_area.get_window()->set_cursor(cursor);
}

void DrawingWindow::CreateNewLayer(){
    ++layerCount;
    std::vector<Point> new_layer;
    Layers.push_back(std::make_pair(new_layer, layerCount));
    std::cout<<"New layer is added" <<std::endl;
}

void DrawingWindow::ActivateLayer(int index){

    std::cout<<"Activating Layer : " <<index<<std::endl;
        CurrentIndex = index;
        if(Layers.size()!=0)
        std::cout<<"============ :  " <<Layers[index-1].second<<std::endl;
        else
            std::cout<<"NULL"<<std::endl;
    queue_draw();
}

