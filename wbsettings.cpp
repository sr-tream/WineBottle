#include "wbsettings.h"
#include <QCloseEvent>
#include <QDebug>
#include <QDir>

WBSettings::WBSettings(QSettings *set, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    this->set = set;

    QStringList sysEnv = QProcess::systemEnvironment();
    for (QString e : sysEnv){
        if (e.indexOf("PATH=") == 0){
            e.remove("PATH=");
            QStringList pathList = e.split(':');
            for (QString path : pathList){
                if (path.at(path.length() - 1) != '/')
                    path += '/';
                if (QFile::exists(path + "winetricks")){
                    winetricks = true;
                    break;
                }
            }
            break;
        }
    }
}

void WBSettings::load()
{
    bottleCount = set->value("BottleCount").toInt();
    bottles->clear();
    bottleNumber.clear();

    for (int i = 1; i < bottleCount + 1; ++i){
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
        for (auto &r : rlist){
            if (r == nullptr)
                continue;
            if (r->isHidden()){
                rlist.removeOne(r);
                delete r;
                r = nullptr;
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
    bottle_rename->setEnabled(enabled);
    bottle_winecfg->setEnabled(enabled);
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

void WBSettings::on_wbCreate(WBCreate *create)
{
    load();
}

void WBSettings::on_wbRename(WBRename *rename)
{
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

void WBSettings::on_bottle_rename_clicked()
{
    WBRename *rename = new WBRename(set, bottleNumber[bottles->currentItem()->text()], this);
    connect(rename, &WBRename::onAccepted, this, &WBSettings::on_wbRename);
    rename->show();
    rlist.push_back(rename);
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
    }

    bottleCount--;
    set->setValue("BottleCount", bottleCount);
    load();
}

void WBSettings::on_bottle_clone_clicked()
{
    QString lastBottle = "Bottle_" + QString::number(bottleCount);
    bottleCount++;
    set->setValue("BottleCount", bottleCount);
    set->setValue(bottleName() + "/bottle", set->value(lastBottle + "/bottle").toString() + "_cloned");
    set->setValue(bottleName() + "/wine", set->value(lastBottle + "/wine").toString());
    set->setValue(bottleName() + "/name", set->value(lastBottle + "/name").toString() + "_cloned");
}
