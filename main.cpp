#include <QApplication>
#include <QTextCodec>
#include <QDateTime>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
	qsrand(QDateTime::currentDateTime().toTime_t());
//	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
