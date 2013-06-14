/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef _PREFERENCES_H_
#define _PREFERENCES_H_

#include <QtGui>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;

class Preferences : public QDialog
{
    Q_OBJECT

public:
    Preferences(QWidget* parent);

public slots:
    void changePage(QListWidgetItem* current, QListWidgetItem* previous);

signals:
    void windowOpacityChange(int);
    void curveOpacityChange(int);
    void curveSmoothingChange(int);
    void highResPositionChange(int);
    void antialiasingChange(int);
    void gradientsChange(int);
    void backgroundChange(int);
    void shadowsChange(int);
    void toolCursorsChange(int);
    void styleChange(int);

    void autosaveChange(int);
    void autosaveNumberChange(int);

    void lengthSizeChange(QString);
    void fontSizeChange(int);
    void frameSizeChange(int);
    void labelChange(int);
    void scrubChange(int);

    void onionLayer1OpacityChange(int);
    void onionLayer2OpacityChange(int);
    void onionLayer3OpacityChange(int);

private:
    void createIcons();

    QListWidget* contentsWidget;
    QStackedWidget* pagesWidget;
};


class GeneralPage : public QWidget
{
    Q_OBJECT

public:
    GeneralPage(QWidget* parent = 0);

};



class TimelinePage : public QWidget
{
    Q_OBJECT

public:
    TimelinePage(QWidget* parent = 0);

};


class FilesPage : public QWidget
{
    Q_OBJECT

public:
    FilesPage(QWidget* parent = 0);

};


class ToolsPage : public QWidget
{
    Q_OBJECT

public:
    ToolsPage(QWidget* parent = 0);
};

#endif