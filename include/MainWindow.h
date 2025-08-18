#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm.h>
#include "DrawingWindow.h"

class DrawingWindow; 

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
    void on_NewButton_clicked();
    void on_CancelButton_clicked();
    void on_CreateButton_clicked();
    void on_testDialog_color_activated(const Gdk::RGBA& color);
    void on_ColorButton_color_set();
    Gdk::RGBA get_current_drawing_color() const;
    void on_drawing_window_clicked(double x, double y, guint button);

    Glib::RefPtr<Gtk::Builder> m_refBuilder;

    enum class Tool { None, Brush, Eraser, Fill, Zoom };
    Tool current_tool = Tool::None;

    public:
        Tool get_current_tool() const { return current_tool; }

    Gdk::RGBA current_color;

    // Widgets you need to access
    Gtk::Button* Eraser = nullptr;
    Gtk::Button* Brush = nullptr;
    Gtk::Button* Bucket = nullptr;
    Gtk::Button* Zoom = nullptr;
    Gtk::ColorButton* ColorButton = nullptr;
    Gtk::Button* CancelButton = nullptr;
    Gtk::Button* CreateButton = nullptr;
    Gtk::Button* NewButton = nullptr;
    Gtk::Button* ExportButton = nullptr;
    Gtk::Scale* BrushScale = nullptr;
    

    Gtk::Box* ToolBar = nullptr;
    Gtk::Box* HorizontalBar = nullptr;
    Gtk::Box* GtkBox1 = nullptr;
    Gtk::Box* GtkBox = nullptr;
    Gtk::Box* subGtkBox = nullptr;

    Gtk::Widget* colorWidget = nullptr;
    Gtk::Widget* colorWidget2 = nullptr;

    Gtk::Window* window = nullptr;

    Gtk::Popover* CanvasPopover = nullptr;

    Gtk::Entry* WidthEntry = nullptr;
    Gtk::Entry* HeightEntry = nullptr;

    Gtk::Label* InvalidLabel1 = nullptr;
    Gtk::Label* InvalidLabel2 = nullptr;

    Gtk::Notebook* NoteBook =  nullptr;

    Gtk::ColorChooserDialog* testDialog = nullptr;
    Gtk::ColorSelection* ColorSelection = nullptr;

    Gtk::Window* SelectionWindow = nullptr;
    Gtk::Window* DrawingWindow_ = nullptr;

    Gtk::DrawingArea* DrawingArea = nullptr;

    DrawingWindow* CurrentDrawingWindow = nullptr;



};

#endif // MAINWINDOW_H
