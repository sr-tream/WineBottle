#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QTextCodec>
#include <thread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QStringList args, QWidget *parent = 0);
    ~MainWindow();

    bool loadBtl();

private slots:
    void on_run_clicked();

    void on_bottle_currentIndexChanged(const QString &arg1);

    void on_winecfg_clicked();

    void on_regedit_clicked();

    void on_control_clicked();

    void on_winetricks_clicked();

    void on_select_clicked();

    void on_ren_bottle_clicked();

    void on_rem_bottle_clicked();

    void on_new_bottle_clicked();

    void on_save_clicked();

    void on_args_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QTextCodec *codec;
    QString app;
    QString bottle;
    QFileInfo exeFile;

    QString getPrefix();
    void exec(QString exe);
    void getBtl();
};

#endif // MAINWINDOW_H
