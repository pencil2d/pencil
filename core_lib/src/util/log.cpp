
#include "log.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(logCanvasPainter, "core.canvasPainter");
Q_LOGGING_CATEGORY(logFileManager, "core.FileManager");

void initCategoryLogging()
{
    const QString logRules =
        "*.debug=false\n"
        "core.canvasPainter.debug=false"
        "core.fileManager.debug=false";

    QLoggingCategory::setFilterRules(logRules);
}