
API_PLATFORM_DEPENDS := platform \
						platform/base/std \
						platform/base/sx \
						platform/base/common \
						platform/chip/defs \
						platform/chip/hal \
						platform/chip/hal/${CT_ASIC}      \
						platform/chip/boot      \
						platform/chip/pal      \
						${PLATFORM_SERVICE} \
						${PLATFORM_SERVICE}/base \
						${PLATFORM_SERVICE}/base/bal \
						${PLATFORM_SERVICE}/cfw \
						${PLATFORM_SERVICE}/wifi \
						platform/edrv/atvd  \
						platform/edrv/gsensor \
						platform/edrv/camd \
						platform/edrv/aud \
						platform/edrv/aud/analog \
						platform/edrv/aud/fm \
						platform/edrv/aud/maxim \
						platform/edrv/aud/null \
						platform/edrv/aud/codec_gallite \
						platform/edrv/cammd \
						platform/edrv/cmmbd \
						platform/edrv/cmmbd/rda5891 \
						platform/edrv/fmd \
						platform/edrv/fmd/${FM_MODEL} \
						platform/edrv/lcdd \
						platform/edrv/mcd \
						platform/edrv/mcd/${MCD_MODEL} \
						platform/edrv/memd \
						platform/edrv/pmd \
						platform/edrv/pmd/${PM_MODEL} \
						platform/edrv/rfd \
						platform/edrv/tsd \
						platform/edrv/btd/asc3600\
						platform/edrv/btd/rdabt_adapter\
						platform/edrv/wifi/rdawifi \
						${PLATFORM_SYSTEM}/calib \
						${PLATFORM_SYSTEM}/svc/uctls \
						${PLATFORM_SYSTEM}/svc/umss \
						${PLATFORM_SYSTEM}/svc/umss/storage/ram      \
						${PLATFORM_SYSTEM}/svc/umss/transport/boscsi \
						${PLATFORM_SYSTEM}/svc/uat \
								${PLATFORM_SYSTEM}/svc/urndis \
								${PLATFORM_SYSTEM}/svc/uwifi \
								${PLATFORM_SYSTEM}/svc/uvideos \
								${PLATFORM_SYSTEM}/svc/uvideos/stream \
								${PLATFORM_SYSTEM}/svc/utraces	\
					            ${PLATFORM_SYSTEM}/svc/vois\
					            ${PLATFORM_SYSTEM}/vpp/lzmadec \
					            ${PLATFORM_SYSTEM}/stack \
					            ${PLATFORM_SYSTEM}/stack/l1 \
					            ${PLATFORM_SYSTEM}/stack/cmn \
					            ${PLATFORM_SYSTEM}/mdi \
								${PLATFORM_SYSTEM}/mdi/mmc \
								${PLATFORM_SYSTEM}/mdi/alg/gifdec  \
								${PLATFORM_SYSTEM}/mdi/alg/common \
								${PLATFORM_SYSTEM}/mdi/alg/audiojpeg_dec_voc \
								target \
								target/${CT_TARGET} \

ifeq "${CFW_TCPIP_SUPPORT}" "y"
API_PLATFORM_DEPENDS += ${PLATFORM_SERVICE}/net/ \
                        ${PLATFORM_SERVICE}/net/lwip/src
endif

