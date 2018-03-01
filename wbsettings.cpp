#include "wbsettings.h"
#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <stdexcept>

WBSettings::WBSettings(QSettings *set, QWidget *parent) :
	QDialog(parent)
{
	setupUi(this);
	this->set = set;
	d3dset = new D3DSettings(set, this);

	findTerminal();
	winetricks = findInPath("winetricks");
}

void WBSettings::load()
{
	bottleCount = set->value("BottleCount").toInt();
	bottles->clear();
	bottleNumber.clear();

	for (quint32 i = 1; i < bottleCount + 1; ++i){
		QString bName = "Bottle_" + QString::number(i);
		QString name = set->value(bName + "/name").toString();
		bottleNumber[name] = i;
		bottles->addItem(name);
	}
	show();
}

void WBSettings::changeEvent(QEvent *e)
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

bool WBSettings::event(QEvent *event)
{
	if (event->type() == QEvent::Hide){
		for (auto &p : plist){
			if (p == nullptr)
				continue;
			if (!p->processId()){
				plist.removeOne(p);
				delete p;
				p = nullptr;
			}
		}
		for (auto &c : clist){
			if (c == nullptr)
				continue;
			if (c->isHidden()){
				clist.removeOne(c);
				delete c;
				c = nullptr;
			}
		}
		for (auto &e : elist){
			if (e == nullptr)
				continue;
			if (e->isHidden()){
				elist.removeOne(e);
				delete e;
				e = nullptr;
			}
		}
		setBottleOpEnabled(false);
		emit onHide();
	}

	return QDialog::event(event);
}

void WBSettings::setBottleOpEnabled(bool enabled)
{
	bottle_clone->setEnabled(enabled);
	bottle_cp->setEnabled(enabled);
	bottle_delete->setEnabled(enabled);
	bottle_explorer->setEnabled(enabled);
	bottle_regedit->setEnabled(enabled);
	bottle_edit->setEnabled(enabled);
	bottle_winecfg->setEnabled(enabled);
	bottle_d3dsettings->setEnabled(enabled);
	bottle_reboot->setEnabled(enabled);
	if (terminal.isEmpty())
		bottle_terminal->setEnabled(false);
	else bottle_terminal->setEnabled(enabled);
	if (enabled){
		QString path = set->value(bottleName() + "/wine").toString() + "bin/";
		if (QFile::exists(path + "ninewinecfg"))
			bottle_ninewinecfg->setEnabled(enabled);
	} else bottle_ninewinecfg->setEnabled(enabled);
	if (winetricks)
		bottle_winetricks->setEnabled(enabled);
}

QStringList WBSettings::env()
{
	QString path = set->value(bottleName() + "/wine").toString() + "bin/";
	QString bottle = set->value(bottleName() + "/bottle").toString();

	QStringList env = QProcess::systemEnvironment();
	env << "WINEPREFIX=" + bottle;
	env << "WINE=" + path + "wine";
	env << "REGEDIT=" + path + "regedit";
	env << "CONSOLE=" + path + "wineconsole";
	env << "WINEFILE=" + path + "winefile";
	env << "WINEBOOT=" + path + "wineboot";

	return env;
}

QString WBSettings::bottleName()
{
	quint32 bottleId = bottleNumber[bottles->currentItem()->text()];
	return "Bottle_" + QString::number(bottleId);
}

bool WBSettings::findInPath(QString filename)
{
	QStringList sysEnv = QProcess::systemEnvironment();
	for (QString e : sysEnv){
		if (e.indexOf("PATH=") == 0){
			e.remove("PATH=");
			QStringList pathList = e.split(':');
			for (QString path : pathList){
				if (path.at(path.length() - 1) != '/')
					path += '/';
				if (QFile::exists(path + filename))
					return true;
			}
			return false;
		}
	}
	throw std::runtime_error("System environment is empty!");
}

void WBSettings::findTerminal()
{
	if (findInPath("konsole"))
		terminal = "konsole";
	else if (findInPath("gnome-terminal"))
		terminal = "gnome-terminal";
	else if (findInPath("xfce4-terminal"))
		terminal = "xfce4-terminal";
	else if (findInPath("lxterminal"))
		terminal = "lxterminal";
	else if (findInPath("lxterminal"))
		terminal = "lxterminal";
	else if (findInPath("mate-terminal"))
		terminal = "mate-terminal";
	else if (findInPath("xterm"))
		terminal = "xterm";
	else terminal = "";
}

void WBSettings::on_wbCreate(WBCreate *)
{
	setBottleOpEnabled(false);
	load();
}

void WBSettings::on_wbRename(WBEdit *)
{
	setBottleOpEnabled(false);
	load();
}

void WBSettings::on_bottles_currentTextChanged(const QString &currentText)
{
	if (!currentText.isEmpty())
		setBottleOpEnabled(true);
}

void WBSettings::on_bottle_winecfg_clicked()
{
	QString path = set->value(bottleName() + "/wine").toString() + "bin/";
	QProcess *proc = new QProcess(this);
	proc->setEnvironment(env());
	proc->setProgram(path + "winecfg");
	proc->start();
	plist.push_back(proc);
}

