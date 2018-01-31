#**************************************************************#
# COPY THIS FILE AS "Makefile" IN THE "src" DIR OF YOUR MODULE #
# AND CUSTOMIZE IT TO FIT YOUR NEEDS.                          #
#**************************************************************#


## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk


## -------------------------------------- ##
## General information about this module. ##
## You must edit these appropriately.     ##
## -------------------------------------- ##

# Name of the module, with toplevel path, e.g. "phy/tests/dishwasher"
LOCAL_NAME := at/ats

# Space-separated list of modules (libraries) your module depends upon.
# These should include the toplevel name, e.g. "phy/dishes ciitech/hotwater"

LOCAL_DEPEND_LIBS := at/ats/cc \
                     at/ats/gc \
                     at/ats/gprs \
                     at/ats/id \
                     at/ats/nw \
                     at/ats/pbk \
                     at/ats/sat \
                     at/ats/sim \
                     at/ats/sms \
                     at/ats/special \
                     at/ats/ss \
                     at/ats/tcpip \
                     at/ats/cfg  \
                     at/ats/ata \
                     at/ats/ftp \
                     at/ats/coap \
                     at/ats/mqtt \
		     at/ats/mqttx \
                     at/ats/http \
                     at/ats/lwm2m \
		     at/ats/gps \
                     at/ats/iperf \
		     
# Add includes from other modules we do not wish to link to
LOCAL_API_DEPENDS := \
                     application/systeminc/mmi \
                     application/coolmmi \
                     at/utility \
                     at/ate \
                     at/ats \
                     at/ats/gps \
                     ${API_PLATFORM_DEPENDS} \


# Set this to a non-null string to signal a toplevel module, like
# phy but not like phy/kitchensink. This defines the behavior of
# make deliv
IS_TOP_LEVEL := yes

# For a toplevel module, define which tests to include in delivery
# Skip the 'tests' in the names. Beware that everything from the
# given tests directories will be included...
TOPLEVEL_DELIVER_TESTS :=


## --------------------------- ##
## Override toplevel settings  ##
## --------------------------- ##

# Set more dependencies as binary : they will not be compiled from source.
LOCAL_BINARY_LIBS ?=

# Set dependencies as source : the library will be generated from source
# This supersedes the BINARY (local+global) setting in case of conflict
LOCAL_SRC_LIBS ?=


## ------------------------------------- ##
##  List all your sources here           ##
## ------------------------------------- ##

# Assembly / C code
S_SRC :=
C_SRC :=
# Simulation specific inputs
INPUT_FILES :=

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/${PLATFORM_SYSTEM}/stack/stack.mk
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk

