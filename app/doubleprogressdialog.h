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
    explicit DoubleProgressDialog(QWidget *parent = 0);
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
