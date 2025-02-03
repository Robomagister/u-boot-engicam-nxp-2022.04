/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2019 NXP
 */

#ifndef __IMX8MP_ICORE_H
#define __IMX8MP_ICORE_H

#include <linux/sizes.h>
#include <linux/stringify.h>
#include <asm/arch/imx-regs.h>
#include "imx_env.h"

#define CONFIG_SYS_BOOTM_LEN		(32 * SZ_1M)

#define CONFIG_SPL_MAX_SIZE		(176 * 1024)
#define CONFIG_SYS_MONITOR_LEN		(512 * 1024)
#define CONFIG_SYS_UBOOT_BASE	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SPL_STACK		0x96dff0
#define CONFIG_SPL_BSS_START_ADDR      0x96e000
#define CONFIG_SPL_BSS_MAX_SIZE		SZ_8K	/* 8 KB */
#define CONFIG_SYS_SPL_MALLOC_START	0x42200000
#define CONFIG_SYS_SPL_MALLOC_SIZE	SZ_512K	/* 512 KB */

/* For RAW image gives a error info not panic */
#define CONFIG_SPL_ABORT_ON_RAW_IMAGE

#endif

#define CONFIG_CMD_READ
#define CONFIG_SERIAL_TAG
#define CONFIG_FASTBOOT_USB_DEV 0

#define CONFIG_REMAKE_ELF
/* ENET Config */
/* ENET1 */

#if defined(CONFIG_CMD_NET)
#define CONFIG_ETHPRIME                 "eth1" /* Set eqos to primary since we use its MDIO */

#define FEC_QUIRK_ENET_MAC

#ifdef CONFIG_TARGET_IMX8MP_ICORE_FASTETH
	#define CONFIG_FEC_XCV_TYPE             RMII
	#define CONFIG_FEC_MXC_PHYADDR          0
	#define DWC_NET_PHYADDR			0
#else
	#define CONFIG_FEC_XCV_TYPE             RGMII
	#define CONFIG_FEC_MXC_PHYADDR          7
	#define DWC_NET_PHYADDR			7
#endif

#ifdef CONFIG_DWC_ETH_QOS
#define CONFIG_SYS_NONCACHED_MEMORY     (1 * SZ_1M)     /* 1M */
#endif

#define PHY_ANEG_TIMEOUT 20000

#endif

#ifdef CONFIG_DISTRO_DEFAULTS
#define BOOT_TARGET_DEVICES(func) \
	func(USB, usb, 0) \
	func(MMC, mmc, 1) \
	func(MMC, mmc, 2)

#include <config_distro_bootcmd.h>
#else
#define BOOTENV
#endif



#ifdef CONFIG_NAND_BOOT
#define MFG_NAND_PARTITION "mtdparts=gpmi-nand:64m(nandboot),16m(nandfit),32m(nandkernel),16m(nanddtb),8m(nandtee),-(nandrootfs)"
#endif

#ifdef CONFIG_IMX8MP_1GB_LPDDR4
	#define CMA_VALUE " cma=128M "	
#else	
	#define CMA_VALUE " "
#endif

