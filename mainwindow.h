#pragma once

#include <QAction>
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
    QString lastOpenDir( void )const;
    void saveLastOpenDir( void )const;
    QString lastSaveDir( void )const;
    void saveLastSaveDir( void )const;
    bool save( const QString& filePath, const QByteArray& key )const;
    void updateWindowTitle( void );
    void setModified( bool onoff );
    void updateRemoveAction( void );

private slots:
    void addNote( void );
    void addFolder( void );
    void remove( void );
    void open( void );
    void save( void );
    void saveAs( void );
    void onCurrentItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );
    void onItemChanged( QTreeWidgetItem* item, int column );
    void onTextChanged( void );

private:
    CryptNoteSettings* m_settings;
    QSplitter* m_splitter;
    QTreeWidget* m_treeWidget;
    QTextEdit* m_textEdit;
    QAction* m_removeAction;

private:
    QString m_filePath;
    QByteArray m_key;
    bool m_modified;
};
