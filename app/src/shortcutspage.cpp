/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "shortcutspage.h"
#include "ui_shortcutspage.h"

#include <QDebug>
#include <QStringRef>
#include <QSettings>
#include <QStandardItemModel>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMessageBox>
#include "pencilsettings.h"
#include <QFile>
#include <QFileDialog>


static const int ACT_NAME_COLUMN = 0;
static const int KEY_SEQ_COLUMN  = 1;

static QString getHumanReadableShortcutName(const QString&);

ShortcutsPage::ShortcutsPage( QWidget* parent )
    : QWidget(parent),
    ui( new Ui::ShortcutsPage )
{
    ui->setupUi(this);
    m_treeModel = new QStandardItemModel(this);
    m_treeModel->setColumnCount(2);
    m_treeModel->setHorizontalHeaderLabels({ tr("Action"), tr("Shortcut") });
    treeModelLoadShortcutsSetting();

    ui->treeView->setModel(m_treeModel);
    ui->treeView->resizeColumnToContents(0);

    connect( ui->treeView, &QTreeView::clicked, this, &ShortcutsPage::tableItemClicked );
    connect( ui->keySequenceEdit, &QKeySequenceEdit::editingFinished, this, &ShortcutsPage::keyCapLineEditTextChanged );
    connect( ui->restoreShortcutsButton, &QPushButton::clicked, this, &ShortcutsPage::restoreShortcutsButtonClicked );
    connect(ui->btnSaveShortcuts, &QPushButton::clicked, this, &ShortcutsPage::saveShortcutsButtonClicked);
    connect(ui->btnLoadShortcuts, &QPushButton::clicked, this, &ShortcutsPage::loadShortcutsButtonClicked);
    connect( ui->clearButton, &QPushButton::clicked, this, &ShortcutsPage::clearButtonClicked );

    ui->treeView->selectionModel()->select(QItemSelection(m_treeModel->index(0, 0), m_treeModel->index(0, m_treeModel->columnCount() - 1)), QItemSelectionModel::Select);
    tableItemClicked(m_treeModel->index(0, 0));
}

void ShortcutsPage::tableItemClicked( const QModelIndex& modelIndex )
{
    int row = modelIndex.row();

    // extract action name
    QStandardItem* actionItem = m_treeModel->item(row, ACT_NAME_COLUMN);
    ui->actionNameLabel->setText(actionItem->text());

    // extract key sequence
    QStandardItem* keySeqItem = m_treeModel->item(row, KEY_SEQ_COLUMN);
    ui->keySequenceEdit->setKeySequence(keySeqItem->text());

    qDebug() << "Command Selected:" << actionItem->text();

    m_currentItemIndex = modelIndex;

    ui->keySequenceEdit->setFocus();
}

void ShortcutsPage::keyCapLineEditTextChanged()
{
    QKeySequence keySequence = ui->keySequenceEdit->keySequence();
    if (!m_currentItemIndex.isValid())
    {
        return;
    }

    int row = m_currentItemIndex.row();
    QStandardItem* actionItem = m_treeModel->item(row, ACT_NAME_COLUMN);
    QStandardItem* keySeqItem = m_treeModel->item(row, KEY_SEQ_COLUMN);

    QString strCmdName = actionItem->data().toString();
    QString strKeySeq  = keySequence.toString( QKeySequence::PortableText );

    QSettings setting( PENCIL2D, PENCIL2D );
    setting.beginGroup("shortcuts");

    if (isKeySequenceExist(setting, strCmdName, keySequence))
    {
        QMessageBox msgBox(this);
        msgBox.setText( tr("Shortcut Conflict!"));
        msgBox.setInformativeText( tr("%1 is already used, overwrite?").arg(keySequence.toString(QKeySequence::NativeText)) );
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon( QMessageBox::Warning );

        int result = msgBox.exec();

        if ( result != QMessageBox::Yes )
        {
            ui->keySequenceEdit->setKeySequence( keySeqItem->text() );
            return;
        }
        removeDuplicateKeySequence(&setting, keySequence);
    }

    setting.setValue(strCmdName, strKeySeq);
    setting.endGroup();
    setting.sync();

    treeModelLoadShortcutsSetting();

    qDebug() << "Shortcut " << strCmdName << " = " << strKeySeq;
}

