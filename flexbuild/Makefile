#####################################
#
# Copyright 2017 NXP
#
#####################################

ifndef TOPDIR
  TOPDIR = $(shell cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd)
endif

include $(TOPDIR)/configs/$(CONFIGLIST)

all:
	@$(MAKE) -C $(TOPDIR)/packages

uboot uefi rcw ppa ppa-optee mc-utils bin-firmware qoriq-engine-pfe-bin:
	@$(MAKE) -C $(TOPDIR)/packages/firmware $@

firmware linux apps:
	@$(MAKE) -C $(TOPDIR)/packages/$@

cryptodev-linux perf lttng-modules:
	@$(MAKE) -C $(TOPDIR)/packages/linux $@

$(APPS_REPO_LIST):
	@$(MAKE) -C $(TOPDIR)/packages/apps $@

distroinstaller:
	@$(MAKE) -C $(TOPDIR)/packages/installer
