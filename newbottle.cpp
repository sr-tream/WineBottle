#include "newbottle.h"
#include "ui_newbottle.h"

NewBottle::NewBottle(QComboBox *bottle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewBottle)
{
    ui->setupUi(this);
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
    QString command = "export WINEARCH=win" + ui->arch->currentText();
    command += "\nexport WINEPREFIX=" + QDir::homePath() + "/.wine_" + bottleName;
    command += "\nwinecfg";
    system(command.toStdString().c_str());
}
