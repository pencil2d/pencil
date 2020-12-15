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

#ifndef PENCIL2D_H
#define PENCIL2D_H

#include <QApplication>

#include "pencilerror.h"

/**
 * A pointer to the unique @ref Pencil2D instance.
 */
#define pencil2D \
    (static_cast<Pencil2D *>(QCoreApplication::instance()))

/**
 * The main application class handling startup as well as the main loop.
 */
class Pencil2D : public QApplication
{
    Q_OBJECT

public:
    /**
     * Initializes the application with the given command line arguments.
     *
     * @param argc Number of arguments
     * @param argv Values of the arguments
     */
    explicit Pencil2D(int &argc, char **argv);

    /**
     * Parses supplied command line arguments and performs the appropriate actions,
     * such as running the command line exporter or preparing the GUI.
     * This method should be called before entering the main loop.
     *
     * @return @li Status::OK if the arguments have been handled and the application is ready to enter the main loop
     *         @li Status::SAFE if all necessary actions have been performed and the application is ready for shutdown
     *         @li any other code if an error occurred and the application is unable to start.
     *  @see exec()
     */
    Status handleCommandLineOptions();

    bool event(QEvent* event) override;

signals:
    /**
     * Emitted when the operating system requests that a file should be opened.
     *
     * @param filename The file to be opened
     * @see QFileOpenEvent
     */
    void openFileRequested(QString filename);

    /** Emitted when app focus changes */
    void focusStateChange();

private:
    /**
     * Sets up translators for the application locale configured by the user or the system locale.
     */
    void installTranslators();
    /**
     * Readies the graphical UI for entering the main loop.
     *
     * @param inputPath Path of a file to be opened on startup.
     */
    void prepareGuiStartup(const QString &inputPath);
};

#endif // PENCIL2D_H
