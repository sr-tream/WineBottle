#include "newbottle.h"
#include "ui_newbottle.h"

NewBottle::NewBottle(QComboBox *bottle, QSettings *sets, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewBottle)
{
    ui->setupUi(this);
    path = sets->value("path").toString();
    this->sets = sets;
    this->bottle = bottle;
}

NewBottle::~NewBottle()
{
    delete ui;
}

void NewBottle::on_buttonBox_accepted()
{
    QString bottleName = ui->name->text().replace(" ", "_");
    for (int i = 0; i < bottle->count(); ++i)
        if (bottle->itemText(i) == bottleName)
            return;
    bottle->addItem(bottleName);

    QStringList env = QProcess::systemEnvironment();
    env << "WINEARCH=win" + ui->arch->currentText();
    env << "WINEPREFIX=" + QDir::homePath() + "/.wine_" + bottleName;
    sets->setValue(bottleName + "/path", path);

    QProcess *proc = new QProcess(this);
    proc->setEnvironment(env);
    proc->setProgram(sets->value(bottleName + "/path").toString() + "wine");
    proc->setArguments(QStringList("wineboot"));
    proc->start();
    proc->waitForStarted();
    proc->waitForFinished(1800000);
}

void NewBottle::on_toolButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"), sets->value("path").toString(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) + "/";
    if (QFile::exists(dir + "wine"))
        path = dir;
}
