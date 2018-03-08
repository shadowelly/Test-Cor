Flexbuild Overview
--------------------------------------------------------------------------------------------------------------------------------------
Flexbuild is a component-oriented integrated build framework with capabilities of flexible system build and distro installation.
It builds desired separate images, composite firmware and packages for various components (linux, u-boot, rcw, ppa,
miscellaneous userspace applications) and Debian/Ubuntu based distro userland.

The following are Flexbuild's main features:
- automatically build linux, u-boot, ppa, rcw and miscellaneous userspace applications
- generate machine-specific composite firmware for various kinds of sd/qspi/nor boot, secure boot, uboot-based/uefi-based boot
- support integrated management with repo-fetch, repo-branch, repo-commit, repo-tag, repo-update for git repositories of all
  components (linux, uboot, uefi, rcw, ppa, fmc, fmlib, restool, openssl, odp, dpdk, ovs-dpdk, qbman_userspace, ceetm, etc)
- support cross build on x86 Ubuntu 16.04 host machine for aarch64/armhf arch target
- support native build on aarch64/armhf machine for ARM arch target
- support building in Docker container hosted on CentOS, RHEL, Fedora, SUSE, Ubuntu, Debian, etc
- scalability of integrating various components of both system firmware and userspace applications.
- capability of generating custom aarch64/armhf Debian/Ubuntu userland integrated configurable packages and proprietary components.

With Flexbuild, LSDK components and custom packages can be built using Ubuntu/Linaro toolchain on Linux host machine or ARM target board.
Flexbuild builds each component and generate the composite firmware (including RCW, U-Boot/UEFI, PHY firmware, kernel, dtb, ramdiskrfs)
for specific or all target platforms and the Ubuntu userland containing the specified packages and apps components.



LSDK Quick Start
-------------------------------------------------------------------------------------------------------------------------------------
	Select the appropriate way of build mechanism:
	  To accommodate various build environments, LSDK provides three ways of build mechanism, please refer to the following recommended way.

	  - Scenario 1: If Ubuntu 16.04 or above is installed on the host machine, the following way of direct cross-build is recommended
	    Prerequisites:
	    - non-root users need achieve sudo permission by running command 'sudoedit /etc/sudoers' and adding lines as below in /etc/sudoers:
	       <user-account-name> ALL=(ALL) NOPASSWD: ALL
	    - For root user, there is no limitation for the build.
	    - To build the Ubuntu userland, the user's network environment must have access to the remote Ubuntu official server.


	  - Scenario 2: If other Linux distro (CentOS, RHEL, Fedora, SUSE, Ubuntu, Debian) is installed on host machine, the way of indirect cross-build in Docker can be used.
	    Please install Docker on the host machine. https://docs.docker.com/engine/installation can be a reference for Docker installation
	    Prerequisites:
	    - Docker is installed on the host machine and non-root user has achieved sudo permission for 'docker' command or has been added to a group of docker as below:
		$ sudo newgrp - docker
		$ sudo usermod -aG docker <accountname>
		$ sudo gpasswd -a <accountname> docker
		$ sudo service docker restart
		Logout from current terminal session, then login again to ensure user can run 'docker ps -a'
	    - To build the Ubuntu userland, the user's network environment must have access to the remote Ubuntu official server.
	    Steps:
	    $ cd flexbuild
	    $ source setup.env  (on host machine)
	    $ run 'flex-builder docker', then click Enter key
	    $ source setup.env  (in docker container environment)
	    if necessary, user can exit from docker container without change current work path by executing 'exit' command.


	  - Scenario 3: If the Ubuntu userland had already been deployed on target board, the way of native build on target board can be used.
	    Prerequisites:
	    - Please set the system date and time by 'date -s' command
	    - To build the Ubuntu userland, the user's network environment must have access to the remote Ubuntu official server.


	Assemble and deploy LSDK on target board with prebuilt images except distro userland
	- Step 1: Login Ubuntu 16.04 host machine and download the flexbuild repository
	  $ cd flexbuild
	  $ source setup.env

	- Step 2: Download prebuilt bootpartition and components tarballs
	  $ wget http://www.nxp.com/lgfiles/sdk/lsdk1712/bootpartition_arm64_lts_4.9.tgz
	 or wget http://www.nxp.com/lgfiles/sdk/lsdk1712/bootpartition_arm64_lts_4.4.tgz
	  $ wget http://www.nxp.com/lgfiles/sdk/lsdk1712/components_arm64.tgz

	- Step 3: Generate distro userland, untar prebuilt components tarball and merge it into distro userland
	  $ flex-builder -i mkrfs -a arm64
	  $ tar xvzf components_arm64.tgz -C build/images
	  $ tar xvzf arm64-modules-<kernel_version>.tgz -C build/images/ubuntu_xenial_arm64_rootfs.d/lib/modules
	  $ tar xvzf linux-lib-firmware.tgz -C build/images/ubuntu_xenial_arm64_rootfs.d/lib
	  $ flex-builder -i merge-component -a arm64

	- Step 4: Install LSDK onto SD/USB/SATA storage drive
	  $ flex-installer -b bootpartition_arm64_lts_4.9.tgz -r build/images/ubuntu_xenial_arm64_rootfs.d -m <machine> -d /dev/sdX
	  The SD/USB/SATA storage drive in the Linux PC will be called /dev/sdX, where X is a letter such as a, b, c ...
	  Be very careful to choose the correct device name because data on this device will be replaced.
	  After the commands above complete, remove the SD/USB/SATA storage drive from the Linux PC machine and connect it to the target board.
	  It is assumed that the board has a working U-Boot, perhaps from a prior software release.
	  Power on the board and press a key if necessary to get a U-Boot prompt, then system will automatically boot Ubuntu distro.



