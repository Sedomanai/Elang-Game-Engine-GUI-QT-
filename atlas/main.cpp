#include "atlas_editor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    el::AtlasEditor w(nullptr, false);
    w.show();
    return a.exec();
}
