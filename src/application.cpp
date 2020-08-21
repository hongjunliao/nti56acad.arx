#include <application.h>
#include <imgui.h>

class Application : public IApplication
{
public:
    virtual bool Setup()
    {
        return true;
    }
    
    virtual void Render3d()
    {
    }
    
    virtual void Render2d()
    {
        //show Main Window
//        ImGui::ShowDemoWindow();
    }
};

extern IApplication* CreateApplication();

IApplication* CreateApplication()
{
    return new Application();
}