How to build various components and Ubuntu based userland with Flexbuild
--------------------------------------------------------------------------------------------------------------------------------------
	$ cd flexbuild
	$ source setup.env
	$ flex-builder -h
	option:
	 -m, --machine	      target machine, supports ls1012ardb, ls1012afrdm, ls1021atwr, ls1043ardb, ls1046ardb,
			      ls1088ardb, ls2088ardb, etc, default all machines if unspecified
	 -a, --arch	      target architect of processor, valid argument: 'arm64', 'arm32', default arm64 if unspecified
	 -b, --boottype	      type of boot media, valid argument: 'nor', 'sd', 'qspi', 'nand', default all types if unspecified
	 -j, --jobs	      number of parallel build jobs, default 16 if unspecified
	 -c, --component      specify single or all components to build, valid argument:
	     linux	      build linux component
	     uboot	      build uboot component
	     fmc	      build fmc component
			      alike all other valid component(e.g. ppa, rcw, restool, dpdk, odp, etc) can be built separately
	     firmware	      build all firmware components (ppa, rcw, uboot, uefi, fm-ucode, qe-ucode, mc-utils, etc)
	     apps	      build all apps components (restool, flib, fmlib, fmc, cst, openssl, odp, ofp, dpdk, ovs-dpdk,
			      aiopsl, ceetm, gpp-aioptool, qbman, ptpd, crconf, etc)
	 -f, --cfgfile	      specify config file, default build_lsdk.cfg is used if unspecified
	 -B, --buildarg	      secondary build argument for misc build commands
	 -i, --instruction    instruction to generate target packages/images or manage component repositories, valid argument:
	     mkfw	      generate composite firmware image for the specified type of nor/qspi/sd boot
	     mkallfw	      generate all composite firmware in all nor/qspi/sd boottype for non-secure and secure boot
	     mkitb	      generate flex_linux_<arch>.itb
	     mkbootpartition  generate boot partition tarball including kernel, dtb, composite firmware, autoboot script, etc
	     mkrfs	      generate raw distro rootfs without custom components
	     mkguestrfs	      convert raw distro rootfs to ext4 rootfs used for qemu guest
	     mkdistroscr      generate distro autoboot script for all machines
	     signimg	      sign images and secure boot headers for specified machine
	     merge-component  merge custom component packages and kernel modules into distro rootfs
	     autobuild-all    autobuild all firmware, kernel, distro userland with apps components for single or all <arch> platforms
	     distclean	      clean all generated images except distro rootfs
	     clean-apps	      clean all generated packages of apps components
	     clean-distrorfs  clean distro rootfs
	     compressrfs      compress distro rootfs as .tgz
	     compressapps     compress apps components as .tgz
	     repo-fetch	      fetch single or all git repositories if not present locally
	     repo-branch      set single or all git repositories to specified branch
	     repo-update      update single or all git repositories to latest HEAD
	     repo-commit      set single or all git repositories to specified commit
	     repo-tag	      set single or all git repositories to specified tag
	     repo-change      change git repo tree and branch
	     list	      list enabled config, machines and components
	 -s, --secure	      enable secure boot option used for generating composite firmware
	 -e, --encapsulation  enable encapsulation and decapsulation feature for secure boot
	 -v, --version	      print the version
	 -h, --help	      print help info

	 for example:
	 flex-builder -i autobuild-all -m <machine>	      # autobuild all firmware, kernel, userland for specified platform
	 flex-builder -i mkrfs -a <arch>		      # generate distro rootfs without custom components
	 flex-builder -i mkbootpartition -a <arch>	      # generate boot partition tarball for all <arch> platforms
	 flex-builder -i merge-component -a <arch>	      # merge all components packages and kernel modules into target distro rootfs
	 flex-builder -i repo-fetch			      # fetch git repositories of all components
	 flex-builder -i repo-update			      # update all git repositories to latest HEAD
	 flex-builder -c ppa-optee -m ls1046ardb	      # build ppa with OPTEE enabled for ls1046ardb
	 flex-builder -c uboot -m ls1043ardb -b sd	      # build uboot for ls1043ardb to generate sd boot image
	 flex-builder -c uboot -m all			      # build uboot with all sd/nor/qspi boottype for all machines
	 flex-builder -c linux -a <arch>		      # build linux for all <arch> machines
	 flex-builder -c firmware -m <machine>		      # build all firmware for <machine> into build/firmware
	 flex-builder -c apps -a <arch>		              # build all apps components into build/images/components_<arch>
	 flex-builder -i mkfw -m ls1043ardb -b sd -B uboot -s # generate uboot-based composite firmware for SD secure boot on ls1043ardb
	 flex-builder -i mkfw -m ls2088ardb -b nor -B uefi    # generate uefi-based composite firmware for NOR boot on ls2088ardb
	 flex-builder -i signimg -m ls1046ardb -b sd	      # sign images and secure boot headers for secure boot on ls1046ardb
	 flex-builder -i mkguestrfs -a <arch> -B 3G	      # convert raw distro rootfs to ext4 rootfs used for qemu guest
	 flex-builder -i mkitb -a <arch>		      # generate flex_linux_<arch>.itb used for all <arch> machines
	 flex-builder -i mkallfw -m ls1046ardb -b sd	      # generate all images(including uboot-based, uefi-based, secureboot) for sd boot
	 flex-builder -i autobuild-all -a <arch>	      # autobuild all firmware, kernel, userland for all <arch> platforms
	 flex-builder all				      # autobuild all firmware, kernel, userland for all arm64+arm32 platforms
	 flex-builder docker				      # create or attach to docker container and run flexbuild in docker
	 flex-builder distclean			              # clean all generated images except distro rootfs


