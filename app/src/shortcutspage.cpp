/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

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
    m_treeModel->setHorizontalHeaderLabels({ tr("Action", "Shortcut table header"), tr("Shortcut", "Shortcut table header") });
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

ShortcutsPage::~ShortcutsPage() {
    delete ui;
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
        const QString &strShortcutName = getHumanReadableShortcutName(strCmdName);
        if (strShortcutName.isEmpty()) {
            // Shortcut not supported by this version of Pencil2D
            continue;
        }

        QString strKeySequence = settings.value(strCmdName).toString();

        //convert to native format
        strKeySequence = QKeySequence(strKeySequence).toString(QKeySequence::NativeText);

        if (m_treeModel->item(row , ACT_NAME_COLUMN) == nullptr)
            m_treeModel->setItem(row, ACT_NAME_COLUMN, new QStandardItem());
        if (m_treeModel->item(row, KEY_SEQ_COLUMN) == nullptr)
            m_treeModel->setItem(row, KEY_SEQ_COLUMN, new QStandardItem());

        m_treeModel->item(row, ACT_NAME_COLUMN)->setData(strCmdName);
        m_treeModel->item(row, ACT_NAME_COLUMN)->setText(strShortcutName);
        m_treeModel->item(row, ACT_NAME_COLUMN)->setEditable(false);
        m_treeModel->item(row, KEY_SEQ_COLUMN)->setText(strKeySequence);
        m_treeModel->item(row, KEY_SEQ_COLUMN)->setEditable(false);

        row++;
    }
    m_treeModel->setRowCount(row);
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
        {CMD_ADD_FRAME, ShortcutsPage::tr("Add Frame", "Shortcut")},
        {CMD_CLEAR_FRAME, ShortcutsPage::tr("Clear Frame", "Shortcut")},
        {CMD_COPY, ShortcutsPage::tr("Copy", "Shortcut")},
        {CMD_PASTE_FROM_PREVIOUS, ShortcutsPage::tr("Paste from Previous Keyframe", "Shortcut")},
        {CMD_CUT, ShortcutsPage::tr("Cut", "Shortcut")},
        {CMD_DELETE_CUR_LAYER, ShortcutsPage::tr("Delete Current Layer", "Shortcut")},
        {CMD_DESELECT_ALL, ShortcutsPage::tr("Deselect All", "Shortcut")},
        {CMD_DUPLICATE_FRAME, ShortcutsPage::tr("Duplicate Frame", "Shortcut")},
        {CMD_EXIT, ShortcutsPage::tr("Exit", "Shortcut")},
        {CMD_EXPORT_IMAGE, ShortcutsPage::tr("Export Image", "Shortcut")},
        {CMD_EXPORT_IMAGE_SEQ, ShortcutsPage::tr("Export Image Sequence", "Shortcut")},
        {CMD_EXPORT_MOVIE, ShortcutsPage::tr("Export Movie", "Shortcut")},
        {CMD_EXPORT_PALETTE, ShortcutsPage::tr("Export Palette", "Shortcut")},
        {CMD_EXPORT_SOUND, ShortcutsPage::tr("Export Sound", "Shortcut")},
        {CMD_FLIP_HORIZONTAL, ShortcutsPage::tr("Horizontal Flip", "Shortcut")},
        {CMD_FLIP_INBETWEEN, ShortcutsPage::tr("Flip In-Between", "Shortcut")},
        {CMD_FLIP_ROLLING, ShortcutsPage::tr("Flip Rolling", "Shortcut")},
        {CMD_FLIP_VERTICAL, ShortcutsPage::tr("Vertical Flip", "Shortcut")},
        {CMD_GOTO_NEXT_FRAME, ShortcutsPage::tr("Next Frame", "Shortcut")},
        {CMD_GOTO_NEXT_KEY_FRAME, ShortcutsPage::tr("Next Keyframe", "Shortcut")},
        {CMD_GOTO_PREV_FRAME, ShortcutsPage::tr("Previous Frame", "Shortcut")},
        {CMD_GOTO_PREV_KEY_FRAME, ShortcutsPage::tr("Previous Keyframe", "Shortcut")},
        {CMD_SELECTION_ADD_FRAME_EXPOSURE, ShortcutsPage::tr("Selection: Add Frame Exposure", "Shortcut")},
        {CMD_SELECTION_SUBTRACT_FRAME_EXPOSURE, ShortcutsPage::tr("Selection: Subtract Frame Exposure", "Shortcut")},
        {CMD_REVERSE_SELECTED_FRAMES, ShortcutsPage::tr("Selection: Reverse Keyframes", "Shortcut")},
        {CMD_REMOVE_SELECTED_FRAMES, ShortcutsPage::tr("Selection: Remove Keyframes", "Shortcut")},
        {CMD_GRID, ShortcutsPage::tr("Toggle Grid", "Shortcut")},
        {CMD_IMPORT_IMAGE, ShortcutsPage::tr("Import Image", "Shortcut")},
        {CMD_IMPORT_IMAGE_SEQ, ShortcutsPage::tr("Import Image Sequence", "Shortcut")},
        {CMD_IMPORT_SOUND, ShortcutsPage::tr("Import Sound", "Shortcut")},
        {CMD_ALL_LAYER_VISIBILITY, ShortcutsPage::tr("Show All Layers", "Shortcut")},
        {CMD_CURRENT_LAYER_VISIBILITY, ShortcutsPage::tr("Show Current Layer Only", "Shortcut")},
        {CMD_RELATIVE_LAYER_VISIBILITY, ShortcutsPage::tr("Show Layers Relative to Current Layer", "Shortcut")},
        {CMD_LOOP, ShortcutsPage::tr("Toggle Loop", "Shortcut")},
        {CMD_MOVE_FRAME_BACKWARD, ShortcutsPage::tr("Move Frame Backward", "Shortcut")},
        {CMD_MOVE_FRAME_FORWARD, ShortcutsPage::tr("Move Frame Forward", "Shortcut")},
        {CMD_NEW_BITMAP_LAYER, ShortcutsPage::tr("New Bitmap Layer", "Shortcut")},
        {CMD_NEW_CAMERA_LAYER, ShortcutsPage::tr("New Camera Layer", "Shortcut")},
        {CMD_NEW_FILE, ShortcutsPage::tr("New File", "Shortcut")},
        {CMD_NEW_SOUND_LAYER, ShortcutsPage::tr("New Sound Layer", "Shortcut")},
        {CMD_NEW_VECTOR_LAYER, ShortcutsPage::tr("New Vector Layer", "Shortcut")},
        {CMD_ONIONSKIN_NEXT, ShortcutsPage::tr("Toggle Next Onion Skin", "Shortcut")},
        {CMD_ONIONSKIN_PREV, ShortcutsPage::tr("Toggle Previous Onion Skin", "Shortcut")},
        {CMD_OPEN_FILE, ShortcutsPage::tr("Open File", "Shortcut")},
        {CMD_PASTE, ShortcutsPage::tr("Paste", "Shortcut")},
        {CMD_PLAY, ShortcutsPage::tr("Play/Stop", "Shortcut")},
        {CMD_PREFERENCE, ShortcutsPage::tr("Preferences", "Shortcut")},
        {CMD_PREVIEW, ShortcutsPage::tr("Preview", "Shortcut")},
        {CMD_REDO, ShortcutsPage::tr("Redo", "Shortcut")},
        {CMD_REMOVE_FRAME, ShortcutsPage::tr("Remove Frame", "Shortcut")},
        {CMD_RESET_WINDOWS, ShortcutsPage::tr("Reset Windows", "Shortcut")},
        {CMD_RESET_ZOOM_ROTATE, ShortcutsPage::tr("Reset View", "Shortcut")},
        {CMD_CENTER_VIEW, ShortcutsPage::tr("Center View", "Shortcut")},
        {CMD_ROTATE_ANTI_CLOCK, ShortcutsPage::tr("Rotate Anticlockwise", "Shortcut")},
        {CMD_ROTATE_CLOCK, ShortcutsPage::tr("Rotate Clockwise", "Shortcut")},
        {CMD_RESET_ROTATION, ShortcutsPage::tr("Reset Rotation", "Shortcut")},
        {CMD_SAVE_AS, ShortcutsPage::tr("Save File As", "Shortcut")},
        {CMD_SAVE_FILE, ShortcutsPage::tr("Save File", "Shortcut")},
        {CMD_SELECT_ALL, ShortcutsPage::tr("Select All", "Shortcut")},
        {CMD_TOGGLE_STATUS_BAR, ShortcutsPage::tr("Toggle Status Bar Visibility", "Shortcut")},
        {CMD_TOGGLE_COLOR_INSPECTOR, ShortcutsPage::tr("Toggle Color Inspector Window Visibility", "Shortcut")},
        {CMD_TOGGLE_COLOR_LIBRARY, ShortcutsPage::tr("Toggle Color Palette Window Visibility", "Shortcut")},
        {CMD_TOGGLE_COLOR_WHEEL, ShortcutsPage::tr("Toggle Color Box Window Visibility", "Shortcut")},
        {CMD_TOGGLE_DISPLAY_OPTIONS, ShortcutsPage::tr("Toggle Display Window Visibility", "Shortcut")},
        {CMD_TOGGLE_ONION_SKIN, ShortcutsPage::tr("Toggle Onion Skins Window Visibility", "Shortcut")},
        {CMD_TOGGLE_TIMELINE, ShortcutsPage::tr("Toggle Timeline Window Visibility", "Shortcut")},
        {CMD_TOGGLE_TOOLBOX, ShortcutsPage::tr("Toggle Tools Window Visibility", "Shortcut")},
        {CMD_TOGGLE_TOOL_OPTIONS, ShortcutsPage::tr("Toggle Options Window Visibility", "Shortcut")},
        {CMD_TOOL_BRUSH, ShortcutsPage::tr("Brush Tool", "Shortcut")},
        {CMD_TOOL_BUCKET, ShortcutsPage::tr("Bucket Tool", "Shortcut")},
        {CMD_TOOL_ERASER, ShortcutsPage::tr("Eraser Tool", "Shortcut")},
        {CMD_TOOL_EYEDROPPER, ShortcutsPage::tr("Eyedropper Tool", "Shortcut")},
        {CMD_TOOL_HAND, ShortcutsPage::tr("Hand Tool", "Shortcut")},
        {CMD_TOOL_MOVE, ShortcutsPage::tr("Move Tool", "Shortcut")},
        {CMD_TOOL_PEN, ShortcutsPage::tr("Pen Tool", "Shortcut")},
        {CMD_TOOL_PENCIL, ShortcutsPage::tr("Pencil Tool", "Shortcut")},
        {CMD_TOOL_POLYLINE, ShortcutsPage::tr("Polyline Tool", "Shortcut")},
        {CMD_TOOL_SELECT, ShortcutsPage::tr("Select Tool", "Shortcut")},
        {CMD_TOOL_SMUDGE, ShortcutsPage::tr("Smudge Tool", "Shortcut")},
        {CMD_UNDO, ShortcutsPage::tr("Undo", "Shortcut")},
        {CMD_ZOOM_100, ShortcutsPage::tr("Set Zoom to 100%", "Shortcut")},
        {CMD_ZOOM_200, ShortcutsPage::tr("Set Zoom to 200%", "Shortcut")},
        {CMD_ZOOM_25, ShortcutsPage::tr("Set Zoom to 25%", "Shortcut")},
        {CMD_ZOOM_300, ShortcutsPage::tr("Set Zoom to 300%", "Shortcut")},
        {CMD_ZOOM_33, ShortcutsPage::tr("Set Zoom to 33%", "Shortcut")},
        {CMD_ZOOM_400, ShortcutsPage::tr("Set Zoom to 400%", "Shortcut")},
        {CMD_ZOOM_50, ShortcutsPage::tr("Set Zoom to 50%", "Shortcut")},
        {CMD_ZOOM_IN, ShortcutsPage::tr("Zoom In", "Shortcut")},
        {CMD_ZOOM_OUT, ShortcutsPage::tr("Zoom Out", "Shortcut")},
    };

    return humanReadableShortcutNames.value(cmdName, QString());
}
