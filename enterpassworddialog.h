#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>

class CryptNoteEnterPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    CryptNoteEnterPasswordDialog( QWidget* parent = 0 );

public:
    QString password( void )const;

private:
    void updateButtonState( void );

private slots:
    void onPasswordEdited( const QString& text );

private:
    QLineEdit* m_password;
    QDialogButtonBox* m_buttonBox;
};
