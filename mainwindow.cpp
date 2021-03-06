#include "mainwindow.h"

#include <QAction>
#include <QCryptographicHash>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSet>
#include <QSignalBlocker>
#include <QStack>
#include <QTextEdit>
#include <QToolBar>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "3rd/qblowfish/src/qblowfish.h"

#include "changepassworddialog.h"
#include "enterpassworddialog.h"

static int NoteRole = Qt::UserRole;

CryptNoteMainWindow::CryptNoteMainWindow( CryptNoteSettings* settings )
    : m_settings( settings ),
      m_modified( false )
{
    m_treeWidget = new QTreeWidget;
    m_treeWidget->setHeaderHidden( true );
    m_treeWidget->setDragDropMode( QAbstractItemView::InternalMove );
    QObject::connect(
        m_treeWidget, SIGNAL( itemChanged( QTreeWidgetItem*, int ) ),
        this, SLOT( onItemChanged( QTreeWidgetItem*, int ) ) );

    m_textEdit = new QTextEdit;
    m_textEdit->setAcceptRichText( true );
    m_textEdit->setEnabled( false );
    QObject::connect(
        m_textEdit, SIGNAL( textChanged() ),
        this, SLOT( onTextChanged() ) );

    m_splitter = new QSplitter;
    m_splitter->addWidget( m_treeWidget );
    m_splitter->addWidget( m_textEdit );
    setCentralWidget( m_splitter );

    QMenuBar* menuBar = this->menuBar();
    QMenu* fileMenu = menuBar->addMenu( tr( "&File" ) );
    QAction* openAction = fileMenu->addAction( tr( "&Open..." ) );
    openAction->setShortcut( QKeySequence::Open );
    connect( openAction, SIGNAL( triggered( bool ) ), SLOT( open() ) );
    QAction* saveAction = fileMenu->addAction( tr( "&Save" ) );
    saveAction->setShortcut( QKeySequence::Save );
    connect( saveAction, SIGNAL( triggered( bool ) ), SLOT( save() ) );
    QAction* saveAsAction = fileMenu->addAction( tr( "Save &as..." ) );
    connect( saveAsAction, SIGNAL( triggered( bool ) ), SLOT( saveAs() ) );

    QToolBar* itemToolBar = addToolBar( tr( "Item" ) ) ;
    itemToolBar->setObjectName( "ItemToolBar" );
    QAction* addFolderAction = itemToolBar->addAction( tr( "Add folder" ) );
    addFolderAction->setIcon( QIcon( ":/icons/Folder_Add.png" ) );
    connect( addFolderAction, SIGNAL( triggered( bool ) ), SLOT( addFolder() ) );
    QAction* addNoteAction = itemToolBar->addAction( tr( "Add note" ) );
    addNoteAction->setIcon( QIcon( ":/icons/File_Add.png" ) );
    connect( addNoteAction, SIGNAL( triggered( bool ) ), SLOT( addNote() ) );
    m_removeAction = itemToolBar->addAction( tr( "Remove" ) );
    m_removeAction->setIcon( QIcon( ":/icons/Trash.png" ) );
    connect( m_removeAction, SIGNAL( triggered( bool ) ), SLOT( remove() ) );

    connect(
        m_treeWidget, SIGNAL( currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ),
        SLOT( onCurrentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ) );

    updateWindowTitle();
    updateRemoveAction();
}

bool CryptNoteMainWindow::event( QEvent* e )
{
    if( e->type() == QEvent::ShowToParent )
    {
        const QVariant treeWidth = m_settings->value( "treeWidth" );
        if( treeWidth.isValid() )
        {
            QList<int> sizes = m_splitter->sizes();
            if( sizes.size() == 2 )
            {
                sizes[1] += sizes[0] - treeWidth.toInt();
                sizes[0] = treeWidth.toInt();
                m_splitter->setSizes( sizes );
            }
        }
    }

    return QMainWindow::event( e );
}

void CryptNoteMainWindow::saveGeometryAndState( void )
{
    m_settings->setValue( "mainWindowGeometry", saveGeometry() );
    m_settings->setValue( "mainWindowState", saveState() );
    const QList<int> sizes = m_splitter->sizes();
    if( sizes.size() > 0 )
    {
        m_settings->setValue( "treeWidth", sizes[0] );
    }
}

