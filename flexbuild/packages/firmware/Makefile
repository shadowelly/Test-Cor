#####################################
#
# Copyright 2017 NXP
#
#####################################

SHELL=/bin/bash
include $(TOPDIR)/configs/$(CONFIGLIST)
include $(TOPDIR)/include/repo.mk


UEFI_MACHINE_LIST := ls1043ardb ls1046ardb ls2088ardb
OPTEE_MACHINE_LIST := ls1043ardb ls1046ardb ls1012ardb
RCWBINBRD := ls1012ardb ls1012afrdm ls1088ardb ls2088ardb
RCWSRCBRD := $(filter-out ls1088ardb ls2088ardb ls1012ardb ls1012afrdm,$(MACHINE_LIST))
PPASUBDIRS = soc-ls1012 soc-ls1028 soc-ls1043 soc-ls1046 soc-ls1088 \
	     soc-ls2080 soc-ls2088 soc-la1575 soc-lx2160


firmware: uboot uefi ppa rcw mc-utils bin-firmware

uboot:
ifeq ($(CONFIG_BUILD_UBOOT), y)
	@$(call fetch-git-tree,$(UBOOT_TREE))
ifeq ($(MACHINE), all)
	@for brd in $(MACHINE_LIST); do \
		if [ $(DESTARCH) = arm64 -a $$brd = ls1021atwr ]; then continue; \
		elif [ $(DESTARCH) = arm32 ] && [ $$brd != ls1021atwr -a $$brd != ls1043ardb -a $$brd != ls1046ardb ]; then continue; fi; \
		echo -e "\n\n******** machine = $$brd ************"; \
		if [ $$brd = ls2088ardb ]; then brdmsk=*ls208?ardb*; else brdmsk=*$$brd*; fi && \
		for cfg in `ls $(UBOOT_TREE)/configs/$$brdmsk 2>/dev/null | cut -d/ -f3 | grep -E 'qspi|sdcard|nor|nand|rdb_defconfig|rdb_SECURE_BOOT'`; do \
		    $(call build-uboot-target,$$cfg) \
		done; \
	 done

else
	@echo "building uboot for $(MACHINE)"
	@if [ $(MACHINE) = ls2088ardb ]; then brdmsk=*ls208?ardb*; else brdmsk=*$(MACHINE)*; fi && \
	 if [ -z "$(BOOTTYPE)" ]; then \
	     for cfg in `ls $(UBOOT_TREE)/configs/$$brdmsk 2>/dev/null | cut -d/ -f3 | grep -E 'qspi|sdcard|nor|nand|rdb_defconfig|rdb_SECURE_BOOT'`; do \
		$(call build-uboot-target,$$cfg) \
	     done; \
	 else \
	     if [ $(BOOTTYPE) = nor ]; then \
	         for cfg in `ls $(UBOOT_TREE)/configs/$$brdmsk 2>/dev/null | cut -d/ -f3 | grep -E 'nor|rdb_defconfig|rdb_SECURE_BOOT'`; do \
		    $(call build-uboot-target,$$cfg) \
		 done; \
	     else \
	         for cfg in `ls $(UBOOT_TREE)/configs/$$brdmsk 2>/dev/null | cut -d/ -f3 | grep -E "$(BOOTTYPE)"`; do \
		     $(call build-uboot-target,$$cfg) \
		 done; \
	     fi; \
	 fi
endif

