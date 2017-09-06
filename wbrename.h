#ifndef WBRENAME_H
#define WBRENAME_H

#include "ui_wbrename.h"
#include <QSettings>

class WBRename : public QDialog, private Ui::WBRename
{
    Q_OBJECT

public:
    explicit WBRename(QSettings *set, qint32 id, QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_bottleName_returnPressed();

private:
    QSettings *set = nullptr;
    qint32    bottleId;

signals:
    void onAccepted(WBRename*);
};

#endif // WBRENAME_H
