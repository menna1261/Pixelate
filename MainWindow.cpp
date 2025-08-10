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
    m_refBuilder->get_widget("GtkBox", GtkBox1);
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

    m_refBuilder->get_widget("Notebook", NoteBook); 
    m_refBuilder->get_widget("colorChooser1", colorWidget); 
    m_refBuilder->get_widget("colorChooser2", colorWidget2); 



    CanvasPopover->set_relative_to(*NewButton);
    CanvasPopover->hide();
    //colorWidget->show();

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

    if(NewButton)
        NewButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_NewButton_clicked));

    if(CreateButton)
        CreateButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_CreateButton_clicked));

    if(CancelButton)
        CancelButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_CancelButton_clicked));

    //Style Widgets with CSS
    if(Eraser) Eraser->set_name("HButton");
    if(Bucket) Bucket->set_name("HButton");
    if(Brush) Brush->set_name("HButton");
    if(Zoom) Zoom->set_name("HButton");
    if(ColorButton) ColorButton->set_name("HButton");
    if(window) window->set_name("mainWindow");
    if(ToolBar) ToolBar->set_name("GtkBox2");
    if(HorizontalBar) HorizontalBar->set_name("GtkBox3");
    if(CanvasPopover) CanvasPopover->set_name("NewCanvas");
    if(WidthEntry) WidthEntry->set_name("CButton");
    if(HeightEntry) HeightEntry->set_name("CButton");
    if(CreateButton) CreateButton->set_name("CButton");
    if(CancelButton) CancelButton->set_name("CButton");
    if(NewButton) NewButton->set_name("TopButtons");
    if(ExportButton) ExportButton->set_name("TopButtons");
   // if(NoteBook)  NoteBook->set_name("Notebook");
    if(colorWidget) colorWidget->set_name("Notebook");
    if(colorWidget2) colorWidget2->set_name("Notebook");

    

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
        std::cout<<"Empty"<<std::endl;
        InvalidLabel1->set_text("Invalid value");
    }


    if(Height == ""){
        InvalidLabel2->set_text("Invalid value");
    }

    else{
    CanvasPopover->hide();
    WidthEntry->set_text("");
    HeightEntry->set_text("");

    

    }
    


}