define build-uboot-target
	if echo $1 | grep -E 'ls1021atwr';  then export ARCH=arm; export CROSS_COMPILE=arm-linux-gnueabihf-; \
	else export ARCH=arm64;export CROSS_COMPILE=aarch64-linux-gnu-; fi && \
	if [ $(MACHINE) != all ]; then brd=$(MACHINE); fi && \
	opdir=$(TOPDIR)/build/firmware/u-boot/$$brd/output/$1 && \
	if [ ! -d $$opdir ]; then mkdir -p $$opdir; fi &&  \
	echo -e "\nconfig = $1" && if [ ! -f $$opdir/.config ]; then $(MAKE) -C $(UBOOT_TREE) $1 O=$$opdir; fi && \
	$(MAKE) -C $(UBOOT_TREE) -j$(JOBS) O=$$opdir && \
	if echo $1 | grep -E 'sdcard|nand'; then \
	   if [ -f $$opdir/u-boot-with-spl-pbl.bin ]; then \
	       srcbin=u-boot-with-spl-pbl.bin; \
	   else \
	       srcbin=u-boot-with-spl.bin; \
	   fi; \
           if echo $1 | grep -E 'SECURE_BOOT'; then \
	       if echo $1 | grep -iE 'sdcard'; then \
                   cp $$opdir/spl/u-boot-spl.bin $(TOPDIR)/build/firmware/u-boot/$$brd/uboot_$${brd}_sdcard_spl.bin ; \
                   cp $$opdir/u-boot-dtb.bin $(TOPDIR)/build/firmware/u-boot/$$brd/uboot_$${brd}_sdcard_dtb.bin ; \
	       elif echo $1 | grep -iE 'nand'; then \
		   cp $$opdir/spl/u-boot-spl.bin $(TOPDIR)/build/firmware/u-boot/$$brd/uboot_$${brd}_nand_spl.bin ; \
		   cp $$opdir/u-boot-dtb.bin $(TOPDIR)/build/firmware/u-boot/$$brd/uboot_$${brd}_nand_dtb.bin ; \
	       fi; \
           fi; \
	else \
	    srcbin=u-boot-dtb.bin; \
	fi && \
	tgtbin=uboot_`echo $1|sed -r 's/(.*)(_.*)/\1/'`.bin && \
	cp $$opdir/$$srcbin $(TOPDIR)/build/firmware/u-boot/$$brd/$$tgtbin && \
	echo -e "\n$(TOPDIR)/build/firmware/u-boot/$$brd/$$tgtbin   [Done]";
endef
endif


