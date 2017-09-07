#ifndef WBRENAME_H
#define WBRENAME_H

#include "ui_wbedit.h"
#include <QSettings>

class WBEdit : public QDialog, private Ui::WBEdit
{
    Q_OBJECT

public:
    explicit WBEdit(QSettings *set, qint32 id, QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_bottleName_returnPressed();

    void on_select_winePath_clicked();

    void on_select_bottlePath_clicked();

private:
    QSettings *set = nullptr;
    qint32    bottleId;

signals:
    void onAccepted(WBEdit*);
};

#endif // WBRENAME_H
