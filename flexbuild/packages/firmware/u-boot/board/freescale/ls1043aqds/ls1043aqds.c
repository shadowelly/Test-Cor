/*
 * Copyright 2015 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <i2c.h>
#include <fdt_support.h>
#include <fsl_ddr_sdram.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/fsl_serdes.h>
#include <asm/arch/ppa.h>
#include <asm/arch/fdt.h>
#include <asm/arch/mmu.h>
#include <asm/arch/soc.h>
#include <ahci.h>
#include <hwconfig.h>
#include <mmc.h>
#include <scsi.h>
#include <fm_eth.h>
#include <fsl_csu.h>
#include <fsl_esdhc.h>
#include <fsl_ifc.h>
#include <spl.h>
#include <mapmem.h>

#include "../common/qixis.h"
#include "ls1043aqds_qixis.h"

DECLARE_GLOBAL_DATA_PTR;

enum {
	MUX_TYPE_GPIO,
};

/* LS1043AQDS serdes mux */
#define CFG_SD_MUX1_SLOT2	0x0 /* SLOT2 TX/RX0 */
#define CFG_SD_MUX1_SLOT1	0x1 /* SLOT1 TX/RX1 */
#define CFG_SD_MUX2_SLOT3	0x0 /* SLOT3 TX/RX0 */
#define CFG_SD_MUX2_SLOT1	0x1 /* SLOT1 TX/RX2 */
#define CFG_SD_MUX3_SLOT4	0x0 /* SLOT4 TX/RX0 */
#define CFG_SD_MUX3_MUX4	0x1 /* MUX4 */
#define CFG_SD_MUX4_SLOT3	0x0 /* SLOT3 TX/RX1 */
#define CFG_SD_MUX4_SLOT1	0x1 /* SLOT1 TX/RX3 */
#define CFG_UART_MUX_MASK	0x6
#define CFG_UART_MUX_SHIFT	1
#define CFG_LPUART_EN		0x1
#define EXTEND_RCW_REG		0x157040c
#define GPIO1_DIRECTION		0x2300000
#define GPIO1_DATA		0x2300008
#define GPIO2_DIRECTION         0x2310000
#define GPIO2_DATA              0x2310008
#define GPIO3_DIRECTION         0x2320000
#define GPIO3_DATA              0x2320008
#define GPIO4_DIRECTION         0x2330000
#define GPIO4_DATA              0x2330008

#define LITTLE_ENDIAN(x)	((x / 8) * 16 + 7) - x

int checkboard(void)
{
	char buf[64];
#ifndef CONFIG_SD_BOOT
	u8 sw;
#endif

	puts("Board: LS1043AQDS, boot from ");
	printf("QSPI\n");

#ifdef CONFIG_SD_BOOT
	puts("SD\n");
#endif
	
/*
#else
	sw = QIXIS_READ(brdcfg[0]);
	sw = (sw & QIXIS_LBMAP_MASK) >> QIXIS_LBMAP_SHIFT;

	if (sw < 0x8)
		printf("vBank: %d\n", sw);
	else if (sw == 0x8)
		puts("PromJet\n");
	else if (sw == 0x9)
		puts("NAND\n");
	else if (sw == 0xF)
		printf("QSPI\n");
	else
		printf("invalid setting of SW%u\n", QIXIS_LBMAP_SWITCH);
#endif

	printf("Sys ID: 0x%02x, Sys Ver: 0x%02x\n",
	       QIXIS_READ(id), QIXIS_READ(arch));

	printf("FPGA:  v%d (%s), build %d\n",
	       (int)QIXIS_READ(scver), qixis_read_tag(buf),
	       (int)qixis_read_minor());
*/
	return 0;
}

bool if_board_diff_clk(void)
{
	u8 diff_conf = QIXIS_READ(brdcfg[11]);

	return diff_conf & 0x40;
}

unsigned long get_board_sys_clk(void)
{
	u8 sysclk_conf = QIXIS_READ(brdcfg[1]);

	switch (sysclk_conf & 0x0f) {
	case QIXIS_SYSCLK_64:
		return 64000000;
	case QIXIS_SYSCLK_83:
		return 83333333;
	case QIXIS_SYSCLK_100:
		return 100000000;
	case QIXIS_SYSCLK_125:
		return 125000000;
	case QIXIS_SYSCLK_133:
		return 133333333;
	case QIXIS_SYSCLK_150:
		return 150000000;
	case QIXIS_SYSCLK_160:
		return 160000000;
	case QIXIS_SYSCLK_166:
		return 166666666;
	}

	return 66666666;
}

