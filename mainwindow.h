#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QTextCodec>
#include <QProcess>

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
    void on_args_textChanged(const QString &);

signals:
public slots:
    void finished_winecfg(int , QProcess::ExitStatus );
    void finished_regedit(int, QProcess::ExitStatus);
    void finished_control(int , QProcess::ExitStatus );
    void finished_winetricks(int, QProcess::ExitStatus);

private:
    Ui::MainWindow *ui;
    QTextCodec *codec;

    QProcess *winecfg;
    QProcess *regedit;
    QProcess *control;
    QProcess *winetricks;

    QString bottle;
    QFileInfo exeFile;
    QStringList arguments;

    QStringList getEnvironments();
    void exec(QString exe, QStringList args = QStringList());
    void getBtl();
    void toggleControls(bool toggle);
};

#endif // MAINWINDOW_H
