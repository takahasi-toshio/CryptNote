#include <QApplication>
#include <QTranslator>

#include "mainwindow.h"
#include "settings.h"

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    QTranslator translator;
    if( translator.load( ":/trans.ja.qm" ) )
    {
        QApplication::installTranslator( &translator );
    }

    CryptNoteSettings settings;

    CryptNoteMainWindow* window = new CryptNoteMainWindow( &settings );
    window->restoreGeometryAndState();
    window->show();

    const int ret = app.exec();

    window->saveGeometryAndState();

    delete window;

    return ret;
}