unsigned long get_board_ddr_clk(void)
{
	u8 ddrclk_conf = QIXIS_READ(brdcfg[1]);

	if (if_board_diff_clk())
		return get_board_sys_clk();
	switch ((ddrclk_conf & 0x30) >> 4) {
	case QIXIS_DDRCLK_100:
		return 100000000;
	case QIXIS_DDRCLK_125:
		return 125000000;
	case QIXIS_DDRCLK_133:
		return 133333333;
	}

	return 66666666;
}

int select_i2c_ch_pca9547(u8 ch)
{
	int ret;

	ret = i2c_write(I2C_MUX_PCA_ADDR_PRI, 0, 1, &ch, 1);
	if (ret) {
		puts("PCA: failed to select proper channel\n");
		return ret;
	}

	return 0;
}

int dram_init(void)
{
	/*
	 * When resuming from deep sleep, the I2C channel may not be
	 * in the default channel. So, switch to the default channel
	 * before accessing DDR SPD.
	 */
	//select_i2c_ch_pca9547(I2C_MUX_CH_DEFAULT);
	fsl_initdram();
#if !defined(CONFIG_SPL) || defined(CONFIG_SPL_BUILD)
	/* This will break-before-make MMU for DDR */
	update_early_mmu_table();
#endif

	return 0;
}

int i2c_multiplexer_select_vid_channel(u8 channel)
{
	return select_i2c_ch_pca9547(channel);
}

void board_retimer_init(void)
{
	u8 reg;

	/* Retimer is connected to I2C1_CH7_CH5 */
	select_i2c_ch_pca9547(I2C_MUX_CH7);
	reg = I2C_MUX_CH5;
	i2c_write(I2C_MUX_PCA_ADDR_SEC, 0, 1, &reg, 1);

	/* Access to Control/Shared register */
	reg = 0x0;
	i2c_write(I2C_RETIMER_ADDR, 0xff, 1, &reg, 1);

	/* Read device revision and ID */
	i2c_read(I2C_RETIMER_ADDR, 1, 1, &reg, 1);
	debug("Retimer version id = 0x%x\n", reg);

	/* Enable Broadcast. All writes target all channel register sets */
	reg = 0x0c;
	i2c_write(I2C_RETIMER_ADDR, 0xff, 1, &reg, 1);

	/* Reset Channel Registers */
	i2c_read(I2C_RETIMER_ADDR, 0, 1, &reg, 1);
	reg |= 0x4;
	i2c_write(I2C_RETIMER_ADDR, 0, 1, &reg, 1);

	/* Enable override divider select and Enable Override Output Mux */
	i2c_read(I2C_RETIMER_ADDR, 9, 1, &reg, 1);
	reg |= 0x24;
	i2c_write(I2C_RETIMER_ADDR, 9, 1, &reg, 1);

	/* Select VCO Divider to full rate (000) */
	i2c_read(I2C_RETIMER_ADDR, 0x18, 1, &reg, 1);
	reg &= 0x8f;
	i2c_write(I2C_RETIMER_ADDR, 0x18, 1, &reg, 1);

	/* Selects active PFD MUX Input as Re-timed Data (001) */
	i2c_read(I2C_RETIMER_ADDR, 0x1e, 1, &reg, 1);
	reg &= 0x3f;
	reg |= 0x20;
	i2c_write(I2C_RETIMER_ADDR, 0x1e, 1, &reg, 1);

	/* Set data rate as 10.3125 Gbps */
	reg = 0x0;
	i2c_write(I2C_RETIMER_ADDR, 0x60, 1, &reg, 1);
	reg = 0xb2;
	i2c_write(I2C_RETIMER_ADDR, 0x61, 1, &reg, 1);
	reg = 0x90;
	i2c_write(I2C_RETIMER_ADDR, 0x62, 1, &reg, 1);
	reg = 0xb3;
	i2c_write(I2C_RETIMER_ADDR, 0x63, 1, &reg, 1);
	reg = 0xcd;
	i2c_write(I2C_RETIMER_ADDR, 0x64, 1, &reg, 1);

	/* Return the default channel */
	select_i2c_ch_pca9547(I2C_MUX_CH_DEFAULT);
}

