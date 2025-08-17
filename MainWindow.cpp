#include "MainWindow.h"
#include <DrawingWindow.h>
#include <iostream>
#include <gtkmm/colorchooser.h>
#include <iomanip>
#include <sstream>
#include <string>

Gdk::RGBA get_color_from_chooser(Gtk::Widget *chooser_widget);
MainWindow::MainWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &refBuilder)
    : Gtk::Window(cobject), m_refBuilder(refBuilder)
{
    // Load CSS
    auto css_provider = Gtk::CssProvider::create();
    css_provider->load_from_path("style.css");

    auto screen = Gdk::Screen::get_default();
    auto context = Gtk::StyleContext::create();
    context->add_provider_for_screen(
        screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    // Get widgets from the UI
    m_refBuilder->get_widget("Eraser", Eraser);
    m_refBuilder->get_widget("brushButton", Brush);
    m_refBuilder->get_widget("FillButton", Bucket);
    m_refBuilder->get_widget("ZoomButton", Zoom);
    //  m_refBuilder->get_widget("ColorButton", ColorButton);
    m_refBuilder->get_widget("mainWindow", window);
    m_refBuilder->get_widget("GtkBox2", ToolBar);
    m_refBuilder->get_widget("GtkBox", GtkBox);
    m_refBuilder->get_widget("subGtkBox", subGtkBox);
   //m_refBuilder->get_widget("BrushScale", BrushScale);
    m_refBuilder->get_widget("GtkBox1", GtkBox1);
    m_refBuilder->get_widget("GtkBox3", HorizontalBar);
    m_refBuilder->get_widget("NewCanvas", CanvasPopover);
    m_refBuilder->get_widget("CancelButton", CancelButton);
    m_refBuilder->get_widget("CreateButton", CreateButton);
    m_refBuilder->get_widget("NewButton", NewButton);
    m_refBuilder->get_widget("WidthEntry", WidthEntry);
    m_refBuilder->get_widget("HeightEntry", HeightEntry);
    m_refBuilder->get_widget("ExportButton", ExportButton);
    m_refBuilder->get_widget("invalidLabel1", InvalidLabel1);
    m_refBuilder->get_widget("invalidLabel2", InvalidLabel2);
    // m_refBuilder->get_widget("ColorSelection", ColorSelection);
    // m_refBuilder->get_widget("SelectionWindow", SelectionWindow);
    // m_refBuilder->get_widget("DrawingArea", DrawingArea);
    // m_refBuilder->get_widget("Notebook", NoteBook);
    m_refBuilder->get_widget("colorChooser1", colorWidget);
    m_refBuilder->get_widget("colorChooser2", colorWidget2);
    m_refBuilder->get_widget("DrawingWindow", DrawingWindow);
    // m_refBuilder->get_widget("testDialog", testDialog);

    // Gdk::RGBA current_color = get_color_from_chooser(colorWidget);

    // if (testDialog)
    // {
    //     testDialog->signal_color_activated().connect(
    //         sigc::mem_fun(*this, &MainWindow::on_testDialog_color_activated)
    //     );
    // }

    CanvasPopover->set_relative_to(*NewButton);
    CanvasPopover->hide();
    //  SelectionWindow->show();
    // colorWidget->show();
    // testDialog->show();
    // DrawingWindow->show();

    // Connect signals
    if (Eraser)
        Eraser->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_Eraser_clicked));

    if (Brush)
        Brush->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_brushButton_clicked));

    if (Bucket)
        Bucket->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_FillButton_clicked));

    if (Zoom)
        Zoom->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_ZoomButton_clicked));

    // if (ColorButton)
    //     ColorButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_ColorButton_clicked));

    if (NewButton)
        NewButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_NewButton_clicked));

    if (CreateButton)
        CreateButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_CreateButton_clicked));

    if (CancelButton)
        CancelButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_CancelButton_clicked));

    // ColorButton->signal_color_set().connect(
    // sigc::mem_fun(*this, &MainWindow::on_ColorButton_color_set)

    // Style Widgets with CSS
    if (Eraser)
        Eraser->set_name("HButton");
    if (Bucket)
        Bucket->set_name("HButton");
    if (Brush)
        Brush->set_name("HButton");
    if (Zoom)
        Zoom->set_name("HButton");
    //  if(ColorButton) ColorButton->set_name("HButton");
    if (window)
        window->set_name("mainWindow");
    if (ToolBar)
        ToolBar->set_name("GtkBox2");
    if (HorizontalBar)
        HorizontalBar->set_name("GtkBox3");
    if (CanvasPopover)
        CanvasPopover->set_name("NewCanvas");
    if (WidthEntry)
        WidthEntry->set_name("CButton");
    if (HeightEntry)
        HeightEntry->set_name("CButton");
    if (CreateButton)
        CreateButton->set_name("CButton");
    if (CancelButton)
        CancelButton->set_name("CButton");
    if (NewButton)
        NewButton->set_name("TopButtons");
    if (ExportButton)
        ExportButton->set_name("TopButtons");
    // if(NoteBook)  NoteBook->set_name("my_notebook");
    if (colorWidget)
        colorWidget->set_name("Notebook");
    if (colorWidget2)
        colorWidget2->set_name("Notebook");
    // ColorSelection->set_name("NewCanvas");

    if (colorWidget)
    {
        GtkWidget *gtk_widget = colorWidget->gobj();
        if (GTK_IS_COLOR_CHOOSER(gtk_widget))
        {
            g_signal_connect(gtk_widget, "notify::rgba",
                             G_CALLBACK(+[](GtkColorChooser *chooser, GParamSpec *pspec, gpointer user_data)
                                        {
                                            MainWindow *window = static_cast<MainWindow *>(user_data);

                                            GdkRGBA color;
                                            gtk_color_chooser_get_rgba(chooser, &color);

                                            int r = static_cast<int>(color.red * 255);
                                            int g = static_cast<int>(color.green * 255);
                                            int b = static_cast<int>(color.blue * 255);

                                            std::stringstream hex_stream;
                                            hex_stream << "#" << std::hex << std::uppercase << std::setfill('0')
                                                       << std::setw(2) << r
                                                       << std::setw(2) << g
                                                       << std::setw(2) << b;

                                            std::cout << "Color: " << hex_stream.str() << " [RGB(" << std::dec << r << ", " << g << ", " << b << ")]" << std::endl;

                                            // Update your drawing color here
                                            window->current_color.set_red(color.red);
                                            window->current_color.set_green(color.green);
                                            window->current_color.set_blue(color.blue);
                                            window->current_color.set_alpha(color.alpha);
                                        }),
                             this);
        }
    }

    if (colorWidget2)
    {
        GtkWidget *gtk_widget2 = colorWidget2->gobj();
        if (GTK_IS_COLOR_CHOOSER(gtk_widget2))
        {
            g_signal_connect(gtk_widget2, "notify::rgba",
                             G_CALLBACK(+[](GtkColorChooser *chooser, GParamSpec *pspec, gpointer user_data)
                                        {
                                            MainWindow *window = static_cast<MainWindow *>(user_data);

                                            GdkRGBA color;
                                            gtk_color_chooser_get_rgba(chooser, &color);

                                            int r = static_cast<int>(color.red * 255);
                                            int g = static_cast<int>(color.green * 255);
                                            int b = static_cast<int>(color.blue * 255);

                                            std::stringstream hex_stream;
                                            hex_stream << "#" << std::hex << std::uppercase << std::setfill('0')
                                                       << std::setw(2) << r
                                                       << std::setw(2) << g
                                                       << std::setw(2) << b;

                                            std::cout << "Color: " << hex_stream.str() << " [RGB(" << std::dec << r << ", " << g << ", " << b << ")]" << std::endl;

                                            // Update the current drawing color
                                            window->current_color.set_red(color.red);
                                            window->current_color.set_green(color.green);
                                            window->current_color.set_blue(color.blue);
                                            window->current_color.set_alpha(color.alpha);
                                        }),
                             this);

            std::cout << "Connected real-time color signal for colorChooser2" << std::endl;
        }
    }
}

