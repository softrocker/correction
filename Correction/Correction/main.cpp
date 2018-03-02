#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QDesktopWidget>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTranslator translator;//��� �������� ���������� �� ������ ����
	bool loaded = translator.load("correction_ru.qm");//�������� ����� ������ qm-���� c ���������
	a.installTranslator(&translator);//��������� ������� � ����������
	a.setWindowIcon(QIcon("icon1.ico"));
	QDesktopWidget dw;
	MainWindow w;
	
	const double c_scale_coef = 0.8;
	w.setMinimumSize(dw.width() * c_scale_coef, dw.height() * c_scale_coef);
	w.show();
	return a.exec();
}
