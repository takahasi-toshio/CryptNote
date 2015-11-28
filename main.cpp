#include <QApplication>

#include "mainwindow.h"
#include "settings.h"

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    CryptNoteSettings settings;

    CryptNoteMainWindow* window = new CryptNoteMainWindow( &settings );
    window->restoreGeometryAndState();
    window->show();

    const int ret = app.exec();

    window->saveGeometryAndState();

    delete window;

    return ret;
}