How to select various git trees and branches for linux and uboot to build in Flexbuild
--------------------------------------------------------------------------------------------------------------------------------------
	- run  flex-builder -i repo-change -B <component>=<repo-name>,branch=<branch-name>
	  e.g. flex-builder -i repo-change -B linux=linux-devel,branch=next
	       flex-builder -i repo-change -B uboot=uboot-devel,branch=devel

	To use a private linux git repo instead of the official git repo:
	- put a private git repo in packages/linux directory
	- run flex-builder -i repo-change -B linux=<custom-linux-repo>,branch=<custom-branch>
	- run flex-builder -c linux -a <arch>


How to generate boot partition image for distro in Flexbuild
--------------------------------------------------------------------------------------------------------------------------------------
	$ flex-builder -i mkbootpartition -a arm64
	$ flex-builder -i mkbootpartition -a arm32
	Generate all needed images including kernel image, dtbs, bootscripts for U-Boot, grub.conf for UEFI, flex_linux_<arch>.itb,
	small ramdiskrfs, etc, flex-builder will automatically build dependent images if unpresent.


How to build Linux kernel in Flexbuild
--------------------------------------------------------------------------------------------------------------------------------------
	Run following command to build kernel with default config:
	$ flex-builder -c linux                          # for 64-bit mode of all armv8 platforms
	$ flex-builder -c linux -a arm32                 # for 32-bit mode of all armv8 platforms
	$ flex-builder -c linux -a arm32 -m ls1021atwr   # for 32-bit armv7 ls1021atwr

	To customize Linux kernel in Flexbuild:
	If user wants to change default kernel config to customize kernel, there are two ways:
	way 1: first run 'flex-builder -c linux -B menuconfig' to customize kernel config in interactive menu
	       then run 'flex-builder -c linux' to build kernel with custom config

	way 2: put user specific configs (e.g. custom1.config, custom2.config) in packages/linux/<kernel>/arch/arm64/configs and run
	       $ flex-builder -c linux -B fragment:"custom1.config custom2.config"

	For arm64: user can replace the default kernel Image with new custom Image in /boot directory and update modules
	           in /lib/modules directory in rootfs on target board.
	For arm32: user can replace the default uImage.v7/v8 with custom kernel in /boot directory and update
	           modules in /lib/modules directory in rootfs on target board.



