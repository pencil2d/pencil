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

#ifndef DOUBLEPROGRESSDIALOG_H
#define DOUBLEPROGRESSDIALOG_H

#include <QDialog>
#include <QProgressBar>

namespace Ui {
class DoubleProgressDialog;
}

class DoubleProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DoubleProgressDialog(QWidget *parent = nullptr);
    ~DoubleProgressDialog();

    QString getStatus();
    void setStatus(QString msg);

    class ProgressBarControl {
        public:
            ProgressBarControl(QProgressBar *b);

            float getMin() { return min; }
            void setMin(float minimum);

            float getMax() { return max; }
            void setMax(float maximum);

            void setRange(float minimum, float maximum) { setMin(minimum); setMax(maximum); }

            float getValue() { return val; }
            void setValue(float value);

            int getPrecision();
            void setPrecision(int e);
        private:
            QProgressBar *bar;
            float min = 0, max = 1, val = 0;

            int convertUnits(float value);
            int unitFactor = 100;
    };

    ProgressBarControl *major, *minor;

signals:
    void canceled();

private:
    Ui::DoubleProgressDialog *ui;
};

#endif // DOUBLEPROGRESSDIALOG_H
