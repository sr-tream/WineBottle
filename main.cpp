#include "mainwindow.h"
#include <QApplication>
#include <QStringList>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/WineBottle.png"));
    QStringList args;
    for (int i = 1; i < argc; ++i)
        args.push_back(argv[i]);
    MainWindow w(args);
    if (QFileInfo(argv[1]).suffix().toLower() != "btl" && w.loadBtl())
        return 0;
    w.show();

    return a.exec();
}
