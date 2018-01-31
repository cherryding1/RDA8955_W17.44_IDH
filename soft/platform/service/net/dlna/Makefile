## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilevars.mk

# Name of the module
LOCAL_NAME := platform/service/net/dlna

# list all modules APIs that are neccessary to compile this module
LOCAL_API_DEPENDS := \
					platform \
					platform/rtos/common \
					platform/stack \
					platform/service \

# list all the modules that need to be compiled prior to using this module
LOCAL_MODULE_DEPENDS := \
						platform/service/net/dlna/avrender \
						platform/service/net/dlna/http \
						platform/service/net/dlna/io \
						platform/service/net/dlna/net \
						platform/service/net/dlna/soap \
						platform/service/net/dlna/upnp \
						platform/service/net/dlna/xml \
						platform/service/net/dlna/util \
						platform/service/net/dlna/xmlparser \
						platform/service/net/dlna/upnptrc2file \

# This is a top-level module
IS_TOP_LEVEL := yes

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##


## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/env/compilation/compilerules.mk