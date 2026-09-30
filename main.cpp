#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("Hello World1");
    w.setGeometry(100, 100, 800, 600);
    w.setStyleSheet("background-color: #f0f0f0;");
    w.setFont(QFont("Arial", 12));
    w.setWindowIcon(QIcon(":/icon.png"));
    w.setWindowFlags(Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    w.setWindowFlags(Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    w.show();
    return QApplication::exec();
}
