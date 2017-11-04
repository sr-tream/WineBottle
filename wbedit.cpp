#include "wbedit.h"
#include <QProcess>
#include <QFileDialog>

WBEdit::WBEdit(QSettings *set, qint32 id, QWidget *parent) :
	QDialog(parent)
{
	setupUi(this);
	this->set = set;
	bottleId = id;
	bottleName->setText(set->value("Bottle_" + QString::number(bottleId) + "/name").toString());
	bottlePath->setText(set->value("Bottle_" + QString::number(bottleId) + "/bottle").toString());
	winePath->setText(set->value("Bottle_" + QString::number(bottleId) + "/wine").toString());
}

void WBEdit::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;
	default:
		break;
	}
}

void WBEdit::on_buttonBox_accepted()
{
	set->setValue("Bottle_" + QString::number(bottleId) + "/name", bottleName->text());
	set->setValue("Bottle_" + QString::number(bottleId) + "/wine", winePath->text());
	QProcess proc;
	proc.setProgram("mv");
	QStringList args;
	args << set->value("Bottle_" + QString::number(bottleId) + "/bottle").toString();
	args << bottlePath->text();
	QDir k(bottlePath->text());
	k.removeRecursively();
	proc.setArguments(args);
	proc.start();
	proc.waitForStarted();
	proc.waitForFinished(-1);
	set->setValue("Bottle_" + QString::number(bottleId) + "/bottle", bottlePath->text());
	QString _launcher = cb_launcher->isChecked() ? launcher->text() : "";
	set->setValue("Bottle_" + QString::number(bottleId) + "/launcher", _launcher);
	emit onAccepted(this);
	close();
}

void WBEdit::on_buttonBox_rejected()
{
	close();
}

void WBEdit::on_bottleName_returnPressed()
{
	on_buttonBox_accepted();
}

void WBEdit::on_select_winePath_clicked()
{
	winePath->setText( QFileDialog::getExistingDirectory(this, tr("Select wine path"),
														 set->value("Bottle_" + QString::number(bottleId) + "/wine").toString()) + '/' );
}

void WBEdit::on_select_bottlePath_clicked()
{
	bottlePath->setText( QFileDialog::getExistingDirectory(this, tr("Select bottle path"),
														   set->value("Bottle_" + QString::number(bottleId) + "/bottle").toString()) + '/' );
}
