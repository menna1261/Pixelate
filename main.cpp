#include <gtkmm.h>
#include<MainWindow.h>
#include<iostream>


int main(int argc, char *argv[])
{

    MainWindow* window = nullptr;
    auto app = Gtk::Application::create(argc, argv, "com.example.pixelapp");

    Glib::RefPtr<Gtk::Builder> builder;
    

    try
    {
        builder = Gtk::Builder::create_from_file("../ui/newUI.glade"); 
    }
    catch (const Glib::FileError& ex)
    {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return 1;
    }
    catch (const Glib::MarkupError& ex)
    {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return 1;
    }
    catch (const Gtk::BuilderError& ex)
    {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    }

   
    builder->get_widget_derived("mainWindow", window);

    if (!window)
    {
        std::cerr << "Failed to load mainWindow from Glade file." << std::endl;
        return 1;
    }



    return app->run(*window);
}
