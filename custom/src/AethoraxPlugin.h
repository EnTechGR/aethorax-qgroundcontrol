#pragma once

#include "QGCCorePlugin.h"

#include <QtQml/QQmlAbstractUrlInterceptor>

/// Loads qrc:/Custom/<path> instead of qrc:/<path> when custom.qrc provides one: how
/// AETHORAX replaces an upstream QML file without changing it (custom/qml/).
class AethoraxQmlOverrides : public QQmlAbstractUrlInterceptor
{
public:
    QUrl intercept(const QUrl &url, QQmlAbstractUrlInterceptor::DataType type) final;
};

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

    QQmlApplicationEngine *createQmlApplicationEngine(QObject *parent) final;
    void destroyQmlApplicationEngine(QQmlApplicationEngine *qmlEngine) final;

    /// For the command picker (qml/MissionCommandDialog.qml): aethorax.json's categoriesFirst.
    Q_INVOKABLE QStringList categoriesFirst() const;

private:
    QQmlApplicationEngine *_qmlEngine = nullptr;
    AethoraxQmlOverrides *_qmlOverrides = nullptr;
};
