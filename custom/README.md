# AETHORAX QGroundControl

QGroundControl as AETHORAX plans missions for Nemo1. It's upstream QGroundControl
(this repository, branch `aethorax`, based on upstream tag `v5.1.5`) plus this
`custom/` folder, built with QGroundControl's own
[custom-build mechanism](../custom-example/README.md). The folder is found
automatically when it exists: no build flags are needed.

## What it changes

| | Stock QGroundControl | AETHORAX QGroundControl |
|---|---|---|
| Firmware and vehicle | every PX4 and ArduPilot vehicle | ArduPilot Sub only. Planning without a vehicle connected is for ArduPilot Sub, and the choice isn't offered |
| A mission item's dropdown | every command ArduPilot Sub supports | the same, plus **AETHORAX's operations**: **Dive** (Depth, Speed). Set in `aethorax.json`, read at startup |
| An operation's fields | MAVLink's generic parameters | only the fields the operation has, with units, defaults and limits from the AETHORAX config |
| Where the commands come from | built into the application | JSON files read at startup, so they change **without a rebuild** (below) |

Each operation is a MAVLink user command:

| Operation | Command | Fields |
|---|---|---|
| Dive | `MAV_CMD_USER_1` (31010) | `param1` Depth (m, ≥ 0), `param2` Speed (m/s, between the descent table's first and last speeds; default: the descent profile's) |

ArduSub doesn't store user commands, so a plan with operations goes to the Jetson
**as a file** (*Save* / *Save As*), through the operator console, not through
*Upload*. The Jetson's plan tool maps each command number to its operation
(`software/jetson/autonomy/src/plan/` in AETHORAX).

## Files

```
custom/
├── CMakeLists.txt, cmake/CustomOverrides.cmake   build hook: name, the firmware factories
├── custom.qrc                                    builds res/config/ into the application
├── res/config/aethorax.json                      the manifest: which files, the dropdown
├── res/config/mission_commands.json              every command: QGroundControl's + the operations (generated)
├── src/AethoraxConfig.*                          finds and reads the configuration at startup
├── src/AethoraxPlugin.*                          core plugin: offline planning is ArduPilot Sub
├── src/AethoraxSubFirmwarePlugin.*               the dropdown, as aethorax.json says
├── src/AethoraxBaseFirmwarePlugin.*              serves mission_commands.json as QGroundControl's base list
├── src/AethoraxFirmwarePluginFactory.*           the only firmware: ArduPilot Sub (and the base list)
└── tools/generate_mission_commands.py            mission_commands.json from the AETHORAX config (--check in CI)
```

Nothing outside `custom/` changes. QGroundControl takes a command's name only from
its **base** list of commands; an override can't name one. So the factory also
serves the generic firmware class, and its plugin returns `mission_commands.json`
as the base list. The generator builds that file from upstream's own
`src/MissionManager/MavCmdInfoCommon.json` plus the operations. After merging an
upstream release, run the generator again; the `--check` in CI fails until you do.

## Configuration (no rebuild)

The application reads two JSON files at startup. Each is looked for by name, in order:

1. the folder in the environment variable `AETHORAX_QGC_CONFIG`;
2. `<Documents>/<application name>/Config`, next to QGroundControl's own `Missions` and `Logs`
   (Windows: `C:\Users\<you>\Documents\AETHORAX-QGroundControl\Config`;
   a build that isn't a release is named `AETHORAX-QGroundControl Daily`);
3. the copy built into the application (`res/config/`).

A file that's found but invalid shows a message in the application and isn't used;
the next one in the order is. Changes take effect on the next start.

| File | What it holds |
|---|---|
| `aethorax.json` | `missionCommands.file`: the commands file's name. `missionCommands.dropdown`: `firmwareCommands` (keep ArduSub's own commands, `true`/`false`), `add` and `remove` (lists of commands by `rawName`, e.g. `"MAV_CMD_USER_1"`, or by number) |
| `mission_commands.json` | every command QGroundControl can show: name, fields, units, defaults, limits. QGroundControl's own format (`fileType` `MavCmdInfo`); generated, don't edit by hand |

To give a PC a newer set of operations without a new installer:

```bash
python3 custom/tools/generate_mission_commands.py --out-dir "<Documents>/AETHORAX-QGroundControl/Config"
```

It writes `mission_commands.json`, and `aethorax.json` only if the folder has none
(a team's own dropdown choice is kept). While developing, point `AETHORAX_QGC_CONFIG`
at a folder instead.

## Adding an operation

1. Engineer it first (AETHORAX `/operation-review`), so its fields and limits are settled.
2. Give it the next free user command: `USER_n` without a position, `WAYPOINT_USER_n` with one.
3. Add it to `tools/generate_mission_commands.py`, reading its limits from the AETHORAX config, and run the tool.
4. Add its `rawName` to `dropdown.add` in `res/config/aethorax.json`.
5. Map the command in the Jetson's plan tool (AETHORAX `software/jetson/autonomy/src/plan/`).

Steps 3–4 change only JSON: PCs can take them with `--out-dir` (above) before the next
release builds them in.

## Build

As upstream (see [tools/README.md](../tools/README.md) and [AGENTS.md](../AGENTS.md)). Linux, in Docker:

```bash
./deploy/docker/run-docker.sh ubuntu Release     # → build/AETHORAX-QGroundControl-x86_64.AppImage
```

Windows: Visual Studio 2022 and Qt 6.11 (QGroundControl's `build-config.json`),
then `cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release` and `cmake --install build`
for the NSIS installer. CI builds both (below).

## Planned: the rest from JSON

The same mechanism (a section in `aethorax.json`, read by `AethoraxConfig`) can carry
the look and the defaults later. What QGroundControl lets a custom build change at
startup, and so what can come from JSON:

| Section (planned) | Through |
|---|---|
| `palette`: colours, light and dark | `QGCCorePlugin::paletteOverride` |
| `toolbar`: background colours, height; which panels and buttons show | `QGCOptions` (`toolbarBackgroundLight/Dark`, `toolbarHeightMultiplier`, `show*`) |
| `settings`: defaults, and which settings are hidden | `QGCCorePlugin::adjustSettingMetaData` (already used for offline planning) |
| `brand`: the logo and images in the application | one QML file replaced once in `custom/` (custom builds can replace upstream QML, as `custom-example/` does), taking the image paths from JSON |
| `windowIcon`: the window and taskbar icon while running | `QGuiApplication::setWindowIcon` |

These stay build-time, because the operating system reads them before the application
runs: the application's name, the installer's and the desktop shortcut's icon, and the
Windows file details. They live in `cmake/CustomOverrides.cmake` and `res/` and change
with a release.

## Versions and releases

- **Versioning:** QGroundControl takes its version from `git describe --tags`. Our releases are tags `vX.Y.Z` on branch `aethorax`, starting at **v0.0.1**.
- **Publishing:** the release is published by AETHORAX, where this repository is the `qgroundcontrol/` submodule:
  1. Commit and tag here: `git tag v0.0.1 && git push origin aethorax v0.0.1`.
  2. In AETHORAX, point the submodule at that commit, commit, and tag `qgc-v0.0.1`.
  3. AETHORAX's `.github/workflows/qgc-release.yml` builds the Windows installer and the Linux AppImage with QGroundControl's own CI actions, and attaches them to the AETHORAX release `qgc-v0.0.1`.

## Following upstream

```bash
git fetch upstream --tags
git merge v5.x.y            # on branch aethorax; resolve, rebuild, test
```

`upstream` is `https://github.com/mavlink/qgroundcontrol.git`. Our changes are
kept to `custom/`, so merges don't conflict.
