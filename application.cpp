#include <application.h>
#include <imgui.h>
#include <iostream>

class Application : public IApplication
{
public:
    virtual bool Setup()
    {
        std::cout << "bool Setup()" << std::endl;
        return true;
    }
    
    virtual void Render3d()
    {
        std::cout << "void Render3d()" << std::endl;
    }
    
    virtual void Render2d()
    {
        std::cout << "void Render2d()" << std::endl;
        //show Main Window
//        ImGui::ShowDemoWindow();
    }
};

extern IApplication* CreateApplication();

IApplication* CreateApplication()
{
    return new Application();
}