How to build u-boot in Flexbuild
--------------------------------------------------------------------------------------------------------------------------------------
	$ flex-builder -c uboot -m <machine> -b <boottype>  # build uboot for <machine> to generate specified nor/sd/qspi boot image
	$ flex-builder -c uboot -m <machine>                # build uboot for <machine> to generate all nor/sd/qspi boot images
	$ flex-builder -c uboot -m all                      # build uboot for all machines to generate all nor/sd/qspi boot images
	optionally with -B quiet|verbose|keep-going



How to build apps components in Flexbuild
--------------------------------------------------------------------------------------------------------------------------------------
	$ flex-builder -c <component> -a <arch>		# build single application component
	$ flex-builder -c apps -a arm64                 # build all apps components for arm64 arch
	$ flex-builder -c odp				# build ODP component for all supported arm64 machines
	$ flex-builder -c dpdk                          # build DPDK component for all supported arm64 machines
	$ flex-builder -c ovs-dpdk                      # build OVS-DPDK component for all supported arm64 machines
	$ flex-builder -c fmc -a <arch>                 # build FMC component for arm32 arch, default for arm64 if arch is unspecified
	$ flex-builder -c restool			# build RESTOOL component for arm64 arch


How to add new apps component in Flexbuild
--------------------------------------------------------------------------------------------------------------------------------------
	- Add new <component-name> to apps_repo_list and set CONFIG_BUILD_<component-name>=y in configs/build_xx.cfg
	- Configure url/branch/tag/commit info for new <component_name> in configs/build_lsdk.cfg, default remote
	  component git repository is specified by GIT_REPOSITORY_URL if <component>_url is not specified, or directly
	  create the new component git repository in packages/apps directory
	- Add build target support for the new component in packages/apps/Makefile
	- Run 'flex-builder -c <component-name> -a <arch>' to build new component
	- Run 'flex-builder -i merge-component -a <arch>' to merger new component package to distro userland


How to use different build config instead of default build config
--------------------------------------------------------------------------------------------------------------------------------------
	Put a custom config file called 'build_custom.cfg' in configs directory, flexbuild
	will preferentially select build_custom.cfg, otherwise, default config file build_lsdk.cfg will be used.
	If there are multiple build_xx.cfg config files in configs directory, user can specify one of them by -f option as below:
	e.g. $ flex-builder -i autobuild-all -a arm64 -f <build_xx.cfg>



How to program firmware to SD/NOR/QSPI flash media
--------------------------------------------------------------------------------------------------------------------------------------
	- For SD/eMMC card
	   On all platforms:
	   $ wget http://www.nxp.com/lgfiles/sdk/lsdk1712/firmware_<machine>_uboot_sdboot.img
	   => tftp a0000000 firmware_<machine>_uboot_sdboot.img
	   or
	   => load mmc 0:2 a0000000 firmware_<machine>_uboot_sdboot.img
	   Under U-Boot:
	   => mmc write a0000000 8 1fff8
	   Under Linux:
	   $ flex-installer -f firmware_<machine>_uboot_sdboot.img -s 8 -d /dev/mmcblk0 (or /dev/sdx)

	- For IFC-NOR flash
	   On LS1043ARDB, LS1021ATWR
	   => load mmc 0:2 a0000000 firmware_<machine>_uboot_norboot.img
	   or tftp a0000000 firmware_<machine>_uboot_norboot.img
	   To program alternate bank:
	   => protect off 64000000 +$filesize && erase 64000000 +$filesize && cp.b a0000000 64000000 $filesize
	   To program current bank
	   => protect off 60000000 +$filesize && erase 60000000 +$filesize && cp.b a0000000 60000000 $filesize

	   On LS2088ARDB:
	   => load mmc 0:2 a0000000 firmware_ls2088ardb_uboot_norboot.img
	   or tftp a0000000 firmware_ls2088ardb_uboot_norboot.img
	   To program alternate bank:
	   => protect off 584000000 +$filesize && erase 584000000 +$filesize && cp.b a0000000 584000000 $filesize
	   To program current bank:
	   => protect off 580000000 +$filesize && erase 580000000 +$filesize && cp.b a0000000 580000000 $filesize

	- For QSPI flash
	   On LS1012ARDB, LS1046ARDB, LS1088ARDB
	   => load mmc 0:2 a0000000 firmware_<machine>_uboot_qspiboot.img
	   or tftp a0000000 firmware_<machine>_uboot_qspiboot.img
	   => sf probe 0:1
	   => sf erase 0 +$filesize && sf write 0xa0000000 0 $filesize


