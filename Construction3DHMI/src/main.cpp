#include <QApplication>
#include <QSurfaceFormat>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
    // OpenGL 3.3 Core
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

    // 必须在 QApplication 和 QOpenGLWidget 创建之前设置
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
