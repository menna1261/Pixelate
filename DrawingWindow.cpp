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

     ZoomFactor = 1.0;
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

void DrawingWindow::ensureSurfacesSize(int width, int height) {
    // Only recreate surfaces if size changed
    if (canvasWidth == width && canvasHeight == height && surfacesInitialized && layerCaches.size() == Layers.size()) {
        return;
    }
    
    canvasWidth = width;
    canvasHeight = height;
    
    // Resize cache vector to match layers
    layerCaches.resize(Layers.size());
    
    // Create or recreate surfaces only when needed
    for (size_t i = 0; i < layerCaches.size(); ++i) {
        if (!layerCaches[i].surface || layerCaches[i].surface->get_width() != width || layerCaches[i].surface->get_height() != height) {
            layerCaches[i].surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
            layerCaches[i].needsRedraw = true;
        }
    }
    
    surfacesInitialized = true;
}

void DrawingWindow::markLayerDirty(int layerIndex) {
    // Ensure the cache vector is big enough
    while (layerCaches.size() <= layerIndex) {
        layerCaches.emplace_back();
    }
    
    if (layerIndex >= 0 && layerIndex < layerCaches.size()) {
        layerCaches[layerIndex].needsRedraw = true;
    }
}

bool DrawingWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    // Get canvas dimensions
    Gtk::Allocation allocation = drawing_area.get_allocation();
    int width = allocation.get_width();
    int height = allocation.get_height();
    
    // Safety check for valid dimensions
    //This mf made the code crash multiple times
    if (width <= 0 || height <= 0) {
        return true;
    }
    
    // Ensure surfaces are correct size
    ensureSurfacesSize(width, height);


    
    // Draw background ONCE
    if (fill_background) {
        cr->set_source_rgb(fill_color.get_red(), fill_color.get_green(), fill_color.get_blue());
        cr->paint();
    } else {
        cr->set_source_rgb(1, 1, 1);
        cr->paint();
    }
    
        if (ZoomFactor && Singleton::getInstance().ZoomClicked) {
        cr->save(); // Save current state
        cr->scale(ZoomFactor, ZoomFactor);
    }
    // Process each layer
    for (size_t layer_idx = 0; layer_idx < Layers.size(); ++layer_idx) {
        const auto& layer = Layers[layer_idx];
        const std::vector<Point>& layer_points = layer.first;
        
        // Skip empty layers
        if (layer_points.empty()) continue;
        
        // Safety check for cache bounds
        if (layer_idx >= layerCaches.size()) {
            std::cerr << "Cache size mismatch! layer_idx: " << layer_idx << ", cache size: " << layerCaches.size() << std::endl;
            continue;
        }
        
        LayerCache& cache = layerCaches[layer_idx];
        
        // Safety check for surface existence
        if (!cache.surface) {
            std::cerr << "Surface is null for layer " << layer_idx << std::endl;
            cache.surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
            cache.needsRedraw = true;
        }
        
        // Check if layer actually changed 
        bool layerChanged = cache.needsRedraw || 
                           cache.lastPointCount != layer_points.size();
        
        // Only re-render if layer actually changed
        if (layerChanged) {
            try {
                auto layer_cr = Cairo::Context::create(cache.surface);
                
                // Clear the layer surface (ONCE)
                layer_cr->set_operator(Cairo::OPERATOR_CLEAR);
                layer_cr->paint();
                layer_cr->set_operator(Cairo::OPERATOR_OVER);
                
                // Set up drawing properties (ONCE per layer)
                layer_cr->set_line_cap(Cairo::LINE_CAP_ROUND);
                layer_cr->set_line_join(Cairo::LINE_JOIN_ROUND);
                
                // Draw this layer's content
                draw_layer_points_optimized(layer_cr, layer_points);
                
                // Update cache state
                cache.needsRedraw = false;
                cache.lastPointCount = layer_points.size();
            } catch (const std::exception& e) {
                std::cerr << "Error rendering layer " << layer_idx << ": " << e.what() << std::endl;
                continue;
            }
        }
        
        try {
            // Composite cached surface onto main canvas (fast operation)
            cr->set_source(cache.surface, 0, 0);
            cr->set_operator(Cairo::OPERATOR_OVER);
            cr->paint();
        } catch (const std::exception& e) {
            std::cerr << "Error compositing layer " << layer_idx << ": " << e.what() << std::endl;
        }
    }

        if (ZoomFactor && Singleton::getInstance().ZoomClicked) {
        cr->restore();
    }
    
    return true;
}

