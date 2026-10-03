#include "settingsmigrator.h"

#include <QSettings>

#include "application/applicationconstants.h"
#include "application/defaultsettings.h"

SettingsMigrationAction SettingsMigrator::prepare(QSettings &settings)
{
    const int configVersion = settings.value("Common/ConfigVersion", -1).toInt();
    if (configVersion == -1)
    {
        DefaultSettings::reset(settings);
        return SettingsMigrationAction::None;
    }
    // Rename the application settings group while preserving existing profiles.
    // Copy only when the new key is absent so an explicitly migrated value wins.
    if (configVersion < ApplicationConstants::ConfigVersion)
    {
        const QString legacyPrefix = "ZJUConnect/";
        const QString currentPrefix = "SHOUConnect/";
        for (const QString &key : settings.allKeys())
        {
            if (key.startsWith(legacyPrefix))
            {
                const QString migratedKey = currentPrefix + key.mid(legacyPrefix.size());
                if (!settings.contains(migratedKey))
                {
                    settings.setValue(migratedKey, settings.value(key));
                }
            }
        }
    }
    if (configVersion == 4)
    {
        settings.setValue("SHOUConnect/Protocol", "easyconnect");
        return SettingsMigrationAction::None;
    }
    if (configVersion == 6)
    {
        return SettingsMigrationAction::MigrateAutoStart;
    }
    if (configVersion == 8)
    {
        return SettingsMigrationAction::None;
    }
    if (configVersion < ApplicationConstants::ConfigVersion)
    {
        return SettingsMigrationAction::RecommendReset;
    }
    return SettingsMigrationAction::None;
}

void SettingsMigrator::finish(QSettings &settings, bool resetToDefaults)
{
    if (resetToDefaults)
    {
        settings.clear();
        DefaultSettings::reset(settings);
    }
    settings.setValue(
        "Common/ConfigVersion",
        ApplicationConstants::ConfigVersion
    );
    settings.sync();
}
