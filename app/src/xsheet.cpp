/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "xsheet.h"
#include "layermanager.h"
#include "ui_xsheet.h"
#include <QSettings>
#include <QStringList>
#include <QMessageBox>
#include "pencildef.h"
#include "layer.h"
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QTextStream>

Xsheet::Xsheet(QWidget *parent) :
    BaseDockWidget(parent),
    ui(new Ui::Xsheet)
{
    ui->setupUi(this);
}

Xsheet::Xsheet(Editor *editor, QWidget *parent) :
    BaseDockWidget(parent),
    ui(new Ui::Xsheet)
{
    ui->setupUi(this);
    mEditor = editor;
}


Xsheet::~Xsheet()
{
    delete ui;
}

void Xsheet::newOpenScene()
{
    if (isHidden()) return;
    erasePapa();
    updateXsheet();
}

void Xsheet::initUI()
{
    mLayerNames = new QStringList;
    mLayerIndexes = new QVector<int>;
    mLayerCount = 0;
    QSettings settings(PENCIL2D, PENCIL2D);
    mTimeLineLength = settings.value(SETTING_TIMELINE_SIZE,240).toInt();
    mPapaLines = new QStringList;
    mXsheet = ui->tableXsheet;
    mCurrentFrame = 1;
    connect(mXsheet->selectionModel(), &QItemSelectionModel::currentChanged, this, &Xsheet::selectLayerFrame);
    connect(mXsheet, &QTableWidget::cellDoubleClicked, this, &Xsheet::addLayerFrame);
    connect(ui->btnPapa, &QPushButton::clicked, this, &Xsheet::loadPapa );
    connect(ui->btnNoPapa, &QPushButton::clicked, this, &Xsheet::erasePapa);
    connect(ui->btnSave, &QPushButton::clicked, this, &Xsheet::saveLipsync);
    connect(ui->btnLoad, &QPushButton::clicked, this, &Xsheet::loadLipsync);
    connect(ui->btnSaveCsv, &QPushButton::clicked, this, &Xsheet::saveCsv);
    connect(ui->btnAddFrame, &QPushButton::clicked, this, &Xsheet::addFrame);
    connect(ui->btnDeleteFrame, &QPushButton::clicked, this, &Xsheet::removeFrame);
    updateXsheet();
}

void Xsheet::updateUI()
{
}

void Xsheet::showScrub(int frame)
{
    mXsheetItem = new QTableWidgetItem(QString::number(frame));
    mXsheetItem->setBackground(QColor(250, 150, 150));
    mXsheet->setItem(frame, 0, mXsheetItem);
    mXsheet->scrollToItem(mXsheetItem);
    mCurrentFrame = frame;
}

void Xsheet::updateScrub(int frame)
{
    if (isHidden()) return;
    mXsheetItem = new QTableWidgetItem(QString::number(mCurrentFrame));
    mXsheetItem->setBackground(QColor(250, 240, 160));
    mXsheet->setItem(mCurrentFrame, 0, mXsheetItem);
    showScrub(frame);
}

void Xsheet::lengthChanged(int frames)
{
    mTimeLineLength = frames;
    updateXsheet();
}

void Xsheet::updateXsheet()
{
    if (isHidden()) return;
    initXsheet();
    fillXsheet();
    writePapa();
    showScrub(mEditor->currentFrame());
}

void Xsheet::selectLayerFrame(const QModelIndex &current, const QModelIndex &previous)
{
    QSettings settings(PENCIL2D, PENCIL2D);
    if (mTimeLineLength != settings.value(SETTING_TIMELINE_SIZE).toInt())
    {
        mTimeLineLength = settings.value(SETTING_TIMELINE_SIZE).toInt();
        updateXsheet();
    }
    Q_UNUSED(previous);
    selectItem(current.row(), current.column());
    mEditor->scrubTo(current.row());
}