void CryptNoteMainWindow::restoreGeometryAndState( void )
{
    const QVariant mainWindowGeometry = m_settings->value( "mainWindowGeometry" );
    if( mainWindowGeometry.isValid() )
    {
        restoreGeometry( mainWindowGeometry.toByteArray() );
    }

    const QVariant mainWindowState = m_settings->value( "mainWindowState" );
    if( mainWindowState.isValid() )
    {
        restoreState( mainWindowState.toByteArray() );
    }
}

bool CryptNoteMainWindow::isFolder( const QTreeWidgetItem* item )const
{
    if( !item )
    {
        return false;
    }

    return !( item->flags() & Qt::ItemNeverHasChildren );
}

bool CryptNoteMainWindow::isNote( const QTreeWidgetItem* item )const
{
    if( !item )
    {
        return false;
    }

    return item->flags() & Qt::ItemNeverHasChildren;
}

void CryptNoteMainWindow::write( QXmlStreamWriter& writer, const QTreeWidgetItem* item )const
{
    if( isNote( item ) )
    {
        writer.writeStartElement( "note" );
        writer.writeAttribute( "name", item->text( 0 ) );
        writer.writeCharacters( item->data( 0, NoteRole ).toString() );
        writer.writeEndElement();
    }
    else if( isFolder( item ) )
    {
        writer.writeStartElement( "folder" );
        writer.writeAttribute( "name", item->text( 0 ) );
        writer.writeAttribute( "expanded", item->isExpanded() ? "true" : "false" );
        for( int i = 0; i < item->childCount(); ++i )
        {
            write( writer, item->child( i ) );
        }
        writer.writeEndElement();
    }
}

QTreeWidgetItem* CryptNoteMainWindow::createNoteItem( void )const
{
    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setIcon( 0, style()->standardIcon( QStyle::SP_FileIcon ) );
    item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemNeverHasChildren | Qt::ItemIsDragEnabled );
    return item;
}

QTreeWidgetItem* CryptNoteMainWindow::createFolderItem( void )const
{
    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setIcon( 0, style()->standardIcon( QStyle::SP_DirIcon ) );
    item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
    return item;
}

QString CryptNoteMainWindow::lastOpenDir( void )const
{
    const QVariant lastOpenDir = m_settings->value( "lastOpenDir" );
    if( lastOpenDir.isValid() )
    {
        return lastOpenDir.toString();
    }
    else
    {
        return QString();
    }
}

void CryptNoteMainWindow::saveLastOpenDir()const
{
    m_settings->setValue( "lastOpenDir", QFileInfo( m_filePath ).dir().path() );
}

QString CryptNoteMainWindow::lastSaveDir( void )const
{
    const QVariant lastSaveDir = m_settings->value( "lastSaveDir" );
    if( lastSaveDir.isValid() )
    {
        return lastSaveDir.toString();
    }
    else
    {
        return QString();
    }
}

void CryptNoteMainWindow::saveLastSaveDir( void )const
{
    m_settings->setValue( "lastSaveDir", QFileInfo( m_filePath ).dir().path() );
}

bool CryptNoteMainWindow::save( const QString& filePath, const QByteArray& key )const
{
    QFile file( filePath );
    if( !file.open( QIODevice::WriteOnly ) )
    {
        return false;
    }

    QTreeWidgetItem* currentItem = m_treeWidget->currentItem();
    if( isNote( currentItem ) )
    {
        QSignalBlocker signalBlocker( m_treeWidget );
        currentItem->setData( 0, NoteRole, m_textEdit->toHtml() );
    }

    QByteArray data;
    QXmlStreamWriter writer( &data );
    writer.writeStartDocument();
    writer.writeStartElement( "notes" );
    for( int i = 0; i < m_treeWidget->topLevelItemCount(); ++i )
    {
        write( writer, m_treeWidget->topLevelItem( i ) );
    }
    writer.writeEndElement();
    writer.writeEndDocument();

    data.append( QByteArray( 8 - data.size() % 8, '\0' ) );

    QBlowfish blowfish( key );
    const QByteArray encryptedData = blowfish.encrypted( data );

    file.write( encryptedData );

    return true;
}

