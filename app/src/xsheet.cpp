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
    erasePapa();
    updateXsheet();
}

void Xsheet::initUI()
{
    mLayerNames = new QStringList;
    mLayerCount = 0;
    QSettings settings(PENCIL2D, PENCIL2D);
    mTimeLineLength = settings.value(SETTING_TIMELINE_SIZE,240).toInt();
    mPapaLines = new QStringList;
    mTableWidget = ui->tableXsheet;
    mCurrentFrame = 1;
    connect(mTableWidget->selectionModel(), &QItemSelectionModel::currentChanged, this, &Xsheet::selectLayerFrame);
    connect(mTableWidget, &QTableWidget::cellDoubleClicked, this, &Xsheet::addLayerFrame);
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
    mTableItem = new QTableWidgetItem(QString::number(frame));
    mTableItem->setBackgroundColor(QColor(250, 150, 150));
    mTableWidget->setItem(frame, 0, mTableItem);
    mTableWidget->scrollToItem(mTableItem);
    mCurrentFrame = frame;
}

void Xsheet::updateScrub(int frame)
{
    mTableItem = new QTableWidgetItem(QString::number(mCurrentFrame));
    mTableItem->setBackgroundColor(QColor(250, 240, 160));
    mTableWidget->setItem(mCurrentFrame, 0, mTableItem);
    showScrub(frame);
}

void Xsheet::lengthChanged(int frames)
{
    mTimeLineLength = frames;
    updateXsheet();
}

void Xsheet::updateXsheet()
{
    initXsheet();
    fillXsheet();
    writePapa();
    showScrub(mEditor->currentFrame());
}