void Xsheet::addLayerFrame(int row, int column)
{
    selectItem(row, column);
    if (column > 0 && column <= mLayerCount)
    {
        mEditor->layers()->currentLayer()->addNewKeyFrameAt(row);
        int type = getLayerType(mEditor->layers()->getLayer(mLayerIndexes->at(column-1)));
        mXsheetItem = new QTableWidgetItem(QString::number(row));
        mXsheetItem->setBackground(getLayerColor(type));
        mXsheet->setItem(row, column, mXsheetItem);
    }
    else if (column == mXsheet->columnCount() - 1)
    {
        bool ok;
        int len;    // accepted length of string
        QString text;
        if (row > 0)
        {
            QString string = mXsheet->item(row, column)->text();
            text = QInputDialog::getText(this, tr("Input text for Lipsync"),
                                                 tr("Maximum 4 chars accepted:"), QLineEdit::Normal,
                                                 string, &ok);
            len = 4;
        }
        else
        {
            text = QInputDialog::getText(this, tr("Input Character name"),
                                                 tr("Maximum 20 chars accepted:"), QLineEdit::Normal,
                                                 tr("Name or...?"), &ok);
            len = 20;
        }
        if (ok && !text.isEmpty())
        {
            mXsheetItem = new QTableWidgetItem(text.left(len));
            mXsheetItem->setBackground(QColor(245, 155, 155, 150));
            mXsheet->setItem(row, column, mXsheetItem);
            if (mPapaLines->size() > 0 && row > 0)
            {   // if mPapaLines was NOT EMPTY and you typed a Text
                mPapaLines->append(QString::number(row) + " " + text.left(len));
            }
            else if (mPapaLines->size() > 0 && row == 0)
            {   // if mPapaLines was NOT EMPTY and you want to change Name of Character
                QStringList tmp = mPapaLines->at(0).split(" ");
                mPapaLines->insert(0, text.left(20) + " " + tmp.at(1) + " " + tmp.at(2));
            }
            else
            {   // if mPapaLines was EMPTY
                mPapaLines->clear();
                if (row > 0)
                {
                    mPapaLines->append("DIAL 12 " + QString::number(mTimeLineLength));
                    mPapaLines->append(QString::number(row) + " " + text);
                }
                else
                {
                    mPapaLines->append(text + " 12 " + QString::number(mTimeLineLength));
                }
            }
        }
    }
}

void Xsheet::fillXsheet()
{
    for (int i = 1; i <= mTimeLineLength; i++)
    {
        mXsheet->setRowHeight(i,16);
        mXsheetItem = new QTableWidgetItem(QString::number(i));
        mXsheetItem->setBackground(QColor(250, 240, 160));
        mXsheet->setItem(i, 0, mXsheetItem);
        for (int j = 1; j <= mLayerCount; j++)
        {
            if (mEditor->layers()->getLayer(mLayerIndexes->at(j-1))->keyExists(i))
            {
                int type = getLayerType(mEditor->layers()->getLayer(mLayerIndexes->at(j-1)));
                mXsheetItem = new QTableWidgetItem(QString::number(i));
                mXsheetItem->setBackground(getLayerColor(type));
                mXsheet->setItem(i, j, mXsheetItem);
            }
        }
    }
}

/*
 * Load *.pgo file and keep in mPapaLines as follows:
 * First entry in mPapaLines:   Character Name OR DIAL - SPACE - FPS - SPACE - Last frame, if any
 * Next entries in mPapaLines:  Frame - SPACE - phonemes (Only lines with two informations is saved)
 *
 * Extra phonemes can be added by doublecliking in xsheet
 */
void Xsheet::loadPapa()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open *.pgo file"), "", tr("Pgo Files (*.pgo)"));
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    mPapaLines->clear();
    QStringList tmpList;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString tmp = in.readLine();
        tmp.remove("\t");
        tmpList.append(tmp);
    }
    file.close();
    QString tmp = tmpList.at(5);
    if (tmp.length() < 1)
        tmp = "DIAL";
    mPapaLines->append(tmp + " " + tmpList.at(2) + " " + tmpList.at(10));
    QStringList lipsync;    // papagayo info on mouths
    for (int i = 12; i < tmpList.size(); i++)
    {
        tmp = tmpList.at(i);
        lipsync = tmp.split(" ");
        if (lipsync.size() == 2)
        {
            mPapaLines->append(tmp);
        }
    }
    writePapa();
}

void Xsheet::erasePapa()
{
    int dial = mXsheet->columnCount();

    // clear column
    for (int i = 0; i <= mTimeLineLength; i++)
    {
        mXsheetItem = new QTableWidgetItem("");
        mXsheetItem->setBackground(Qt::white);
        mXsheet->setItem(i, dial - 1, mXsheetItem);
    }
    mXsheetItem = new QTableWidgetItem("DIAL");
    mXsheetItem->setBackground(QColor(244, 167, 167, 150));
    mXsheet->setItem(0, mLayerIndexes->size() + 1, mXsheetItem);
    mPapaLines->clear();
}

