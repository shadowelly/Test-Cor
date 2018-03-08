#####################################
#
# Copyright 2017 NXP
#
#####################################

include $(TOPDIR)/configs/$(CONFIG_BUILD_CFGLIST)

TGTDIRS := firmware linux apps installer

all: $(TGTDIRS)

$(TGTDIRS): ECHO
	@$(MAKE) -C $(TOPDIR)/packages/$@

ECHO:
	@echo Building $(TGTDIRS)