// Optimized drawing function with fewer Cairo calls
void DrawingWindow::draw_layer_points_optimized(const Cairo::RefPtr<Cairo::Context>& cr,
                                               const std::vector<Point>& layer_points) {
    if (layer_points.empty() || !cr) return;
    
    try {
        // Group consecutive points by stroke properties to minimize context switches
        size_t i = 0;
        while (i < layer_points.size()) {
            if (!layer_points[i].new_stroke && i > 0) {
                i++;
                continue;
            }
            
            // Find the end of this stroke
            size_t stroke_end = i + 1;
            while (stroke_end < layer_points.size() && !layer_points[stroke_end].new_stroke) {
                stroke_end++;
            }
            
            // Draw entire stroke at once
            draw_single_stroke_optimized(cr, layer_points, i, stroke_end);
            
            i = stroke_end;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in draw_layer_points_optimized: " << e.what() << std::endl;
    }
}

void DrawingWindow::draw_single_stroke_optimized(const Cairo::RefPtr<Cairo::Context>& cr,
                                                const std::vector<Point>& points,
                                                size_t start, size_t end) {
    if (start >= points.size() || start >= end || !cr) return;
    
    try {
        const Point& first_point = points[start];
        
        // Set stroke properties ONCE per stroke
        cr->set_line_width(1.0 + first_point.pressure * first_point.PointStroke);
        
        if (first_point.is_eraser) {
            cr->set_operator(Cairo::OPERATOR_CLEAR);
            cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);
        } else {
            cr->set_operator(Cairo::OPERATOR_OVER);
            cr->set_source_rgba(first_point.color.get_red(), 
                               first_point.color.get_green(),
                               first_point.color.get_blue(), 
                               0.5 + first_point.pressure * first_point.PointOpacity / 100);
        }
        
        // Build path ONCE
        cr->move_to(first_point.x, first_point.y);
        
        for (size_t i = start + 1; i < end && i < points.size(); ++i) {
            const Point& point = points[i];
            
            if (i < end - 1 && i > start + 1 && (i - 2) < points.size()) {
                // Use smooth curves for middle points - with bounds checking
                const Point& prev = points[i-1];
                const Point& next = points[i+1];
                const Point& prev2 = points[i-2];
                
                double cp1x = prev.x + (point.x - prev2.x) * 0.1;
                double cp1y = prev.y + (point.y - prev2.y) * 0.1;
                double cp2x = point.x - (next.x - prev.x) * 0.1;
                double cp2y = point.y - (next.y - prev.y) * 0.1;
                
                cr->curve_to(cp1x, cp1y, cp2x, cp2y, point.x, point.y);
            } else {
                cr->line_to(point.x, point.y);
            }
        }
        
        // Stroke ONCE per stroke
        cr->stroke();
    } catch (const std::exception& e) {
        std::cerr << "Error in draw_single_stroke_optimized: " << e.what() << std::endl;
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
    m_signal_mouse_clicked.emit(event->x, event->y, event->button);
    
    if (!Singleton::getInstance().BrushClicked && !Singleton::getInstance().EraserClicked) {
        return false;
    }
    
    if (event->button == 1) {
        double pressure = 0.5;
        if (event->device) {
            gdouble value;
            GdkAxisUse axis_use = GDK_AXIS_PRESSURE;
            if (gdk_device_get_axis(event->device, event->axes, axis_use, &value)) {
                pressure = value;
            }
        }
        
        if (currentColor) {
            current_stroke_color = *currentColor;
        }
        if (Stroke) {
            StrokeSize = Stroke;
        }
        if (Opacity) {
            OpacityVal = Opacity;
        }
        
        bool is_eraser = Singleton::getInstance().EraserClicked;
        Point new_point = {event->x, event->y, pressure, true, current_stroke_color, is_eraser, StrokeSize, OpacityVal};
        
        // Add point and mark layer dirty
        if (CurrentIndex > 0 && CurrentIndex <= Layers.size()) {
            Layers[CurrentIndex-1].first.push_back(new_point);
            
            // Ensure we have enough cache slots BEFORE marking dirty
            while (layerCaches.size() < Layers.size()) {
                layerCaches.emplace_back();
                layerCaches.back().needsRedraw = true;
            }
            
            markLayerDirty(CurrentIndex - 1);  // Only mark current layer dirty
        }
        
        queue_draw();
    }
    return true;
}

bool DrawingWindow::on_motion_notify_event(GdkEventMotion* event) {
    if (!Singleton::getInstance().BrushClicked && !Singleton::getInstance().EraserClicked) {
        return false;
    }

    if (event->state & GDK_BUTTON1_MASK) {
        double pressure = 0.5;
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
            
            if (CurrentIndex > 0 && CurrentIndex <= Layers.size()) {
                Layers[CurrentIndex-1].first.push_back(new_point);
                markLayerDirty(CurrentIndex - 1);  // Only mark current layer dirty
            }
            
            queue_draw();
        }
    }
    return true;
}

