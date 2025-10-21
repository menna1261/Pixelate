#include <gtkmm.h>
#include<iostream>
class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance; //instantiated on first use
        return instance;
    }

    bool BrushClicked = false;
    bool EraserClicked = false;
    bool BucketClicked = false;
    bool ZoomClicked = false;
    Gdk::RGBA background_color ;

    void setBrushClicked(bool value) {
        std::cout << "Brush clicked: " << value << std::endl;
        BrushClicked = value;
        EraserClicked = !value;
        BucketClicked = !value;
        ZoomClicked = !value;
    }
    void setEraserClicked(bool value) {
        BrushClicked = !value;
        EraserClicked = value;
        BucketClicked = !value;
        ZoomClicked = !value;
    }
    void setBucketClicked(bool value) {
        BrushClicked = !value;
        EraserClicked = !value;
        BucketClicked = value;
        ZoomClicked = !value;
    }
    void setZoomClicked(bool value) {
        BrushClicked = !value;
        EraserClicked = !value;
        BucketClicked = !value;
        ZoomClicked = value;
    }

    // Delete copy constructor and assignment operator to prevent copies
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton() {}

};