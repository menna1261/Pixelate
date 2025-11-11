#include "MainWindow.h"
#include <DrawingWindow.h>
#include <iostream>
#include <gtkmm/colorchooser.h>
#include <iomanip>
#include <sstream>
#include <string>
#include "Singleton.cpp"

Gdk::RGBA get_color_from_chooser(Gtk::Widget *chooser_widget);
MainWindow::MainWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &refBuilder)
    : Gtk::Window(cobject), m_refBuilder(refBuilder)
{

    std::cout<<"==============================================================="<<std::endl;

    Singleton::getInstance(); // Ensure Singleton is initialized


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
    m_refBuilder->get_widget("DrawingWindow", DrawingWindow_);
    m_refBuilder->get_widget("ButtomArea", ButtomArea);
    m_refBuilder->get_widget("LayerArea", LayerArea);
    m_refBuilder->get_widget("StrokeArea", StrokeArea);
    m_refBuilder->get_widget("LayerArea", LayerArea);
    m_refBuilder->get_widget("DelLayer", DelLayer);
    m_refBuilder->get_widget("AddLayer", AddLayer);
    m_refBuilder->get_widget("StrokeScale", StrokeScale);
    m_refBuilder->get_widget("OpacityScale", OpacityScale);
    m_refBuilder->get_widget("layers_listbox", layers_listbox);
    
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
    if(AddLayer)
        AddLayer->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_AddLayer_clicked));

    if(DelLayer)
        DelLayer->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_DelLayer_clicked));

    if (NewButton)
        NewButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_NewButton_clicked));

    if (CreateButton)
        CreateButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_CreateButton_clicked));

    if (CancelButton)
        CancelButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_CancelButton_clicked));

    if(StrokeScale)
        StrokeScale->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_StrokeScale_value_changed));

    if(OpacityScale)
        OpacityScale->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_OpacityScale_value_changed));

    

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

    layers_listbox->set_name("Notebook");
    if(AddLayer)
        AddLayer->set_name("HButton");
    if(DelLayer)
        DelLayer->set_name("HButton");

    
    // ColorSelection->set_name("NewCanvas");

    // if(LayerArea)
    //     LayerArea->set_name("NoteBook");

    // if(ButtomArea)
    //     ButtomArea->set_name("Notebook");
    LayerArea->set_name("Notebook");
    StrokeArea->set_name("Notebook");
    DelLayer->set_name("HButton");
    AddLayer->set_name("HButton");
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
    Singleton::getInstance().setEraserClicked(true);
}

void MainWindow::on_brushButton_clicked()
{
    std::cout << "brushButton button clicked!" << std::endl;
    Singleton::getInstance().setBrushClicked(true);
}



void MainWindow::on_FillButton_clicked()
{
    std::cout << "fill button clicked!" << std::endl;
    Singleton::getInstance().setBucketClicked(true);
        if(CurrentDrawingWindow) {
        

        CurrentDrawingWindow->signal_mouse_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::on_drawing_window_clicked)
);
    }


}

void MainWindow::on_ZoomButton_clicked()
{
    std::cout << "zoom button clicked!" << std::endl;
    Singleton::getInstance().setZoomClicked(true);
            CurrentDrawingWindow->signal_mouse_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::on_drawing_window_clicked));
}
void MainWindow::on_drawing_window_clicked(double x, double y, guint button) {
    std::cout << "Mouse clicked in DrawingWindow at (" << x << ", " << y << ") with button " << button << std::endl;
    
    // Handle different tools here
    if (button == 1) {  // Left click
        if (Singleton::getInstance().BucketClicked) {
            // Handle fill tool
            std::cout << "Fill tool activated at click position" << std::endl;
            CurrentDrawingWindow->fill_with_color(current_color);
        }

        else if(Singleton::getInstance().ZoomClicked){
            CurrentDrawingWindow->ApplyZoom();
        }
        // Add other tool handling here
    }
}

void MainWindow::on_ColorButton_clicked()
{
    std::cout << "color button clicked!" << std::endl;


}

void MainWindow::on_NewButton_clicked()
{
    if(CurrentDrawingWindow){
        return;
    }
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
        CurrentDrawingWindow = new DrawingWindow(Width_, Height_, &current_color);

        // Connect the delete event BEFORE showing
        CurrentDrawingWindow->signal_delete_event().connect(
            sigc::mem_fun(*this, &MainWindow::on_DrawingWindow_delete_event)
        );
        // drawingwindow->set_current_color(&current_color);
        CurrentDrawingWindow->set_transient_for(*this); // make it a child of main window
        CurrentDrawingWindow->set_keep_above(true);     // always on top
        CurrentDrawingWindow->show();
    }
}

