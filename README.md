# SPDX-License-Identifier:	GPL-2.0+
# u-boot

This is a version of u-boot, modified to support Fuchsia on the Khadas vim2,
vim3, and NXP imx8mevk boards.

The master branch in this repository is empty.

Please check out one of the platform specific branches, like vim2 or imx8mevk
for access to the source.

u-boot uses the Gerrit code review system to review and accept patches. Our
instance is located at [https://third-party-mirror-review.googlesource.com/].
Please take a look at the CONTRIBUTING file and at that link if you're
interested.

Building vim2 Bootloader
========================

To build the vim2 bootloader for Fuchsia:

Prerequisites - arm and aarch64 toolchains. On Debian/Ubuntu systems, for example:
```
apt-get install gcc-arm-none-eabi gcc-aarch64-linux-gnu
```

1. Check out the vim2 branch
```
git checkout -b vim2 origin/vim2
```
2. Configure the target
```
make kvim2_defconfig
```
3. Build u-boot.bin
```
make CROSS_COMPILE=aarch64-linux-gnu-
```

You now have a u-boot.bin that you can flash to a target with using fastboot.
