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
| Plan's tool strip | **Takeoff** inserts a Takeoff item | **Submerge** inserts the Submerge operation after the selected item. Name and command set in `aethorax.json` |
| The first mission item | Initial Camera Settings | **Initial Settings**: each device **on or off** at the mission's start (Camera, DVL A50, forward-looking sonar), saved in the `.plan` file; then the camera settings |
| The Plan view's side panel | a fixed width | drag its left edge to widen it; kept between runs, double-click the edge to reset. Limits, edge and cursor set in `aethorax.json` |

Each operation is a MAVLink user command:

| Operation | Command | Fields |
|---|---|---|
| Dive | `MAV_CMD_USER_1` (31010) | `param1` Depth (m, ≥ 0), `param2` Speed (m/s, between the descent table's first and last speeds; default: the descent profile's) |
| Submerge | `MAV_CMD_USER_2` (31011) | none yet: from the surface down a slant path to the next waypoint's position and depth; it starts the dive part of a mission. Its fields come from its operation review |

The plan file also carries the devices' status at the start, which stock QGroundControl ignores:

```json
"aethorax": {"version": 1, "initialStatus": {"camera": true, "dvl": true, "sonar": true}}
```

ArduSub doesn't store user commands, so a plan with operations goes to the Jetson
**as a file** (*Save* / *Save As*), through the operator console, not through
*Upload*. The Jetson's plan tool maps each command number to its operation
(`software/jetson/autonomy/src/plan/` in AETHORAX).

## Files

```
custom/
├── CMakeLists.txt, cmake/CustomOverrides.cmake   build hook: name, the firmware factories
├── custom.qrc                                    builds res/config/ and qml/ into the application
├── qml/                                          upstream screens replaced by our copies (each says what it changes)
├── res/config/aethorax.json                      the manifest: which files, the dropdown
├── res/config/mission_commands.json              every command: QGroundControl's + the operations (generated)
├── src/AethoraxConfig.*                          finds and reads the configuration at startup
├── src/AethoraxLabels.*                          renames the application's text, from aethorax.json's labels
├── src/AethoraxMissionStart.*                    the devices' status at the start, in the .plan file
├── src/AethoraxPlugin.*                          core plugin: offline planning is ArduPilot Sub
├── src/AethoraxSubFirmwarePlugin.*               the dropdown, as aethorax.json says
├── src/AethoraxBaseFirmwarePlugin.*              serves mission_commands.json as QGroundControl's base list
├── src/AethoraxFirmwarePluginFactory.*           the only firmware: ArduPilot Sub (and the base list)
└── tools/generate_mission_commands.py            mission_commands.json from the AETHORAX config (--check)
```

Nothing outside `custom/` changes. QGroundControl takes a command's name only from
its **base** list of commands; an override can't name one. So the factory also
serves the generic firmware class, and its plugin returns `mission_commands.json`
as the base list. The generator builds that file from upstream's own
`src/MissionManager/MavCmdInfoCommon.json` plus the operations. After merging an
upstream release, run the generator again (`--check` tells whether it's needed).

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
| `aethorax.json` | `missionCommands.file`: the commands file's name. `missionCommands.dropdown`: `firmwareCommands` (keep ArduSub's own commands, `true`/`false`), `add` and `remove` (lists of commands by `rawName`, e.g. `"MAV_CMD_USER_1"`, or by number), `categoriesFirst` (the picker's first categories). `planView.rightPanel`: `resizable`, `minimumWidth` (characters), `maximumWidth` (fraction of the window), `edge` (`width`, `cursor`: a [Qt cursor name](https://doc.qt.io/qt-6/qt.html#CursorShape-enum), `grip`: `length`, `thickness`, `opacity`, `hoverOpacity`). `planView.toolStrip.takeoff.command`: what the Takeoff button inserts, by `rawName` (none: QGroundControl's Takeoff item). `labels`: text renamed, by context (the C++ class or QML file's name) and original English text, e.g. `"PlanView": {"Takeoff": "Submerge"}`. `missionStart.devices`: the devices with an on/off status at the start (`key`, saved in the plan; `label`; `default`) |
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

Windows, from a clone on a local drive (a WSL folder is too slow), in a Command Prompt:

```bat
custom\tools\windows-build.cmd            :: test build → build\AETHORAX-QGroundControl-installer*.exe
custom\tools\windows-build.cmd --stable   :: release build: the version from the git tag, no "Daily"
```

The first build compiles everything (≈ 30–60 min); later ones only what changed.

### Windows build tools (once per PC)

QGroundControl's own installers, with the versions in `.github/build-config.json`
(≈ 12 GB on C:). Python 3.12 first (`winget install -e --id Python.Python.3.12 --scope user`), then:

```bat
:: as Administrator: Visual Studio 2022 Build Tools (C++, with CMake and Ninja), NSIS, GStreamer
python tools\setup\install_dependencies --platform windows --msvc --nsis

:: as yourself, with Python's Scripts folder on PATH: Qt 6.11.1 in C:\Qt. The aqtinstall
:: release can't find Qt 6.11 yet; this is the commit QGroundControl's CI uses (AQT_SOURCE)
python tools\setup\install_qt.py install --version 6.11.1 --host windows --target desktop ^
    --arch win64_msvc2022_64 --outdir C:\Qt ^
    --aqt-source "git+https://github.com/miurahr/aqtinstall.git@8d961e61720b3c06583517fbc68d57ec0a4e9f95" ^
    --modules "qtgraphs qtlocation qtpositioning qtspeech qtmultimedia qtserialport qtimageformats qtshadertools qtconnectivity qtquick3d qtsensors qtscxml qtwebsockets qthttpserver"
```

## Planned: the rest from JSON

The same mechanism (a section in `aethorax.json`, read by `AethoraxConfig`; QML reads one with
`QGroundControl.corePlugin.config("planView.rightPanel")`, with its own defaults for anything
missing) carries every customization, and can carry the look and the defaults later. What QGroundControl lets a custom build change at
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

This repository is **public** so that GitHub builds its releases for free, and the
releases can be downloaded from its Releases page. AETHORAX (private) only points at a
commit of it, as its `qgroundcontrol/` submodule.

- **Versioning:** QGroundControl takes its version from `git describe --tags`. Our releases are tags `vX.Y.Z` on branch `aethorax`, starting at **v0.0.1**.
- **Releasing:**
  1. Commit, then tag and push: `git tag v0.0.2 && git push origin aethorax v0.0.2`.
  2. `.github/workflows/aethorax-release.yml` builds the Windows installer and the Linux AppImage
     (a stable build: no "Daily") and attaches them, with `SHA256SUMS.txt`, to a **draft** release `v0.0.2`.
  3. Test the downloads from the draft, then press **Publish**.
  4. In AETHORAX, point the submodule at the tagged commit and commit.
- **A test build without a release:** *Actions → AETHORAX Release → Run workflow*, or on this PC
  (Build, above).
- **Upstream's workflows are disabled** in *Settings → Actions* (they're written for upstream's
  repository and would fail or build on every pull request), and Dependabot is off
  (`.github/dependabot.yml` removed). A workflow added by an upstream merge starts enabled:
  disable it (`gh workflow disable <name>`).
- **Before committing a change to the operations,** run `tools/generate_mission_commands.py --check`
  from AETHORAX: it reads the AETHORAX config, which this public repository doesn't have.

## Following upstream

```bash
git fetch upstream --tags
git merge v5.x.y            # on branch aethorax; resolve, rebuild, test
```

`upstream` is `https://github.com/mavlink/qgroundcontrol.git`. Our changes are
kept to `custom/`, so merges don't conflict.
