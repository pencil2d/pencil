#ifndef THEMING_H
#define THEMING_H

class QStyle;
class QPalette;

class Theming
{
public:
    static QStringList availableStyles();
    static QStringList availablePalettes();

    static QStyle* getStyle(const QString& key);
    static QPalette* getPalette(const QString& key);

private:
    static QPalette* loadPaletteConf(const QString& filename);
};

#endif // THEMING_H
