#include "rec.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Rec w;
    w.show();

    return a.exec();
}
