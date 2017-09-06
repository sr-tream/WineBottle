#include "wbgui.h"
#include <QApplication>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/WineBottle.png"));
    QFileInfo file;
    if (argc >=2)
        file.setFile(argv[1]);
    WBGui w(file);
    w.show();

    return a.exec();
}