void Xsheet::loadLipsync()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Lipsync file"), "",
        tr("Pencil2D Lipsync file (*.lip2d)"));
    if (fileName.isEmpty()) { return; }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(this,
                     tr("Unable to open file"),
                     file.errorString());
        return;
    }
    QTextStream in(&file);
    QString first = in.readLine();
    QStringList lipsync = first.split(" ");
    mXsheetItem = new QTableWidgetItem(lipsync.at(0));
    mXsheetItem->setBackground(QColor(250, 240, 160));
    mXsheet->setItem(0, mXsheet->columnCount() - 1, mXsheetItem);
    if (lipsync.size() > 2)
    {
        mXsheetItem = new QTableWidgetItem(lipsync.at(2));
        mXsheetItem->setBackground(QColor(250, 240, 160));
        QString tmp = lipsync.at(2);
        mXsheet->setItem(tmp.toInt(), mXsheet->columnCount() - 1, mXsheetItem);
    }
    mPapaLines->clear();
    mPapaLines->append(first + '\n');
    while (!in.atEnd()) {
        mPapaLines->append(in.readLine() + '\n');
    }
    file.close();
    writePapa();
}

void Xsheet::saveLipsync()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Lipsync column"), "",
            tr("Pencil2D Lipsync file (*.lip2d)"));
    if (fileName.isEmpty()) { return; }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::information(this,
                     tr("Unable to open file"),
                     file.errorString());
        return;
    }
    QTextStream out(&file);
    for (int i = 0; i < mPapaLines->length(); i++)
    {
        out << mPapaLines->at(i) << '\n';
    }
    file.close();
}

void Xsheet::saveCsv()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save xsheet as CSV"), "",
            tr("Csv file (*.csv)"));
    if (fileName.isEmpty()) { return; }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::information(this,
                     tr("Unable to open file"),
                     file.errorString());
        return;
    }
    QTextStream out(&file);
    QString tmp;
    for (int i = 0; i <= mTimeLineLength; i++)
    {
        tmp.clear();
        for (int j = 0; j < mXsheet->columnCount(); j++)
        {
            tmp += mXsheet->item(i, j)->text() + ",";
        }
        tmp.chop(1);
        tmp += '\n';
        out << tmp;
    }
    file.close();
}

void Xsheet::addFrame()
{
    if (mXsheet->currentColumn() == 0 ||
        mXsheet->currentColumn() == mXsheet->columnCount() - 1 ||
        mXsheet->currentRow() == 0) { return; }
    int frame = mXsheet->currentRow();
    Layer* layer = mEditor->layers()->getLayer(mLayerIndexes->at(mXsheet->currentColumn() - 1));
    if (!layer->keyExists(frame))
    {
        mXsheetItem = new QTableWidgetItem(QString::number(mXsheet->currentRow()));
        mXsheetItem->setBackground(QColor(getLayerColor(layer->type())));
        mXsheet->setItem(mXsheet->currentRow(), mXsheet->currentColumn(), mXsheetItem);
        mEditor->layers()->currentLayer()->addNewKeyFrameAt(mXsheet->currentRow());
        emit mEditor->layers()->notifyLayerChanged(layer);
    }
}

void Xsheet::removeFrame()
{
    QString tmp = mXsheet->item(mXsheet->currentRow(), mXsheet->currentColumn())->text();
    if (mXsheet->currentColumn() == 0) { return; }
    mXsheetItem = new QTableWidgetItem("");
    mXsheetItem->setBackground(QColor(Qt::white));
    mXsheet->setItem(mXsheet->currentRow(), mXsheet->currentColumn(), mXsheetItem);
    if (mXsheet->currentColumn() == mXsheet->columnCount() - 1)
    {   // if it is a lipsync column
        for (int i = 1; i < mPapaLines->size(); i++)
        {
            if (mPapaLines->at(i).startsWith(QString::number(mXsheet->currentRow())))
            {
                mPapaLines->removeAt(i);
            }
        }
    }
    else
    {   // if it is a Bitmap or Vector layer
        if (tmp.isEmpty()) { return; }
        Layer* layer = mEditor->layers()->getLayer(mLayerIndexes->at(mXsheet->currentColumn() - 1));
        int frame = mXsheet->currentRow();
        if (layer->keyExists(frame))
        {
            layer->removeKeyFrame(frame);
            emit mEditor->layers()->notifyLayerChanged(layer);
        }
    }
}

