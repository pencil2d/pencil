#include "presetdialog.h"
#include "ui_presetdialog.h"

#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>


PresetDialog::PresetDialog(PreferenceManager* preferences, QWidget* parent) :
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
    int index = getPresetIndex();
    return PresetDialog::getPresetPath(index);
}

int PresetDialog::getPresetIndex()
{
    bool ok = true;
    int index = ui->presetComboBox->currentData().toInt(&ok);
    Q_ASSERT(ok);
    return index;
}

bool PresetDialog::shouldAlwaysUse()
{
    return ui->alwaysUse->isChecked();
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

int PresetDialog::exec()
{
    if (ui->presetComboBox->count() == 1)
    {
        return QDialog::Accepted;
    }
    return QDialog::exec();
}

void PresetDialog::initPresets()
{
    // Make sure the presets directory in the data directory exists and navigate to it
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dataDir(dataPath);
    dataDir.mkdir("presets");
    if (dataDir.cd("presets") == false)
    {
        accept(); // the presets folder doesn't exist and cannot be created
    }

    // Find all presets in the preferences and add them to the combo box
    int defaultIndex = mPrefs->getInt(SETTING::DEFAULT_PRESET);
    ui->presetComboBox->addItem("Default", 0);
    ui->presetComboBox->setCurrentIndex(0);

    if (!dataDir.exists("presets.ini")) accept();
    QSettings presets(dataDir.filePath("presets.ini"), QSettings::IniFormat, this);
    
    bool ok = true;
    for (const QString key : presets.allKeys())
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
