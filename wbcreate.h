#ifndef WBCREATE_H
#define WBCREATE_H

#include "ui_wbcreate.h"
#include <QSettings>

class WBCreate : public QDialog, private Ui::WBCreate
{
    Q_OBJECT

public:
    explicit WBCreate(QSettings *set, QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
private slots:
    void on_bottle_name_textEdited(const QString &arg1);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_bottle_name_returnPressed();

    void on_select_winePath_clicked();

    void on_select_bottlePath_clicked();

private:
    QSettings *set = nullptr;

signals:
    void onAccepted(WBCreate*);
};

#endif // WBCREATE_H
