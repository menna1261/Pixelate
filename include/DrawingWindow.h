#include <gtkmm.h>

class DrawingWindow : public Gtk::Window {
public:

    DrawingWindow(float width , float Height){
        set_title("DrawingArea Example");

        // Create DrawingArea
       // drawing_area.set_size_request(width, Height); // width=400px, height=300px
        set_default_size(width, Height);
        // Add it to the window
        //set_child(drawing_area); // For GTKmm 4
         add(drawing_area);    // For GTKmm 3

        // Optional: Connect to draw signal
        drawing_area.signal_draw().connect(sigc::mem_fun(*this, &DrawingWindow::on_draw), false);

        show();
    }

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
        cr->set_source_rgb(1, 1, 1); // blue color
        cr->paint();
        return true;
    }

    Gtk::DrawingArea drawing_area;
};