int board_early_init_f(void)
{
#ifdef CONFIG_HAS_FSL_XHCI_USB
	struct ccsr_scfg *scfg = (struct ccsr_scfg *)CONFIG_SYS_FSL_SCFG_ADDR;
	u32 usb_pwrfault;
#endif
#ifdef CONFIG_LPUART
	u8 uart;
#endif

#ifdef CONFIG_SYS_I2C_EARLY_INIT
	i2c_early_init_f();
#endif
	fsl_lsch2_early_init_f();

#ifdef CONFIG_HAS_FSL_XHCI_USB
	out_be32(&scfg->rcwpmuxcr0, 0x3333);
	out_be32(&scfg->usbdrvvbus_selcr, SCFG_USBDRVVBUS_SELCR_USB1);
	usb_pwrfault =
		(SCFG_USBPWRFAULT_DEDICATED << SCFG_USBPWRFAULT_USB3_SHIFT) |
		(SCFG_USBPWRFAULT_DEDICATED << SCFG_USBPWRFAULT_USB2_SHIFT) |
		(SCFG_USBPWRFAULT_SHARED << SCFG_USBPWRFAULT_USB1_SHIFT);
	out_be32(&scfg->usbpwrfault_selcr, usb_pwrfault);
#endif

#ifdef CONFIG_LPUART
	/* We use lpuart0 as system console */
	uart = QIXIS_READ(brdcfg[14]);
	uart &= ~CFG_UART_MUX_MASK;
	uart |= CFG_LPUART_EN << CFG_UART_MUX_SHIFT;
	QIXIS_WRITE(brdcfg[14], uart);
#endif

	return 0;
}

#ifdef CONFIG_FSL_DEEP_SLEEP
/* determine if it is a warm boot */
bool is_warm_boot(void)
{
#define DCFG_CCSR_CRSTSR_WDRFR	(1 << 3)
	struct ccsr_gur __iomem *gur = (void *)CONFIG_SYS_FSL_GUTS_ADDR;

	if (in_be32(&gur->crstsr) & DCFG_CCSR_CRSTSR_WDRFR)
		return 1;

	return 0;
}
#endif


#if 0
int config_board_mux(int ctrl_type)
{
	u8 reg14;

	reg14 = QIXIS_READ(brdcfg[14]);

	switch (ctrl_type) {
	case MUX_TYPE_GPIO:
		reg14 = (reg14 & (~0x30)) | 0x20;
		break;
	default:
		puts("Unsupported mux interface type\n");
		return -1;
	}

	QIXIS_WRITE(brdcfg[14], reg14);

	return 0;
}
#endif


void config_board_mux(void)
{
#if 0
#ifdef CONFIG_HAS_FSL_XHCI_USB
	struct ccsr_scfg *scfg = (struct ccsr_scfg *)CONFIG_SYS_FSL_SCFG_ADDR;
	u32 usb_pwrfault;

	/* USB3 is not used, configure mux to IIC4_SCL/IIC4_SDA */
	out_be32(&scfg->rcwpmuxcr0, 0x3300);
	out_be32(&scfg->usbdrvvbus_selcr, SCFG_USBDRVVBUS_SELCR_USB1);
	usb_pwrfault = (SCFG_USBPWRFAULT_DEDICATED <<
			SCFG_USBPWRFAULT_USB3_SHIFT) |
			(SCFG_USBPWRFAULT_DEDICATED <<
			SCFG_USBPWRFAULT_USB2_SHIFT) |
			(SCFG_USBPWRFAULT_SHARED <<
			SCFG_USBPWRFAULT_USB1_SHIFT);
	out_be32(&scfg->usbpwrfault_selcr, usb_pwrfault);
#endif
#endif
}


int config_serdes_mux(void)
{
	return 0;
}


#ifdef CONFIG_MISC_INIT_R
int misc_init_r(void)
{
	if (hwconfig("gpio"))
		//config_board_mux(MUX_TYPE_GPIO);
		config_board_mux();
	return 0;
}
#endif

void gpio_write_bitl(ulong addr, int pos, int value)
{
	pos = LITTLE_ENDIAN(pos);
	gpio_write_bit(addr, pos, value);
}

