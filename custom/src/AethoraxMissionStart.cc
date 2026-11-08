#include "AethoraxMissionStart.h"
#include "AethoraxConfig.h"

#include "MissionController.h"
#include "MissionSettingsItem.h"
#include "PlanMasterController.h"
#include "QmlObjectListModel.h"

#include <QtCore/QJsonValue>

namespace {

constexpr const char *kStatusProperty = "aethoraxInitialStatus";  // on the MissionSettingsItem
constexpr const char *kPlanKey = "aethorax";
constexpr const char *kInitialStatusKey = "initialStatus";
constexpr int kPlanSectionVersion = 1;

MissionSettingsItem *settingsItem(PlanMasterController *plan)
{
    QmlObjectListModel *items = plan ? plan->missionController()->visualItems() : nullptr;
    return (items && items->count() > 0) ? items->value<MissionSettingsItem *>(0) : nullptr;
}

}  // namespace

QVariantMap AethoraxMissionStart::initialStatus(QObject *settingsItem)
{
    const QVariantMap own = settingsItem ? settingsItem->property(kStatusProperty).toMap() : QVariantMap();
    QVariantMap status;
    for (const QVariant &entry : AethoraxConfig::section(QStringLiteral("missionStart")).value(QStringLiteral("devices")).toList()) {
        const QVariantMap device = entry.toMap();
        const QString key = device.value(QStringLiteral("key")).toString();
        if (!key.isEmpty()) {
            status.insert(key, own.value(key, device.value(QStringLiteral("default"), true)).toBool());
        }
    }
    return status;
}

void AethoraxMissionStart::setInitialStatus(QObject *settingsItem, const QString &device, bool on)
{
    auto *item = qobject_cast<VisualMissionItem *>(settingsItem);
    if (!item) {
        return;
    }
    QVariantMap own = item->property(kStatusProperty).toMap();
    own.insert(device, on);
    item->setProperty(kStatusProperty, own);
    item->setDirty(true);
}

void AethoraxMissionStart::save(PlanMasterController *plan, QJsonObject &planJson)
{
    QJsonObject section;
    section.insert(QStringLiteral("version"), kPlanSectionVersion);
    section.insert(QString::fromLatin1(kInitialStatusKey), QJsonObject::fromVariantMap(initialStatus(settingsItem(plan))));
    planJson.insert(QString::fromLatin1(kPlanKey), section);
}

void AethoraxMissionStart::load(PlanMasterController *plan, const QJsonObject &planJson)
{
    MissionSettingsItem *item = settingsItem(plan);
    if (!item) {
        return;
    }
    // A plan without the section (an older one, or stock QGroundControl's) takes the defaults
    const QJsonObject section = planJson.value(QString::fromLatin1(kPlanKey)).toObject();
    item->setProperty(kStatusProperty, section.value(QString::fromLatin1(kInitialStatusKey)).toObject().toVariantMap());
}
