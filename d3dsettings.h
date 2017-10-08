#ifndef D3DSETTINGS_H
#define D3DSETTINGS_H

#include "ui_d3dsettings.h"
#include <QSettings>
#include <QMap>

class D3DSettings : public QDialog, private Ui::D3DSettings
{
	Q_OBJECT

public:
	explicit D3DSettings(QSettings *set, QWidget *parent = 0);
	void setBottleId(quint32 bottleId);

protected:
	void changeEvent(QEvent *e);
	bool event(QEvent *event);
	virtual QStringList env();
	virtual void regedit(QString file);
	virtual void createRegFile(QString file, QString section);
	virtual QMap<QString, QString> readRegFile(QString section);
	virtual uint readDWord(QString value);
	virtual QString readString(QString value);

	QString tmp_dir;

private slots:
	void on_buttonBox_accepted();

private:
	QSettings	*set = nullptr;
	quint32		bottleId;
	const QString D3DSection = "HKEY_CURRENT_USER\\Software\\Wine\\Direct3D";
	const QString regTitle = "Windows Registry Editor Version 5.00\r\n\r\n";
	QMap<QString, QString> D3DParams;
};

#endif // D3DSETTINGS_H