void Xsheet::initXsheet()
{
    mLayerCount = 0;
    mLayerIndexes->clear();
    for (int i = 0; i < mEditor->layers()->count(); i++)
    {   // count Bitmap and Vector layers
        bool visi = mEditor->layers()->getLayer(i)->visible();
        if (visi && (mEditor->layers()->getLayer(i)->type() == 1 || mEditor->layers()->getLayer(i)->type() == 2))
        {
            mLayerCount++;
            mLayerIndexes->append(i);
        }
    }
    for (int i = 1; i <= mLayerIndexes->size() + 1; i++)
    {
        for (int j = 1; j <= mTimeLineLength; j++)
        {
            mXsheetItem = new QTableWidgetItem("");
            mXsheetItem->setBackground(Qt::white);
            mXsheet->setItem(j, i, mXsheetItem);
        }
    }
    this->setMinimumWidth(mLayerCount * 40 + 100);
    mXsheet->setRowCount(mTimeLineLength + 1);
    mXsheet->setColumnCount(mLayerCount + 2);
    // set column width for layers
    for (int i = 0; i < mXsheet->columnCount(); i++)
    {
        mXsheet->setColumnWidth(i, 40);
    }
    // set headers of Xsheet
    mXsheet->setRowHeight(0,16);
    mXsheetItem = new QTableWidgetItem("#");
    mXsheetItem->setBackground(QColor(250, 240, 160));
    mXsheet->setItem(0, 0, mXsheetItem);
    for (int i = 0; i < mLayerIndexes->size(); i++)
    {
        int type = getLayerType(mEditor->layers()->getLayer(mLayerIndexes->at(i)));
        mXsheetItem = new QTableWidgetItem(mEditor->layers()->getLayer(mLayerIndexes->at(i))->name());
        mXsheetItem->setBackground(getLayerColor(type));
        mXsheet->setItem(0, i + 1, mXsheetItem);
    }
//    if (!mPapaLines->isEmpty()) { return; }
    mXsheetItem = new QTableWidgetItem("DIAL");
    mXsheetItem->setBackground(QColor(244, 167, 167, 150));
    mXsheet->setItem(0, mLayerIndexes->size() + 1, mXsheetItem);
    if (mFirstUpdate) // hack to make sure a cell is selected at least once
    {
        mXsheet->setCurrentCell(1, 2);
        mFirstUpdate = false;
    }
}

void Xsheet::writePapa()
{
    if (!mPapaLines->isEmpty())
    {
        int dial = mXsheet->columnCount();

        // clear column
        for (int i = 0; i < mTimeLineLength; i++)
        {
            mXsheetItem = new QTableWidgetItem("");
            mXsheetItem->setBackground(Qt::white);
            mXsheet->setItem(i, dial - 1, mXsheetItem);
        }

        // write header
        QStringList lipsync;
        QString tmp;
        tmp = mPapaLines->at(0);
        lipsync = tmp.split(" ");
        mXsheetItem = new QTableWidgetItem(lipsync.at(0));
        mXsheetItem->setBackground(QColor(245, 155, 155, 150));
        mXsheet->setItem(0, dial - 1, mXsheetItem);
        if (lipsync.size() > 2)
        {
            tmp = lipsync.at(2);
            int row = tmp.toInt();
            mXsheet->setItem(row, dial - 1, new QTableWidgetItem("-"));
        }
        for (int i = 1; i < mPapaLines->size(); i++)
        {
            tmp = mPapaLines->at(i);
            lipsync = tmp.split(" ");
            if (lipsync.size() == 2)
            {
                tmp = lipsync.at(0);    // frame number
                int row = tmp.toInt();
                mXsheetItem = new QTableWidgetItem(lipsync.at(1)); // audio to animate
                mXsheetItem->setBackground(QColor(245, 155, 155, 150));
                mXsheet->setItem(row, dial - 1, mXsheetItem);
            }
        }
    }
    else
    {
        erasePapa();
    }
}

int Xsheet::getLayerType(Layer *layer)
{
    return layer->type();
}

void Xsheet::selectItem(int row, int column)
{
    if (column > 0 && column <= mLayerCount)
    {
        mXsheetItem = new QTableWidgetItem();
        mXsheetItem = mXsheet->item(0, column);
        Layer* layer = mEditor->layers()->getLayer(mLayerIndexes->at(column-1));
        if (layer == nullptr) { return; }
        mEditor->layers()->setCurrentLayer(layer);
        mEditor->scrubTo(row);
    }
}

QColor Xsheet::getLayerColor(int color)
{
    if (color == 1) { return QColor(151, 176, 244); }
    else            { return QColor(150, 242, 150); }
}
