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
#ifndef UTIL_H
#define UTIL_H

#include <cstddef>
#include <functional>

class QAbstractSpinBox;

/**
 * Clips a given line to a clipping window using the Liang-Barsky algorithm.
 * @see https://www2.eecs.berkeley.edu/Pubs/TechRpts/1992/6271.html
 *
 * @param line The line to be clipped
 * @param clip The clipping window to use
 * @param t0 The starting point of the line to check, as a percentage
 * @param t0 The ending point of the line to check, as a percentage
 * @return The clipped line, or a null line if the line is completely outside the clipping window
 */
QLineF clipLine(const QLineF& line, const QRect& clip, qreal t0, qreal t1);

void clearFocusOnFinished(QAbstractSpinBox *spinBox);

// NOTE: Replace this implementation with QScopeGuard once we drop support for Qt < 5.12
class ScopeGuard
{
public:
    explicit ScopeGuard(std::function< void() > onScopeExit) { m_onScopeExit = onScopeExit; }
    ScopeGuard(const ScopeGuard&) = delete;
    ~ScopeGuard() { if(m_invoke) { m_onScopeExit(); } }

    void dismiss() { m_invoke = false; };
private:
    std::function<void()> m_onScopeExit;
    bool m_invoke = true;
};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)

#define OnScopeExit( callback ) ScopeGuard SCOPEGUARD_LINENAME( myScopeGuard, __LINE__ ) ( [&] { callback; } );

template <typename Container, typename Pred>
Container filter(const Container& container, Pred predicate) {
    Container result;
    std::copy_if(container.begin(), container.end(), std::back_inserter(result), predicate);
    return result;
}

QString ffprobeLocation();
QString ffmpegLocation();

quint64 imageSize(const QImage&);
QString uniqueString(int len);

/**
 * Performs safety checks for paths to data directory assets.
 *
 * Validates that the given file path is contained within the given
 * data directory after resolving symlinks. Also requires paths to
 * be relative to prevent project portability issues or intentional
 * platform-dependent behavior.
 *
 * This function does not verify if the path actually exists.
 *
 * This function should be called for every file being read from the data directory.
 * For writing files to the data directory, it is only necessary to call this
 * function if:
 * - An existing file is being modified/appended in-place (not overwritten) in the data directory.
 * - The data directory is not guaranteed to be the immediate parent directory of the file being written.
 *
 * @param filePath A path to a data file.
 * @param dataDir The path to the data directory.
 * @return The valid resolved path, or empty if the path is not valid.
 */
QString validateDataPath(QString filePath, QString dataDirPath);

#endif // UTIL_H