void CryptNoteMainWindow::updateWindowTitle( void )
{
    QString windowTitle;
    if( m_filePath.isEmpty() )
    {
        windowTitle += tr( "(No name)" );
    }
    else
    {
        windowTitle += QDir::toNativeSeparators( m_filePath );
    }

    if( m_modified )
    {
        windowTitle += " ";
        windowTitle += tr( "(Modified)" );
    }

    windowTitle += " - CryptNote";

    setWindowTitle( windowTitle );
}

void CryptNoteMainWindow::setModified( bool onoff )
{
    if( onoff )
    {
        if( !m_modified )
        {
            m_modified = true;
            updateWindowTitle();
        }
    }
    else
    {
        if( m_modified )
        {
            m_modified = false;
            updateWindowTitle();
        }
    }
}

void CryptNoteMainWindow::updateRemoveAction( void )
{
    m_removeAction->setEnabled( m_treeWidget->currentItem() );
}

void CryptNoteMainWindow::addNote( void )
{
    QTreeWidgetItem* item = createNoteItem();
    item->setText( 0, tr( "No name" ) );
    m_treeWidget->addTopLevelItem( item );
    m_treeWidget->scrollToItem( item );
    m_treeWidget->setCurrentItem( item );
    setModified( true );
}

void CryptNoteMainWindow::addFolder( void )
{
    QTreeWidgetItem* item = createFolderItem();
    item->setText( 0, tr( "No name" ) );
    m_treeWidget->addTopLevelItem( item );
    m_treeWidget->scrollToItem( item );
    m_treeWidget->setCurrentItem( item );
    setModified( true );
}

void CryptNoteMainWindow::remove( void )
{
    QTreeWidgetItem* currentItem = m_treeWidget->currentItem();
    if( currentItem )
    {
        delete currentItem;
        setModified( true );
    }
}

void CryptNoteMainWindow::open( void )
{
    const QString filePath = QFileDialog::getOpenFileName(
                                 this,
                                 QString(),
                                 lastOpenDir(),
                                 tr( "CryptNote data (*.cnd)" ) );
    if( filePath.isEmpty() )
    {
        return;
    }

    QFile file( filePath );
    if( !file.open( QIODevice::ReadOnly ) )
    {
        QMessageBox::critical( this, tr( "Error" ), tr( "Failed to open the specified file." ) );
        return;
    }

    const QByteArray encryptedData = file.readAll();

    file.close();

    CryptNoteEnterPasswordDialog passwordDialog( this );
    if( passwordDialog.exec() == QDialog::Rejected )
    {
        return;
    }

    QCryptographicHash hash( QCryptographicHash::Sha1 );
    hash.addData( passwordDialog.password().toUtf8() );

    const QByteArray key = hash.result();

    QBlowfish blowfish( key );
    QByteArray data = blowfish.decrypted( encryptedData );
    while( data.endsWith( QByteArray( 1, '\0' ) ) )
    {
        data.chop( 1 );
    }

    QTreeWidgetItem* item = NULL;
    QList<QTreeWidgetItem*> itemList;
    QStack<QTreeWidgetItem*> folderItemStack;
    QSet<QTreeWidgetItem*> expandedFolderItemSet;

    const QString strNote( "note" );
    const QString strFolder( "folder" );

    QXmlStreamReader reader( data );
    while( !reader.atEnd() )
    {
        reader.readNext();

        if( reader.isStartElement() )
        {
            if( reader.qualifiedName() == strNote )
            {
                item = createNoteItem();
                item->setText( 0, reader.attributes().value( "name" ).toString() );
                if( folderItemStack.isEmpty() )
                {
                    itemList.push_back( item );
                }
                else
                {
                    folderItemStack.top()->addChild( item );
                }
            }
            else if( reader.qualifiedName() == strFolder )
            {
                item = createFolderItem();
                item->setText( 0, reader.attributes().value( "name" ).toString() );
                if( reader.attributes().value( "expanded" ).toString() == "true" )
                {
                    expandedFolderItemSet.insert( item );
                }
                if( folderItemStack.isEmpty() )
                {
                    itemList.push_back( item );
                }
                else
                {
                    folderItemStack.top()->addChild( item );
                }
                folderItemStack.push( item );
            }
            else
            {
                item = NULL;
            }
        }
        else if( reader.isCharacters() )
        {
            if( item )
            {
                item->setData( 0, NoteRole, reader.text().toString() );
            }
        }
        else if( reader.isEndElement() )
        {
            if( reader.qualifiedName() == strFolder )
            {
                if( !folderItemStack.isEmpty() )
                {
                    folderItemStack.pop();
                }
            }
        }
    }
    if( reader.hasError() )
    {
        for( int i = 0; i < itemList.size(); ++i )
        {
            delete itemList[i];
        }
        QMessageBox::critical( this, tr( "Error" ), tr( "The specified password does not match." ) );
        return;
    }

    m_filePath = filePath;
    m_key = key;

    saveLastOpenDir();
    updateWindowTitle();

    m_treeWidget->clear();
    m_textEdit->setEnabled( false );
    for( int i = 0; i < itemList.size(); ++i )
    {
        m_treeWidget->addTopLevelItem( itemList[i] );
    }

    for( QSet<QTreeWidgetItem*>::const_iterator itr = expandedFolderItemSet.begin();
         itr != expandedFolderItemSet.end(); ++itr )
    {
        ( *itr )->setExpanded( true );
    }
}

