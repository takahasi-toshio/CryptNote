#include "changepassworddialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

CryptNoteChangePasswordDialog::CryptNoteChangePasswordDialog( QWidget* parent )
    : QDialog( parent )
{
    setWindowTitle( tr( "Change password" ) );

    m_buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    m_buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
    connect( m_buttonBox, SIGNAL( accepted() ), SLOT( accept() ) );
    connect( m_buttonBox, SIGNAL( rejected() ), SLOT( reject() ) );

    m_currentPasswordLabel = new QLabel( tr( "Current password:" ) );
    m_currentPasswordLabel->setEnabled( false );

    m_currentPassword = new QLineEdit;
    m_currentPassword->setEchoMode( QLineEdit::Password );
    m_currentPassword->setEnabled( false );

    m_newPassword = new QLineEdit;
    m_newPassword->setEchoMode( QLineEdit::Password );
    connect( m_newPassword, SIGNAL( textEdited( const QString& ) ), SLOT( onNewPasswordEdited( const QString& ) ) );
    connect( m_newPassword, SIGNAL( returnPressed() ), SLOT( onNewPasswordReturnPressed() ) );

    m_retypePassword = new QLineEdit;
    m_retypePassword->setEchoMode( QLineEdit::Password );
    connect( m_retypePassword, SIGNAL( textEdited( const QString& ) ), SLOT( onRetypePasswordEdited( const QString& ) ) );

    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget( m_currentPasswordLabel, 0, 0 );
    gridLayout->addWidget( m_currentPassword, 0, 1 );
    gridLayout->addWidget( new QLabel( tr( "New password:" ) ), 1, 0 );
    gridLayout->addWidget( m_newPassword, 1, 1 );
    gridLayout->addWidget( new QLabel( tr( "Retype password:" ) ), 2, 0 );
    gridLayout->addWidget( m_retypePassword, 2, 1 );

    QVBoxLayout* baseLayout = new QVBoxLayout;
    baseLayout->addLayout( gridLayout );
    baseLayout->addWidget( m_buttonBox );
    setLayout( baseLayout );

    m_currentPassword->setFocus();
}

QString CryptNoteChangePasswordDialog::newPassword( void )const
{
    return m_newPassword->text();
}

void CryptNoteChangePasswordDialog::updateButtonState( void )
{
    QPushButton* okButton = m_buttonBox->button( QDialogButtonBox::Ok );
    const QString newPassword = m_newPassword->text();
    const QString retypePassword = m_retypePassword->text();
    okButton->setEnabled(
        !newPassword.isEmpty() &&
        !retypePassword.isEmpty() &&
        ( newPassword == retypePassword ) );
}

void CryptNoteChangePasswordDialog::onNewPasswordEdited( const QString& )
{
    updateButtonState();
}

void CryptNoteChangePasswordDialog::onNewPasswordReturnPressed( void )
{
    m_retypePassword->setFocus();
}

void CryptNoteChangePasswordDialog::onRetypePasswordEdited( const QString& )
{
    updateButtonState();
}
