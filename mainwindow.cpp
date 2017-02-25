#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "renamebottle.h"
#include "newbottle.h"

MainWindow::MainWindow(QStringList args, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName("cp1251");

    winecfg = new QProcess(this);
    connect(winecfg, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished_winecfg(int, QProcess::ExitStatus)));
    regedit = new QProcess(this);
    connect(regedit, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished_regedit(int, QProcess::ExitStatus)));
    control = new QProcess(this);
    connect(control, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished_control(int, QProcess::ExitStatus)));
    winetricks = new QProcess(this);
    connect(winetricks, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished_winetricks(int, QProcess::ExitStatus)));

    QDir dir = QDir::homePath() + "/.wine";
    if (dir.exists())
        ui->bottle->addItem("default");

    dir.setPath(QDir::homePath());
    dir.setFilter(QDir::Dirs | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.isFile())
            continue;
        if (!fileInfo.fileName().indexOf(".wine_")){
            ui->bottle->addItem(fileInfo.fileName().remove(".wine_"));
        }
    }

    if (!args.isEmpty()){
        exeFile = args.at(0);
        args.pop_front();
        arguments = args;
    }
    if(exeFile.suffix().toLower() != "btl"){
        qDebug() << "Application:" << exeFile.fileName();

        if (!QFile::exists(exeFile.filePath())){
            ui->run->setEnabled(false);
            ui->save->setEnabled(false);
        }
        else ui->select->setEnabled(false);
    } else {
        getBtl();
        exeFile = exeFile.fileName().remove(".btl");
    }

    if (QFile::exists("/usr/bin/winetricks"))
        ui->winetricks->setEnabled(true);
    else if (QFile::exists("/bin/winetricks"))
        ui->winetricks->setEnabled(true);
    else if (QFile::exists("/usr/local/bin/winetricks"))
        ui->winetricks->setEnabled(true);
    else qDebug() << "Can't find winetricks";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_run_clicked()
{
    QStringList args = QStringList(exeFile.filePath());
    args << arguments;
    exec("wine", args);
    winecfg->kill();
    regedit->kill();
    control->kill();
    winetricks->kill();
    this->close();
}

void MainWindow::on_bottle_currentIndexChanged(const QString &arg1)
{
    bottle = arg1;
    if (arg1 == "default"){
        ui->rem_bottle->setEnabled(false);
        ui->ren_bottle->setEnabled(false);
    } else {
        ui->rem_bottle->setEnabled(true);
        ui->ren_bottle->setEnabled(true);
    }
    if (ui->run->isEnabled())
        ui->save->setEnabled(true);

}

void MainWindow::on_winecfg_clicked()
{
    winecfg->setEnvironment(getEnvironments());
    winecfg->setProgram("winecfg");
    winecfg->start();
    ui->winecfg->setEnabled(false);
    toggleControls(false);
}

void MainWindow::on_regedit_clicked()
{
    regedit->setEnvironment(getEnvironments());
    regedit->setProgram("wine");
    regedit->setArguments(QStringList("regedit"));
    regedit->start();
    ui->regedit->setEnabled(false);
    toggleControls(false);
}

void MainWindow::on_control_clicked()
{
    control->setEnvironment(getEnvironments());
    control->setProgram("wine");
    control->setArguments(QStringList("control"));
    control->start();
    ui->control->setEnabled(false);
    toggleControls(false);
}

void MainWindow::on_winetricks_clicked()
{
    winetricks->setEnvironment(getEnvironments());
    winetricks->setProgram("winetricks");
    winetricks->start();
    ui->winetricks->setEnabled(false);
    toggleControls(false);
}

QStringList MainWindow::getEnvironments()
{
    QStringList env = QProcess::systemEnvironment();
    QString prefix = "WINEPREFIX=" + QDir::homePath();
    if (bottle == "default")
        prefix += "/.wine";
    else prefix += "/.wine_" + bottle;
    env << prefix;
    return env;
}

void MainWindow::exec(QString exe, QStringList args)
{
    QProcess *proc = new QProcess(nullptr);
    proc->setEnvironment(getEnvironments());
    proc->setProgram(exe);
    proc->setArguments(args);
    proc->start();
    proc->waitForStarted();
}

void MainWindow::getBtl()
{
    QFile btlFile(exeFile.filePath());
    if (!btlFile.exists()){
        return;
    }
    btlFile.open(QIODevice::ReadOnly);
    if (!btlFile.isReadable()){
        btlFile.close();
        return;
    }

    uint len;
    btlFile.read((char*)&len, 4);
    char *value = new char[len + 1];
    btlFile.read(value, len);
    value[len] = 0;
    bottle = codec->toUnicode(value);
    for (int i = 0; i < ui->bottle->count(); ++i)
        if (ui->bottle->itemText(i) == bottle)
            ui->bottle->setCurrentIndex(i);
    delete[] value;

    btlFile.read((char*)&len, 4);
    value = new char[len + 1];
    btlFile.read(value, len);
    value[len] = 0;
    QString args = codec->toUnicode(value);
    ui->args->setText(args);
    delete[] value;

    btlFile.close();
}

