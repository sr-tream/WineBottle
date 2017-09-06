#include "wbgui.h"
#include <QProcess>
#include <QRegExp>
#include <QDebug>

WBGui::WBGui(QFileInfo file, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    set = new QSettings("Prime-Hack", "WineBottle", this);
    wbset = new WBSettings(set, this);
    connect(wbset, &WBSettings::onHide,
            this,  &WBGui::wbs_closed);

    loadBottles();
    QString bName = "Bottle_" + set->value("LastBottle").toString();
    bottles->setCurrentText(set->value(bName + "/name").toString());

    prog = file;
    if (!prog.filePath().isEmpty())
        if (prog.filePath().at(0) != '/')
            prog.setFile(QDir::currentPath(), file.filePath());
    loadProgramm();
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
    set->setValue(appName + "/Window",  hasDesktop->isChecked());
    set->setValue(appName + "/WinX",    desktop_X->value());
    set->setValue(appName + "/WinY",    desktop_Y->value());
    set->setValue(appName + "/Args",    prog_args->text());
    set->setValue(appName + "/Bottle",  bottles->currentText());
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

    prog_name->setText(prog.fileName().remove("." + prog.suffix()));
    prog_run->setEnabled(true);

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
    hasDesktop->setChecked(set->value(appName + "/Window").toBool());
    desktop_X->setValue(   set->value(appName + "/WinX").toUInt());
    desktop_Y->setValue(   set->value(appName + "/WinY").toUInt());
    prog_args->setText(    set->value(appName + "/Args").toString());
}

void WBGui::wbs_closed()
{
    wb_settings->setEnabled(true);
    loadBottles();
    if (bottles->currentIndex() > bottles->count())
        bottles->setCurrentIndex(bottles->count() - 1);
}

void WBGui::on_wb_settings_clicked()
{
    wbset->load();
    wb_settings->setEnabled(false);
}

void WBGui::on_bottles_currentIndexChanged(const QString &arg1)
{
    toggleUAC();
}

void WBGui::on_prog_select_clicked()
{
    prog = QFileDialog::getOpenFileName(this, "Select Windows programm", QDir::homePath(),
                                        "MS/Windows (*.exe *.msi *.cmd *.bat *.lnk *.url)\n"
                                        "Executable (*.exe *.msi)\n"
                                        "Batch (*.cmd *.bat)\n"
                                        "Links (*.lnk *.url)\n"
                                        "All (*.*)");
    loadProgramm();
}

void WBGui::on_prog_run_clicked()
{
    quint32 bottleId = bottleNumber[bottles->currentText()];
    QString bName = "Bottle_" + QString::number(bottleId);
    QString path = set->value(bName + "/wine").toString() + "bin/";
    QString bottle = set->value(bName + "/bottle").toString();

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