#include "wbrun.h"

WBRun::WBRun(QStringList env, QString prog, QStringList args, QString logFile, QObject *parent) : QThread(parent)
{
    _env = env;
    _prog = prog;
    _args = args;
    _log = logFile;
}

void WBRun::run()
{
    QProcess proc;
    proc.setEnvironment(_env);
    proc.setProgram(_prog);
    if (!_args.isEmpty())
        proc.setArguments(_args);
    if (!_log.isEmpty())
        proc.setStandardErrorFile(_log);
    proc.start();
    proc.waitForStarted();
    proc.waitForFinished();
}
