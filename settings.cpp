#include "settings.h"

CryptNoteSettings::CryptNoteSettings( void )
    : QSettings( QSettings::UserScope, "motools", "CryptNote" )
{
}