.PHONY: uefi
uefi:
ifeq ($(CONFIG_BUILD_UEFI), y)
	@echo -e "\nBuilding UEFI ..."
	@$(call fetch-git-tree,qoriq-uefi-binary)
	@for brd in $(UEFI_MACHINE_LIST); do \
	       if [ ! -d $(TOPDIR)/build/firmware/uefi/$$brd ]; then mkdir -p $(TOPDIR)/build/firmware/uefi/$$brd ; fi; \
	        if [ ! -f $(TOPDIR)/build/firmware/uefi/$$brd/*RDB_EFI* ]; then \
	            cp qoriq-uefi-binary/$$brd/*.fd $(TOPDIR)/build/firmware/uefi/$$brd/; \
	        fi; \
	 done
	@if [ ! -d $(TOPDIR)/build/firmware/uefi/grub ]; then mkdir -p $(TOPDIR)/build/firmware/uefi/grub; fi
	@cp qoriq-uefi-binary/grub/BOOTAA64.EFI $(TOPDIR)/build/firmware/uefi/grub
endif


.PHONY: rcw
rcw:
ifeq ($(CONFIG_BUILD_RCW), y)
	@echo -e "\nBuilding RCW ..."
	@$(call fetch-git-tree,$(rcw_tree_name))
	@$(call fetch-git-tree,$(rcw_tree_name)-bin)
	@for brd in $(RCWSRCBRD); do $(MAKE) -C $(rcw_tree_name)/$$brd; done
	@if [ ! -d $(TOPDIR)/build/firmware/rcw ]; then mkdir -p $(TOPDIR)/build/firmware/rcw; fi
	@for brd in $(RCWSRCBRD); do \
	    if [ ! -d $(TOPDIR)/build/firmware/rcw/$$brd ]; then mkdir -p $(TOPDIR)/build/firmware/rcw/$$brd; fi; \
	    cd $(TOPDIR)/packages/firmware/$(rcw_tree_name)/$$brd &&  \
	    for tmpdir in `ls -l | grep ^d | awk '{print $$9}'`; do \
		cp -rf $$tmpdir $(TOPDIR)/build/firmware/rcw/$$brd; \
		rm -f $(TOPDIR)/build/firmware/rcw/$$brd/$$tmpdir/*.rcw; \
	    done; \
	 done
	@for brd in $(RCWBINBRD); do \
	    if [ ! -d $(TOPDIR)/build/firmware/rcw/$$brd ]; then mkdir -p $(TOPDIR)/build/firmware/rcw/$$brd; fi; \
	    cd $(TOPDIR)/packages/firmware/$(rcw_tree_name)-bin/$$brd &&  \
	    for tmpdir in `ls -l | grep ^d | awk '{print $$9}'`; do \
		cp -rf $$tmpdir $(TOPDIR)/build/firmware/rcw/$$brd; \
	    done; \
	 done
	@cd $(TOPDIR) && byteswaptcl=$(TOPDIR)/packages/firmware/$(rcw_tree_name)/tools/byte_swap.tcl  && \
	 for brd in $(RCWSRCBRD); do \
	   if [ $$brd = ls1046ardb ]; then \
	     rcw=`grep rcw_qspi_noswap=  manifest/$$brd.manifest | cut -d= -f2` && \
	     tclsh $$byteswaptcl $$rcw $$rcw.swap 8 && \
	     rcw=`grep rcw_qspi_sec_noswap=  manifest/$$brd.manifest | cut -d= -f2` && \
	     tclsh $$byteswaptcl $$rcw $$rcw.swap 8; \
	   fi; \
	 done
endif


.PHONY: ppa
ppa:
ifeq ($(CONFIG_BUILD_PPA), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding PPA source ... "
	@$(call fetch-git-tree,ppa-generic)
	@find ppa-generic/ppa -name soc-l* | cut -d/ -f3 | xargs -i mkdir -p $(TOPDIR)/build/firmware/ppa/{}
	@cd ppa-generic/ppa && \
	 if [ $(MACHINE) = all ]; then \
	     ./build rdb-fit all; for name in $(PPASUBDIRS); do cp -f $$name/build/obj/ppa.itb $(TOPDIR)/build/firmware/ppa/$$name; done; \
	     ./build frdm-fit ls1012 && cp -f soc-ls1012/build/obj/ppa.itb $(TOPDIR)/build/firmware/ppa/soc-ls1012/ppa_frdm.itb; \
	 elif [ $(MACHINE) = ls1012afrdm ]; then \
	     ./build frdm-fit ls1012 && cp -f soc-ls1012/build/obj/ppa.itb $(TOPDIR)/build/firmware/ppa/soc-ls1012/ppa_frdm.itb; \
	 else \
	     ./build rdb-fit ls`echo $(MACHINE)|tr -cd [:digit:]` && cp soc-ls`echo $(MACHINE)|tr -cd [:digit:]`/build/obj/ppa.itb \
	     $(TOPDIR)/build/firmware/ppa/soc-ls`echo $(MACHINE)|tr -cd [:digit:]`; \
	 fi
endif
endif

ppa-optee:
ifeq ($(CONFIG_BUILD_PPA), y)
ifeq ($(DESTARCH),arm64)
ifeq ($(CONFIG_BUILD_OPTEE_OS), y)
	@$(call fetch-git-tree,ppa-generic)
	@find ppa-generic/ppa -name soc-l* | cut -d/ -f3 | xargs -i mkdir -p $(TOPDIR)/build/firmware/ppa/{}
ifeq ($(MACHINE), all)
	@for brd in $(OPTEE_MACHINE_LIST); do $(call build-ppa-optee,$$brd) done
else
	@$(call build-ppa-optee,$(MACHINE))
endif

define build-ppa-optee
	for optee_brd in $(OPTEE_MACHINE_LIST); do \
		board=`echo $${optee_brd}|tr -cd [:digit:]`; \
		mcno=`echo $1|tr -cd [:digit:]`; \
		if [ "$$optee_brd" =  "$1" ]; then\
			echo "Building PPA source with OPTEE-OS for $$optee_brd"; \
			if [ ! -f $(TOPDIR)/packages/apps/optee_os/out/arm-plat-ls/core/tee_$1.bin ]; then \
				echo "Build OPTEE-OS first before starting PPA ..." ;\
				cd $(TOPDIR) && flex-builder -c optee_os -a $(DESTARCH) -m $1;\
			fi && \
			if [ ! -f $(TOPDIR)/packages/apps/optee_os/out/arm-plat-ls/core/tee_$1.bin ]; then \
				echo "Error building OPTEE-OS. Doing Nothing & Exiting ..." ;\
			else \
			       echo "Copying tee_$1.bin to ppa/soc-ls$$mcno before starting building PPA ..." ; \
			       cp $(TOPDIR)/packages/apps/optee_os/out/arm-plat-ls/core/tee_$1.bin\
				  $(TOPDIR)/packages/firmware/ppa-generic/ppa/soc-ls$$mcno/tee.bin ;\
			       cd $(TOPDIR)/packages/firmware/ppa-generic/ppa && ./build rdb-fit spd=on ls$$mcno;\
			       cp soc-ls$$mcno/build/obj/ppa.itb $(TOPDIR)/build/firmware/ppa/soc-ls$$mcno/;\
		       fi; \
		fi; \
	done
endef
endif
endif
endif

ifeq ($(CONFIG_BUILD_PPA_BIN), y)
	@echo -e "\nBuilding PPA binary ..."
	@$(call fetch-git-tree,qoriq-ppa-binary)
	@if [ ! -h $(TOPDIR)/build/firmware/ppa ]; then \
	 ln -s $(TOPDIR)/packages/firmware/qoriq-ppa-binary $(TOPDIR)/build/firmware/ppa; fi
endif


.PHONY: mc-utils
mc-utils:
	@$(call fetch-git-tree,mc-utils)
	@if [ ! -h $(TOPDIR)/build/firmware/mc-utils ]; then \
         ln -s $(TOPDIR)/packages/firmware/mc-utils $(TOPDIR)/build/firmware/mc-utils; fi
	@$(MAKE) -C mc-utils/config && echo Build mc-utils done!


bin-firmware: qoriq-fm-ucode qoriq-qe-ucode qoriq-mc-binary qoriq-firmware-cortina qoriq-engine-pfe-bin

.PHONY: qoriq-fm-ucode
qoriq-fm-ucode:
	@$(call fetch-git-tree,qoriq-fm-ucode)
	@if [ ! -h $(TOPDIR)/build/firmware/qoriq-fm-ucode ]; then \
	 ln -s $(TOPDIR)/packages/firmware/qoriq-fm-ucode $(TOPDIR)/build/firmware/qoriq-fm-ucode; fi


.PHONY: qoriq-qe-ucode
qoriq-qe-ucode:
	@$(call fetch-git-tree,qoriq-qe-ucode)
	@if [ ! -h $(TOPDIR)/build/firmware/qoriq-qe-ucode ]; then \
	 ln -s $(TOPDIR)/packages/firmware/qoriq-qe-ucode $(TOPDIR)/build/firmware/qoriq-qe-ucode; fi


.PHONY: qoriq-mc-binary
qoriq-mc-binary:
	@$(call fetch-git-tree,qoriq-mc-binary)
	@if [ ! -h $(TOPDIR)/build/firmware/qoriq-mc-binary ]; then \
	 ln -s $(TOPDIR)/packages/firmware/qoriq-mc-binary $(TOPDIR)/build/firmware/qoriq-mc-binary; fi


.PHONY: qoriq-firmware-cortina
qoriq-firmware-cortina:
	@$(call fetch-git-tree,qoriq-firmware-cortina)
	@if [ ! -h $(TOPDIR)/build/firmware/qoriq-firmware-cortina ]; then \
	 ln -s $(TOPDIR)/packages/firmware/qoriq-firmware-cortina $(TOPDIR)/build/firmware/qoriq-firmware-cortina; fi


.PHONY: qoriq-engine-pfe-bin
qoriq-engine-pfe-bin:
	@$(call fetch-git-tree,qoriq-engine-pfe-bin)
	@if [ ! -h $(TOPDIR)/build/firmware/qoriq-engine-pfe-bin ]; then \
	 ln -s $(TOPDIR)/packages/firmware/qoriq-engine-pfe-bin $(TOPDIR)/build/firmware/qoriq-engine-pfe-bin; fi


repo_fetch:
	@echo -e "\nfetch firmware repositories ..."
	@$(call repo-update,fetch,$(FIRMWARE_REPO_LIST))

repo_update_branch:
	@echo -e "\nfirmware repositories update for branch ..."
	@$(call repo-update,branch,$(FIRMWARE_REPO_LIST))

repo_update_tag:
	@echo -e "\napps repositories update for tag ..."
	@$(call repo-update,tag,$(FIRMWARE_REPO_LIST))

repo_update_latest:
	@echo -e "\nfirmware repositories update to latest HEAD commit ..."
	@$(call repo-update,update,$(FIRMWARE_REPO_LIST))

repo_update_commit:
	@echo -e "\nfirmware repositories update to specified commit ID ..."
	@$(call repo-update,commit,$(FIRMWARE_REPO_LIST))
