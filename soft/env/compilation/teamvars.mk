#############################################################################
# Depending on the team, selects which module must be used as lib.
#############################################################################

TEAM_BINARY_LIBS := 

ifeq "${CT_USER}" "ADMIN"
else ifeq "${CT_USER}" "CMUX"
    TEAM_BINARY_LIBS += platform/base/packed
    TEAM_BINARY_LIBS += ${PLATFORM_SERVICE}/packed
    TEAM_BINARY_LIBS += ${PLATFORM_SYSTEM}/mdi/packed
    TEAM_BINARY_LIBS += ${PLATFORM_SYSTEM}/stack/packed
    TEAM_BINARY_LIBS += ${PLATFORM_SYSTEM}/svc/packed
    TEAM_BINARY_LIBS += ${PLATFORM_SYSTEM}/vpp/packed
else ifeq "${CT_USER}" "MODEM"
    TEAM_BINARY_LIBS += at/packed
    TEAM_BINARY_LIBS += ${PLATFORM_SERVICE}/packed
    TEAM_BINARY_LIBS += ${PLATFORM_SYSTEM}/stack/packed
else ifeq "${CT_USER}" "FAE"
    TEAM_BINARY_LIBS += ${PLATFORM_SERVICE}/fota/fota_core/packed
    TEAM_BINARY_LIBS += ${PLATFORM_SYSTEM}/stack/packed
    TEAM_BINARY_LIBS += application/coolmmi/mmi/wapmms/jcore/packed
    TEAM_BINARY_LIBS += toolpool/blfpc/goke_flash_tool/packed
endif


# At release, some modules are released with library.
# Release script shall add library modules after here.

