#####################################
#
# Copyright 2017 NXP
#
######################################

SHELL=/bin/bash
include $(TOPDIR)/configs/$(CONFIGLIST)
include $(TOPDIR)/include/repo.mk

LINUX_REPO_LIST = $(KERNEL_TREE) cryptodev-linux

all: setup-repo build-linux

setup-repo:
	@$(call fetch-git-tree,$(KERNEL_TREE))

build-linux:
ifeq ($(CONFIG_BUILD_LINUX), y)
	@echo "Building $(KERNEL_TREE) with branch `cd $(KERNEL_PATH) && \
	 git branch -v | grep '^*' | cut -d' ' -f 2,3,14`"
	@echo KERNEL_CFG = $(KERNEL_CFG)
	@echo Compiler = `$(CROSS_COMPILE)gcc --version | head -1`
	@if [ $(DESTARCH) = arm64 ]; then locarch=arm64; dtbstr=freescale/*.dtb; \
	elif [ $(DESTARCH) = arm32 -a $(MACHINE) = ls1021atwr ]; then \
	locarch=arm; dtbstr=ls*.dtb; extname=v7; else locarch=arm; dtbstr=freescale/*.dtb; extname=v8; \
	cp -rf $(KERNEL_PATH)/arch/arm64/boot/dts/freescale $(KERNEL_PATH)/arch/arm/boot/dts; fi; \
	curbrch=`cd $(KERNEL_PATH) && git branch -v | grep '^*' | cut -d' ' -f2` && \
	opdir=$(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH)/output/$$curbrch && \
	if [ $(DESTARCH) = arm32 -a $(MACHINE) = ls1021atwr ]; then opdir=$$opdir/v7; \
	elif [ $(DESTARCH) = arm32 -a $(MACHINE) != ls1021atwr ]; then opdir=$$opdir/v8; fi && \
	if [ ! -d $$opdir ]; then mkdir -p $$opdir; fi && \
	if [ ! -f $$opdir/.config ]; then $(MAKE) $(KERNEL_CFG) -C $(KERNEL_PATH) O=$$opdir 1>/dev/null; fi && \
	if [ "$(BUILDARG)" = "menuconfig" ]; then $(MAKE) menuconfig -C $(KERNEL_PATH) O=$$opdir && \
	echo Custom kernel config: $$opdir/.config  [Done] && exit; fi; \
	if [ "`echo $(BUILDARG) | cut -d: -f1`" = "fragment" ]; then mergeconfig=`echo $(BUILDARG) | cut -d: -f2`; \
	echo -e "\n$(KERNEL_TREE): Total Config List = $(KERNEL_CFG) $$mergeconfig\n" && \
	$(MAKE) $$mergeconfig -C $(KERNEL_PATH) O=$$opdir; fi && $(MAKE) -j$(JOBS) all -C $(KERNEL_PATH) O=$$opdir && \
	if [ $(DESTARCH) = arm32 ]; then $(MAKE) -j$(JOBS) uImage LOADADDR=80008000 -C $(KERNEL_PATH) O=$$opdir; fi && \
	$(MAKE) modules -C $(KERNEL_PATH) O=$$opdir && \
	$(MAKE) INSTALL_MOD_PATH=$(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH) modules_install -C $(KERNEL_PATH) O=$$opdir && \
	$(MAKE) zinstall INSTALL_PATH=$(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH) -C $(KERNEL_PATH) O=$$opdir && \
	if [ $(DESTARCH) = arm32 -a $(MACHINE) != ls1021atwr ]; then $(MAKE) freescale/fsl-ls1043a-rdb-sdk.dtb \
	freescale/fsl-ls1046a-rdb-sdk.dtb freescale/fsl-ls1012a-rdb.dtb \
	freescale/fsl-ls1012a-frdm.dtb -C $(KERNEL_PATH) O=$$opdir; fi && \
	if [ $(DESTARCH) = arm64 ]; then cp $$opdir/arch/$$locarch/boot/Image* \
	$(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH); fi && \
	if [ $(DESTARCH) = arm32 ]; then cp $$opdir/arch/$$locarch/boot/uImage \
	$(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH)/uImage.$$extname; \
	cp $$opdir/arch/$$locarch/boot/zImage $(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH)/zImage.$$extname; fi && \
	ls $$opdir/arch/$$locarch/boot/dts/$$dtbstr | grep -E 'rdb|frdm|twr|qds' | xargs -I {} cp {} \
	$(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH) && \
	if [ $(DESTARCH) = arm32 ]; then rm -f $(TOPDIR)/build/linux/$(KERNEL_TREE)/arm32/fsl-ls2*.dtb; fi && \
	cd $(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH) && if [ -f fsl-ls1043a-rdb-sdk.dtb ]; then \
	mv fsl-ls1043a-rdb-sdk.dtb fsl-ls1043a-rdb.dtb; mv fsl-ls1046a-rdb-sdk.dtb fsl-ls1046a-rdb.dtb; \
	if [ -f fsl-ls1043a-qds-sdk.dtb ]; then mv fsl-ls1043a-qds-sdk.dtb fsl-ls1043a-qds.dtb; \
	mv fsl-ls1046a-qds-sdk.dtb fsl-ls1046a-qds.dtb; fi; fi && \
	ls -l $(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH) && \
	echo Build $(KERNEL_TREE) $$curbrch in $(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH)   [Done]
endif


.PHONY: cryptodev-linux
cryptodev-linux:
ifeq ($(CONFIG_BUILD_CRYPTODEV_LINUX), y)
	@$(call fetch-git-tree,cryptodev-linux)
	@$(call fetch-git-tree,$(KERNEL_TREE))
	@if [ ! -d $(TOPDIR)/build/linux/kernel/$(DESTARCH) ]; then cd $(TOPDIR) && \
	 flex-builder -c linux -a $(DESTARCH) -f $(CONFIGLIST) && cd -; fi && \
	 curbrch=`cd $(KERNEL_PATH) && git branch -v | grep '^*' | cut -d' ' -f2` && \
	 opdir=$(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH)/output/$$curbrch && \
	 if [ $(DESTARCH) = arm32 -a $(MACHINE) = ls1021atwr ]; then opdir=$$opdir/v7; \
	 elif [ $(DESTARCH) = arm32 -a $(MACHINE) != ls1021atwr ]; then opdir=$$opdir/v8; fi && \
	 cd cryptodev-linux && echo -e "\nBuilding CRYPTODEV_LINUX ..." && export KERNEL_DIR=$$opdir && \
	 export KERNEL_MAKE_OPTS="-lcrypto -L$(RFSDIR)/usr/lib/aarch64-linux-gnu" && $(MAKE) && $(MAKE) install && \
	 echo -e "build CRYPTODEV_LINUX  [Done]\n"
endif


perf:
	@$(call fetch-git-tree,$(KERNEL_TREE))
	@curbrch=`cd $(KERNEL_PATH) && git branch -v | grep '^*' | cut -d' ' -f2` && \
	 opdir=$(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH)/output/$$curbrch && \
	 if [ $(DESTARCH) = arm32 -a $(MACHINE) = ls1021atwr ]; then opdir=$$opdir/v7; \
	 elif [ $(DESTARCH) = arm32 -a $(MACHINE) != ls1021atwr ]; then opdir=$$opdir/v8; fi && \
	 echo -e "\nBuilding kernel tools/perf ..." && \
	 if [ ! -d $$opdir ]; then mkdir -p $$opdir; fi && \
	 if [ ! -f $$opdir/.config ]; then $(MAKE) $(KERNEL_CFG) -C $(KERNEL_PATH) O=$$opdir 1>/dev/null; fi && \
	 $(MAKE) -j$(JOBS) tools/perf -C $(KERNEL_PATH) O=$$opdir && \
	 cp $$opdir/tools/perf/perf $(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH) && \
	 echo -e "build kernel tools/perf  [Done]\n"


.PHONY: lttng-modules
lttng-modules:
ifeq ($(CONFIG_BUILD_LTTNG), y)
	@$(call fetch-git-tree,lttng-modules)
	@$(call fetch-git-tree,$(KERNEL_TREE))
	@if [ ! -d $(TOPDIR)/build/linux/kernel/$(DESTARCH) ]; then cd $(TOPDIR) && \
	 flex-builder -c linux -a $(DESTARCH) -f $(CONFIGLIST) && cd -; fi && \
	 cd lttng-modules && echo -e "\nBuilding LTTng modules ..." && \
	 $(MAKE) KERNELDIR=$(KERNEL_PATH) && \
	 $(MAKE) KERNELDIR=$(KERNEL_PATH) modules_install && \
	 echo build LTTng modules done!
endif


repo_fetch:
	@echo -e "\nfetch linux repositories ..."
	@$(call repo-update,fetch,$(LINUX_REPO_LIST))

repo_update_branch:
	@echo -e "\nlinux repositories update for branch ..."
	@$(call repo-update,branch,$(LINUX_REPO_LIST))

repo_update_tag:
	@echo -e "\napps repositories update for tag ..."
	@$(call repo-update,tag,$(LINUX_REPO_LIST))

repo_update_latest:
	@echo -e "\nlinux repositories update to latest HEAD commit ..."
	@$(call repo-update,update,$(LINUX_REPO_LIST))

repo_update_commit:
	@echo -e "\nlinux repositories update to specified commit ID ..."
	@$(call repo-update,commit,$(LINUX_REPO_LIST))
