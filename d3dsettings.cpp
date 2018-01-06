#include "d3dsettings.h"
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QRegExp>
#include <QDebug>

D3DSettings::D3DSettings(QSettings *set, QWidget *parent) :
	QDialog(parent)
{
	setupUi(this);
	this->set = set;
}

void D3DSettings::setBottleId(quint32 bottleId)
{
	this->bottleId = bottleId;
	tmp_dir = "/tmp/WineBottle/Bottle_" + QString::number(bottleId);
	QDir dir("/tmp/WineBottle");
	if (!dir.exists())
		dir.mkdir(dir.path());
	dir.setPath(tmp_dir);
	if (!dir.exists())
		dir.mkdir(dir.path());
}

void D3DSettings::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			retranslateUi(this);
			break;
		default:
			break;
	}
}

bool D3DSettings::event(QEvent *event)
{
	if (event->type() == QEvent::Show){
		D3DParams = readRegFile(D3DSection);
		csmt->setChecked(readDWord("csmt"));
		glsl->setChecked(readString("UseGLSL") == "enabled" || readString("UseGLSL").isEmpty());
		multisampling->setChecked(readString("Multisampling") == "enabled");
		if (readString("OffscreenRenderingMode") == "fbo" || readString("OffscreenRenderingMode").isEmpty())
			orm->setCurrentIndex(0);
		else orm->setCurrentIndex(1);
		sdo->setChecked(readString("StrictDrawOrdering") == "enabled");
		alwaysOff->setChecked(readString("AlwaysOffscreen") == "enabled");
		cfc->setChecked(readString("CheckFloatConstants") == "enabled");
		if (readString("RenderTargetLockMode").isEmpty())
			rtlm->setCurrentIndex(0);
		else rtlm->setCurrentText(readString("RenderTargetLockMode"));
		if (readString("DirectDrawRenderer") == "opengl" || readString("DirectDrawRenderer").isEmpty())
			ddr->setCurrentIndex(0);
		else ddr->setCurrentIndex(1);
		if (readString("MaxShaderModelGS").isEmpty())
			gsm->setValue(3);
		else gsm->setValue(readString("MaxShaderModelGS").toUInt());
		if (readString("MaxShaderModelPS").isEmpty())
			psm->setValue(3);
		else psm->setValue(readString("MaxShaderModelPS").toUInt());
		if (readString("MaxShaderModelVS").isEmpty())
			vsm->setValue(3);
		else vsm->setValue(readString("MaxShaderModelVS").toUInt());

		if (readString("VideoMemorySize").toUInt() > 16){
			vmem->setValue(readString("VideoMemorySize").toUInt());
			cb_vmem->setChecked(true);
		}
	}

	return QDialog::event(event);
}

QStringList D3DSettings::env()
{
	QString path = set->value("Bottle_" + QString::number(bottleId) + "/wine").toString() + "bin/";
	QString bottle = set->value("Bottle_" + QString::number(bottleId) + "/bottle").toString();

	QStringList env = QProcess::systemEnvironment();
	env << "WINEPREFIX=" + bottle;
	env << "WINE=" + path + "wine";
	env << "REGEDIT=" + path + "regedit";
	env << "CONSOLE=" + path + "wineconsole";
	env << "WINEFILE=" + path + "winefile";
	env << "WINEBOOT=" + path + "wineboot";

	return env;
}

void D3DSettings::regedit(QString file)
{
	QStringList args;
	args << "cmd";
	args << "/c";
	args << "regedit";
	args << tmp_dir + "/" + file;
	QString path = set->value("Bottle_" + QString::number(bottleId) + "/wine").toString() + "bin/";
	QProcess proc;
	proc.setEnvironment(env());
	proc.setProgram(path + "wine");
	proc.setArguments(args);
	proc.start();
	proc.waitForFinished();
}

