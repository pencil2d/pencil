/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2015 Matt Chiawen Chang

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

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QGroupBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>

#include "pencildef.h"
#include "scribblearea.h"
#include "shortcutspage.h"
#include "preferencemanager.h"

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QComboBox;
class PreferenceManager;


class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog(QWidget* parent);
    ~PreferencesDialog();

    void init( PreferenceManager* m );
    
    void changePage(QListWidgetItem* current, QListWidgetItem* previous);

Q_SIGNALS:
    void windowOpacityChange(int);
    void curveOpacityChange(int);

protected:
    void closeEvent( QCloseEvent* ) override;

private:
    void createIcons();
    void makeConnections();

    QListWidget* contentsWidget = nullptr;
    QStackedWidget* pagesWidget = nullptr;

    PreferenceManager* mPrefManager = nullptr;
};


class GeneralPage : public QWidget
{
    Q_OBJECT
public:
    GeneralPage(QWidget* parent = 0);
    void setManager( PreferenceManager* p ) { mManager = p; }


public slots:
    void updateValues();

private:
    void languageChanged( int i );
    void shadowsCheckboxStateChanged(bool b);
    void antiAliasCheckboxStateChanged( bool b );
    void toolCursorsCheckboxStateChanged( bool b );
    void dottedCursorCheckboxStateChanged( bool b );
    void highResCheckboxStateChanged(bool b);
    void curveSmoothingChange(int value);
    void backgroundChange(int value);

    PreferenceManager* mManager = nullptr;

    QComboBox* mLanguageCombo = nullptr;
    QSlider* mWindowOpacityLevel;
    QSlider* mCurveSmoothingLevel;
    QCheckBox* mShadowsBox;
    QCheckBox* mToolCursorsBox;
    QCheckBox* mAntialiasingBox;
    QCheckBox* mHighResBox;
    QButtonGroup *mBackgroundButtons;
    QCheckBox* mDottedCursorBox;

};

class GridPage : public QWidget
{
    Q_OBJECT
public:
    GridPage(QWidget* parent = 0);
    void setManager( PreferenceManager* p ) { mManager = p; }


public slots:
    void updateValues();
    void gridSizeChange(int value);

private:

    PreferenceManager* mManager = nullptr;

    QSpinBox* mGridSizeInput;

};


class TimelinePage : public QWidget
{
    Q_OBJECT
public:
    TimelinePage(QWidget* parent = 0);
    void setManager( PreferenceManager* p ) { mManager = p; }

public slots:
    void updateValues();

    void lengthSizeChange(QString);
    void fontSizeChange(int);
    void frameSizeChange(int);
    void labelChange(bool);
    void scrubChange(bool);

private:
    PreferenceManager* mManager = nullptr;
    QCheckBox* mDrawLabel;
    QSpinBox* mFontSize;
    QSlider* mFrameSize;
    QLineEdit* mLengthSize;
    QCheckBox* mScrubBox;
};

class FilesPage : public QWidget
{
    Q_OBJECT

public:
    FilesPage(QWidget* parent = 0);
    void setManager( PreferenceManager* p ) { mManager = p; }

public slots:
    void updateValues();
    void autosaveChange(bool b);
    void autosaveNumberChange(int number);

private:
    PreferenceManager* mManager = nullptr;
    QCheckBox* mAutosaveCheckBox;
    QSpinBox* mAutosaveNumberBox;

};


class ToolsPage : public QWidget
{
    Q_OBJECT
public:
    ToolsPage(QWidget* parent = 0);
    void setManager( PreferenceManager* p ) { mManager = p; }

public slots:
    void updateValues();
    void onionMaxOpacityChange(int);
    void onionMinOpacityChange(int);
    void onionPrevFramesNumChange(int);
    void onionNextFramesNumChange(int);
    void quickSizingChange(bool);
private:
    PreferenceManager* mManager = nullptr;
    QSpinBox* mOnionMaxOpacityBox;
    QSpinBox* mOnionMinOpacityBox;
    QSpinBox* mOnionPrevFramesNumBox;
    QSpinBox* mOnionNextFramesNumBox;
    QCheckBox * mUseQuickSizingBox;
};

#endif
