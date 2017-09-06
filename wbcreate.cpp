#include "wbcreate.h"
#include <QProcess>
#include <QFileDialog>
#include <QDir>

WBCreate::WBCreate(QSettings *set, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    this->set = set;
    bottlePath->setText( QDir::homePath() + "/.wine/" );
}

void WBCreate::changeEvent(QEvent *e)
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

void WBCreate::on_bottle_name_textEdited(const QString &arg1)
{
    if (arg1.isEmpty())
        bottlePath->setText( QDir::homePath() + "/.wine/" );
    else bottlePath->setText(QDir::homePath() + "/.wine-" + QString(arg1).replace("/", "\\") + "/");
}

void WBCreate::on_buttonBox_accepted()
{
    quint32 bottleId = set->value("BottleCount").toUInt() + 1;
    QString bottleName = "Bottle_" + QString::number(bottleId);
    QString path = winePath->text() + "bin/";
    QString bottle = bottlePath->text();

    set->setValue("BottleCount", bottleId);
    set->setValue(bottleName + "/wine", winePath->text());
    set->setValue(bottleName + "/bottle", bottle);
    set->setValue(bottleName + "/name", bottle_name->text());

    QStringList env = QProcess::systemEnvironment();
    env << "WINEPREFIX=" + bottle;
    env << "WINE=" + path + "wine";
    env << "REGEDIT=" + path + "regedit";
    env << "CONSOLE=" + path + "wineconsole";
    env << "WINEFILE=" + path + "winefile";
    env << "WINEBOOT=" + path + "wineboot";
    if (wineArch->currentIndex() == 0)
        env << "WINEARCH=win32";
    else env << "WINEARCH=win64";

    QProcess proc;
    proc.setEnvironment(env);
    proc.setProgram(path + "wineboot");
    proc.start();
    proc.waitForStarted();
    proc.waitForFinished(-1);

    emit onAccepted(this);
    close();
}

void WBCreate::on_buttonBox_rejected()
{
    close();
}

void WBCreate::on_bottle_name_returnPressed()
{
    on_buttonBox_accepted();
}

void WBCreate::on_select_winePath_clicked()
{
    winePath->setText( QFileDialog::getExistingDirectory(this, tr("Select wine path")) + '/' );
}

void WBCreate::on_select_bottlePath_clicked()
{
    bottlePath->setText( QFileDialog::getExistingDirectory(this, tr("Select bottle path")) + '/' );
}
