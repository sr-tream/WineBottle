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
        else qDebug() << "Left directory:" << fileInfo.fileName();
    }

    app = "wine \"";
    bool execute = true;
    foreach (auto str, args){
        app += str;
        if (execute){
            app += "\"";
            exeFile = str;
            execute = false;
        } else ui->args->setText(ui->args->text() + " " + str);
        app += " ";
    }
    if(exeFile.suffix().toLower() != "btl"){
        qDebug() << "Application:" << app;

        if (app == "wine \""){
            ui->run->setEnabled(false);
            ui->save->setEnabled(false);
        }
        else ui->select->setEnabled(false);
    } else {
        getBtl();
        exeFile = exeFile.fileName().remove(".btl");
        app = "wine \"" + exeFile.fileName() + "\"" + ui->args->text();
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

void MainWindow::closeEvent(QCloseEvent * e)
{
    QString command = "export WINEPREFIX=" + QDir::homePath() + "/.wine";
    system(command.toStdString().c_str());
}

void MainWindow::on_run_clicked()
{
    exec(app);
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
    ui->save->setEnabled(true);
}

void MainWindow::on_winecfg_clicked()
{
    exec("winecfg");
}

void MainWindow::on_regedit_clicked()
{
    exec("wine regedit");
}

void MainWindow::on_control_clicked()
{
    exec("wine control");
}

void MainWindow::on_winetricks_clicked()
{
    exec("winetricks");
}

QString MainWindow::getPrefix()
{
    QString command = "export WINEPREFIX=" + QDir::homePath();
    if (bottle == "default")
        command += "/.wine\n";
    else command += "/.wine_" + bottle + "\n";
    return command;
}

void thrExec(QString exe)
{
    system(exe.toStdString().c_str());
}

void MainWindow::exec(QString exe)
{
    qDebug() << "exec(" << getPrefix() + exe << ");";
    std::thread thread(thrExec, getPrefix() + exe);
    thread.detach();
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

        app += exe + "\" " + ui->args->text();
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
        QString command = "mv ~/.wine_" + bottle + " ~/.wine_" + rn.getName();
        system(command.toStdString().c_str());
        bottle = rn.getName();
        ui->bottle->setItemText(ui->bottle->currentIndex(), bottle);
    }
}

void MainWindow::on_rem_bottle_clicked()
{
    QString command = "rm -rf ~/.wine_" + bottle;
    system(command.toStdString().c_str());
    ui->bottle->removeItem(ui->bottle->currentIndex());
    bottle = ui->bottle->currentText();
}

void MainWindow::on_new_bottle_clicked()
{
    NewBottle btl(ui->bottle, this);
    btl.exec();
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

void MainWindow::on_args_textChanged(const QString &arg1)
{
    ui->save->setEnabled(true);
}
