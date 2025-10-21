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
   // void markCurrentLayerDirty();
    void FillBackGround( Gdk::RGBA* color);
    void fill_with_color(const Gdk::RGBA& color);
    void ApplyZoom();
    void createBrushSizeCursor();
    void CreateNewLayer();
    void ActivateLayer(int index);
    void ClearLayers();
    int getLayerIndex();
    sigc::signal<void(double, double, guint)> signal_mouse_clicked();
    sigc::signal<void()> signal_draw_cursor();
    void draw_layer_points(const Cairo::RefPtr<Cairo::Context>& cr, const std::vector<Point>& layer_points);
    void signal_cursor();
    double Stroke{};
    double StrokeSize;
    double Opacity;
    double OpacityVal;
    int layerCount = 0;
    int CurrentIndex=0;
    double ZoomFactor = 1.0;
    

protected:
    // Signal handlers
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    bool on_button_press_event(GdkEventButton* event) override;
    bool on_motion_notify_event(GdkEventMotion* event) override;
    bool should_add_point(double x, double y);
    

private:
    sigc::signal<void(double, double, guint)> m_signal_mouse_clicked;
    sigc::signal<void()> m_signal_draw_cursor;
    bool fill_background = false; 
    Gdk::RGBA fill_color;       
    bool brushClicked;
    Gtk::DrawingArea drawing_area;
    //std::vector<Point> points;
    //std::vector<Point>CurrentLayer;
    std::vector<std::pair<std::vector<Point>, int>> Layers;
    Gdk::RGBA* currentColor;  // Reference to MainWindow's current color
    Gdk::RGBA current_stroke_color;  // Color of the current stroke being drawn
    Cairo::RefPtr<Cairo::Context> crr;
    private:
    struct LayerCache {
        Cairo::RefPtr<Cairo::ImageSurface> surface;
        bool needsRedraw = true;
        size_t lastPointCount = 0;
    };
    
    std::vector<LayerCache> layerCaches;
    int canvasWidth = 0, canvasHeight = 0;
    bool surfacesInitialized = false;
    
    void markLayerDirty(int layerIndex);
    void ensureSurfacesSize(int width, int height);
    void draw_layer_points_optimized(const Cairo::RefPtr<Cairo::Context>& cr,
                                    const std::vector<Point>& layer_points);
    void draw_single_stroke_optimized(const Cairo::RefPtr<Cairo::Context>& cr,
                                     const std::vector<Point>& points,
                                     size_t start, size_t end);
};

#endif // DRAWINGWINDOW_H