#####################################
#
# Copyright 2017 NXP
#
######################################

SHELL=/bin/bash
include $(TOPDIR)/configs/$(CONFIGLIST)
include $(TOPDIR)/include/repo.mk

OPTEE_MACHINE_LIST := ls1043ardb ls1046ardb ls1012ardb

.PHONY: all

all: $(APPS_REPO_LIST)
	@echo Build apps components Done!

.PHONY: restool
restool:
ifeq ($(CONFIG_BUILD_RESTOOL), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding restool ..."
	@$(call fetch-git-tree,restool)
	@$(MAKE) -C restool
	@$(MAKE) -C restool install && \
	 echo build restool done!
endif
endif


.PHONY: odp
odp:
ifeq ($(CONFIG_BUILD_ODP), y)
ifeq ($(DESTARCH),arm64)
	@$(call fetch-git-tree,odp)
	@curbrch=`cd $(KERNEL_PATH) && git branch -v | grep '^*' | cut -d' ' -f2` && \
	 kerneloutdir=$(TOPDIR)/build/linux/$(KERNEL_TREE)/$(DESTARCH)/output/$$curbrch && \
	 if [ ! -d $(TOPDIR)/build/linux/kernel/$(DESTARCH)/lib/modules ] || \
	 [ ! -f $$kerneloutdir/include/config/kernel.release ]; then cd $(TOPDIR) && \
	 flex-builder -c linux -a $(DESTARCH) -f $(CONFIGLIST) && cd -; fi && \
	 echo -e "\nBuilding ODP ..." && cd odp && ./bootstrap && \
	 ./configure --with-platform=linux-dpaa2 --host=aarch64-linux-gnu --with-cunit-path="$(RFSDIR)/usr" \
	 --enable-test-vald && $(MAKE) -j$(JOBS) && export PRE_INSTALL=$(DESTDIR) && $(MAKE) install && \
	 cd kern && echo -e "\nBuilding ODP-KNI ..." && export KERNEL_CFG_PATH=$$kerneloutdir && $(MAKE) -j$(JOBS) \
	 KERNEL_PATH=$$kerneloutdir && if [ -n "$(MODULES_PATH)" ]; then cp -f odpfsl_kni.ko $(MODULES_PATH)/extra; fi && \
	 echo build ODP done!
endif
endif


.PHONY: ofp
ofp:
ifeq ($(CONFIG_BUILD_OFP), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding OFP ..."
	@$(call fetch-git-tree,ofp)
	@cd ofp && ./bootstrap  && export CFLAGS="-I$(DESTDIR)/usr/local/include" && \
	 export LDFLAGS="-L$(RFSDIR)/lib/aarch64-linux-gnu/ -L$(DESTDIR)/lib" && \
	 ./configure CC=$(CROSS_COMPILE)gcc --host=aarch64-linux-gnu --disable-shared \
	 --with-odp=$(DESTDIR)/usr/local && $(MAKE) -j$(JOBS) && $(MAKE) install && \
	 echo build OFP done!
endif
endif


.PHONY: dpdk
dpdk:
ifeq ($(CONFIG_BUILD_DPDK), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding DPDK ..."
	@$(call fetch-git-tree,dpdk)
	@if [ ! -d $(DESTDIR)/usr/local/include/openssl ]; then echo building dependent openssl ... && \
	 cd $(TOPDIR) && flex-builder -c openssl -f $(CONFIGLIST); fi
	@cd $(TOPDIR)/packages/apps/dpdk && export CROSS=${CROSS_COMPILE} && export RTE_SDK=${PWD} && export RTE_TARGET=arm64-dpaa-linuxapp-gcc \
	 && export OPENSSL_PATH=$(DESTDIR)/usr/local && $(MAKE) install T=arm64-dpaa-linuxapp-gcc DESTDIR=${DESTDIR}/usr/local CONFIG_RTE_KNI_KMOD=n && cd - \
	 && echo -e "\nBuilding DPDK Examples ..." && cd ${TOPDIR}/packages/apps/dpdk/examples \
	 && export CROSS=${CROSS_COMPILE} && export RTE_SDK=${TOPDIR}/packages/apps/dpdk \
	 && export RTE_TARGET=arm64-dpaa-linuxapp-gcc && export OPENSSL_PATH=$(DESTDIR)/usr/local && export RTE_SDK_BIN=$(DESTDIR)/usr/local \
	 && $(MAKE) -j$(JOBS) -C l2fwd && $(MAKE) -j$(JOBS) -C l3fwd && $(MAKE) -j$(JOBS) -C l2fwd-crypto && $(MAKE) -j$(JOBS) -C vhost \
	 && $(MAKE) -j$(JOBS) -C ipsec-secgw && $(MAKE) -j$(JOBS) -C ip_fragmentation && $(MAKE) -j$(JOBS) -C ip_reassembly \
	 && find . -perm 755 -a -type f | xargs -I {} cp {} ${DESTDIR}/usr/local/bin \
	 && mkdir -p ${DESTDIR}/usr/local/dpdk/examples/ipsec_secgw \
	 && cp -f ipsec-secgw/*.cfg ${DESTDIR}/usr/local/dpdk/examples/ipsec_secgw && cd - \
	 && echo -e "\nInstalling dpdk-extras ...${DESTDIR} " && cp -rf ${TOPDIR}/packages/apps/dpdk/nxp/* ${DESTDIR}/usr/local/dpdk/ \
	 && rm -rf ${DESTDIR}/usr/local/share/dpdk/examples \
	 && cd ${DESTDIR}/usr/local/bin && rm -f dpdk-pdump && rm -f dpdk-pmdinfo && rm -f dpdk-procinfo && rm -f dpdk-test-crypto-perf && cd - && \
	 echo build DPDK done!
endif
endif


.PHONY: ovs-dpdk
ovs-dpdk:
ifeq ($(CONFIG_BUILD_OVS_DPDK), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding OVS_DPDK ..."
	@$(call fetch-git-tree,ovs-dpdk)
	@if [ ! -d $(RFSDIR)/usr/lib/aarch64-linux-gnu ]; then \
	 echo dependent $(RFSDIR) unpresent, building it ... && cd $(TOPDIR) && flex-builder -i mkrfs -f $(CONFIGLIST); cd -; fi; \
	 if [ ! -d $(DESTDIR)/usr/local/dpdk ]; then echo building dependent DPDK ... && \
	 cd $(TOPDIR) && flex-builder -c dpdk -f $(CONFIGLIST) && cd -; fi; \
	 cd ovs-dpdk && export CROSS=$(CROSS_COMPILE) && export RTE_SDK=$(TOPDIR)/packages/apps/dpdk && \
	 export RTE_TARGET=arm64-dpaa-linuxapp-gcc && ./boot.sh && export LDFLAGS="-L$(RFSDIR)/lib/aarch64-linux-gnu" && \
	 export LIBS="-ldl -lnuma" && ./configure --prefix=/usr/local --host=aarch64-linux-gnu \
	 --with-dpdk=$(DESTDIR)/usr/local --with-openssl=$(DESTDIR)/usr/local CFLAGS="-g -Wno-cast-align -Ofast \
	 -I$(DESTDIR)/usr/local/include/dpdk -lpthread" && $(MAKE) -j$(JOBS) install && cd - && \
	 echo build OVS_DPDK done!
endif
endif


.PHONY: flib
flib:
ifeq ($(CONFIG_BUILD_FLIB), y)
	@echo -e "\nBuilding FLIB ..."
	@$(call fetch-git-tree,flib)
	@$(MAKE) -C flib install
	@echo build flib done!
endif


.PHONY: fmlib
fmlib:
ifeq ($(CONFIG_BUILD_FMLIB), y)
	@echo -e "\nBuilding FMLIB ..."
	@$(call fetch-git-tree,fmlib)
	@if [ ! -d $(KERNEL_PATH)/include/uapi/linux/fmd ]; then echo building dependent kernel ... && \
	 cd $(TOPDIR) && flex-builder -c linux -a $(DESTARCH) -f $(CONFIGLIST) && cd -; fi && \
	 cd fmlib && export KERNEL_SRC=$(KERNEL_PATH) && \
	 $(MAKE) clean && $(MAKE) && $(MAKE) install-libfm-arm && \
	 echo build fmlib done!
endif


.PHONY: fmc
fmc:
ifeq ($(CONFIG_BUILD_FMC), y)
	@echo -e "\nBuilding FMC ..."
	@$(call fetch-git-tree,fmc)
	@if [ $(DESTARCH) = arm64 ]; then host=aarch64-linux-gnu; else host=arm-linux-gnueabihf; fi && \
	 if [ ! -d $(TOPDIR)/packages/apps/fmlib/include/fmd/Peripherals ]; then cd $(TOPDIR) && \
	 flex-builder -c fmlib -a $(DESTARCH) -f $(CONFIGLIST) && cd -; fi && if [ ! -d $(RFSDIR)/usr/include/libxml2 ]; then \
	 echo building dependent libxml in target rootfs ... && cd $(TOPDIR) && \
	 flex-builder -i mkrfs -a $(DESTARCH) -f $(CONFIGLIST) && cd -; fi && \
	 if [ -d $(RFSDIR)/usr/include/libxml2 -a ! -f $(RFSDIR)/usr/include/libxml2/libxml/parser.h ]; then \
	 echo Error:dependent libxml missing in incomplete rootfs, regenerating it .. && cd $(TOPDIR) && flex-builder \
	 -i clean-distrorfs -a $(DESTARCH) -f $(CONFIGLIST) && flex-builder -i mkrfs -a $(DESTARCH) -f $(CONFIGLIST) && cd -; fi && \
	 if [ ! -d $(KERNEL_PATH)/include/uapi/linux/fmd ]; then echo building dependent kernel && cd $(TOPDIR) \
	 && flex-builder -c linux -a $(DESTARCH) -f $(CONFIGLIST) && cd -; fi && \
	 if [ ! -f $(TOPDIR)/build/images/components_arm64/lib/libfm-arm.a ]; then echo building dependent fmlib && cd $(TOPDIR) \
	 && flex-builder -c fmlib -f $(CONFIGLIST) && cd -; fi && \
	 export LDFLAGS="-L$(RFSDIR)/lib/$$host -Wl,-rpath=$(RFSDIR)/lib/$$host:$(RFSDIR)/usr/lib/$$host" && \
	 export CFLAGS="-I$(RFSDIR)/usr/include/$$host -I$(TOPDIR)/packages/apps/fmlib/include/fmd \
	 -I$(TOPDIR)/packages/apps/fmlib/include/fmd/Peripherals \
	 -I$(TOPDIR)/packages/apps/fmlib/include/fmd/integrations" && \
	 $(MAKE) clean -C fmc/source && \
	 $(MAKE) FMD_USPACE_HEADER_PATH=$(KERNEL_PATH)/include/uapi/linux/fmd \
	 FMLIB_HEADER_PATH=$(TOPDIR)/packages/apps/fmlib/include \
	 LIBXML2_HEADER_PATH=$(RFSDIR)/usr/include/libxml2 \
	 FMD_USPACE_LIB_PATH=$(DESTDIR)/lib  TCLAP_HEADER_PATH=$(RFSDIR)/usr/include \
	 CXX=$(CROSS_COMPILE)g++ CC=$(CROSS_COMPILE)gcc -C fmc/source
	@install -d $(DESTDIR)/usr/local/bin; \
	 install -m 755 fmc/source/fmc $(DESTDIR)/usr/local/bin/fmc; \
	 install -d $(DESTDIR)/etc/fmc/config; \
	 install -m 644 fmc/etc/fmc/config/hxs_pdl_v3.xml $(DESTDIR)/etc/fmc/config; \
	 install -d $(DESTDIR)/usr/local/include/fmc; \
	 install fmc/source/fmc.h $(DESTDIR)/usr/local/include/fmc; \
	 install -d $(DESTDIR)/usr/local/lib/$$host; \
	 install fmc/source/libfmc.a $(DESTDIR)/usr/local/lib/$$host && \
	 echo build FMC done!
endif


.PHONY: cst
cst:
ifeq ($(CONFIG_BUILD_CST), y)
	@echo -e "\nBuilding CST ..."
	@$(call fetch-git-tree,cst)
	@cd cst && $(MAKE) -j$(JOBS)
	@if [ ! -f cst/srk.pri -o ! -f cst/srk.pub ]; then cd cst && ./gen_keys 1024 && \
	 echo "Generated new keys !"; else echo "Using default keys srk.pri and srk.pub"; fi && \
	 echo build CST done!
endif


.PHONY: usdpaa
usdpaa:
ifeq ($(CONFIG_BUILD_USDPAA), y)
	@echo -e "\nBuilding USDPAA ..."
	@$(call fetch-git-tree,usdpaa)
	@cd usdpaa && $(MAKE) EXTRA_CFLAGS=-I$(RFSDIR)/usr/include/libxml2 && \
	 echo build USDPAA done!
endif


.PHONY: aiopsl
aiopsl:
ifeq ($(CONFIG_BUILD_AIOPSL), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding AIOPSL ..."
	@$(call fetch-git-tree,aiopsl)
	@cd aiopsl && \
	 if [ ! -d $(DESTDIR)/usr/local/aiop/bin ]; then mkdir -p $(DESTDIR)/usr/local/aiop/bin; fi && \
	 cp -rf misc/setup/scripts $(DESTDIR)/usr/local/aiop  && \
	 cp -rf misc/setup/traffic_files $(DESTDIR)/usr/local/aiop && \
	 cp -rf demos/images/* $(DESTDIR)/usr/local/aiop/bin && \
	 echo build AIOPSL done!
endif
endif


.PHONY: ceetm
ceetm:
ifeq ($(CONFIG_BUILD_CEETM), y)
	@echo -e "\nBuilding CEETM ..."
	@$(call fetch-git-tree,ceetm)
	@cd ../apt-packages/source-pkg && ./iproute2.sh
	@export IPROUTE2_DIR=$(TOPDIR)/packages/apt-packages/source-pkg/pkg-iproute && \
	 cd ceetm && $(MAKE) clean && $(MAKE) && $(MAKE) install && \
	 echo build CEETM done!
endif


.PHONY: dce
dce:
ifeq ($(CONFIG_BUILD_DCE), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding DCE ..."
	@$(call fetch-git-tree,dce)
	@cd dce && $(MAKE) && $(MAKE) install && \
	 echo build DCE done!
endif
endif


.PHONY: optee_os
optee_os:
ifeq ($(CONFIG_BUILD_OPTEE_OS), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding OP-TEE OS ..."
	@$(call fetch-git-tree,optee_os)
	@if [ $(MACHINE) = "all" ]; then\
		for brd in $(OPTEE_MACHINE_LIST); do\
			cd $(TOPDIR)/packages/apps/optee_os && $(MAKE) -j$(JOBS) CFG_ARM64_core=y PLATFORM=ls-$$brd ARCH=arm;\
			$(CROSS_COMPILE)\objcopy -v -O binary out/arm-plat-ls/core/tee.elf out/arm-plat-ls/core/tee_$$brd.bin;\
		done;\
	else\
		cd $(TOPDIR)/packages/apps/optee_os && $(MAKE) CFG_ARM64_core=y PLATFORM=ls-$(MACHINE) ARCH=arm;\
		$(CROSS_COMPILE)\objcopy -v -O binary out/arm-plat-ls/core/tee.elf out/arm-plat-ls/core/tee_$(MACHINE).bin;\
	fi && echo build OPTEE-OS done!
endif
endif


.PHONY: optee_client
optee_client:
ifeq ($(CONFIG_BUILD_OPTEE_CLIENT), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding OP-TEE Client ..."
	@$(call fetch-git-tree,optee_client)
	@cd optee_client && $(MAKE) -j$(JOBS) ARCH=arm64 && echo build OPTEE-Client done!

endif
endif


.PHONY: optee_test
optee_test:
ifeq ($(CONFIG_BUILD_OPTEE_TEST), y)
ifeq ($(DESTARCH),arm64)
ifeq ($(CONFIG_BUILD_OPTEE_CLIENT), y)
	@if [ ! -f $(TOPDIR)/build/images/components_arm64/lib/libteec.so.1.0 ]; then \
		echo "Building Optee-client";\
		cd $(TOPDIR) && flex-builder -c optee_client -a $(DESTARCH) -m $(MACHINE);\
	fi;
	@echo -e "\nBuilding OP-TEE TEST ..."
	@$(call fetch-git-tree,optee_test)
	@cd $(TOPDIR)/packages/apps/optee_client && $(MAKE) -j$(JOBS) ARCH=arm64;
	@cd optee_test && $(MAKE) CFG_ARM64=y OPTEE_CLIENT_EXPORT=$(TOPDIR)/build/images/components_arm64/ \
	 TA_DEV_KIT_DIR=$(TOPDIR)/packages/apps/optee_os/out/arm-plat-ls/export-ta_arm64/
	@if [ ! -d $(TOPDIR)/build/images/components_arm64/lib/optee_armtz ]; then \
		mkdir $(TOPDIR)/build/images/components_arm64/lib/optee_armtz;\
	fi;
	@cp $(TOPDIR)/packages/apps/optee_test/out/ta/*/*.ta $(TOPDIR)/build/images/components_arm64/lib/optee_armtz/;
	@cp $(TOPDIR)/packages/apps/optee_test/out/xtest/xtest $(TOPDIR)/build/images/components_arm64/bin/;
	@echo build OPTEE-Test done!
endif
endif
endif


.PHONY: openssl
openssl:
ifeq ($(CONFIG_BUILD_OPENSSL), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding OpenSSL ..."
	@$(call fetch-git-tree,openssl)
	@if [ ! -d $(DESTDIR)/usr/local/include/crypto ]; then cd $(TOPDIR) && flex-builder -c cryptodev-linux -f $(CONFIGLIST); fi
	@cd openssl && ./Configure -DHAVE_CRYPTODEV -I$(DESTDIR)/usr/local/include \
	 linux-aarch64 shared --prefix=$(DESTDIR)/usr/local --openssldir=$(DESTDIR)/usr/local/openssl && \
	 $(MAKE) depend && $(MAKE) && $(MAKE) install && \
	 echo build OpenSSL done!
endif
endif


.PHONY: openstack-demo
openstack-demo:
ifeq ($(CONFIG_BUILD_OPENSTACK_DEMO), y)
	@echo -e "\nBuilding OpenStack Demo ..."
	@$(call fetch-git-tree,openstack-demo)
	@cd openstack-demo && $(MAKE) && \
	 echo build OPENSTACK_DEMO done!
endif


.PHONY: gpp-aioptool
gpp-aioptool:
ifeq ($(CONFIG_BUILD_GPP_AIOPTOOL), y)
	@echo -e "\nBuilding AIOP Tool ..."
	@$(call fetch-git-tree,gpp-aioptool)
	@cd gpp-aioptool && $(MAKE) clean && $(MAKE) && $(MAKE) install && \
	 echo build GPP_AIOPTOOL done!
endif


.PHONY: qbman_userspace
qbman_userspace:
ifeq ($(CONFIG_BUILD_QBMAN_USERSPACE), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding QBMAN_USERSPACE ..."
	@$(call fetch-git-tree,qbman_userspace)
	@cd qbman_userspace && export PREFIX=/usr/local && export ARCH=aarch64 && \
	 $(MAKE) && $(MAKE) install && \
	 echo build QBMAN_USERSPACE done!
endif
endif


.PHONY: eth-config
eth-config:
ifeq ($(CONFIG_BUILD_ETH_CONFIG), y)
	@echo -e "\nBuilding ETH_CONFIG ..."
	@$(call fetch-git-tree,eth-config)
	@mkdir -p $(DESTDIR)/etc/fmc/config
	@cd eth-config && cp -rf private $(DESTDIR)/etc/fmc/config && \
	 cp -rf shared_mac $(DESTDIR)/etc/fmc/config && \
	 echo build ETH_CONFIG done!
endif


.PHONY: ptpd
ptpd:
ifeq ($(CONFIG_BUILD_PTPD), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding PTPd ..."
	@$(call fetch-git-tree,ptpd)
	@if [ $(DESTARCH) = arm64 ]; then host=aarch64-linux-gnu; else host=arm-linux-gnueabihf; fi && \
	 if [ ! -f $(RFSDIR)/lib/$$host/libpcap.so ]; then echo dependent $(RFSDIR)/lib/$$host/libpcap.so unpresent, building it ... && \
         cd $(TOPDIR) && flex-builder -i mkrfs -a $(DESTARCH) -f $(CONFIGLIST); cd -; fi; \
	 if [ $(DESTARCH) = arm32 ]; then export CROSS_COMPILE=arm-linux-gnueabihf-; fi && \
	 cd ptpd && export LDFLAGS="-L$(RFSDIR)/lib/$$host" && autoreconf -vi && \
	 ./configure --host=$$host ac_cv_func_malloc_0_nonnull=yes && $(MAKE) && $(MAKE) install && \
	 echo build PTPd done!
endif
endif


.PHONY: crconf
crconf:
ifeq ($(CONFIG_BUILD_CRCONF), y)
	@echo -e "\nBuilding CRCONF ..."
	@$(call fetch-git-tree,crconf)
	@if [ -d code -a ! -d crconf ]; then mv code crconf; fi
	@sed -i -e 's/CC =/CC ?=/' -e 's/DESTDIR=/DESTDIR?=/' crconf/Makefile
	@cd crconf && export CC=$(CROSS_COMPILE)gcc && export DESTDIR=${DESTDIR}/usr/local && \
	 $(MAKE) clean && $(MAKE) && $(MAKE) install && echo build CRCONF done!
endif


.PHONY: edgescale-eds
edgescale-eds:
ifeq ($(CONFIG_BUILD_EDGESCALE_EDS), y)
	@echo -e "\nBuilding EDGESCALE_EDS ..."
	@$(call fetch-git-tree,edgescale-eds)
	@if [ ! -f $(RFSDIR)/usr/aptpkg/.rfsblddone ]; then echo target $(RFSDIR) unpresent, generating it ... && \
	 cd $(TOPDIR) && flex-builder -i mkrfs -f $(CONFIGLIST); cd -; fi && \
	 cd edgescale-eds && $(MAKE) -j$(JOBS) && $(MAKE) install INSTALL_DIR=$(RFSDIR) && \
	 echo build EDGESCALE_EDS done!
endif


.PHONY: eds-kubelet
eds-kubelet:
ifeq ($(CONFIG_BUILD_EDS_KUBELET), y)
ifeq ($(DESTARCH),arm64)
	@echo -e "\nBuilding eds-kubelet ..."
	@$(call fetch-git-tree,eds-kubelet)
	@if [ ! -f $(RFSDIR)/usr/aptpkg/.rfsblddone ]; then echo target $(RFSDIR) unpresent, generating it ... && \
	 cd $(TOPDIR) && flex-builder -i mkrfs -f $(CONFIGLIST); cd -; fi && \
	 cd eds-kubelet && $(MAKE) && $(MAKE) install INSTALL_DIR=$(RFSDIR) && \
	 echo build eds-kubelet done!
else
	@echo eds-kubelet for $(DESTARCH) is not supported yet.
endif
endif


.PHONY: eds-puppet
eds-puppet:
ifeq ($(CONFIG_BUILD_EDS_PUPPET), y)
	@echo -e "\nsetup eds-puppet ..."
	@$(call fetch-git-tree,eds-puppet)
	@if [ ! -f $(RFSDIR)/usr/aptpkg/.rfsblddone ]; then echo target $(RFSDIR) unpresent, generating it ... && \
	 cd $(TOPDIR) && flex-builder -i mkrfs -f $(CONFIGLIST); cd -; fi && \
	 if [ ! -f $(RFSDIR)/proc/uptime ];  then sudo chroot $(RFSDIR) mount -t proc proc /proc; fi && \
	 if [ ! -f $(RFSDIR)/usr/bin/ruby ]; then sudo chroot $(RFSDIR) apt install -y ruby-full rubygems-integration; fi && \
	 if [ ! -f $(RFSDIR)//usr/local/bin/puppet ]; then sudo chroot $(RFSDIR) gem install puppet --version 4.10.9 -N; fi && \
	 $(MAKE) -C eds-puppet install INSTALL_DIR=$(RFSDIR) && \
         echo setup eds-puppet done!
endif


repo_fetch:
	@echo -e "\nfetch apps repositories ..."
	@$(call repo-update,fetch,$(APPS_REPO_LIST))

repo_update_branch:
	@echo -e "\napps repositories update for branch ..."
	@$(call repo-update,branch,$(APPS_REPO_LIST))

repo_update_tag:
	@echo -e "\napps repositories update for tag ..."
	@$(call repo-update,tag,$(APPS_REPO_LIST))

repo_update_latest:
	@echo -e "\napps repositories update to latest HEAD commit ..."
	@$(call repo-update,update,$(APPS_REPO_LIST))

repo_update_commit:
	@echo -e "\napps repositories update to specified commit ID ..."
	@$(call repo-update,commit,$(APPS_REPO_LIST))
