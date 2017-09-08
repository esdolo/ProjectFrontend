#include "stdafx.h"
#include "ProjectFrontend.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ProjectFrontend w;
	w.show();
	return a.exec();
}
