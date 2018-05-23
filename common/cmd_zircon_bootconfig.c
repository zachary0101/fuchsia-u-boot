/*
 * Copyright (c) 2018 The Fuchsia Authors
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include <common.h>
#include <kvstore.h>
#include <amlogic/storage_if.h>
#include <zircon/sysconfig.h>

static int do_zircon_bootconfig(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    const char* sysconfig;
    const char* zircon_a;
    const char* zircon_b;
    const char* zircon_r;
    const char* zircon_boot;
    struct kvstore kvs;
    int rc;

    if (argc != 5) {
        return -1;
    }

    sysconfig = argv[1];
    zircon_a = argv[2];
    zircon_b = argv[3];
    zircon_r = argv[4];

    uint8_t buffer[ZX_SYSCONFIG_KVSTORE_SIZE];

    // first try boot-oneshot
    int oneshot = 1;
    rc = store_read_ops((unsigned char*)sysconfig, buffer, ZX_SYSCONFIG_BOOT_ONESHOT_OFFSET,
                         sizeof(buffer));
    if (rc) {
        printf("do_sysconfig: store_read_ops failed for %s\n", sysconfig);
        return rc;
    }
    rc = kvs_load(&kvs, buffer, sizeof(buffer));
    if (rc < 0) {
        // use boot-default instead
        oneshot = 0;
        rc = store_read_ops((unsigned char*)sysconfig, buffer, ZX_SYSCONFIG_BOOT_DEFAULT_OFFSET,
                             sizeof(buffer));
        if (rc) {
            printf("do_sysconfig: store_read_ops failed for %s\n", sysconfig);
            return rc;
        }
        rc = kvs_load(&kvs, buffer, sizeof(buffer));
        if (rc < 0) {
            printf("do_sysconfig: kvs_load failed for %s\n", sysconfig);
            return rc;
        }
    }

    const char* boot_part = kvs_get(&kvs, "boot", NULL);
    if (!boot_part) {
        printf("boot partition not set\n");
        return -1;
    }
    if (!strcmp(boot_part, "a")) {
        zircon_boot = zircon_a;
    } else if (!strcmp(boot_part, "b")) {
        zircon_boot = zircon_b;
    } else if (!strcmp(boot_part, "r")) {
        zircon_boot = zircon_r;
    } else {
        printf("invalid boot partition %s\n", boot_part);
        return -1;
    }

    printf("zircon_bootconfig setting boot_part to %s\n", zircon_boot);
    setenv("boot_part", zircon_boot);

    if (oneshot) {
        // erase the boot-oneshot section
        memset(buffer, 0, sizeof(buffer));
        printf("erasing boot-oneshot section\n");
        store_write_ops((unsigned char*)sysconfig, buffer, ZX_SYSCONFIG_BOOT_ONESHOT_OFFSET,
                        sizeof(buffer));
    }

    return 0;
}

U_BOOT_CMD(
    zircon_bootconfig, 5, 0, do_zircon_bootconfig,
    "sets Zircon boot partition based on information in sysconfig partition",
    "    argv: zircon_bootconfig <sysconfig partition> <Zircon-A partition> "
                                "<Zircon-B partition> <Zircon-R partition>\n"
    "    This command reads the Zircon boot configuration from partition <sysconfig partition>,\n"
    "    The environment variable \"boot_part\" is set to one of the partition names\n"
    "    <Zircon-A partition>,  <Zircon-B partition> or <Zircon-R partition> based on the\n"
    "    configuration in the sysconfig partition.\n"
);
