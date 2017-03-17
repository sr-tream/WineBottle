#ifndef RENAMEBOTTLE_H
#define RENAMEBOTTLE_H

#include <QDialog>

namespace Ui {
class RenameBottle;
}

class RenameBottle : public QDialog
{
    Q_OBJECT

public:
    explicit RenameBottle(QString name, QWidget *parent = 0);
    ~RenameBottle();

    QString getName();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::RenameBottle *ui;
    QString name;
};

#endif // RENAMEBOTTLE_H