bool MainWindow::on_DrawingWindow_delete_event(GdkEventAny* event){

    std::cout <<" window is destroyed "<<std::endl;
    CurrentDrawingWindow = nullptr;
    return false;
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

void MainWindow::on_StrokeScale_value_changed(){
    double stroke = StrokeScale->get_value();
    CurrentDrawingWindow->Stroke = stroke;
    std::cout<<stroke<<std::endl;
}

void MainWindow::on_OpacityScale_value_changed(){
    double Opacity  = OpacityScale->get_value();
    CurrentDrawingWindow->Opacity = Opacity;
    std::cout<<"Opacity : " <<Opacity<<std::endl;
}

void MainWindow::on_AddLayer_clicked(){
    
    if(!CurrentDrawingWindow){
        std::cout << "Can't create layer" << std::endl;
        
        // Clear the listbox when drawing window doesn't exist
        auto children = layers_listbox->get_children();
        for (auto child : children) {
            layers_listbox->remove(*child);
        }
        
        // Reset layer counter
        layer_counter = 0;
        
        return;
    }
    
    std::string layer_name = "Layer" + std::to_string(++layer_counter);
    
    // Create button that looks like a label
    auto* layer_button = Gtk::manage(new Gtk::Button(layer_name));
    //layer_button->set_relief(Gtk::RELIEF_NONE); // Remove button appearance
    //m_refBuilder->get_widget("LayerButton",layer_button );
    layer_button->set_halign(Gtk::ALIGN_START);
    layer_button->set_margin_left(5);
    layer_button->set_margin_right(5);
    layer_button->set_margin_top(2);
    layer_button->set_margin_bottom(2);
    layer_button->set_size_request(210, 40);
    layer_button->set_relief(Gtk::RELIEF_NONE);
    layer_button->set_name("lbutton");

    // Connect click signal
    layer_button->signal_clicked().connect([this, layer_name]() {

    std::string prefix = "Layer";
    if (layer_name.substr(0, prefix.length()) == prefix) {
        std::string number_str = layer_name.substr(prefix.length());
        int number = std::stoi(number_str);
        std::cout << "Clicked on: " << layer_name <<"  id:  " <<number << std::endl;
        selected_layer = number;
        
        if(CurrentDrawingWindow)
            CurrentDrawingWindow->ActivateLayer(number);
    }
        
    });
    
    // Create row and add buttonj
    auto* row = Gtk::manage(new Gtk::ListBoxRow());
    row->set_size_request(80, 40);
    
    row->add(*layer_button);
    row->show_all();
    
    layers_listbox->insert(*row, 0);
    if(CurrentDrawingWindow)
        CurrentDrawingWindow->CreateNewLayer();

   // layers_listbox->select_row(*row);
}

void MainWindow::on_DelLayer_clicked(){

    auto  children = layers_listbox->get_children();

    for(auto* child : children){

        Gtk::ListBoxRow* row = dynamic_cast<Gtk::ListBoxRow*>(child);

        if (row ){
            Gtk::Button* button = dynamic_cast<Gtk::Button*>(row->get_child());

            if(button){
                if(button->get_label() == "Layer"+ std::__cxx11::to_string(selected_layer)){
                    std::cout<<"selscted layer is " <<selected_layer;
                    layers_listbox->remove(*child);
                }
            }
        }
    }
    layer_counter--;
    reOrderLayers();
}

void MainWindow::reOrderLayers(){
    auto  children = layers_listbox->get_children();

    int counter = layer_counter;
    for(auto* child : children){

        Gtk::ListBoxRow* row = dynamic_cast<Gtk::ListBoxRow*>(child);

        if (row ){
            Gtk::Button* button = dynamic_cast<Gtk::Button*>(row->get_child());

            if(button){
                if(counter>=0){

                    button->set_label("Layer" + std::__cxx11::to_string(counter+1));
                    counter--;
                }
                
            }
        }
    }
}

// Gtk::Widget* MainWindow::create_layer_widget(const std::string& layer_name) {
//     // Create a horizontal box to hold layer elements
//     auto* layer_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 20));
//     layer_box->set_margin_top(5);
//     layer_box->set_margin_bottom(5);
//     layer_box->set_margin_left(10);
//     layer_box->set_margin_right(10);

//     layer_box->set_name("LayerWidget");
    
   
//     auto* layer_button = Gtk::manage(new Gtk::Button(layer_name));
//     layer_button->set_hexpand(true);

//     auto* layer_label = Gtk::manage(new Gtk::Label(layer_name));
//     layer_label->set_halign(Gtk::ALIGN_START);
//     layer_label->set_hexpand(true);

    
//     // Pack widgets
//     // layer_box->pack_start(*layer_button, true, true, 0);
//     // layer_box->pack_start(*visibility_button, false, false, 0);
//     // layer_box->pack_start(*delete_button, false, false, 0);
    
//     // Connect signals
//     layer_button->signal_clicked().connect([this, layer_name]() {
//         // Handle layer selection
//         //int id = std::to_integer(layer_name[5]);
//         std::string id = layer_name[5]+"";
        
//         std::cout << "Layer selected: " << layer_name <<"id: " <<id << std::endl;
//         //CurrentDrawingWindow->ActivateLayer(id);
//         // Add your layer switching logic here
//     });
    
//     // visibility_button->signal_clicked().connect([this, layer_name]() {
//     //     // Handle visibility toggle
//     //     std::cout << "Toggle visibility for: " << layer_name << std::endl;
//     // });
    
//     DelLayer->signal_clicked().connect([this, layer_box]() {
//         // Handle layer deletion
//         layer_box->get_parent()->remove(*layer_box);
//     });
    
//     layer_box->show_all();
    
//     return layer_box;
// }

