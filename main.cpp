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
        builder = Gtk::Builder::create_from_file("../ui/newUI.glade"); // Change filename if needed
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

// Load CSS
auto css_provider = Gtk::CssProvider::create();
css_provider->load_from_path("style.css");

auto screen = Gdk::Screen::get_default();
auto context = Gtk::StyleContext::create();
context->add_provider_for_screen(
    screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER
);

// Give widgets IDs so CSS can target them
window->set_name("mainWindow");

Gtk::Box* box2 = nullptr;
builder->get_widget("GtkBox2", box2);
if (box2) box2->set_name("GtkBox2");

Gtk::Box* box3 = nullptr;
builder->get_widget("GtkBox3", box3);
if (box3) box3->set_name("GtkBox3");



    return app->run(*window);
}
