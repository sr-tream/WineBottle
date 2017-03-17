#ifndef NEWBOTTLE_H
#define NEWBOTTLE_H

#include <QDialog>
#include <QDir>
#include <QComboBox>
#include <QProcess>
#include <QSettings>
#include <QFileDialog>

namespace Ui {
class NewBottle;
}

class NewBottle : public QDialog
{
    Q_OBJECT

public:
    explicit NewBottle(QComboBox *bottle, QSettings *sets, QWidget *parent = 0);
    ~NewBottle();

private slots:
    void on_buttonBox_accepted();

    void on_toolButton_clicked();

private:
    Ui::NewBottle *ui;
    QComboBox *bottle;
    QSettings *sets;
    QString path;
};

#endif // NEWBOTTLE_H
