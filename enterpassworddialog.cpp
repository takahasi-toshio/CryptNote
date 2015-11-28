#include "enterpassworddialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

CryptNoteEnterPasswordDialog::CryptNoteEnterPasswordDialog( QWidget* parent )
    : QDialog( parent )
{
    setWindowTitle( tr( "Enter password" ) );

    m_buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    m_buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
    connect( m_buttonBox, SIGNAL( accepted() ), SLOT( accept() ) );
    connect( m_buttonBox, SIGNAL( rejected() ), SLOT( reject() ) );

    m_password = new QLineEdit;
    m_password->setEchoMode( QLineEdit::Password );
    connect( m_password, SIGNAL( textEdited( const QString& ) ), SLOT( onPasswordEdited( const QString& ) ) );

    QHBoxLayout* passwordLayout = new QHBoxLayout;
    passwordLayout->addWidget( new QLabel( tr( "Password:" ) ) );
    passwordLayout->addWidget( m_password );

    QVBoxLayout* baseLayout = new QVBoxLayout;
    baseLayout->addLayout( passwordLayout );
    baseLayout->addWidget( m_buttonBox );
    setLayout( baseLayout );

    m_password->setFocus();
}

QString CryptNoteEnterPasswordDialog::password( void )const
{
    return m_password->text();
}

void CryptNoteEnterPasswordDialog::updateButtonState( void )
{
    QPushButton* okButton = m_buttonBox->button( QDialogButtonBox::Ok );
    okButton->setEnabled( !password().isEmpty() );
}

void CryptNoteEnterPasswordDialog::onPasswordEdited( const QString& )
{
    updateButtonState();
}