void ShortcutsPage::saveShortcutsButtonClicked()
{
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.beginGroup( "LastSavePath" );

    QString fDir = settings.value("Shortcuts").toString();
    if (fDir.isEmpty())
        fDir = QDir::homePath();

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Pencil2D Shortcut file"),
                                                    fDir + "/" + tr("untitled.pcls"),
                                                    tr("Pencil2D Shortcut File(*.pcls)"));
    settings.setValue("Shortcuts", fileName);
    settings.endGroup();

    QSettings out(fileName, QSettings::IniFormat);
    settings.beginGroup( "shortcuts" );
    out.beginGroup("shortcuts");

    foreach (QString key, settings.allKeys())
    {
        out.setValue(key, settings.value(key));
    }
}

void ShortcutsPage::loadShortcutsButtonClicked()
{
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.beginGroup("LastSavePath");
    QString fDir = settings.value("Shortcuts").toString();
    settings.endGroup();
    if (fDir.isEmpty())
        fDir = QDir::homePath();

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Pencil2D Shortcut file"),
                                                    fDir,
                                                    tr("Pencil2D Shortcut File(*.pcls)"));

    if (!QFileInfo(fileName).isReadable())
    {
        qDebug() << "Shortcut file not readable";
        return;
    }

    QSettings input(fileName, QSettings::IniFormat);
    if (input.status() != QSettings::NoError || !input.childGroups().contains("shortcuts"))
    {
        qDebug() << "Error accessing or parsing shortcut file" << input.status();
        return;
    }

    input.beginGroup("shortcuts");
    settings.beginGroup("shortcuts");
    foreach (QString pShortcutsKey, input.allKeys())
    {
        settings.setValue(pShortcutsKey, input.value(pShortcutsKey));
    }

    treeModelLoadShortcutsSetting();
}

void ShortcutsPage::restoreShortcutsButtonClicked()
{
    restoreShortcutsToDefault();
    treeModelLoadShortcutsSetting();
}

bool ShortcutsPage::isKeySequenceExist(const QSettings& settings, QString strTargetCmdName, QKeySequence targetkeySeq)
{
    foreach (QString strCmdName, settings.allKeys())
    {
        if (strTargetCmdName == strCmdName)
        {
            continue;
        }

        QString strCmdKeySeq = settings.value(strCmdName).toString();
        if (QKeySequence(strCmdKeySeq) == targetkeySeq)
        {
            return true;
        }
    }
    return false;
}

void ShortcutsPage::removeDuplicateKeySequence(QSettings* settings, QKeySequence keySeq)
{
    foreach(QString strCmdName, settings->allKeys())
    {
        QString strCmdKeySeq = settings->value(strCmdName).toString();

        if ( strCmdKeySeq == keySeq.toString(QKeySequence::PortableText))
        {
            settings->setValue(strCmdName, "");
        }
    }
}

void ShortcutsPage::treeModelLoadShortcutsSetting()
{
    // Load shortcuts from settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.beginGroup("shortcuts");

    m_treeModel->setRowCount(settings.allKeys().size());
    Q_ASSERT(m_treeModel->columnCount() == 2);

    int row = 0;
    foreach (QString strCmdName, settings.allKeys())
    {
        QString strKeySequence = settings.value(strCmdName).toString();

        //convert to native format
        strKeySequence = QKeySequence(strKeySequence).toString(QKeySequence::NativeText);

        if (m_treeModel->item(row , ACT_NAME_COLUMN) == nullptr)
            m_treeModel->setItem(row, ACT_NAME_COLUMN, new QStandardItem());
        if (m_treeModel->item(row, KEY_SEQ_COLUMN) == nullptr)
            m_treeModel->setItem(row, KEY_SEQ_COLUMN, new QStandardItem());

        m_treeModel->item(row, ACT_NAME_COLUMN)->setData(strCmdName);
        m_treeModel->item(row, ACT_NAME_COLUMN)->setText(getHumanReadableShortcutName(strCmdName));
        m_treeModel->item(row, ACT_NAME_COLUMN)->setEditable(false);
        m_treeModel->item(row, KEY_SEQ_COLUMN)->setText(strKeySequence);
        m_treeModel->item(row, KEY_SEQ_COLUMN)->setEditable(false);

        row++;
    }
    settings.endGroup();

    ui->treeView->resizeColumnToContents( 0 );
}

