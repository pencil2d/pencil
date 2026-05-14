#ifndef THEMING_H
#define THEMING_H

#include <QPalette>

class QDir;
class QStyle;

class Status;
class ThemeColorPalette;

class Theming
{
public:
    static QStringList availableStyles();
    static QList<ThemeColorPalette> availablePalettes();

    static QStyle* getStyle(const QString& key);
    static ThemeColorPalette getPalette(const QString& key);

    static std::pair<Status, ThemeColorPalette> addPalette(const QString& filePath);
    static Status removePalette(const QString& key);

private:
    static QPalette* loadPaletteConf(const QString& filename);

    static const QDir userPaletteDir();
};

class ThemeColorPalette
{
public:
    ThemeColorPalette() {}
    ThemeColorPalette(const QString& filePath);

    bool loadFromFile(const QString& filePath);
    void setInvalidWithId(const QString& id);

    bool isValid() const { return m_valid; }
    QString id() const;
    QString displayName() const { return m_displayName; }
    QPalette palette() const { return m_valid ? m_palette : QPalette(); }
    bool isDark() const;
    bool isBuiltIn() const { return m_filePath.startsWith(':'); }

private:
    enum Mode {
        Light,
        Dark,
        Unknown
    };

    bool tryLoad(const QString& filePath);

    bool m_valid = false;
    QString m_filePath;
    QString m_displayName;
    QPalette m_palette;
    Mode m_mode;
};

#endif // THEMING_H