void D3DSettings::createRegFile(QString file, QString section)
{
	QFile f(tmp_dir + "/" + file);
	if (f.exists())
		f.remove();

	f.open(QIODevice::WriteOnly);
	f.write("\xFF\xFE");
	f.write((char*)regTitle.toStdU16String().c_str(), regTitle.length() * sizeof(char16_t));
	f.write((char*)section.toStdU16String().c_str(), section.length() * sizeof(char16_t));
	f.write("\r\x00\n\x00", 4);
	for (auto param : D3DParams.toStdMap()){
		QString value = "\"" + param.first + "\"=" + param.second + "\r\n";
		f.write((char*)value.toStdU16String().c_str(), value.length() * sizeof(char16_t));
	}
	f.close();
}

QMap<QString, QString> D3DSettings::readRegFile(QString section)
{ // TODO: save all into my settings file, for fast access
	QStringList args;
	args << "cmd";
	args << "/c";
	args << "regedit";
	args << "/E";
	args << tmp_dir + "/export.reg";
	args << section;
	QString path = set->value("Bottle_" + QString::number(bottleId) + "/wine").toString() + "bin/";
	QProcess proc;
	proc.setEnvironment(env());
	proc.setProgram(path + "wine");
	proc.setArguments(args);
	proc.start();
	proc.waitForStarted();
	proc.waitForFinished(-1);

	QFile f(tmp_dir + "/export.reg");
	f.open(QIODevice::ReadOnly);
	QByteArray arr = f.readAll();
	f.close();

	arr.remove(0, 2);
	QStringList file = QString::fromUtf16(
										  (ushort*)arr.data(),
										  arr.size()
										  ).split("\n");
	QMap<QString, QString> ret;

	QRegExp re(R"(\"(.*)\"=(.*))");
	for (auto &str : file){
		str.remove('\r');
		if (re.indexIn(str) == 0)
			ret[re.cap(1)] = re.cap(2);
	}

	return ret;
}

uint D3DSettings::readDWord(QString value)
{
	QRegExp re(R"(dword\:([a-f0-9]+))", Qt::CaseInsensitive);

	if (re.indexIn(D3DParams[value]) == 0)
		return re.cap(1).toUInt(0, 16);
	return  0;
}

QString D3DSettings::readString(QString value)
{
	QRegExp re(R"(\"(.*)\")", Qt::CaseInsensitive);

	if (re.indexIn(D3DParams[value]) == 0)
		return re.cap(1);
	return "";
}

void D3DSettings::on_buttonBox_accepted()
{
	D3DParams["csmt"].sprintf("dword:%08x", csmt->isChecked());
	D3DParams["UseGLSL"].sprintf("\"%s\"", glsl->isChecked() ? "enabled" : "disabled");
	D3DParams["Multisampling"].sprintf("\"%s\"", multisampling->isChecked() ? "enabled" : "disabled");
	D3DParams["OffscreenRenderingMode"].sprintf("\"%s\"", orm->currentIndex() ? "backbuffer" : "fbo");
	D3DParams["StrictDrawOrdering"].sprintf("\"%s\"", sdo->isChecked() ? "enabled" : "disabled");
	D3DParams["AlwaysOffscreen"].sprintf("\"%s\"", alwaysOff->isChecked() ? "enabled" : "disabled");
	D3DParams["CheckFloatConstants"].sprintf("\"%s\"", cfc->isChecked() ? "enabled" : "disabled");
	D3DParams["RenderTargetLockMode"].sprintf("\"%s\"", rtlm->currentText().toStdString().c_str());
	D3DParams["DirectDrawRenderer"].sprintf("\"%s\"", ddr->currentIndex() ? "gdi" : "opengl");
	D3DParams["MaxShaderModelGS"].sprintf("\"%d\"", gsm->value());
	D3DParams["MaxShaderModelPS"].sprintf("\"%d\"", psm->value());
	D3DParams["MaxShaderModelVS"].sprintf("\"%d\"", vsm->value());

	if (cb_vmem->isChecked())
		D3DParams["VideoMemorySize"].sprintf("\"%d\"", vmem->value());
	else D3DParams["VideoMemorySize"] = "-";

	createRegFile("import.reg", "[" + D3DSection + "]");
	regedit("import.reg");
}
