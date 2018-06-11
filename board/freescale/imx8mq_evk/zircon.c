// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <zircon/zircon.h>

#define PDEV_VID_NXP                9
#define PDEV_PID_IMX8MEVK           1

const char* BOOTLOADER_VERSION = "zircon-bootloader=0.05";

static const zbi_cpu_config_t cpu_config = {
    .cluster_count = 1,
    .clusters = {
        {
            .cpu_count = 4,
        },
    },
};

static const zbi_mem_range_t mem_config[] = {
    {
        .type = ZBI_MEM_RANGE_RAM,
        .paddr = 0x40000000,
        .length = 0xc0000000, // 3GB
    },
    {
        .type = ZBI_MEM_RANGE_PERIPHERAL,
        .paddr = 0,
        .length = 0x40000000,
    },
};

static const dcfg_simple_t uart_driver = {
    .mmio_phys = 0x30860000,
    .irq = 58,
};

static const dcfg_arm_gicv3_driver_t gicv3_driver = {
    .mmio_phys = 0x38800000,
    .gicd_offset = 0x00000,
    .gicr_offset = 0x80000,
    .gicr_stride = 0x20000,
    .ipi_base = 9,
    // Used for Errata e11171
    .mx8_gpr_phys = 0x30340000,
};

static const dcfg_arm_psci_driver_t psci_driver = {
    .use_hvc = false,
};

static const dcfg_arm_generic_timer_driver_t timer_driver = {
    .irq_phys = 30,
    .irq_virt = 27,
    .freq_override = 8333333,
};

static const zbi_platform_id_t platform_id = {
    .vid = PDEV_VID_NXP,
    .pid = PDEV_PID_IMX8MEVK,
    .board_name = "imx8mevk",
};

int zircon_preboot(zbi_header_t* zbi) {
    // add CPU configuration
    zircon_append_boot_item(zbi, ZBI_TYPE_CPU_CONFIG, 0, &cpu_config,
                    sizeof(zbi_cpu_config_t) +
                    sizeof(zbi_cpu_cluster_t) * cpu_config.cluster_count);

    // add memory configuration
    zircon_append_boot_item(zbi, ZBI_TYPE_MEM_CONFIG, 0, &mem_config,
                    sizeof(zbi_mem_range_t) * (sizeof(mem_config)/sizeof(mem_config[0])));

    // add kernel drivers
    zircon_append_boot_item(zbi, ZBI_TYPE_KERNEL_DRIVER, KDRV_NXP_IMX_UART, &uart_driver,
                    sizeof(uart_driver));
    zircon_append_boot_item(zbi, ZBI_TYPE_KERNEL_DRIVER, KDRV_ARM_GIC_V3, &gicv3_driver,
                    sizeof(gicv3_driver));
    zircon_append_boot_item(zbi, ZBI_TYPE_KERNEL_DRIVER, KDRV_ARM_PSCI, &psci_driver,
                    sizeof(psci_driver));
    zircon_append_boot_item(zbi, ZBI_TYPE_KERNEL_DRIVER, KDRV_ARM_GENERIC_TIMER, &timer_driver,
                    sizeof(timer_driver));

    zircon_append_boot_item(zbi, ZBI_TYPE_CMDLINE, 0, BOOTLOADER_VERSION, strlen(BOOTLOADER_VERSION) + 1);

    // add platform ID
    zircon_append_boot_item(zbi, ZBI_TYPE_PLATFORM_ID, 0, &platform_id, sizeof(platform_id));
    return 0;
}