/* Initial environment variables */
#define CONFIG_EXTRA_ENV_SETTINGS		\
	BOOTENV \
	"prepare_mcore=setenv mcore_clk clk-imx8mp.mcore_booted;\0" \
	"scriptaddr=0x43500000\0" \
	"kernel_addr_r=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	"bsp_script=boot.scr\0" \
	"image=Image\0" \
	"splashimage=0x50000000\0" \
	"console=ttymxc1,115200\0" \
	"fdt_addr_r=0x43000000\0"			\
	"fdt_addr=0x43000000\0"			\
	"boot_fdt=try\0" \
	"fdt_high=0xffffffffffffffff\0"		\
	"bootdir=/boot\0" \
	"boot_fit=no\0" \
 	"fdtfile=imx8mp-icore-fasteth-robomagister.dtb\0" \
	"bootm_size=0x10000000\0" \
	"mmcdev=2\0" \
	"mmcpart=2\0" \
	"mmcroot=" CONFIG_MMCROOT " rootwait rw\0" \
	"mmcautodetect=yes\0" \
	"mmcargs=if test -e mmc ${mmcdev}:${mmcpart} /sbin/preinit; then setenv initarg init=/sbin/preinit; fi; " \
		"setenv bootargs ${jh_clk} ${mcore_clk} console=${console} root=/dev/mmcblk${mmcdev}p${mmcpart} rootwait rw ${initarg} " CMA_VALUE "\0 " \
	"loadbootscript=load mmc ${mmcdev}:${mmcpart} ${loadaddr} ${bootdir}/${bsp_script};\0" \
	"bootscript=echo Running bootscript from mmc ...; " \
		"source\0" \
	"loadimage=load mmc ${mmcdev}:${mmcpart} ${loadaddr} ${bootdir}/${image}\0" \
	"loadfdt=load mmc ${mmcdev}:${mmcpart} ${fdt_addr_r} ${bootdir}/${fdtfile}\0" \
	"bootlimit=3\0" \
	"mmcboot=echo Booting from mmc ...; " \
		"run mmcargs; " \
		"if test ${boot_fit} = yes || test ${boot_fit} = try; then " \
			"bootm ${loadaddr}; " \
		"else " \
			"if run loadfdt; then " \
				"booti ${loadaddr} - ${fdt_addr_r}; " \
			"else " \
				"echo WARN: Cannot load the DT; " \
			"fi; " \
		"fi;\0" \
	"restorebootcmd=echo Falling back to previous boot partition...;" \
		"if test ${mmcpart} = 1; then "\
			"setenv mmcpart 2; " \
		"else " \
			"setenv mmcpart 1; " \
		"fi; " \
		"setenv upgrade_available 0; " \
		"saveenv;\0" \
	"altbootcmd=run restorebootcmd;" \
		"run bootcmd\0" \
	"bsp_bootcmd=echo Running BSP bootcmd ...; " \
		"mmc dev ${mmcdev}; if mmc rescan; then " \
		   "if run loadbootscript; then " \
			   "run bootscript; " \
		   "else " \
			   "if run loadimage; then " \
				   "run mmcboot; " \
			   "else " \
			   	   "if test ${upgrade_available} -gt 0; then " \
			   	       "setexpr bootcount ${bootcount} + 1; " \
				       "if test ${bootcount} -gt ${bootlimit}; then " \
			               "run restorebootcmd; " \
			           "fi; " \
			       "fi; " \
			       "boot; " \
			   "fi; " \
		   "fi; " \
	   "fi;"

/* Link Definitions */

#define CONFIG_SYS_INIT_RAM_ADDR	0x40000000
#define CONFIG_SYS_INIT_RAM_SIZE	0x80000
#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

#define CONFIG_MMCROOT			"/dev/mmcblk1p2"  /* USDHC2 */

/* Totally 2GB DDR */
#define CONFIG_SYS_SDRAM_BASE		0x40000000
#define PHYS_SDRAM			0x40000000

#ifdef CONFIG_IMX8MP_1GB_LPDDR4
	/* Totally 1GB DDR */	
	#define PHYS_SDRAM_SIZE			0x40000000	/* 1 GB */
#elif defined(CONFIG_IMX8MP_4GB_LPDDR4)
	#define PHYS_SDRAM_SIZE		0xC0000000	/* 3 GB */
	#define PHYS_SDRAM_2		0x100000000
	#define PHYS_SDRAM_2_SIZE	0x40000000	/* 1 GB */
#else
	/* Totally 2GB DDR */
	#define PHYS_SDRAM_SIZE			0x80000000	/* 2 GB */
#endif

#define CONFIG_MXC_UART_BASE		UART2_BASE_ADDR

/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_MAXARGS		64
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)

#define CONFIG_IMX_BOOTAUX

#define CONFIG_SYS_FSL_USDHC_NUM	2


#define CONFIG_SYS_I2C_SPEED		100000

/* USB configs */

#define CONFIG_USB_MAX_CONTROLLER_COUNT         2
#define CONFIG_USBD_HS
#define CONFIG_USB_GADGET_VBUS_DRAW 2

#ifdef CONFIG_ANDROID_SUPPORT
#include "imx8mp_evk_android.h"
#endif

#endif
