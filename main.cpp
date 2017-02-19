#include "mainwindow.h"
#include <QApplication>
#include <QStringList>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList args;
    QTextCodec *codec = QTextCodec::codecForName("cp1251");
    for (int i = 1; i < argc; ++i)
        args.push_back(codec->toUnicode(argv[i]));
    MainWindow w(args);
    if (QFileInfo(argv[1]).suffix().toLower() != "btl" && w.loadBtl())
        return 0;
    w.show();

    return a.exec();
}
