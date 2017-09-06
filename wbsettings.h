#ifndef WBSETTINGS_H
#define WBSETTINGS_H

#include "ui_wbsettings.h"
#include <QProcess>
#include <QSettings>
#include "wbcreate.h"
#include "wbrename.h"

class WBSettings : public QDialog, private Ui::WBSettings
{
    Q_OBJECT

public:
    explicit WBSettings(QSettings *set, QWidget *parent = 0);

    void load();

protected:
    void changeEvent(QEvent *e);
    bool event(QEvent *event);

    virtual void setBottleOpEnabled(bool enabled);
    virtual QStringList env();
    virtual QString bottleName();
private slots:
    void on_wbCreate(WBCreate*);
    void on_wbRename(WBRename*);

    void on_bottles_currentTextChanged(const QString &currentText);

    void on_bottle_winecfg_clicked();

    void on_bottle_cp_clicked();

    void on_bottle_explorer_clicked();

    void on_bottle_regedit_clicked();

    void on_bottle_winetricks_clicked();

    void on_bottle_new_clicked();

    void on_bottle_rename_clicked();

    void on_bottle_delete_clicked();

signals:
    void onHide();

private:
    QList<QProcess*>        plist;
    QList<WBCreate*>        clist;
    QList<WBRename*>        rlist;
    QSettings               *set = nullptr;
    quint32                 bottleCount;
    QMap<QString, quint32>  bottleNumber;
    bool                    winetricks;
};

#endif // WBSETTINGS_H