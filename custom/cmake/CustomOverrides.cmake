# ============================================================================
# AETHORAX QGroundControl: branding and feature set
# Loaded by the top-level CMakeLists.txt when custom/ exists (QGC_CUSTOM_DIR).
# ============================================================================

set(QGC_APP_NAME "AETHORAX-QGroundControl" CACHE STRING "App Name" FORCE)
set(QGC_APP_DESCRIPTION "AETHORAX mission planning for Nemo1 (QGroundControl custom build)" CACHE STRING "Application description" FORCE)
set(QGC_ANDROID_PACKAGE_NAME "gr.entech.aethorax.qgroundcontrol" CACHE STRING "Android package identifier" FORCE)

# One vehicle: ArduPilot Sub. The stock ArduPilot and PX4 factories are not
# registered; custom/src/AethoraxFirmwarePluginFactory serves ArduPilot Sub only.
set(QGC_DISABLE_APM_PLUGIN_FACTORY ON CACHE BOOL "Disable APM Plugin Factory" FORCE)
set(QGC_DISABLE_PX4_PLUGIN_FACTORY ON CACHE BOOL "Disable PX4 Plugin Factory" FORCE)
