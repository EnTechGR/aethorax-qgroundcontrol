#pragma once

#include "QGCMAVLink.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>

/// AETHORAX's runtime configuration: JSON files read once at startup, so what the
/// planner offers can change without rebuilding the application.
///
/// Each file is looked for by name, in order:
///   1. the directory in the environment variable AETHORAX_QGC_CONFIG
///   2. <Documents>/<application name>/Config
///   3. the copy built into the application (custom/res/config/)
/// A file that is found but invalid is reported, and the next one in the order is used.
///
///   aethorax.json          the manifest: which files to use and how (fileType "AethoraxConfig")
///   mission_commands.json  every mission command, QGroundControl's and AETHORAX's
///                          operations (fileType "MavCmdInfo"); the name is set in the manifest
namespace AethoraxConfig
{
    /// The directories searched, in order (the built-in one last).
    QStringList searchDirectories();

    /// The mission command list to load as QGroundControl's base list.
    QString missionCommandsFile();

    /// The mission item dropdown: the firmware's own commands (when the manifest keeps
    /// them), plus the manifest's "add", minus its "remove".
    QList<MAV_CMD> missionDropdown(const QList<MAV_CMD> &firmwareCommands);

    /// The manifest's dropdown.categoriesFirst: the categories the command picker lists first, in order.
    QStringList categoriesFirst();

    /// A section of the manifest by its dotted path (e.g. "planView.rightPanel"), for the QML
    /// that reads it; empty when the manifest has none, and the QML then uses its own defaults.
    QVariantMap section(const QString &path);

    /// A command's number from its rawName in the commands file (e.g. "MAV_CMD_USER_2"); 0 if none.
    int commandNumber(const QString &rawName);
}
