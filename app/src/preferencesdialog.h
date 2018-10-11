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
#ifndef _PREFERENCES_H_
#define _PREFERENCES_H_

#include <QDialog>

#include "pencildef.h"
#include "scribblearea.h"
#include "preferencemanager.h"

class QListWidgetItem;
class PreferenceManager;

namespace Ui {
class PreferencesDialog;
class GeneralPage;
class TimelinePage;
class FilesPage;
class ToolsPage;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog(QWidget* parent);
    ~PreferencesDialog();

    void init(PreferenceManager* m);
    void updateRecentListBtn(bool isEmpty);

public slots:
    void changePage(QListWidgetItem* current, QListWidgetItem* previous);

Q_SIGNALS:
    void windowOpacityChange(int);
    void curveOpacityChange(int);
    void clearRecentList();
    void updateRecentFileListBtn();

protected:
    void closeEvent(QCloseEvent*) override;

private:
    Ui::PreferencesDialog* ui = nullptr;

    PreferenceManager* mPrefManager = nullptr;
};


class GeneralPage : public QWidget
{
    Q_OBJECT
public:
    GeneralPage();
    ~GeneralPage();
    void setManager(PreferenceManager* p) { mManager = p; }

public slots:
    void updateValues();
    void gridWidthChanged(int value);
    void gridHeightChanged(int value);

signals:
    void windowOpacityChange(int value);

private slots:
    void languageChanged(int i);
    void shadowsCheckboxStateChanged(int b);
    void antiAliasCheckboxStateChanged(int b);
    void toolCursorsCheckboxStateChanged(int b);
    void dottedCursorCheckboxStateChanged(int b);
    void highResCheckboxStateChanged(int b);
    void gridCheckBoxStateChanged(int b);
    void curveSmoothingChanged(int value);
    void backgroundChanged(int value);
    void frameCacheNumberChanged(int value);

private:
    Ui::GeneralPage* ui = nullptr;
    PreferenceManager* mManager = nullptr;
};

class TimelinePage : public QWidget
{
    Q_OBJECT
public:
    TimelinePage();
    ~TimelinePage();

    void setManager(PreferenceManager* p) { mManager = p; }

public slots:
    void updateValues();

    void timelineLengthChanged(int);
    void fontSizeChanged(int);
    void frameSizeChanged(int);
    void scrubChanged(int);
    void playbackStateChanged(int);
    void drawEmptyKeyRadioButtonToggled(bool);

private:
    Ui::TimelinePage* ui = nullptr;
    PreferenceManager* mManager = nullptr;
};

class FilesPage : public QWidget
{
    Q_OBJECT

public:
    FilesPage();
    ~FilesPage();
    void setManager(PreferenceManager* p) { mManager = p; }

public slots:
    void updateValues();
    void autosaveChange(int b);
    void autosaveNumberChange(int number);

Q_SIGNALS:
    void clearRecentList();

private:
    Ui::FilesPage* ui = nullptr;
    PreferenceManager* mManager = nullptr;
};


class ToolsPage : public QWidget
{
    Q_OBJECT
public:
    ToolsPage();
    ~ToolsPage();
    void setManager(PreferenceManager* p) { mManager = p; }

public slots:
    void updateValues();
    void onionMaxOpacityChange(int);
    void onionMinOpacityChange(int);
    void onionPrevFramesNumChange(int);
    void onionNextFramesNumChange(int);
    void quickSizingChange(int);
private:
    Ui::ToolsPage* ui = nullptr;
    PreferenceManager* mManager = nullptr;
};

#endif
