#include "wbgui.h"
#include <QProcess>
#include <QRegExp>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QEvent>

WBGui::WBGui(QFileInfo file, QWidget *parent) :
	QWidget(parent)
{
	setupUi(this);
	set = new QSettings("Prime-Hack", "WineBottle", this);
	wbset = new WBSettings(set, this);
	connect(wbset, &WBSettings::onHide,
			this,  &WBGui::wbs_closed);

	resize(set->value("wbgui/width", 380).toInt(),
		   set->value("wbgui/height", 310).toInt()
		   );

	loadBottles();
	QString bName = "Bottle_" + set->value("LastBottle").toString();
	bottles->setCurrentText(set->value(bName + "/name").toString());

	prog = file;
	if (!prog.filePath().isEmpty())
		if (prog.filePath().at(0) != '/')
			prog.setFile(QDir::currentPath(), file.filePath());
	loadProgramm();

	prog_run->setFocus(Qt::OtherFocusReason);
}

void WBGui::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;
	default:
		break;
	}
}

void WBGui::closeEvent(QCloseEvent *event)
{
	set->setValue("LastBottle", bottleNumber[bottles->currentText()]);

	if (!prog.isFile())
		return;
	QString appName = "Programm_" + prog.filePath().replace('/', '\\');

	set->setValue(appName + "/Logging", hasLogging->isChecked());
	set->setValue(appName + "/UAC",     hasUAC->isChecked());
	set->setValue(appName + "/Console", hasConsole->isChecked());
	set->setValue(appName + "/NoVSync", noVSync->isChecked());
	set->setValue(appName + "/Window",  hasDesktop->isChecked());
	set->setValue(appName + "/WinX",    desktop_X->value());
	set->setValue(appName + "/WinY",    desktop_Y->value());
	set->setValue(appName + "/Args",    prog_args->text());
	set->setValue(appName + "/Bottle",  bottles->currentText());
	set->setValue(appName + "/AddEnv",  addEnv->toPlainText());

	if (dx_opengl->isChecked())
		set->setValue(appName + "/DirectX", 0);
	else if (dx_csmt->isChecked())
		set->setValue(appName + "/DirectX", 1);
	else // Gallium Nine
		set->setValue(appName + "/DirectX", 2);
	if (!isHidden())
		applyDirectX();

	set->setValue(appName + "/width", size().width());
	set->setValue(appName + "/height", size().height());
}

void WBGui::toggleUAC()
{
	QRegExp re(R"(wine-(\d+)\.(\d+).*)");
	quint32 bottleId = bottleNumber[bottles->currentText()];
	QString bName = "Bottle_" + QString::number(bottleId);
	QString path = set->value(bName + "/wine").toString() + "bin/";

	QProcess proc;
	proc.setProgram(path + "wine");
	proc.setArguments(QStringList("--version"));
	proc.start();
	proc.waitForStarted();
	proc.waitForFinished();
	QString out = proc.readAllStandardOutput().toStdString().c_str();

	hasUAC->setEnabled(false);
	if (re.indexIn(out) == 0)
		if (re.cap(1).toInt() >= 2 && re.cap(2).toInt() >= 14)
			hasUAC->setEnabled(true);

	if (!hasUAC->isEnabled())
		hasUAC->setChecked(false);
}

void WBGui::toggleCSMT()
{
	quint32 bottleId = bottleNumber[bottles->currentText()];
	QString bName = "Bottle_" + QString::number(bottleId);
	QString bottle = set->value(bName + "/bottle").toString();
	QString dllPath = bottle + "drive_c/windows/system32/d3d9-nine.dll";

	if (QFile::exists(dllPath))
		dx_nine->setEnabled(true);
	else dx_nine->setEnabled(false);
}

void WBGui::toggleNINE()
{
	quint32 bottleId = bottleNumber[bottles->currentText()];
	QString bName = "Bottle_" + QString::number(bottleId);
	QString bottle = set->value(bName + "/bottle").toString();
	QString dllPath = bottle + "drive_c/windows/system32/wined3d-csmt.dll";

	if (QFile::exists(dllPath))
		dx_csmt->setEnabled(true);
	else dx_csmt->setEnabled(false);
}

void WBGui::loadBottles()
{
	bottleCount = set->value("BottleCount").toInt();
	bottles->clear();
	bottleNumber.clear();

	if (bottleCount == 0){
		QDir defWine(QDir::homePath() + "/.wine/");
		if (defWine.exists()){
			bottleCount++;
			set->setValue("BottleCount", bottleCount);
			QString bName = "Bottle_" + QString::number(bottleCount);
			set->setValue(bName + "/wine", "/usr/");
			set->setValue(bName + "/bottle", QDir::homePath() +"/.wine/");
			set->setValue(bName + "/name", "Default");
			bottleNumber["Default"] = bottleCount;
			bottles->addItem("Default");
		}
	} else {
		for (int i = 1; i < bottleCount + 1; ++i){
			QString bName = "Bottle_" + QString::number(i);
			QString name = set->value(bName + "/name").toString();
			bottleNumber[name] = i;
			bottles->addItem(name);
		}
	}
}

