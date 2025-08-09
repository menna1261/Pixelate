#include <gtkmm.h>
#include<iostream>

class MainWindow : public Gtk::Window
{
public:
    MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
        : Gtk::Window(cobject), m_refBuilder(refBuilder)
    {
        // You can connect signals here if needed
    }

private:
    Glib::RefPtr<Gtk::Builder> m_refBuilder;
};

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create(argc, argv, "com.example.pixelapp");

    Glib::RefPtr<Gtk::Builder> builder;

    try
    {
        builder = Gtk::Builder::create_from_file("ui.glade"); // Change filename if needed
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

    MainWindow* window = nullptr;
    builder->get_widget_derived("mainWindow", window);

    if (!window)
    {
        std::cerr << "Failed to load mainWindow from Glade file." << std::endl;
        return 1;
    }

    return app->run(*window);
}
