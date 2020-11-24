#include "TestCommunicate.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TestCommunicate w;
	w.show();
	return a.exec();
}
