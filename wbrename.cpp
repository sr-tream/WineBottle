#include "wbrename.h"

WBRename::WBRename(QSettings *set, qint32 id, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    this->set = set;
    bottleId = id;
    bottleName->setText(set->value("Bottle_" + QString::number(bottleId) + "/name").toString());
}

void WBRename::changeEvent(QEvent *e)
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

void WBRename::on_buttonBox_accepted()
{
    set->setValue("Bottle_" + QString::number(bottleId) + "/name", bottleName->text());
    emit onAccepted(this);
    close();
}

void WBRename::on_buttonBox_rejected()
{
    close();
}

void WBRename::on_bottleName_returnPressed()
{
    on_buttonBox_accepted();
}
