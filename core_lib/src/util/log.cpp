
#include "log.h"

Q_LOGGING_CATEGORY(logCanvasPainter, "core.canvasPainter");
Q_LOGGING_CATEGORY(logFileManager, "core.FileManager");

void initCategoryLogging()
{
    const QString logRules =
        "*.debug=false\n"
        "default.debug=true\n"
        "core.canvasPainter.debug=false\n"
        "core.fileManager.debug=false";

    QLoggingCategory::setFilterRules(logRules);
}