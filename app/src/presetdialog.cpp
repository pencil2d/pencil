#include "PresetDialog.h"
#include "ui_PresetDialog.h"

#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>

PresetDialog::PresetDialog(PreferenceManager *preferences, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PresetDialog),
    mPrefs(preferences)
{
    ui->setupUi(this);

    initPresets();

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

PresetDialog::~PresetDialog()
{
    delete ui;
}

QString PresetDialog::getPreset()
{
    bool ok = true;
    int index = ui->presetComboBox->currentData().toInt(&ok);
    Q_ASSERT(ok);
    return PresetDialog::getPresetPath(index);
}

QString PresetDialog::getPresetPath(int index)
{
    QString filename = QString("%1.pclx").arg(index);
    QDir dataDir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (dataDir.cd("presets"))
    {
        return dataDir.filePath(filename);
    }
    return QString();
}

void PresetDialog::initPresets()
{
    // Make sure the standard data directory for this app exists and navigate to it
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    QDir dataDir = QDir(dataPath);
    if (!dataDir.exists()) accept();

    // Make sure the presets directory in the data directory exists and navigate to it
    if (!dataDir.exists("presets")) dataDir.mkdir("presets");
    dataDir.cd("presets");
    if (!dataDir.exists()) accept();

    // Find all presets in the preferences and add them to the combo box
    int defaultIndex = mPrefs->getInt(SETTING::DEFAULT_PRESET);
    ui->presetComboBox->addItem("Default", 0);
    ui->presetComboBox->setCurrentIndex(0);
    if (!dataDir.exists("presets.ini")) accept();
    QSettings presets(dataDir.filePath("presets.ini"), QSettings::IniFormat, this);
    bool ok = true;
    foreach(const QString key, presets.allKeys())
    {
        int index = key.toInt(&ok);
        if (!ok || index == 0 || !dataDir.exists(QString("%1.pclx").arg(index))) continue;
        QString name = presets.value(key, QString()).toString();
        if (name.isEmpty()) continue;
        ui->presetComboBox->addItem(name, index);
        if (index == defaultIndex)
        {
            ui->presetComboBox->setCurrentIndex(ui->presetComboBox->count()-1);
        }
    }

    ui->presetComboBox->model()->sort(0);
}
