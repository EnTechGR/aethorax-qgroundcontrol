# ============================================================================
# AETHORAX QGroundControl custom build (see README.md)
# ============================================================================

message(STATUS "QGC: Adding the AETHORAX custom plugin")

list(APPEND CUSTOM_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/custom.qrc)
set(QGC_RESOURCES ${QGC_RESOURCES} ${CUSTOM_RESOURCES} CACHE STRING "Paths to .qrc Resources" FORCE)

set(CUSTOM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AethoraxPlugin.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AethoraxPlugin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AethoraxSubFirmwarePlugin.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AethoraxSubFirmwarePlugin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AethoraxFirmwarePluginFactory.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AethoraxFirmwarePluginFactory.h
    CACHE INTERNAL "" FORCE
)

set(CUSTOM_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    CACHE INTERNAL "" FORCE
)

set(CUSTOM_DEFINITIONS
    QGC_CUSTOM_BUILD
    CUSTOMHEADER="AethoraxPlugin.h"
    CUSTOMCLASS=AethoraxPlugin
    CACHE INTERNAL "" FORCE
)

set(CUSTOM_QT_COMPONENTS
    Core
    CACHE INTERNAL "" FORCE
)
