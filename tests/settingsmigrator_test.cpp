#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QTemporaryDir>

#include "application/settingsmigrator.h"

namespace
{
bool migratesKnownVersions()
{
    QTemporaryDir directory;
    QSettings settings(directory.filePath("profile.ini"), QSettings::IniFormat);
    settings.setValue("Common/ConfigVersion", 4);

    const auto action = SettingsMigrator::prepare(settings);
    const bool passed =
        action == SettingsMigrationAction::None
        && settings.value("SHOUConnect/Protocol").toString() == "easyconnect";
    if (!passed)
    {
        qCritical() << "migratesKnownVersions failed";
    }
    return passed;
}

bool migratesLegacySettingsNamespace()
{
    QTemporaryDir directory;
    QSettings settings(directory.filePath("profile.ini"), QSettings::IniFormat);
    settings.setValue("Common/ConfigVersion", 8);
    settings.setValue("ZJUConnect/ServerAddress", "legacy.example.edu");

    const auto action = SettingsMigrator::prepare(settings);
    const bool passed =
        action == SettingsMigrationAction::None
        && settings.value("SHOUConnect/ServerAddress").toString() == "legacy.example.edu";
    if (!passed)
    {
        qCritical() << "migratesLegacySettingsNamespace failed";
    }
    return passed;
}

bool recommendsResetForUnsupportedVersions()
{
    QTemporaryDir directory;
    QSettings settings(directory.filePath("profile.ini"), QSettings::IniFormat);
    settings.setValue("Common/ConfigVersion", 5);
    const bool passed =
        SettingsMigrator::prepare(settings) ==
        SettingsMigrationAction::RecommendReset;
    if (!passed)
    {
        qCritical() << "recommendsResetForUnsupportedVersions failed";
    }
    return passed;
}
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    return migratesKnownVersions() && migratesLegacySettingsNamespace() && recommendsResetForUnsupportedVersions() ? 0 : 1;
}
