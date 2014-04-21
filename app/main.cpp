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
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDir>
#include <QIcon>
#include "editor.h"
#include "mainwindow2.h"

void initialise();


int main(int argc, char* argv[])
{   
    QApplication app(argc, argv);
    app.setApplicationName("Pencil");

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("pencil2d_" + QLocale::system().name());
    app.installTranslator(&myappTranslator);

#ifndef Q_WS_MAC
    app.setWindowIcon(QIcon(":/icons/icon.png"));
#endif

    initialise();

    MainWindow2 mainWindow;
    mainWindow.setWindowTitle( "Pencil2D" );

    if (argc == 1)
    {
        mainWindow.show();
        return app.exec();
    }
    else
    {
        QString inputFile = "";

        bool jobExportSequence = false;
        QString jobExportSequenceOutput = "";

        // Extracting options
        int i;
        for (i = 1; i < argc; i++)
        {
            if (jobExportSequence && jobExportSequenceOutput == "")
            {
                jobExportSequenceOutput = argv[i];
                continue;
            }
            if (QString(argv[i]) == QString("--export-sequence"))
            {
                jobExportSequence = true;
                continue;
            }
            if (inputFile == "")
            {
                inputFile = QString(argv[i]);
            }
        }

        bool error = false;
        if ( jobExportSequence )
        {
            qDebug() << "Exporting image sequence...";
            if (inputFile.isEmpty())
            {
                qDebug() << "Error: No input file specified.";
                error = true;
            }
            // TODO: Check if input file exists
            if ( jobExportSequenceOutput.isEmpty())
            {
                qDebug() << "Error: No output file specified.";
                error = true;
            }
            // TODO: Check if output path exists

            if ( !error )
            {
                mainWindow.openObject(inputFile);
                // Detecting format
                QString format = "";
                if (jobExportSequenceOutput.endsWith(".png"))
                {
                    format = "PNG";
                }
                else if (jobExportSequenceOutput.endsWith(".jpg"))
                {
                    format = "JPG";
                }
                else if (jobExportSequenceOutput.endsWith(".tif"))
                {
                    format = "TIF";
                }
                else if (jobExportSequenceOutput.endsWith(".bmp"))
                {
                    format = "BMP";
                }
                else
                {
                    qDebug() << "Warning: Output format is not specified or unsupported.";
                    qDebug() << "         Using PNG.";
                    format = "PNG";
                }
                mainWindow.m_pCore->exportSeqCLI(jobExportSequenceOutput, format);
                qDebug() << "Done.";
            }
        }
        else if ( inputFile != "" )
        {
            mainWindow.show();
            mainWindow.openObject(inputFile);
            return app.exec();
        }
        else
        {
            qDebug() << "Error: Invalid commandline options.";
            error = true;
        }

        if (error)
        {
            qDebug() << "Syntax:";
            qDebug() << "   " << argv[0] << "FILENAME --export-sequence PATH";
            qDebug() << "Example:";
            qDebug() << "   " << argv[0] << "/path/to/your/file.pcl --export-sequence /path/to/export/file.png";
            return 1;
        }
        else
        {
            return 0;
        }
    }
}