void WBGui::loadProgramm()
{
	if (!prog.isFile())
		return;

	setWindowTitle(prog.fileName().remove("." + prog.suffix()));
	prog_run->setEnabled(true);
	prog_desktop->setEnabled(true);

	QString appName = "Programm_" + prog.filePath().replace('/', '\\');
	QString bName = set->value(appName + "/Bottle").toString();
	if (bName.isEmpty())
		return;
	quint32 bottleId = bottleNumber[bName];
	if (bottleId > 0)
		bottles->setCurrentText(bName);

	hasLogging->setChecked(set->value(appName + "/Logging").toBool());
	hasUAC->setChecked(    set->value(appName + "/UAC").toBool());
	hasConsole->setChecked(set->value(appName + "/Console").toBool());
	noVSync->setChecked(   set->value(appName + "/NoVSync").toBool());
	hasDesktop->setChecked(set->value(appName + "/Window").toBool());
	desktop_X->setValue(   set->value(appName + "/WinX").toUInt());
	desktop_Y->setValue(   set->value(appName + "/WinY").toUInt());
	prog_args->setText(    set->value(appName + "/Args").toString());
	addEnv->setPlainText(  set->value(appName + "/AddEnv").toString());

	int dx = set->value(appName + "/DirectX").toInt();
	if (dx){
		if (dx == 1)
			dx_csmt->setChecked(true);
		else dx_nine->setChecked(true);
	} else dx_opengl->setChecked(true);

	resize(set->value(appName + "/width", 380).toInt(),
		   set->value(appName + "/height", 310).toInt()
		   );
}

void WBGui::applyDirectX()
{
	hide();
	quint32 bottleId = bottleNumber[bottles->currentText()];
	QString bName = "Bottle_" + QString::number(bottleId);
	QString path = set->value(bName + "/wine").toString() + "bin/";
	QString bottle = set->value(bName + "/bottle").toString();

	QStringList env = QProcess::systemEnvironment();
	env << "WINEPREFIX=" + bottle;
	env << "WINE=" + path + "wine";
	env << "REGEDIT=" + path + "regedit";
	env << "CONSOLE=" + path + "wineconsole";
	env << "WINEFILE=" + path + "winefile";
	env << "WINEBOOT=" + path + "wineboot";

	QString tmp_dir = "/tmp/WineBottle/Bottle_" + QString::number(bottleId);
	QDir dir("/tmp/WineBottle");
	if (!dir.exists())
		dir.mkdir(dir.path());
	dir.setPath(tmp_dir);
	if (!dir.exists())
		dir.mkdir(dir.path());

	QString section = "[" + regSection + prog.fileName() + "\\DllRedirects]";

	QFile f(tmp_dir + "/" + prog.fileName() + ".reg");
	if (f.exists())
		f.remove();
	f.open(QIODevice::WriteOnly);
	f.write("\xFF\xFE");
	f.write((char*)regTitle.toStdU16String().c_str(), regTitle.length() * sizeof(char16_t));
	f.write((char*)section.toStdU16String().c_str(), section.length() * sizeof(char16_t));
	f.write("\r\x00\n\x00", 4);

	QString csmt = "\"wined3d\"=" + QString(dx_csmt->isChecked() ? "\"wined3d-csmt.dll\"" : "-") + "\r\n";
	QString nine = "\"d3d9\"=" + QString(dx_nine->isChecked() ? "\"d3d9-nine.dll\"" : "-") + "\r\n";

	f.write((char*)csmt.toStdU16String().c_str(), csmt.length() * sizeof(char16_t));
	f.write((char*)nine.toStdU16String().c_str(), nine.length() * sizeof(char16_t));
	f.close();

	QStringList reg_args;
	reg_args << "cmd";
	reg_args << "/c";
	reg_args << "regedit";
	reg_args << tmp_dir + "/" + prog.fileName() + ".reg";
	QProcess reg_proc;
	reg_proc.setEnvironment(env);
	reg_proc.setProgram(path + "wine");
	reg_proc.setArguments(reg_args);
	reg_proc.start();
	reg_proc.waitForFinished();
}

void WBGui::wbs_closed()
{
	wb_settings->setEnabled(true);
	int index = bottles->currentIndex();
	loadBottles();
	if (index > bottles->count())
		bottles->setCurrentIndex(bottles->count() - 1);
	else bottles->setCurrentIndex(index);
}

void WBGui::on_wb_settings_clicked()
{
	wbset->load();
	wb_settings->setEnabled(false);
}

void WBGui::on_bottles_currentIndexChanged(const QString &arg1)
{
	toggleUAC();
	toggleCSMT();
	toggleNINE();
}

