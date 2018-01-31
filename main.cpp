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

	if (file.isFile()){
		auto gui = new WBGui(file);
		gui->show();
	} else {
		auto set = new QSettings("Prime-Hack", "WineBottle");
		auto wbset = new WBSettings(set);
		wbset->load();
		wbset->show();
	}

	return a.exec();
}
