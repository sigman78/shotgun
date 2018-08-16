#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>

using namespace Magnum;

class MyApplication: public Platform::Application {
    public:
        explicit MyApplication(const Arguments& arguments);

    private:
        void drawEvent() override;
};

MyApplication::MyApplication(const Arguments& arguments): Platform::Application{arguments} {
    using namespace Magnum::Math::Literals;

    GL::Renderer::setClearColor(0xa5c9ea_rgbf);
}

void MyApplication::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    /* TODO: Add your drawing code here */

    swapBuffers();
}

#if defined(WIN32)
int __stdcall WinMain(void*, void*, char*, int) {
    int argc{};
    MyApplication app({argc, nullptr});
    return app.exec();
}
#else
int main(int argc, char* argv[]) {
    MyApplication app({argc, argv});
    return app.exec();
}
#endif

