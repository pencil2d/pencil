/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#pragma once
#include<QLoggingCategory>


/**
 * With Qt Category logging, it allows devs to turn on/off the debug log for certain modules
 * Uncomment the following #define DEBUG_LOG_ModuleName to enable the category logging
 */

//#define DEBUG_LOG_CANVASPAINTER
//#define DEBUG_LOG_FILEMANAGER

#ifdef DEBUG_LOG_CANVASPAINTER
  Q_DECLARE_LOGGING_CATEGORY(logCanvasPainter);
  #define CANVASPAINTER_LOG(...) qCDebug(logCanvasPainter, __VA_ARGS__)
#else
  #define CANVASPAINTER_LOG(...) (0)
#endif

#ifdef DEBUG_LOG_FILEMANAGER
  Q_DECLARE_LOGGING_CATEGORY(logFileManager);
  #define FILEMANAGER_LOG(...) qCDebug(logFileManager, __VA_ARGS__)
#else
  #define FILEMANAGER_LOG(...) (0)
#endif

void initCategoryLogging();
