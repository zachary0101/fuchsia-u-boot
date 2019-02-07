# Copyright 2019 The Fuchsia Authors. All rights reserved.
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

obj-$(CONFIG_LIBAVB)+= \
avb_hashtree_descriptor.o \
avb_kernel_cmdline_descriptor.o \
avb_vbmeta_image.o \
avb_property_descriptor.o \
avb_hash_descriptor.o \
avb_slot_verify.o \
avb_chain_partition_descriptor.o \
avb_rsa.o \
avb_sysdeps_posix.o \
avb_sha256.o \
avb_version.o \
avb_util.o \
avb_footer.o \
avb_crc32.o \
avb_crypto.o \
avb_descriptor.o \
avb_sha512.o \
avb_cmdline.o

ccflags-y+= -DAVB_COMPILATION
