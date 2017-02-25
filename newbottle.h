#ifndef NEWBOTTLE_H
#define NEWBOTTLE_H

#include <QDialog>
#include <QDir>
#include <QComboBox>
#include <QProcess>

namespace Ui {
class NewBottle;
}

class NewBottle : public QDialog
{
    Q_OBJECT

public:
    explicit NewBottle(QComboBox *bottle, QWidget *parent = 0);
    ~NewBottle();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::NewBottle *ui;
    QComboBox *bottle;
};

#endif // NEWBOTTLE_H
