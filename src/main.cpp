#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("Hello World1");
    w.setGeometry(100, 100, 800, 600);
    w.setStyleSheet("background-color:rgb(29, 114, 189);");
    w.show();
    return QApplication::exec();
}