void Xsheet::selectLayerFrame(const QModelIndex &current, const QModelIndex &previous)
{
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
        int type = getLayerType(mEditor->layers()->findLayerByName(mTableWidget->item(0, column)->text()));
        mTableItem = new QTableWidgetItem(QString::number(row));
        mTableItem->setBackgroundColor(getLayerColor(type));
        mTableWidget->setItem(row, column, mTableItem);
    }
    else if (column == mTableWidget->columnCount() - 1)
    {
        bool ok;
        int len;    // accepted length of string
        QString text;
        if (row > 0)
        {
            QString string = mTableWidget->item(row, column)->text();
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
            mTableItem = new QTableWidgetItem(text.left(len));
            mTableItem->setBackgroundColor(QColor(245, 155, 155, 150));
            mTableWidget->setItem(row, column, mTableItem);
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
        mTableWidget->setRowHeight(i,16);
        mTableItem = new QTableWidgetItem(QString::number(i));
        mTableItem->setBackgroundColor(QColor(250, 240, 160));
        mTableWidget->setItem(i, 0, mTableItem);
        for (int j = 1; j <= mLayerCount; j++)
        {
            if (mEditor->layers()->findLayerByName(mTableWidget->item(0,j)->text())->keyExists(i))
            {
                int type = getLayerType(mEditor->layers()->findLayerByName(mTableWidget->item(0,j)->text()));
                mTableItem = new QTableWidgetItem(QString::number(i));
                mTableItem->setBackgroundColor(getLayerColor(type));
                mTableWidget->setItem(i, j, mTableItem);
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
    int dial = mTableWidget->columnCount();

    // clear column
    for (int i = 0; i <= mTimeLineLength; i++)
    {
        mTableItem = new QTableWidgetItem("");
        mTableItem->setBackgroundColor(Qt::white);
        mTableWidget->setItem(i, dial - 1, mTableItem);
    }
    mTableItem = new QTableWidgetItem("DIAL");
    mTableItem->setBackgroundColor(QColor(244, 167, 167, 150));
    mTableWidget->setItem(0, mLayerNames->size() + 1, mTableItem);
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
    mTableItem = new QTableWidgetItem(lipsync.at(0));
    mTableItem->setBackgroundColor(QColor(250, 240, 160));
    mTableWidget->setItem(0, mTableWidget->columnCount() - 1, mTableItem);
    if (lipsync.size() > 2)
    {
        mTableItem = new QTableWidgetItem(lipsync.at(2));
        mTableItem->setBackgroundColor(QColor(250, 240, 160));
        QString tmp = lipsync.at(2);
        mTableWidget->setItem(tmp.toInt(), mTableWidget->columnCount() - 1, mTableItem);
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
        for (int j = 0; j < mTableWidget->columnCount(); j++)
        {
            tmp += mTableWidget->item(i, j)->text() + ",";
        }
        tmp.chop(1);
        tmp += '\n';
        out << tmp;
    }
    file.close();
}

void Xsheet::addFrame()
{
    if (mTableWidget->currentColumn() == 0 ||
        mTableWidget->currentColumn() == mTableWidget->columnCount() - 1 ||
        mTableWidget->currentRow() == 0) { return; }
    QString name = mTableWidget->item(0, mTableWidget->currentColumn())->text();
    int frame = mTableWidget->currentRow();
    if (!mEditor->layers()->findLayerByName(name)->keyExists(frame))
    {
        mTableItem = new QTableWidgetItem(QString::number(mTableWidget->currentRow()));
        mTableItem->setBackgroundColor(QColor(getLayerColor(getLayerType(mEditor->layers()->currentLayer()))));
        mTableWidget->setItem(mTableWidget->currentRow(), mTableWidget->currentColumn(), mTableItem);
        mEditor->layers()->currentLayer()->addNewKeyFrameAt(mTableWidget->currentRow());
        emit mEditor->layers()->notifyLayerChanged(mEditor->layers()->findLayerByName(name));
    }
}

void Xsheet::removeFrame()
{
    QString tmp = mTableWidget->item(mTableWidget->currentRow(), mTableWidget->currentColumn())->text();
    if (mTableWidget->currentColumn() == 0) { return; }
    mTableItem = new QTableWidgetItem("");
    mTableItem->setBackgroundColor(QColor(Qt::white));
    mTableWidget->setItem(mTableWidget->currentRow(), mTableWidget->currentColumn(), mTableItem);
    if (mTableWidget->currentColumn() == mTableWidget->columnCount() - 1)
    {   // if it is a lipsync column
        for (int i = 1; i < mPapaLines->size(); i++)
        {
            if (mPapaLines->at(i).startsWith(QString::number(mTableWidget->currentRow())))
            {
                mPapaLines->removeAt(i);
            }
        }
    }
    else
    {   // if it is a Bitmap or Vector layer
        if (tmp.isEmpty()) { return; }
        QString name = mTableWidget->item(0, mTableWidget->currentColumn())->text();
        int frame = mTableWidget->currentRow();
        if (mEditor->layers()->findLayerByName(name)->keyExists(frame))
        {
            mEditor->layers()->findLayerByName(name)->removeKeyFrame(frame);
            emit mEditor->layers()->notifyLayerChanged(mEditor->layers()->findLayerByName(name));
        }
    }
}

void Xsheet::initXsheet()
{
    mLayerCount = 0;
    mLayerNames->clear();
    for (int i = 0; i < mEditor->layers()->count(); i++)
    {   // count Bitmap and Vector layers (duplicate names NOT supported)
        bool visi = mEditor->layers()->getLayer(i)->getVisibility();
        if (visi && (mEditor->layers()->getLayer(i)->type() == 1 || mEditor->layers()->getLayer(i)->type() == 2))
        {
            if (!mLayerNames->contains(mEditor->layers()->getLayer(i)->name()))
            {
                mLayerCount++;
                mLayerNames->append(mEditor->layers()->getLayer(i)->name());
            }
            else
            {
                int ret = QMessageBox::critical(new QWidget,
                               tr("Layer name duplicate!"),
                               tr("Identical layer names '%1' not supported in Xsheet").arg(mEditor->layers()->getLayer(i)->name()),
                               QMessageBox::Ok);
                Q_UNUSED(ret);
            }
        }
    }
    for (int i = 1; i <= mLayerNames->size() + 1; i++)
    {
        for (int j = 1; j <= mTimeLineLength; j++)
        {
            mTableItem = new QTableWidgetItem("");
            mTableItem->setBackgroundColor(Qt::white);
            mTableWidget->setItem(j, i, mTableItem);
        }
    }
    this->setMinimumWidth(mLayerCount * 40 + 100);
    mTableWidget->setRowCount(mTimeLineLength + 1);
    mTableWidget->setColumnCount(mLayerCount + 2);
    // set column width for layers
    for (int i = 0; i < mTableWidget->columnCount(); i++)
    {
        mTableWidget->setColumnWidth(i, 40);
    }
    // set headers of Xsheet
    mTableWidget->setRowHeight(0,16);
    mTableItem = new QTableWidgetItem("#");
    mTableItem->setBackgroundColor(QColor(250, 240, 160));
    mTableWidget->setItem(0, 0, mTableItem);
    for (int i = 0; i < mLayerNames->size(); i++)
    {
        int type = getLayerType(mEditor->layers()->findLayerByName(mLayerNames->at(i)));
        mTableItem = new QTableWidgetItem(mLayerNames->at(i));
        mTableItem->setBackgroundColor(getLayerColor(type));
        mTableWidget->setItem(0, i + 1, mTableItem);
    }
//    if (!mPapaLines->isEmpty()) { return; }
    mTableItem = new QTableWidgetItem("DIAL");
    mTableItem->setBackgroundColor(QColor(244, 167, 167, 150));
    mTableWidget->setItem(0, mLayerNames->size() + 1, mTableItem);
}

void Xsheet::writePapa()
{
    if (!mPapaLines->isEmpty())
    {
        int dial = mTableWidget->columnCount();

        // clear column
        for (int i = 0; i < mTimeLineLength; i++)
        {
            mTableItem = new QTableWidgetItem("");
            mTableItem->setBackgroundColor(Qt::white);
            mTableWidget->setItem(i, dial - 1, mTableItem);
        }

        // write header
        QStringList lipsync;
        QString tmp;
        tmp = mPapaLines->at(0);
        lipsync = tmp.split(" ");
        mTableItem = new QTableWidgetItem(lipsync.at(0));
        mTableItem->setBackgroundColor(QColor(245, 155, 155, 150));
        mTableWidget->setItem(0, dial - 1, mTableItem);
        if (lipsync.size() > 2)
        {
            tmp = lipsync.at(2);
            int row = tmp.toInt();
            mTableWidget->setItem(row, dial - 1, new QTableWidgetItem("-"));
        }
        for (int i = 1; i < mPapaLines->size(); i++)
        {
            tmp = mPapaLines->at(i);
            lipsync = tmp.split(" ");
            if (lipsync.size() == 2)
            {
                tmp = lipsync.at(0);    // frame number
                int row = tmp.toInt();
                mTableItem = new QTableWidgetItem(lipsync.at(1)); // audio to animate
                mTableItem->setBackgroundColor(QColor(245, 155, 155, 150));
                mTableWidget->setItem(row, dial - 1, mTableItem);
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
        mTableItem = new QTableWidgetItem();
        mTableItem = mTableWidget->item(0, column);
        Layer* layer = mEditor->layers()->findLayerByName(mTableItem->text());
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
