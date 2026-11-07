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
| A mission item's dropdown | every command ArduPilot supports | **AETHORAX's operations:** **Dive** (Depth, Speed), plus Waypoint, Change speed and Set servo until those become operations of their own |
| An operation's fields | MAVLink's generic parameters | only the fields the operation has, with units, defaults and limits from the AETHORAX config |

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
├── custom.qrc, res/AETHORAX-MavCmdInfoSub.json   the operations as QGroundControl commands (generated)
├── src/AethoraxPlugin.*                          core plugin: offline planning is ArduPilot Sub
├── src/AethoraxSubFirmwarePlugin.*               the dropdown's commands, and where their fields come from
├── src/AethoraxFirmwarePluginFactory.*           the only firmware: ArduPilot Sub
└── tools/generate_mission_commands.py            res/…json from the AETHORAX config (--check in CI)
```

Outside `custom/`, one upstream file changes: `src/MissionManager/MavCmdInfoCommon.json`
gains base entries for the MAVLink user commands (`WAYPOINT_USER_1-5`, `USER_1-5`).
An override can't add a command that the base list lacks, and stock builds never list them.

## Adding an operation

1. Engineer it first (AETHORAX `/operation-review`), so its fields and limits are settled.
2. Give it the next free user command: `USER_n` without a position, `WAYPOINT_USER_n` with one.
3. Add it to `tools/generate_mission_commands.py`, reading its limits from the AETHORAX config, and run the tool.
4. Add the command to `AethoraxSubFirmwarePlugin::supportedMissionCommands`.
5. Map the command in the Jetson's plan tool (AETHORAX `software/jetson/autonomy/src/plan/`).

## Build

As upstream (see [tools/README.md](../tools/README.md) and [AGENTS.md](../AGENTS.md)). Linux, in Docker:

```bash
./deploy/docker/run-docker.sh ubuntu Release     # → build/AETHORAX-QGroundControl-x86_64.AppImage
```

Windows: Visual Studio 2022 and Qt 6.11 (QGroundControl's `build-config.json`),
then `cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release` and `cmake --install build`
for the NSIS installer. CI builds both (below).

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
kept to `custom/` and the one JSON file above, so merges stay small.
