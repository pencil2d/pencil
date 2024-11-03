#ifndef THEMING_H
#define THEMING_H

class QDir;
class QStyle;
class QPalette;

class Status;

class Theming
{
public:
    static QStringList availableStyles();
    static QStringList availablePalettes();

    static QStyle* getStyle(const QString& key);
    static QPalette* getPalette(const QString& key);

    static Status addPalette(const QString& filePath);
    static Status removePalette(const QString& key);
private:
    static QPalette* loadPaletteConf(const QString& filename);

    static const QDir userPaletteDir();
};

#endif // THEMING_H