int board_init(void)
{
	//SENAO
	const void *buf = map_sysmem(EXTEND_RCW_REG, 1);
	*(volatile uint32_t *)buf = 0x11110000;
	unmap_sysmem(buf);
#ifdef CONFIG_SYS_FSL_ERRATUM_A010315
	erratum_a010315();
#endif

	/* SENAO - LED reset */

	//STATUS_LED_R - Output Active_high
	gpio_write_bitl(GPIO1_DIRECTION, 20, 1);	
	gpio_write_bitl(GPIO1_DATA,      20, 1);
	//STATUS_LED_G - Output Active_high
	gpio_write_bitl(GPIO1_DIRECTION, 22, 1);
	gpio_write_bitl(GPIO1_DATA,      22, 1);
	//RADIO_LED_G - Ouput Active_high
	gpio_write_bitl(GPIO1_DIRECTION, 29, 1);
	gpio_write_bitl(GPIO1_DATA,      29, 1);
	//RADIO_LED_R - Ouput Active_high
	gpio_write_bitl(GPIO1_DIRECTION, 30, 1);
	gpio_write_bitl(GPIO1_DATA,      30, 1);
	//POWER_LED_G - Ouput Active_high
	gpio_write_bitl(GPIO4_DIRECTION, 2, 1);
	gpio_write_bitl(GPIO4_DATA,      2, 1);
	//POWER_LED_R - Ouput Active_high
	gpio_write_bitl(GPIO4_DIRECTION, 3, 1);
	gpio_write_bitl(GPIO4_DATA,      3, 1);

	udelay(20000);
	//STATUS_LED_R
	gpio_write_bitl(GPIO1_DATA,      20, 0);
	//STATUS_LED_G
	gpio_write_bitl(GPIO1_DATA,      22, 0);	
	//RADIO_LED_G
	gpio_write_bitl(GPIO1_DATA,      29, 0);
	//RADIO_LED_R
	gpio_write_bitl(GPIO1_DATA,      30, 0);
	//POWER_LED_G
	gpio_write_bitl(GPIO4_DATA,      2, 0);
	//POWER_LED_R
	gpio_write_bitl(GPIO4_DATA,      3, 0);

	/* SENAO - PSE init, disable the PSE function for development */
	
	//CPU_PSE1
	gpio_write_bitl(GPIO2_DIRECTION, 11, 1);
	gpio_write_bitl(GPIO2_DATA, 	 11, 0);
	//CPU_PSE2
	gpio_write_bitl(GPIO2_DIRECTION, 12, 1);
	gpio_write_bitl(GPIO2_DATA, 	 12, 0);
	//CPU_PSE3
	gpio_write_bitl(GPIO2_DIRECTION, 13, 1);
	gpio_write_bitl(GPIO2_DATA, 	 13, 0);
	//CPU_PSE4
	gpio_write_bitl(GPIO2_DIRECTION, 14, 1);
	gpio_write_bitl(GPIO2_DATA, 	 14, 0);
	//CPU_PSE5
	gpio_write_bitl(GPIO2_DIRECTION, 15, 1);
	gpio_write_bitl(GPIO2_DATA, 	 15, 0);

	/* SENAO - PHY reset */

	//PHY1_RSTN - Output Active_low
	gpio_write_bitl(GPIO1_DATA,      23, 0);
	gpio_write_bitl(GPIO1_DIRECTION, 23, 1);
	//PHY2_RSTN - Output Active_low
        gpio_write_bitl(GPIO1_DATA,      24, 0);
	gpio_write_bitl(GPIO1_DIRECTION, 24, 1);
	//PHY3_RSTN - Output Active_low
        gpio_write_bitl(GPIO1_DATA,      25, 0);
	gpio_write_bitl(GPIO1_DIRECTION, 25, 1);

	udelay(20000);

	//PHY1_RSTN
	gpio_write_bitl(GPIO1_DATA,      23, 1);
	//PHY2_RSTN
	gpio_write_bitl(GPIO1_DATA,      24, 1);
	//PHY3_RSTN
	gpio_write_bitl(GPIO1_DATA,      25, 1);


//select_i2c_ch_pca9547(I2C_MUX_CH_DEFAULT);
//board_retimer_init();


#ifdef CONFIG_SYS_FSL_SERDES
	config_serdes_mux();
#endif

#ifdef CONFIG_FSL_LS_PPA
	ppa_init();
#endif

	return 0;
}

#ifdef CONFIG_OF_BOARD_SETUP
int ft_board_setup(void *blob, bd_t *bd)
{
	u64 base[CONFIG_NR_DRAM_BANKS];
	u64 size[CONFIG_NR_DRAM_BANKS];
	u8 reg;

	/* fixup DT for the two DDR banks */
	base[0] = gd->bd->bi_dram[0].start;
	size[0] = gd->bd->bi_dram[0].size;
	base[1] = gd->bd->bi_dram[1].start;
	size[1] = gd->bd->bi_dram[1].size;

	fdt_fixup_memory_banks(blob, base, size, 2);
	ft_cpu_setup(blob, bd);

#ifdef CONFIG_SYS_DPAA_FMAN
	fdt_fixup_fman_ethernet(blob);
	//fdt_fixup_board_enet(blob);
#endif

	reg = QIXIS_READ(brdcfg[0]);
	reg = (reg & QIXIS_LBMAP_MASK) >> QIXIS_LBMAP_SHIFT;

	/* Disable IFC if QSPI is enabled */
	if (reg == 0xF)
		do_fixup_by_compat(blob, "fsl,ifc",
				   "status", "disabled", 8 + 1, 1);

	return 0;
}
#endif

u8 flash_read8(void *addr)
{
	return __raw_readb(addr + 1);
}

void flash_write16(u16 val, void *addr)
{
	u16 shftval = (((val >> 8) & 0xff) | ((val << 8) & 0xff00));

	__raw_writew(shftval, addr);
}

u16 flash_read16(void *addr)
{
	u16 val = __raw_readw(addr);

	return (((val) >> 8) & 0x00ff) | (((val) << 8) & 0xff00);
}