void WBGui::on_prog_run_clicked()
{
	quint32 bottleId = bottleNumber[bottles->currentText()];
	QString bName = "Bottle_" + QString::number(bottleId);
	QString path = set->value(bName + "/wine").toString() + "bin/";
	QString bottle = set->value(bName + "/bottle").toString();
	QString launcher = set->value(bName + "/launcher").toString();

	if (!launcher.isEmpty()){
		int key = QMessageBox::warning(this, "!!!WarninG!!!",
									   "Launcher can be used only from link!\n"
									   "You can must create link in WineBottle.\n\n"
									   "Continue starting programm without launcher?",
									   QMessageBox::Yes, QMessageBox::No);
		if (key == QMessageBox::No)
			return;
	}

	QStringList args;
	if (hasDesktop->isChecked()){
		QString dskName = prog.fileName().remove("." + prog.suffix());
		QString desktop = "/desktop=" + dskName + "," +
				QString::number(desktop_X->value()) + "x" +
				QString::number(desktop_Y->value());
		args << "explorer";
		args << desktop;
	}
	if (hasConsole->isChecked())
		args << "wineconsole";
	if (hasUAC->isChecked()){
		args << "runas";
		args << "/trustlevel:0x20000";
	}
	if (prog.suffix().toLower() == "msi"){
		args << "msiexec";
		args << "/i";
	}
	args << prog.filePath();
	if (!prog_args->text().isEmpty())
		args << prog_args->text().split(" ");

	QStringList env = QProcess::systemEnvironment();
	env << "WINEPREFIX=" + bottle;
	env << "WINE=" + path + "wine";
	env << "REGEDIT=" + path + "regedit";
	env << "CONSOLE=" + path + "wineconsole";
	env << "WINEFILE=" + path + "winefile";
	env << "WINEBOOT=" + path + "wineboot";

	QStringList addEnvList = addEnv->toPlainText().split("\n");
	for (QString e : addEnvList){
		if (e.isEmpty())
			continue;

		QRegExp re(R"(\w+=(\"(.*)\"))");
		if (re.indexIn(e) == 0)
			e.replace(re.cap(1), re.cap(2));

		env << e;
	}

	if (noVSync->isChecked()){
		env << "__GL_SYNC_TO_VBLANK=0";
		env << "vblank_mode=0";
	}

	applyDirectX();
	QProcess *proc = new QProcess(nullptr);
	proc->setProgram(path + "wine");
	proc->setArguments(args);
	proc->setEnvironment(env);
	if (hasLogging->isChecked())
		proc->setStandardErrorFile(prog.filePath().replace(prog.suffix(), "winelog"));
	proc->start();
	proc->waitForStarted();
	close();
}

void WBGui::on_prog_desktop_clicked()
{
	quint32 bottleId = bottleNumber[bottles->currentText()];
	QString bName = "Bottle_" + QString::number(bottleId);
	QString path = set->value(bName + "/wine").toString() + "bin/";
	QString bottle = set->value(bName + "/bottle").toString();
	QFile desktop(prog.filePath().replace(prog.suffix(), "desktop"));
	QString launcher = set->value(bName + "/launcher").toString();
	QString vsync = "vblank_mode=1 __GL_SYNC_TO_VBLANK=1";
	if (noVSync->isChecked()){
		vsync = "vblank_mode=0 __GL_SYNC_TO_VBLANK=0";
	}
	desktop.open(QIODevice::WriteOnly);

	desktop.write("[Desktop Entry]\n");
	desktop.write("Type=Application\n");
	desktop.write(QString("Name=" + prog.fileName() + "\n").toStdString().c_str());
	desktop.write(QString("GenericName=" + prog.fileName() + "\n").toStdString().c_str());
	desktop.write("Categories=Wine;WineBottle;\n");
	desktop.write(QString("Exec=env WINEPREFIX='" + bottle + "' " + vsync + " " + addEnv->toPlainText().replace("\n", " ") + " " + launcher + " '" + path + "wine' ").toStdString().c_str());
	if (hasDesktop->isChecked()){
		QString dskName = prog.fileName().remove("." + prog.suffix());
		QString virtualDesktop = "explorer /desktop=" + dskName + "," +
				QString::number(desktop_X->value()) + "x" +
				QString::number(desktop_Y->value()) + " ";
		desktop.write(virtualDesktop.toStdString().c_str());
	}
	if (hasConsole->isChecked())
		desktop.write("wineconsole ");
	if (hasUAC->isChecked())
		desktop.write("runas /trustlevel:0x20000 ");
	desktop.write(QString("'" + prog.filePath() + "' ").toStdString().c_str());
	if (!prog_args->text().isEmpty())
		desktop.write(prog_args->text().toStdString().c_str());
	if (hasLogging->isChecked())
		desktop.write(QString(" &> '" + prog.filePath().replace(prog.suffix(), "winelog'")).toStdString().c_str());
	desktop.write(QString("\nPath=" + prog.path() + "/\n").toStdString().c_str());
	desktop.write("Icon=/usr/share/pixmaps/WineBottle.png\n");
	desktop.write("Actions=WineBottle;\n\n");
	desktop.write("[Desktop Action WineBottle]\n");
	desktop.write(QString("Exec=/usr/bin/WineBottle '" + prog.filePath() + "'\n").toStdString().c_str());
	desktop.write("Name=Open in WineBottle\n");
	desktop.write("Icon=/usr/share/pixmaps/WineBottle.png\n");
	desktop.close();

	QMessageBox msg;
	msg.setText(tr("Link has created"));
	msg.exec();
}