void WBSettings::on_bottle_cp_clicked()
{
	QString path = set->value(bottleName() + "/wine").toString() + "bin/";
	QProcess *proc = new QProcess(this);
	proc->setEnvironment(env());
	proc->setProgram(path + "wine");
	proc->setArguments(QStringList("control"));
	proc->start();
	plist.push_back(proc);
}

void WBSettings::on_bottle_explorer_clicked()
{
	QString path = set->value(bottleName() + "/wine").toString() + "bin/";
	QProcess *proc = new QProcess(this);
	proc->setEnvironment(env());
	proc->setProgram(path + "wine");
	proc->setArguments(QStringList("explorer"));
	proc->start();
	plist.push_back(proc);
}

void WBSettings::on_bottle_regedit_clicked()
{
	QString path = set->value(bottleName() + "/wine").toString() + "bin/";
	QProcess *proc = new QProcess(this);
	proc->setEnvironment(env());
	proc->setProgram(path + "wine");
	proc->setArguments(QStringList("regedit"));
	proc->start();
	plist.push_back(proc);
}

void WBSettings::on_bottle_winetricks_clicked()
{
	QProcess *proc = new QProcess(this);
	proc->setEnvironment(env());
	proc->setProgram("winetricks");
	proc->setArguments(QStringList("--gui"));
	proc->start();
	plist.push_back(proc);
}

void WBSettings::on_bottle_new_clicked()
{
	WBCreate *create = new WBCreate(set, this);
	connect(create, &WBCreate::onAccepted, this, &WBSettings::on_wbCreate);
	create->show();
	clist.push_back(create);
}

void WBSettings::on_bottle_edit_clicked()
{
	WBEdit *rename = new WBEdit(set, bottleNumber[bottles->currentItem()->text()], this);
	connect(rename, &WBEdit::onAccepted, this, &WBSettings::on_wbRename);
	rename->show();
	elist.push_back(rename);
}

void WBSettings::on_bottle_delete_clicked()
{
	QDir bottlePath( set->value(bottleName() + "/bottle").toString() );

	if (bottlePath.exists())
		bottlePath.removeRecursively();

	if (bottleCount > 1 && bottleCount != bottleNumber[bottles->currentItem()->text()]){
		QString lastBottle = "Bottle_" + QString::number(bottleCount);
		set->setValue(bottleName() + "/bottle", set->value(lastBottle + "/bottle").toString());
		set->setValue(bottleName() + "/wine", set->value(lastBottle + "/wine").toString());
		set->setValue(bottleName() + "/name", set->value(lastBottle + "/name").toString());
		set->setValue(bottleName() + "/launcher", set->value(lastBottle + "/launcher").toString());
		set->remove(lastBottle);
	}

	bottleCount--;
	set->setValue("BottleCount", bottleCount);
	load();
}

void WBSettings::on_bottle_clone_clicked()
{
	QString currentBottle = bottleName();
	bottleCount++;
	QString newBottle = "Bottle_" + QString::number(bottleCount);

	set->setValue("BottleCount", bottleCount);
	QString bottle = set->value(currentBottle + "/bottle").toString();
	if (bottle[bottle.length() - 1] == '/')
		bottle.remove(bottle.length() - 1, 1);
	set->setValue(newBottle + "/bottle", bottle + "_cloned/");
	set->setValue(newBottle + "/wine", set->value(currentBottle + "/wine").toString());
	set->setValue(newBottle + "/name", set->value(currentBottle + "/name").toString() + " (cloned)");
	set->setValue(newBottle + "/launcher", set->value(currentBottle + "/launcher").toString());

	QProcess proc;
	proc.setEnvironment(env());
	proc.setProgram("cp");
	proc.setArguments(QStringList() << "-r" <<
					  set->value(currentBottle + "/bottle").toString() <<
					  bottle + "_cloned");
	proc.start();
	proc.waitForFinished(-1);
	load();
}

void WBSettings::on_bottle_reboot_clicked()
{
	QString path = set->value(bottleName() + "/wine").toString() + "bin/";
	QProcess *proc = new QProcess(this);
	proc->setEnvironment(env());
	proc->setProgram(path + "wineboot");
	proc->setArguments(QStringList("-r"));
	proc->start();
	plist.push_back(proc);
}

void WBSettings::on_bottle_d3dsettings_clicked()
{
	d3dset->setBottleId(bottleNumber[bottles->currentItem()->text()]);
	d3dset->show();
}

void WBSettings::on_bottle_ninewinecfg_clicked()
{
	QString path = set->value(bottleName() + "/wine").toString() + "bin/";
	QProcess *proc = new QProcess(this);
	proc->setEnvironment(env());
	proc->setProgram(path + "ninewinecfg");
	proc->start();
	plist.push_back(proc);
}

void WBSettings::on_bottle_terminal_clicked()
{
	QProcess *proc = new QProcess(this);
	proc->setEnvironment(env());
	proc->setProgram(terminal);
	proc->start();
	plist.push_back(proc);
}