void ShortcutsPage::clearButtonClicked()
{
    if ( !m_currentItemIndex.isValid() )
    {
        return;
    }

    int row = m_currentItemIndex.row();
    QStandardItem* actionItem = m_treeModel->item(row, ACT_NAME_COLUMN);

    QString strCmdName = QString("shortcuts/%1").arg( actionItem->data().toString() );

    QSettings setting( PENCIL2D, PENCIL2D );
    setting.setValue( strCmdName, "" );
    setting.sync();

    ui->keySequenceEdit->clear();

    treeModelLoadShortcutsSetting();
}

/**
 * Translates internal to human-readable shortcut names
 *
 * @param[in] cmdName The name of the setting corresponding to the shortcut
 * @return The translated, human-readable name of the shortcut
 */
static QString getHumanReadableShortcutName(const QString& cmdName)
{
    static QHash<QString, QString> humanReadableShortcutNames = QHash<QString, QString>{
        {"CmdAddFrame", QObject::tr("Add Frame")},
        {"CmdClearFrame", QObject::tr("Clear Frame")},
        {"CmdCopy", QObject::tr("Copy")},
        {"CmdCut", QObject::tr("Cut")},
        {"CmdDeleteCurrentLayer", QObject::tr("Delete Current Layer")},
        {"CmdDeselectAll", QObject::tr("Deselect All")},
        {"CmdDuplicateFrame", QObject::tr("Duplicate Frame")},
        {"CmdExit", QObject::tr("Exit")},
        {"CmdExportImage", QObject::tr("Export Image")},
        {"CmdExportImageSequence", QObject::tr("Export Image Sequence")},
        {"CmdExportMovie", QObject::tr("Export Movie")},
        {"CmdExportPalette", QObject::tr("Export Palette")},
        {"CmdExportSound", QObject::tr("Export Sound")},
        {"CmdFlipHorizontal", QObject::tr("Horizontal Flip")},
        {"CmdFlipInBetween", QObject::tr("Flip In-Between")},
        {"CmdFlipRolling", QObject::tr("Flip Rolling")},
        {"CmdFlipVertical", QObject::tr("Vertical Flip")},
        {"CmdGotoNextFrame", QObject::tr("Next Frame")},
        {"CmdGotoNextKeyFrame", QObject::tr("Next Keyframe")},
        {"CmdGotoPreviousFrame", QObject::tr("Previous Frame")},
        {"CmdGotoPreviousKeyFrame", QObject::tr("Previous Keyframe")},
        {"CmdGrid", QObject::tr("Toggle Grid")},
        {"CmdImportImage", QObject::tr("Import Image")},
        {"CmdImportImageSequence", QObject::tr("Import Image Sequence")},
        {"CmdImportSound", QObject::tr("Import Sound")},
        {"CmdLayerVisibilityAll", QObject::tr("Show All Layers")},
        {"CmdLayerVisibilityCurrentOnly", QObject::tr("Show Current Layer Only")},
        {"CmdLayerVisibilityRelative", QObject::tr("Show Layers Relative to Current Layer")},
        {"CmdLoop", QObject::tr("Toggle Loop")},
        {"CmdMoveFrameBackward", QObject::tr("Move Frame Backward")},
        {"CmdMoveFrameForward", QObject::tr("Move Frame Forward")},
        {"CmdNewBitmapLayer", QObject::tr("New Bitmap Layer")},
        {"CmdNewCameraLayer", QObject::tr("New Camera Layer")},
        {"CmdNewFile", QObject::tr("New File")},
        {"CmdNewSoundLayer", QObject::tr("New Sound Layer")},
        {"CmdNewVectorLayer", QObject::tr("New Vector Layer")},
        {"CmdOnionSkinNext", QObject::tr("Toggle Next Onion Skin")},
        {"CmdOnionSkinPrevious", QObject::tr("Toggle Previous Onion Skin")},
        {"CmdOpenFile", QObject::tr("Open File")},
        {"CmdPaste", QObject::tr("Paste")},
        {"CmdPlay", QObject::tr("Play/Stop")},
        {"CmdPreferences", QObject::tr("Preferences")},
        {"CmdRedo", QObject::tr("Redo")},
        {"CmdRemoveFrame", QObject::tr("Remove Frame")},
        {"CmdResetWindows", QObject::tr("Reset Windows")},
        {"CmdResetZoomRotate", QObject::tr("Reset Zoom/Rotate")},
        {"CmdRotateAntiClockwise", QObject::tr("Rotate Anticlockwise")},
        {"CmdRotateClockwise", QObject::tr("Rotate Clockwise")},
        {"CmdSaveAs", QObject::tr("Save File As")},
        {"CmdSaveFile", QObject::tr("Save File")},
        {"CmdSelectAll", QObject::tr("Select All")},
        {"CmdToggleColorInspector", QObject::tr("Toggle Color Inspector Window Visibility")},
        {"CmdToggleColorLibrary", QObject::tr("Toggle Color Palette Window Visibility")},
        {"CmdToggleColorWheel", QObject::tr("Toggle Color Box Window Visibility")},
        {"CmdToggleDisplayOptions", QObject::tr("Toggle Display Window Visibility")},
        {"CmdToggleOnionSkin", QObject::tr("Toggle Onion Skins Window Visibility")},
        {"CmdToggleTimeline", QObject::tr("Toggle Timeline Window Visibility")},
        {"CmdToggleToolBox", QObject::tr("Toggle Tools Window Visibility")},
        {"CmdToggleToolOptions", QObject::tr("Toggle Options Window Visibility")},
        {"CmdToolBrush", QObject::tr("Brush Tool")},
        {"CmdToolBucket", QObject::tr("Bucket Tool")},
        {"CmdToolEraser", QObject::tr("Eraser Tool")},
        {"CmdToolEyedropper", QObject::tr("Eyedropper Tool")},
        {"CmdToolHand", QObject::tr("Hand Tool")},
        {"CmdToolMove", QObject::tr("Move Tool")},
        {"CmdToolPen", QObject::tr("Pen Tool")},
        {"CmdToolPencil", QObject::tr("Pencil Tool")},
        {"CmdToolPolyline", QObject::tr("Polyline Tool")},
        {"CmdToolSelect", QObject::tr("Select Tool")},
        {"CmdToolSmudge", QObject::tr("Smudge Tool")},
        {"CmdUndo", QObject::tr("Undo")},
        {"CmdZoom100", QObject::tr("Set Zoom to 100%")},
        {"CmdZoom200", QObject::tr("Set Zoom to 200%")},
        {"CmdZoom25", QObject::tr("Set Zoom to 25%")},
        {"CmdZoom300", QObject::tr("Set Zoom to 300%")},
        {"CmdZoom33", QObject::tr("Set Zoom to 33%")},
        {"CmdZoom400", QObject::tr("Set Zoom to 400%")},
        {"CmdZoom50", QObject::tr("Set Zoom to 50%")},
        {"CmdZoomIn", QObject::tr("Zoom In")},
        {"CmdZoomOut", QObject::tr("Zoom Out")},
    };

    return humanReadableShortcutNames.value(cmdName, cmdName);
}