Gdk::RGBA MainWindow::get_current_drawing_color() const
{
    return current_color;
}

void MainWindow::on_Eraser_clicked()
{
    std::cout << "Eraser button clicked!" << std::endl;
}

void MainWindow::on_brushButton_clicked()
{
    std::cout << "brushButton button clicked!" << std::endl;
    current_tool = Tool::Brush;
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

void MainWindow::on_NewButton_clicked()
{
    std::cout << "new clicked" << std::endl;
    CanvasPopover->show_all();
    InvalidLabel1->set_text("");
    InvalidLabel2->set_text("");
}

void MainWindow::on_CancelButton_clicked()
{
    std::cout << "cancel" << std::endl;
    CanvasPopover->hide();
    WidthEntry->set_text("");
    HeightEntry->set_text("");
}

void MainWindow::on_CreateButton_clicked()
{
    std::cout << "new clicked" << std::endl;

    Glib::ustring Width = WidthEntry->get_text();
    std::cout << "User typed: " << Width << std::endl;

    Glib::ustring Height = HeightEntry->get_text();
    std::cout << "User typed: " << Height << std::endl;

    if (Width == "")
    {
        std::cout << "Empty" << std::endl;
        InvalidLabel1->set_text("Invalid value");
    }

    if (Height == "")
    {
        InvalidLabel2->set_text("Invalid value");
    }

    else
    {
        CanvasPopover->hide();
        WidthEntry->set_text("");
        HeightEntry->set_text("");

        int Width_ = std::stoi(Width);
        int Height_ = std::stoi(Height);
        auto drawingwindow = Gtk::make_managed<::DrawingWindow>(Width_, Height_, &current_color);
        // drawingwindow->set_current_color(&current_color);
        drawingwindow->set_transient_for(*this); // make it a child of main window
        drawingwindow->set_keep_above(true);     // always on top
        drawingwindow->show();
    }
}

void MainWindow::on_testDialog_color_activated(const Gdk::RGBA &color)
{
    std::cout << "Signal Activated" << std::endl;
    int r = static_cast<int>(color.get_red() * 255);
    int g = static_cast<int>(color.get_green() * 255);
    int b = static_cast<int>(color.get_blue() * 255);

    std::cout << "Color selected: " << r << ", " << g << ", " << b << std::endl;
}

void MainWindow::on_ColorButton_color_set()
{
    Gdk::RGBA color = ColorButton->get_rgba();

    int r = static_cast<int>(color.get_red() * 255);
    int g = static_cast<int>(color.get_green() * 255);
    int b = static_cast<int>(color.get_blue() * 255);

    // std::cout << "Color selected from button: "
    //    << r << ", " << g << ", " << b << std::endl;
}

// Gdk::RGBA get_color_from_chooser(Gtk::Widget* chooser_widget) {
//     if (!chooser_widget) return Gdk::RGBA();

//     GtkWidget* gtk_widget = chooser_widget->gobj();
//     if (!GTK_IS_COLOR_CHOOSER(gtk_widget)) return Gdk::RGBA();

//     GdkRGBA color;
//     gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(gtk_widget), &color);

//     Gdk::RGBA gdk_color;
//     gdk_color.set_red(color.red);
//     gdk_color.set_green(color.green);
//     gdk_color.set_blue(color.blue);
//     gdk_color.set_alpha(color.alpha);

//     int r = static_cast<int>(gdk_color.get_red() * 255);
//     int g = static_cast<int>(gdk_color.get_green() * 255);
//     int b = static_cast<int>(gdk_color.get_blue() * 255);

//     std::cout << "Color selected from notebook: "
//               << r << ", " << g << ", " << b << std::endl;

//     return gdk_color;
// }