void DrawingWindow::CreateNewLayer() {
    ++layerCount;
    std::vector<Point> new_layer;
    Layers.push_back(std::make_pair(new_layer, layerCount));
    
    // Add cache entry for new layer
    layerCaches.emplace_back();
    layerCaches.back().needsRedraw = true;
    
    // Force surface recreation to match new layer count
    surfacesInitialized = false;
    
    std::cout << "New layer added, total: " << Layers.size() << std::endl;
}


//TBH Claude wrote this function
void DrawingWindow::signal_cursor() {
    
    int cursor_diameter = std::max(8, std::min(64, (int)Stroke));
    int cursor_size = cursor_diameter + 4; 
   
    auto surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, cursor_size, cursor_size);
    auto cr = Cairo::Context::create(surface);
    
   
    cr->set_operator(Cairo::OPERATOR_CLEAR);
    cr->paint();
    cr->set_operator(Cairo::OPERATOR_OVER);
    
    
    cr->set_source_rgba(0.0, 0.0, 0.0, 0.8); 
    cr->set_line_width(1.0);
    cr->arc(cursor_size/2.0, cursor_size/2.0, cursor_diameter/2.0, 0, 2 * M_PI);
    cr->stroke();
    
    
    cr->move_to(cursor_size/2.0 - 4, cursor_size/2.0);
    cr->line_to(cursor_size/2.0 + 4, cursor_size/2.0);
    cr->move_to(cursor_size/2.0, cursor_size/2.0 - 4);
    cr->line_to(cursor_size/2.0, cursor_size/2.0 + 4);
    cr->stroke();
    
    
    auto pixbuf = Gdk::Pixbuf::create(surface, 0, 0, cursor_size, cursor_size);
    
   
    auto display = get_display();
    auto cursor = Gdk::Cursor::create(display, pixbuf, cursor_size/2, cursor_size/2);
    
    drawing_area.get_window()->set_cursor(cursor);
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

void DrawingWindow::DeleteLayer(int index){
    int layerSize = Layers[index-1].first.size();
    Layers[index-1].first.clear();

    std::cout<<"Layer " << index <<"is cleared"<<std::endl;

    // for(int i = 0 ; i <layerSize;i++){
    //     Layers[index-1].first[i] = nullptr;
    // }

}

void DrawingWindow::ApplyZoom(){

    ZoomFactor = 10;
    queue_draw();
}


   