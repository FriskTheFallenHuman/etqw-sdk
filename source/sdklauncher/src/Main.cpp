#include "precompiled.h"
#pragma hdrstop

int main(int argc, char** argv) {
	//QCoreApplication::setOrganizationName("id Software");
	QCoreApplication::setApplicationName(PROJECT_NAME.data());
	QCoreApplication::setApplicationVersion(PROJECT_VERSION.data());

#if !defined(__APPLE__) && !defined(_WIN32)
	QGuiApplication::setDesktopFileName(PROJECT_NAME.data());
#endif

	QApplication app(argc, argv);

	if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
		QApplication::setStyle("fusion");
	}

	auto* window = new Window;
	window->show();

	return QApplication::exec();
}
