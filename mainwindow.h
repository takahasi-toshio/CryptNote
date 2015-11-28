#pragma once

#include <QByteArray>
#include <QMainWindow>
#include <QSplitter>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QXmlStreamWriter>

#include "settings.h"

class CryptNoteMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CryptNoteMainWindow( CryptNoteSettings* settings );

public:
    virtual bool event( QEvent* e );

public:
    void saveGeometryAndState( void );
    void restoreGeometryAndState( void );

private:
    bool isFolder( const QTreeWidgetItem* item )const;
    bool isNote( const QTreeWidgetItem* item )const;
    void write( QXmlStreamWriter& writer, const QTreeWidgetItem* item )const;
    QTreeWidgetItem* createNoteItem( void )const;
    QTreeWidgetItem* createFolderItem( void )const;

private slots:
    void addNote( void );
    void addFolder( void );
    void open( void );
    void save( void );
    void saveAs( void );
    void onCurrentItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );

private:
    CryptNoteSettings* m_settings;
    QSplitter* m_splitter;
    QTreeWidget* m_treeWidget;
    QTextEdit* m_textEdit;

private:
    QString m_filePath;
    QByteArray m_key;
};
