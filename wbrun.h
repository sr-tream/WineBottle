#ifndef WBRUN_H
#define WBRUN_H

#include <QThread>
#include <QProcess>

class WBRun : public QThread
{
    Q_OBJECT
public:
    explicit WBRun(QStringList env, QString prog, QStringList args = QStringList(), QString logFile = QString(), QObject *parent = nullptr);

private:
    void run();

    QStringList _env;
    QString     _prog;
    QStringList _args;
    QString     _log;
};

#endif // WBRUN_H
