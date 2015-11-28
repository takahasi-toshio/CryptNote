#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>

class CryptNoteChangePasswordDialog : public QDialog
{
    Q_OBJECT

public:
    CryptNoteChangePasswordDialog( QWidget* parent = 0 );

public:
    QString newPassword( void )const;

private:
    void updateButtonState( void );

private slots:
    void onNewPasswordEdited( const QString& text );
    void onNewPasswordReturnPressed( void );
    void onRetypePasswordEdited( const QString& text );

private:
    QLabel* m_currentPasswordLabel;
    QLineEdit* m_currentPassword;
    QLineEdit* m_newPassword;
    QLineEdit* m_retypePassword;
    QDialogButtonBox* m_buttonBox;
};