How to generate a custom ubuntu root filesystem with custom additional package list during build stage
--------------------------------------------------------------------------------------------------------------------------------------
	In Flexbuild there are two default additional package lists: additional_packages_list_full(plentiful packages) and
	additional_packages_list_minimal(less packages).
	  $ flex-builder -i mkrfs -a <arch>    (default use additional_packages_list_full)
	  $ flex-builder -i mkrfs -a <arch> -B additional_packages_list_minimal
	optionally, user can modify additional_packages_list ondemand or put a new file <custom_packages_list> in packages/apt-packages
	  $ flex-builder -i mkrfs -a <arch> -B <custom_packages_list>

	If users want to build a different Ubuntu distro (e.g. artful 17.10) instead of the default ubuntu xenial(16.04), users
	can change default 'CODENAME=xenial' to 'CODENAME=artful' in configs/build_xx.cfg, then run
	e.g. $ flex-builder -i mkrfs -a arm64 -B additional_packages_list_xx -f build_xx.cfg

	If users want to quickly install a new package to build/images/<codename>_<arch>_rootfs.d filesystem, run:
	  $ sudo chroot build/images/ubunutu_xenial_arm64_rootfs.d
	  $ apt-get install <new_package_name>


How to add extrinsic package unsupported by official to ubuntu userland during build stage
--------------------------------------------------------------------------------------------------------------------------------------
	- add extrinsic package name to extrinsic_packages_list in packages/apt-packages/additional_packages_list
	- put custom script of extrinsic package to packages/apt-packages/extrinsic-pkg (e.g. refer to nginx.sh)
	- run flex-builder -i mkrfs -a <arch> to generate new Ubuntu rootfs
	- install the new Ubuntu rootfs to target machine via flex-installer


How to install distro with flex-installer
--------------------------------------------------------------------------------------------------------------------------------------
	Install custom distro to SD/USB/SATA disk on Linux host machine or QorIQ development board
	1. Plug SD/USB/SATA storage device to Linux Host machine or QorIQ board

	2. Install distro to SD/USB/SATA storage device
	Prerequisites: non-root users need achieve sudo permission by adding '<user-account-name> ALL=(ALL) NOPASSWD: ALL' in /etc/sudoers.
	 - If the prebuilt distro tarball generated by flexbuild is available on Linux host machine, run
	   $ flex-installer -b bootpartition_arm64_xx.tgz -r ubuntu_xenial_arm64_rootfs_xx.tgz -m <machine> -d /dev/sdx

	 - If users want to directly install disrto rootfs to SD/USB/SATA disk on QorIQ board on which Linux is unavailable
	   Download prebuilt image by 'wget http://www.nxp.com/lgfiles/sdk/lsdk1712/flex_linux_arm64.itb'
	   or locally build it by command 'flex-builder -i mkitb -a arm64' to generate flex_linux_arm64.itb
	   put flex_linux_<arch>.itb to tftp server directory.
	   For arm64 QorIQ board:
	   => tftp a0000000 flex_linux_arm64.itb
	   => bootm a0000000#<board-name>
	   e.g. <board-name>: ls1012ardb, ls1012afrdm, ls1043ardb, ls1046ardb, ls1088ardb, ls2088ardb

	   For arm32 QorIQ board:
	   => tftp a0000000 flex_linux_arm32.itb
	   => bootm a0000000#<board-name>
	   e.g. <board-name>: ls1012ardb, ls1021atwr, ls1043ardb, ls1046ardb

	   After booting and login Linux on QorIQ board, download prebuilt distro tarballs generated by flexbuild and install as below:
	   step 1: $ flex-installer -i pf --device=sd|usb|sata
	   step 2: $ cd /run/media/{mmcblk0p3 or sdx3}, then download distro images to sd/usb/sata disk partitioned by step 1
	   step 3: $ flex-installer -i install -b bootpartition_arm64_xx.tgz -r ubuntu_xenial_arm64_rootfs_xx.tgz -m <machine> -d sd|usb|sata

	3. Power on or reboot target board after finishing the distro installation,  QorIQ machine would enter bootloader(U-Boot or UEFI)
	   and automatically scan distro bootscript from attached SD/USB/SATA disk and autoboot desired distro if found, otherwise falls back
	   to boot flex_linux_<arch>.itb from NOR/QSPI/SD flash media.
