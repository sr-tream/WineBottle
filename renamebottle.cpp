#include "renamebottle.h"
#include "ui_renamebottle.h"

RenameBottle::RenameBottle(QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenameBottle)
{
    ui->setupUi(this);
    ui->name->setText(name);
}

RenameBottle::~RenameBottle()
{
    delete ui;
}

QString RenameBottle::getName()
{
    return name;
}

void RenameBottle::on_buttonBox_accepted()
{
    name = ui->name->text().replace(" ", "_");
}
