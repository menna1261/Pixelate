#include "MainWindow.h"
#include <iostream>

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
    : Gtk::Window(cobject), m_refBuilder(refBuilder)
{
    // Load CSS
    auto css_provider = Gtk::CssProvider::create();
    css_provider->load_from_path("style.css");

    auto screen = Gdk::Screen::get_default();
    auto context = Gtk::StyleContext::create();
    context->add_provider_for_screen(
        screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    // Get widgets from the UI
    m_refBuilder->get_widget("Eraser", Eraser);
    m_refBuilder->get_widget("brushButton", Brush);
    m_refBuilder->get_widget("FillButton", Bucket);
    m_refBuilder->get_widget("ZoomButton", Zoom);
    m_refBuilder->get_widget("ColorButton", ColorButton);
    m_refBuilder->get_widget("mainWindow", window);
    m_refBuilder->get_widget("GtkBox2", ToolBar);
    m_refBuilder->get_widget("GtkBox3", HorizontalBar);

    // Connect signals
    if (Eraser)
        Eraser->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_Eraser_clicked));
    
    if (Brush)
        Brush->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_brushButton_clicked));

    if (Bucket)
        Bucket->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_FillButton_clicked));

    if (Zoom)
        Zoom->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_ZoomButton_clicked));

    if (ColorButton)
        ColorButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_ColorButton_clicked));

    //Style Widgets with CSS
    if(Eraser) Eraser->set_name("HButton");
    if(Bucket) Bucket->set_name("HButton");
    if(Brush) Brush->set_name("HButton");
    if(Zoom) Zoom->set_name("HButton");
    if(ColorButton) ColorButton->set_name("HButton");
    if(window) window->set_name("mainWindow");
    if(ToolBar) ToolBar->set_name("GtkBox2");
    if(HorizontalBar) HorizontalBar->set_name("GtkBox3");

}

void MainWindow::on_Eraser_clicked()
{
    std::cout << "Eraser button clicked!" << std::endl;
}

void MainWindow::on_brushButton_clicked()
{
    std::cout << "brushButton button clicked!" << std::endl;
}

void MainWindow::on_ZoomButton_clicked()
{
    std::cout << "zoom button clicked!" << std::endl;
}

void MainWindow::on_FillButton_clicked()
{
    std::cout << "fill button clicked!" << std::endl;
}

void MainWindow::on_ColorButton_clicked()
{
    std::cout << "color button clicked!" << std::endl;
}