void CryptNoteMainWindow::save( void )
{
    if( m_filePath.isEmpty() )
    {
        saveAs();
        return;
    }

    QByteArray key = m_key;
    if( key.isEmpty() )
    {
        CryptNoteChangePasswordDialog passwordDialog( this );
        if( passwordDialog.exec() == QDialog::Rejected )
        {
            return;
        }

        QCryptographicHash hash( QCryptographicHash::Sha1 );
        hash.addData( passwordDialog.newPassword().toUtf8() );

        key = hash.result();
    }

    if( save( m_filePath, key ) )
    {
        m_key = key;
        m_modified = false;

        saveLastSaveDir();
        updateWindowTitle();
    }
    else
    {
        QMessageBox::critical( this, tr( "Error" ), tr( "Failed to save the specified file." ) );
    }
}

void CryptNoteMainWindow::saveAs( void )
{
    const QString filePath = QFileDialog::getSaveFileName(
                                 this,
                                 QString(),
                                 lastSaveDir(),
                                 tr( "CryptNote data (*.cnd)" ) );
    if( filePath.isEmpty() )
    {
        return;
    }

    QByteArray key = m_key;
    if( key.isEmpty() )
    {
        CryptNoteChangePasswordDialog passwordDialog( this );
        if( passwordDialog.exec() == QDialog::Rejected )
        {
            return;
        }

        QCryptographicHash hash( QCryptographicHash::Sha1 );
        hash.addData( passwordDialog.newPassword().toUtf8() );

        key = hash.result();
    }

    if( save( filePath, key ) )
    {
        m_filePath = filePath;
        m_key = key;
        m_modified = false;

        saveLastSaveDir();
        updateWindowTitle();
    }
    else
    {
        QMessageBox::critical( this, tr( "Error" ), tr( "Failed to save the specified file." ) );
    }
}

void CryptNoteMainWindow::onCurrentItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
    if( isNote( previous ) )
    {
        QSignalBlocker signalBlocker( m_treeWidget );
        previous->setData( 0, NoteRole, m_textEdit->toHtml() );
    }

    if( isFolder( current ) )
    {
        m_textEdit->setEnabled( false );
        QSignalBlocker signalBlocker( m_textEdit );
        m_textEdit->clear();
    }
    else if( isNote( current ) )
    {
        m_textEdit->setEnabled( true );
        QSignalBlocker signalBlock( m_textEdit );
        m_textEdit->setHtml( current->data( 0, NoteRole ).toString() );
    }

    updateRemoveAction();
}

void CryptNoteMainWindow::onItemChanged( QTreeWidgetItem* item, int column )
{
    setModified( true );
}

void CryptNoteMainWindow::onTextChanged( void )
{
    setModified( true );
}
