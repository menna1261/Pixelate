#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm.h>

class MainWindow : public Gtk::Window
{
public:
    MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);

    // Signal handlers
    void on_Eraser_clicked();
    void on_brushButton_clicked();
    void on_ZoomButton_clicked();
    void on_FillButton_clicked();
    void on_ColorButton_clicked();

    Glib::RefPtr<Gtk::Builder> m_refBuilder;

    // Widgets you need to access
    Gtk::Button* Eraser = nullptr;
    Gtk::Button* Brush = nullptr;
    Gtk::Button* Bucket = nullptr;
    Gtk::Button* Zoom = nullptr;
    Gtk::Button* ColorButton = nullptr;

    Gtk::Box* ToolBar = nullptr;
    Gtk::Box* HorizontalBar = nullptr;

    Gtk::Window* window = nullptr;

};

#endif // MAINWINDOW_H
