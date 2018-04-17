/*
 * Copyright (c) 2018 The Fuchsia Authors
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include <part.h>

#define PDEV_VID_KHADAS             4
#define PDEV_PID_VIM2               2

const char* BOOTLOADER_VERSION = "zircon-bootloader=0.05";

static const bootdata_cpu_config_t cpu_config = {
    .cluster_count = 2,
    .clusters = {
        {
            .cpu_count = 4,
        },
        {
            .cpu_count = 4,
        },
    },
};

static bootdata_mem_range_t mem_config[] = {
    {
        .type = BOOTDATA_MEM_RANGE_RAM,
        .length = 0x80000000, // 2GB
    },
    {
        .type = BOOTDATA_MEM_RANGE_PERIPHERAL,
        .paddr = 0xc0000000,
        .length = 0x20000000,
    },
    {
        .type = BOOTDATA_MEM_RANGE_RESERVED,
        .paddr = 0,
        .length = 0x001000000,
    },
    {
        .type = BOOTDATA_MEM_RANGE_RESERVED,
        .paddr = 0x10000000,
        .length = 0x00200000,
    },
    {
        .type = BOOTDATA_MEM_RANGE_RESERVED,
        .paddr = 0x05100000,
        .length = 0x2000000,
    },
    {
        .type = BOOTDATA_MEM_RANGE_RESERVED,
        .paddr = 0x7300000,
        .length = 0x100000,
    },
    {
        .type = BOOTDATA_MEM_RANGE_RESERVED,
        .paddr = 0x75000000,
        .length = 0x9000000,
    },
};

static const dcfg_simple_t uart_driver = {
    .mmio_phys = 0xc81004c0,
    .irq = 225,
};

static const dcfg_arm_gicv2_driver_t gicv2_driver = {
    .mmio_phys = 0xc4300000,
    .gicd_offset = 0x1000,
    .gicc_offset = 0x2000,
    .gich_offset = 0x4000,
    .gicv_offset = 0x6000,
    .ipi_base = 5,
};

static const dcfg_arm_psci_driver_t psci_driver = {
    .use_hvc = false,
    .reboot_args = { 1, 0, 0 },
    .reboot_bootloader_args = { 4, 0, 0 },
    .reboot_recovery_args = { 2, 0, 0 },
};

static const dcfg_arm_generic_timer_driver_t timer_driver = {
    .irq_phys = 30,
};

static const dcfg_amlogic_hdcp_driver_t hdcp_driver = {
    .preset_phys = 0xc1104000,
    .hiu_phys = 0xc883c000,
    .hdmitx_phys = 0xc883a000,
};

static const bootdata_platform_id_t platform_id = {
    .vid = PDEV_VID_KHADAS,
    .pid = PDEV_PID_VIM2,
    .board_name = "vim2",
};

enum {
    PART_BOOTLOADER,
    PART_ZIRCON_A,
    PART_ZIRCON_B,
    PART_ZIRCON_R,
    PART_SYS_CONFIG,
    PART_FACTORY_CONFIG,
    PART_FVM,
    PART_COUNT,
};

static bootdata_partition_map_t partition_map = {
    // .block_count filled in below
    // .block_size filled in below
    .guid = {},
    .partition_count = PART_COUNT,
    .partitions = {
        {
            .type_guid = GUID_BOOTLOADER_VALUE,
            .uniq_guid = {},
            // .first_block filled in below
            // .last_block filled in below
            .flags = 0,
            .name = "bootloader",
        },
        {
            .type_guid = GUID_ZIRCON_A_VALUE,
            .uniq_guid = {},
            // .first_block filled in below
            // .last_block filled in below
            .flags = 0,
            .name = "zircon-a",
        },
        {
            .type_guid = GUID_ZIRCON_B_VALUE,
            .uniq_guid = {},
            // .first_block filled in below
            // .last_block filled in below
            .flags = 0,
            .name = "zircon-b",
        },
        {
            .type_guid = GUID_ZIRCON_R_VALUE,
            .uniq_guid = {},
            // .first_block filled in below
            // .last_block filled in below
            .flags = 0,
            .name = "zircon-r",
        },
        {
            .type_guid = GUID_SYS_CONFIG_VALUE,
            .uniq_guid = {},
            // .first_block filled in below
            // .last_block filled in below
            .flags = 0,
            .name = "sys-config",
        },
        {
            .type_guid = GUID_FACTORY_CONFIG_VALUE,
            .uniq_guid = {},
            // .first_block filled in below
            // .last_block filled in below
            .flags = 0,
            .name = "factory",
        },
        {
            .type_guid = GUID_FVM_VALUE,
            .uniq_guid = {},
            // .first_block filled in below
            // .last_block filled in below
            .flags = 0,
            .name = "fvm",
        },
    },
};

static void add_partition_map(bootdata_t* bootdata) {
    block_dev_desc_t* dev_desc;
    disk_partition_t bootloader_info;
    disk_partition_t boot_info;
    disk_partition_t misc_info;
    disk_partition_t recovery_info;
    disk_partition_t tee_info;
    disk_partition_t crypt_info;
    disk_partition_t system_info;
    disk_partition_t data_info;

    dev_desc = get_dev("mmc", CONFIG_FASTBOOT_FLASH_MMC_DEV);
    if (!dev_desc || dev_desc->type == DEV_TYPE_UNKNOWN) {
        printf("could not find MMC device for partition map\n");
        return;
    }

    if (get_partition_info_aml_by_name(dev_desc, "bootloader", &bootloader_info)) {
        printf("could not find bootloader partition\n");
        return;
    }
    if (get_partition_info_aml_by_name(dev_desc, "boot", &boot_info)) {
        printf("could not find boot partition\n");
        return;
    }
    if (get_partition_info_aml_by_name(dev_desc, "misc", &misc_info)) {
        printf("could not find misc partition\n");
        return;
    }
    if (get_partition_info_aml_by_name(dev_desc, "recovery", &recovery_info)) {
        printf("could not find recovery partition\n");
        return;
    }
    if (get_partition_info_aml_by_name(dev_desc, "tee", &tee_info)) {
        printf("could not find tee partition\n");
        return;
    }
    if (get_partition_info_aml_by_name(dev_desc, "crypt", &crypt_info)) {
        printf("could not find crypt partition\n");
        return;
    }
    if (get_partition_info_aml_by_name(dev_desc, "system", &system_info)) {
        printf("could not find system partition\n");
        return;
    }
    if (get_partition_info_aml_by_name(dev_desc, "data", &data_info)) {
        printf("could not find data partition\n");
        return;
    }

    // map bootloader partition to BOOTLOADER
    partition_map.partitions[PART_BOOTLOADER].first_block = bootloader_info.start;
    partition_map.partitions[PART_BOOTLOADER].last_block = bootloader_info.start +
                                                           bootloader_info.size - 1;

    // map boot partition to ZIRCON_A
    partition_map.partitions[PART_ZIRCON_A].first_block = boot_info.start;
    partition_map.partitions[PART_ZIRCON_A].last_block = boot_info.start + boot_info.size - 1;

    // map misc partition to ZIRCON_B
    partition_map.partitions[PART_ZIRCON_B].first_block = misc_info.start;
    partition_map.partitions[PART_ZIRCON_B].last_block = misc_info.start + misc_info.size - 1;

    // map recovery partition to ZIRCON_R
    partition_map.partitions[PART_ZIRCON_R].first_block = recovery_info.start;
    partition_map.partitions[PART_ZIRCON_R].last_block = recovery_info.start +
                                                         recovery_info.size - 1;

    // map tee partition to SYS_CONFIG
    partition_map.partitions[PART_SYS_CONFIG].first_block = tee_info.start;
    partition_map.partitions[PART_SYS_CONFIG].last_block = tee_info.start + tee_info.size - 1;

    // map crypt partition to FACTORY_CONFIG
    partition_map.partitions[PART_FACTORY_CONFIG].first_block = crypt_info.start;
    partition_map.partitions[PART_FACTORY_CONFIG].last_block = crypt_info.start +
                                                               crypt_info.size - 1;

    // map system and data partitions to FVM
    partition_map.partitions[PART_FVM].first_block = system_info.start;
    partition_map.partitions[PART_FVM].last_block = data_info.start + data_info.size - 1;

    partition_map.block_count = data_info.start + data_info.size;
    partition_map.block_size = data_info.blksz;

    append_bootdata(bootdata, BOOTDATA_PARTITION_MAP, 0, &partition_map,
                    sizeof(bootdata_partition_map_t) +
                    partition_map.partition_count * sizeof(bootdata_partition_t));
}

static int hex_digit(char ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    } else if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    } else if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    } else {
        return -1;
    }
}

static void add_eth_mac_address(bootdata_t* bootdata) {
    char* str = getenv("eth_mac");
    uint8_t addr[6];

    // this would be easier with sscanf
    int i;
    for (i = 0; i < 6; i++) {
        unsigned left, right;
        if (str[0] && str[1] && (left = hex_digit(*str++)) >= 0 &&
                (right = hex_digit(*str++)) >= 0) {
            addr[i] = (left << 4) | right;
        } else {
            goto failed;
        }
        if (i < 5 && *str++ != ':') {
            goto failed;
        }
    }

    append_bootdata(bootdata, BOOTDATA_MAC_ADDRESS, 0, addr, sizeof(addr));
    return;

failed:
    printf("MAC address parsing failed for \"%s\"\n", getenv("eth_mac"));
}

static void append_board_bootdata(bootdata_t* bootdata) {
    // add CPU configuration
    append_bootdata(bootdata, BOOTDATA_CPU_CONFIG, 0, &cpu_config,
                    sizeof(bootdata_cpu_config_t) +
                    sizeof(bootdata_cpu_cluster_t) * cpu_config.cluster_count);

    const char* ddr_size = getenv("ddr_size");
    if (!strcmp(ddr_size, "3")) {
        mem_config[0].length = 0xc0000000;
    }

    // add memory configuration
    append_bootdata(bootdata, BOOTDATA_MEM_CONFIG, 0, &mem_config, sizeof(mem_config));

    // add kernel drivers
    append_bootdata(bootdata, BOOTDATA_KERNEL_DRIVER, KDRV_AMLOGIC_UART, &uart_driver,
                    sizeof(uart_driver));
    append_bootdata(bootdata, BOOTDATA_KERNEL_DRIVER, KDRV_ARM_GIC_V2, &gicv2_driver,
                    sizeof(gicv2_driver));
    append_bootdata(bootdata, BOOTDATA_KERNEL_DRIVER, KDRV_ARM_PSCI, &psci_driver,
                    sizeof(psci_driver));
    append_bootdata(bootdata, BOOTDATA_KERNEL_DRIVER, KDRV_ARM_GENERIC_TIMER, &timer_driver,
                    sizeof(timer_driver));
    append_bootdata(bootdata, BOOTDATA_KERNEL_DRIVER, KDRV_AMLOGIC_HDCP, &hdcp_driver,
                    sizeof(hdcp_driver));

    append_bootdata(bootdata, BOOTDATA_KERNEL_DRIVER, KDRV_AMLOGIC_HDCP, &hdcp_driver,
                    sizeof(hdcp_driver));

    append_bootdata(bootdata, BOOTDATA_CMDLINE, 0, BOOTLOADER_VERSION, strlen(BOOTLOADER_VERSION) + 1);

    // add platform ID
    append_bootdata(bootdata, BOOTDATA_PLATFORM_ID, 0, &platform_id, sizeof(platform_id));

    add_partition_map(bootdata);
    add_eth_mac_address(bootdata);
 }
