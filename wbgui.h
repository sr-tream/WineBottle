#ifndef WBGUI_H
#define WBGUI_H

#include "ui_wbgui.h"
#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include "wbsettings.h"

class WBGui : public QWidget, private Ui::WBGui
{
    Q_OBJECT

public:
    explicit WBGui(QFileInfo file = QString(),QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

    virtual void toggleUAC();
    virtual void loadBottles();
    virtual void loadProgramm();
private slots:
    void wbs_closed();

    void on_wb_settings_clicked();

    void on_bottles_currentIndexChanged(const QString &arg1);

    void on_prog_select_clicked();

    void on_prog_run_clicked();

private:
    WBSettings *wbset = nullptr;
    QSettings  *set   = nullptr;

    quint32                 bottleCount;
    QMap<QString, quint32>  bottleNumber;
    QFileInfo               prog;
};

#endif // WBGUI_H
