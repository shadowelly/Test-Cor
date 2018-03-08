#####################################
#
# Copyright 2017 NXP
#
######################################

SHELL=/bin/bash
include $(TOPDIR)/configs/$(CONFIGLIST)
include $(TOPDIR)/include/repo.mk

all:
	@if [ ! -d ramdiskrfs ]; then git clone $(GIT_REPOSITORY_URL)/ramdiskrfs.git; fi;
	@echo "run flex-builder -i mkinstaller -a $(DESTARCH) -f $(CONFIGLIST) to generate flex_linux_$(DESTARCH).itb"

repo_fetch:
	@$(call repo-update,fetch,ramdiskrfs)

repo_update_branch:
	@$(call repo-update,branch,ramdiskrfs)

repo_update_tag:
	@echo repositories update for tag  ...
	@$(call repo-update,tag,ramdiskrfs)

repo_update_latest:
	@$(call repo-update,update,ramdiskrfs)

repo_update_commit:
	@$(call repo-update,commit,ramdiskrfs)
	