void MainWindow::toggleControls(bool toggle)
{
    if (!toggle){
        ui->bottle->setEnabled(false);
        ui->new_bottle->setEnabled(false);
        ui->rem_bottle->setEnabled(false);
        ui->ren_bottle->setEnabled(false);
    } else {
        if (!ui->winecfg->isEnabled())
            return;
        if (!ui->regedit->isEnabled())
            return;
        if (!ui->control->isEnabled())
            return;
        if (!ui->winetricks->isEnabled())
            return;

        ui->bottle->setEnabled(true);
        ui->new_bottle->setEnabled(true);

        if (bottle != "default"){
            ui->rem_bottle->setEnabled(true);
            ui->ren_bottle->setEnabled(true);
        }
    }
}

bool MainWindow::loadBtl()
{
    QFile btlFile(exeFile.filePath() + ".btl");
    qDebug() << "try load btl:" << exeFile.filePath();
    if (!btlFile.exists()){
        qDebug() << "btl not exist";
        return false;
    }
    btlFile.open(QIODevice::ReadOnly);
    if (!btlFile.isReadable()){
        qDebug() << "Can't read btl";
        btlFile.close();
        return false;
    }

    uint len;
    btlFile.read((char*)&len, 4);
    char *value = new char[len + 1];
    btlFile.read(value, len);
    value[len] = 0;
    bottle = codec->toUnicode(value);
    delete[] value;

    btlFile.read((char*)&len, 4);
    value = new char[len + 1];
    btlFile.read(value, len);
    value[len] = 0;
    QString args = codec->toUnicode(value);
    delete[] value;

    btlFile.close();
    qDebug() << "btl is loaded";
    exec("wine \"" + exeFile.filePath() + "\" " + args);
    return true;
}

void MainWindow::on_select_clicked()
{
    QString exe = QFileDialog::getOpenFileName(this, tr("Select executable file"), "" ,
                                               tr("*.exe\n*.msi\n*.bat\n*.cmd"));

    if (QFile::exists(exe)){
        exeFile = exe;

        arguments = ui->args->text().split(" ");
        ui->run->setEnabled(true);
        ui->save->setEnabled(true);
        ui->select->setEnabled(false);
    }
}

void MainWindow::on_ren_bottle_clicked()
{
    RenameBottle rn(bottle, this);
    rn.exec();
    if (!rn.getName().isEmpty()){
        QDir dir(QDir::homePath());
        dir.rename(".wine_" + bottle, ".wine_" + rn.getName());
        bottle = rn.getName();
        ui->bottle->setItemText(ui->bottle->currentIndex(), bottle);
    }
}

void MainWindow::on_rem_bottle_clicked()
{
    QDir dir(QDir::homePath() + "/.wine_" + bottle);
    if (dir.removeRecursively()){
        ui->bottle->removeItem(ui->bottle->currentIndex());
        bottle = ui->bottle->currentText();
    } else qDebug() << "Can't remove bottle" << bottle;
}

void MainWindow::on_new_bottle_clicked()
{
    this->setEnabled(false);
    NewBottle btl(ui->bottle, this);
    btl.exec();
    this->setEnabled(true);
}

void MainWindow::on_save_clicked()
{
    QFile btlFile(exeFile.filePath() + ".btl");
    qDebug() << "try save btl:" << exeFile.filePath();
    if (btlFile.exists()){
        qDebug() << "remove old btl";
        btlFile.remove();
    }
    btlFile.open(QIODevice::WriteOnly);
    if (!btlFile.isWritable()){
        qDebug() << "Can't save to btl";
        btlFile.close();
        return;
    }

    uint len = bottle.length();
    btlFile.write((const char*)&len, 4);
    btlFile.write(codec->fromUnicode(bottle).toStdString().c_str(), len);

    len = ui->args->text().length();
    btlFile.write((const char*)&len, 4);
    btlFile.write(codec->fromUnicode(ui->args->text()).toStdString().c_str(), len);

    btlFile.close();
    qDebug() << "btl is saved";
    ui->save->setEnabled(false);
}

void MainWindow::on_args_textChanged(const QString &)
{
    if (ui->run->isEnabled())
        ui->save->setEnabled(true);
}

void MainWindow::finished_winecfg(int, QProcess::ExitStatus)
{
    ui->winecfg->setEnabled(true);
    toggleControls(true);
}

void MainWindow::finished_regedit(int , QProcess::ExitStatus )
{
    ui->regedit->setEnabled(true);
    toggleControls(true);
}

void MainWindow::finished_control(int, QProcess::ExitStatus)
{
    ui->control->setEnabled(true);
    toggleControls(true);
}

void MainWindow::finished_winetricks(int , QProcess::ExitStatus )
{
    ui->winetricks->setEnabled(true);
    toggleControls(true);
}
