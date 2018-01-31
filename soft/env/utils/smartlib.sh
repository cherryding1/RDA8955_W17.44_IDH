#!/usr/bin/env bash
#===============================================================================

# Exit immediately if command exits with a non-zero status.
set -e

#{SOFT_WORKDIR}/env/utils/releaselib_at.sh -2 -e -pack smartphone 8810_r8810
#{SOFT_WORKDIR}/env/utils/releaselib_csw.sh -2 -e -pack smartphone 8810_r8810
${SOFT_WORKDIR}/env/utils/releaselib_stack.sh -2 -e 8810_r8810
