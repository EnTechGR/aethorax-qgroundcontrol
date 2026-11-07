#pragma once

#include "QGCCorePlugin.h"

/// AETHORAX's core plugin: planning defaults for one vehicle, Nemo1 (ArduPilot Sub).
class AethoraxPlugin : public QGCCorePlugin
{
    Q_OBJECT

public:
    explicit AethoraxPlugin(QObject *parent = nullptr);

    static QGCCorePlugin *instance();

    /// Plans made without a vehicle connected are for ArduPilot Sub; the choice is not offered.
    void adjustSettingMetaData(const QString &settingsGroup, FactMetaData &metaData, bool &userVisible) final;

    QString stableDownloadLocation() const final { return QStringLiteral("github.com/EnTechGR/AETHORAX/releases"); }
};
