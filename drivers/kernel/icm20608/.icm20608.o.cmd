cmd_/home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/icm20608/icm20608.o := arm-linux-gnueabihf-gcc -Wp,-MD,/home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/icm20608/.icm20608.o.d  -nostdinc -isystem /usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/bin/../lib/gcc/arm-linux-gnueabihf/4.9.4/include -I/home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include -Iarch/arm/include/generated/uapi -Iarch/arm/include/generated  -I/home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include -Iinclude -I/home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi -Iinclude/generated/uapi -include /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/kconfig.h   -I/home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/icm20608 -D__KERNEL__ -mlittle-endian -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -std=gnu89 -fno-dwarf2-cfi-asm -fno-ipa-sra -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -fno-delete-null-pointer-checks -O2 --param=allow-store-data-races=0 -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -fno-var-tracking-assignments -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -Werror=date-time -DCC_HAVE_ASM_GOTO   -I/home/pointer/imx6ull/projects/include/uapi  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(icm20608)"  -D"KBUILD_MODNAME=KBUILD_STR(icm20608)" -c -o /home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/icm20608/.tmp_icm20608.o /home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/icm20608/icm20608.c

source_/home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/icm20608/icm20608.o := /home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/icm20608/icm20608.c

deps_/home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/icm20608/icm20608.o := \
    $(wildcard include/config/2.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/delay.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/kernel.h \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/panic/timeout.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/lib/gcc/arm-linux-gnueabihf/4.9.4/include/stdarg.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/linkage.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
    $(wildcard include/config/gcov/kernel.h) \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/types.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/types.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/int-ll64.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/int-ll64.h \
  arch/arm/include/generated/asm/bitsperlong.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/bitsperlong.h \
    $(wildcard include/config/64bit.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/bitsperlong.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/posix_types.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/stddef.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/stddef.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/uapi/asm/posix_types.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/posix_types.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/stringify.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/linkage.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/bitops.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/bitops.h \
    $(wildcard include/config/smp.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/typecheck.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/irqflags.h \
    $(wildcard include/config/cpu/v7m.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/uapi/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/hwcap.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/uapi/asm/hwcap.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/barrier.h \
    $(wildcard include/config/cpu/32v6k.h) \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/arch/has/barriers.h) \
    $(wildcard include/config/arm/dma/mem/bufferable.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/outercache.h \
    $(wildcard include/config/outer/cache/sync.h) \
    $(wildcard include/config/outer/cache.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/bitops/non-atomic.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/bitops/fls64.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/bitops/sched.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/bitops/hweight.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/bitops/arch_hweight.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/bitops/const_hweight.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/bitops/lock.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/bitops/le.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/uapi/asm/byteorder.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/byteorder/little_endian.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/byteorder/little_endian.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/swab.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/swab.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/swab.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/uapi/asm/swab.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/byteorder/generic.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/bitops/ext2-atomic-setbit.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/printk.h \
    $(wildcard include/config/message/loglevel/default.h) \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/init.h \
    $(wildcard include/config/broken/rodata.h) \
    $(wildcard include/config/lto.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/kern_levels.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/kernel.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/sysinfo.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/cache.h \
    $(wildcard include/config/arm/l1/cache/shift.h) \
    $(wildcard include/config/aeabi.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/dynamic_debug.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/string.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/string.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/errno.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/errno.h \
  arch/arm/include/generated/asm/errno.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/errno.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/errno-base.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/div64.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/compiler.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
    $(wildcard include/config/arm/lpae.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/opcodes.h \
    $(wildcard include/config/cpu/endian/be32.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/delay.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/memory.h \
    $(wildcard include/config/need/mach/memory/h.h) \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/dram/base.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/have/tcm.h) \
    $(wildcard include/config/arm/patch/phys/virt.h) \
    $(wildcard include/config/phys/offset.h) \
    $(wildcard include/config/virt/to/bus.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/const.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/sizes.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/sparsemem.h) \
  arch/arm/include/generated/asm/param.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/param.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/fs.h \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/ima.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fs/dax.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/migration.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/wait.h \
    $(wildcard include/config/lockdep.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/context/tracking.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  arch/arm/include/generated/asm/preempt.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/preempt.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
    $(wildcard include/config/debug/stack/usage.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/bug.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/crunch.h) \
    $(wildcard include/config/arm/thumbee.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/page.h \
    $(wildcard include/config/cpu/copy/v4wt.h) \
    $(wildcard include/config/cpu/copy/v4wb.h) \
    $(wildcard include/config/cpu/copy/feroceon.h) \
    $(wildcard include/config/cpu/copy/fa.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/xscale.h) \
    $(wildcard include/config/cpu/copy/v6.h) \
    $(wildcard include/config/kuser/helpers.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/glue.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/pgtable-2level-types.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/getorder.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
    $(wildcard include/config/cpu/use/domains.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/bottom_half.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/preempt_mask.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/spinlock_types.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/spinlock_types.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/lockdep.h \
    $(wildcard include/config/lock/stat.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/rwlock_types.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/spinlock.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/prefetch.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/processor.h \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/arm/errata/754327.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/hw_breakpoint.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/rwlock.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/spinlock_api_smp.h \
    $(wildcard include/config/inline/spin/lock.h) \
    $(wildcard include/config/inline/spin/lock/bh.h) \
    $(wildcard include/config/inline/spin/lock/irq.h) \
    $(wildcard include/config/inline/spin/lock/irqsave.h) \
    $(wildcard include/config/inline/spin/trylock.h) \
    $(wildcard include/config/inline/spin/trylock/bh.h) \
    $(wildcard include/config/uninline/spin/unlock.h) \
    $(wildcard include/config/inline/spin/unlock/bh.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/inline/spin/unlock/irqrestore.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/rwlock_api_smp.h \
    $(wildcard include/config/inline/read/lock.h) \
    $(wildcard include/config/inline/write/lock.h) \
    $(wildcard include/config/inline/read/lock/bh.h) \
    $(wildcard include/config/inline/write/lock/bh.h) \
    $(wildcard include/config/inline/read/lock/irq.h) \
    $(wildcard include/config/inline/write/lock/irq.h) \
    $(wildcard include/config/inline/read/lock/irqsave.h) \
    $(wildcard include/config/inline/write/lock/irqsave.h) \
    $(wildcard include/config/inline/read/trylock.h) \
    $(wildcard include/config/inline/write/trylock.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/inline/read/unlock/bh.h) \
    $(wildcard include/config/inline/write/unlock/bh.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/inline/read/unlock/irqrestore.h) \
    $(wildcard include/config/inline/write/unlock/irqrestore.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/atomic.h \
    $(wildcard include/config/arch/has/atomic/or.h) \
    $(wildcard include/config/generic/atomic64.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/atomic.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/cmpxchg.h \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/v6.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/cmpxchg-local.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/atomic-long.h \
  arch/arm/include/generated/asm/current.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/current.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/wait.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/kdev_t.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/kdev_t.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/dcache.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/rculist.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/rcupdate.h \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/rcu/trace.h) \
    $(wildcard include/config/rcu/stall/common.h) \
    $(wildcard include/config/rcu/user/qs.h) \
    $(wildcard include/config/rcu/nocb/cpu.h) \
    $(wildcard include/config/tasks/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/prove/rcu.h) \
    $(wildcard include/config/rcu/boost.h) \
    $(wildcard include/config/rcu/nocb/cpu/all.h) \
    $(wildcard include/config/no/hz/full/sysidle.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/bitmap.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/seqlock.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/completion.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/rcutree.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/rculist_bl.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/list_bl.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/bit_spinlock.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/lockref.h \
    $(wildcard include/config/arch/use/cmpxchg/lockref.h) \
  include/generated/bounds.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/path.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/stat.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/uapi/asm/stat.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/stat.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/math64.h \
    $(wildcard include/config/arch/supports/int128.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/time64.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/time.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/uidgid.h \
    $(wildcard include/config/multiuser.h) \
    $(wildcard include/config/user/ns.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/highuid.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/list_lru.h \
    $(wildcard include/config/memcg/kmem.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/nodemask.h \
    $(wildcard include/config/movable/node.h) \
    $(wildcard include/config/numa.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/shrinker.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/llist.h \
    $(wildcard include/config/arch/have/nmi/safe/cmpxchg.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/radix-tree.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/rbtree.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/pid.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/mutex/spin/on/owner.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/osq_lock.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/rwsem.h \
    $(wildcard include/config/rwsem/spin/on/owner.h) \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  arch/arm/include/generated/asm/rwsem.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/rwsem.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/capability.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/capability.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/semaphore.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/fiemap.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/migrate_mode.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/percpu-rwsem.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/virtual.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/smp.h \
    $(wildcard include/config/up/late/init.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/smp.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/pfn.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/percpu.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/percpu.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/blk_types.h \
    $(wildcard include/config/blk/cgroup.h) \
    $(wildcard include/config/blk/dev/integrity.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/fs.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/limits.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/ioctl.h \
  arch/arm/include/generated/asm/ioctl.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/ioctl.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/ioctl.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/percpu_counter.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/gfp.h \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/cma.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/isolation.h) \
    $(wildcard include/config/memcg.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/have/memblock/node/map.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/page/extension.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/numa/balancing.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/page-flags-layout.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/memory_hotplug.h \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/have/bootmem/info/node.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/notifier.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/srcu.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/ktime.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/jiffies.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/timex.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/timex.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/param.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/timex.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/timekeeping.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/topology.h \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
    $(wildcard include/config/sched/smt.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/topology.h \
    $(wildcard include/config/arm/cpu/topology.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/topology.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/dqblk_xfs.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/dqblk_v1.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/dqblk_v2.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/dqblk_qtree.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/projid.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/quota.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/nfs_fs_i.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/fcntl.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/fcntl.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/uapi/asm/fcntl.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/fcntl.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/err.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/miscdevice.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/major.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/module.h \
    $(wildcard include/config/module/sig.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/livepatch.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/debug/set/module/ronx.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/kmod.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/sysctl.h \
    $(wildcard include/config/sysctl.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/sysctl.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/elf.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/elf.h \
    $(wildcard include/config/vdso.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/auxvec.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/uapi/asm/auxvec.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/vdso_datapage.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/user.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/elf.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/elf-em.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/kobject.h \
    $(wildcard include/config/uevent/helper.h) \
    $(wildcard include/config/debug/kobject/release.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/sysfs.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/kernfs.h \
    $(wildcard include/config/kernfs.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/idr.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/kobject_ns.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/kref.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ppc64.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/jump_label.h \
    $(wildcard include/config/jump/label.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/module.h \
    $(wildcard include/config/arm/unwind.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/module.h \
    $(wildcard include/config/have/mod/arch/specific.h) \
    $(wildcard include/config/modules/use/elf/rel.h) \
    $(wildcard include/config/modules/use/elf/rela.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/of.h \
    $(wildcard include/config/sparc.h) \
    $(wildcard include/config/of/dynamic.h) \
    $(wildcard include/config/of.h) \
    $(wildcard include/config/attach/node.h) \
    $(wildcard include/config/detach/node.h) \
    $(wildcard include/config/add/property.h) \
    $(wildcard include/config/remove/property.h) \
    $(wildcard include/config/update/property.h) \
    $(wildcard include/config/no/change.h) \
    $(wildcard include/config/change/add.h) \
    $(wildcard include/config/change/remove.h) \
    $(wildcard include/config/of/resolve.h) \
    $(wildcard include/config/of/overlay.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/mod_devicetable.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/uuid.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/uuid.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/property.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/fwnode.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/slab.h \
    $(wildcard include/config/debug/slab.h) \
    $(wildcard include/config/kmemcheck.h) \
    $(wildcard include/config/failslab.h) \
    $(wildcard include/config/slab.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/slob.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/kmemleak.h \
    $(wildcard include/config/debug/kmemleak.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/kasan.h \
    $(wildcard include/config/kasan.h) \
    $(wildcard include/config/kasan/shadow/offset.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/spi/spi.h \
    $(wildcard include/config/spi.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/pinctrl.h) \
    $(wildcard include/config/dma/cma.h) \
    $(wildcard include/config/devtmpfs.h) \
    $(wildcard include/config/sysfs/deprecated.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/ioport.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/klist.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/pinctrl/devinfo.h \
    $(wildcard include/config/pm.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/pinctrl/consumer.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/seq_file.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/pinctrl/pinctrl-state.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/pm.h \
    $(wildcard include/config/vt/console/sleep.h) \
    $(wildcard include/config/pm/clk.h) \
    $(wildcard include/config/pm/generic/domains.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/ratelimit.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/device.h \
    $(wildcard include/config/dmabounce.h) \
    $(wildcard include/config/iommu/api.h) \
    $(wildcard include/config/arm/dma/use/iommu.h) \
    $(wildcard include/config/arch/omap.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/pm_wakeup.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/kthread.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/sched.h \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/no/hz/common.h) \
    $(wildcard include/config/lockup/detector.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/sched/autogroup.h) \
    $(wildcard include/config/virt/cpu/accounting/native.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/fanotify.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/perf/events.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/sched/mc.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/cgroup/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/cc/stackprotector.h) \
    $(wildcard include/config/virt/cpu/accounting/gen.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/uprobes.h) \
    $(wildcard include/config/bcache.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/irq/time/accounting.h) \
    $(wildcard include/config/no/hz/full.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/stack/growsup.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/sched.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/sched/prio.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/arch/enable/split/pmd/ptlock.h) \
    $(wildcard include/config/have/cmpxchg/double.h) \
    $(wildcard include/config/have/aligned/struct/page.h) \
    $(wildcard include/config/transparent/hugepage.h) \
    $(wildcard include/config/pgtable/levels.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mmu/notifier.h) \
    $(wildcard include/config/x86/intel/mpx.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/auxvec.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/auxvec.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/uprobes.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/mmu.h \
    $(wildcard include/config/cpu/has/asid.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/cputime.h \
  arch/arm/include/generated/asm/cputime.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/cputime.h \
    $(wildcard include/config/virt/cpu/accounting.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/cputime_jiffies.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/sem.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/sem.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/ipc.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/ipc.h \
  arch/arm/include/generated/asm/ipcbuf.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/ipcbuf.h \
  arch/arm/include/generated/asm/sembuf.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/sembuf.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/shm.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/shm.h \
  arch/arm/include/generated/asm/shmbuf.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/shmbuf.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/shmparam.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/signal.h \
    $(wildcard include/config/old/sigaction.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/signal.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/signal.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/uapi/asm/signal.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/signal-defs.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/uapi/asm/sigcontext.h \
  arch/arm/include/generated/asm/siginfo.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/siginfo.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/siginfo.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/proportions.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
    $(wildcard include/config/have/arch/seccomp/filter.h) \
    $(wildcard include/config/seccomp/filter.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/seccomp.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/resource.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/resource.h \
  arch/arm/include/generated/asm/resource.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/resource.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/asm-generic/resource.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/timerfd.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/timerqueue.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/latencytop.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/cred.h \
    $(wildcard include/config/debug/credentials.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/key.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/assoc_array.h \
    $(wildcard include/config/associative/array.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/selinux.h \
    $(wildcard include/config/security/selinux.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/uapi/linux/magic.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/scatterlist.h \
    $(wildcard include/config/debug/sg.h) \
    $(wildcard include/config/arch/has/sg/chain.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/mm.h \
    $(wildcard include/config/mem/soft/dirty.h) \
    $(wildcard include/config/x86.h) \
    $(wildcard include/config/ppc.h) \
    $(wildcard include/config/parisc.h) \
    $(wildcard include/config/metag.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/debug/vm/rb.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/hibernation.h) \
    $(wildcard include/config/hugetlbfs.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/range.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/page_ext.h \
    $(wildcard include/config/page/owner.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/stacktrace.h \
    $(wildcard include/config/stacktrace.h) \
    $(wildcard include/config/user/stacktrace/support.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/pgtable.h \
    $(wildcard include/config/highpte.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/proc-fns.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/glue-proc.h \
    $(wildcard include/config/cpu/arm7tdmi.h) \
    $(wildcard include/config/cpu/arm720t.h) \
    $(wildcard include/config/cpu/arm740t.h) \
    $(wildcard include/config/cpu/arm9tdmi.h) \
    $(wildcard include/config/cpu/arm920t.h) \
    $(wildcard include/config/cpu/arm922t.h) \
    $(wildcard include/config/cpu/arm925t.h) \
    $(wildcard include/config/cpu/arm926t.h) \
    $(wildcard include/config/cpu/arm940t.h) \
    $(wildcard include/config/cpu/arm946e.h) \
    $(wildcard include/config/cpu/arm1020.h) \
    $(wildcard include/config/cpu/arm1020e.h) \
    $(wildcard include/config/cpu/arm1022.h) \
    $(wildcard include/config/cpu/arm1026.h) \
    $(wildcard include/config/cpu/mohawk.h) \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/cpu/v6k.h) \
    $(wildcard include/config/cpu/pj4b.h) \
    $(wildcard include/config/cpu/v7.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/pgtable-nopud.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/pgtable-hwdef.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/pgtable-2level-hwdef.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/tlbflush.h \
    $(wildcard include/config/smp/on/up.h) \
    $(wildcard include/config/cpu/tlb/v4wt.h) \
    $(wildcard include/config/cpu/tlb/fa.h) \
    $(wildcard include/config/cpu/tlb/v4wbi.h) \
    $(wildcard include/config/cpu/tlb/feroceon.h) \
    $(wildcard include/config/cpu/tlb/v4wb.h) \
    $(wildcard include/config/cpu/tlb/v6.h) \
    $(wildcard include/config/cpu/tlb/v7.h) \
    $(wildcard include/config/arm/errata/720789.h) \
    $(wildcard include/config/arm/errata/798181.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/pgtable-2level.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/pgtable.h \
    $(wildcard include/config/have/arch/soft/dirty.h) \
    $(wildcard include/config/have/arch/huge/vmap.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/page-flags.h \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/ksm.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/huge_mm.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
    $(wildcard include/config/debug/tlbflush.h) \
    $(wildcard include/config/debug/vm/vmacache.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/vm_event_item.h \
    $(wildcard include/config/memory/balloon.h) \
    $(wildcard include/config/balloon/compaction.h) \
  arch/arm/include/generated/asm/scatterlist.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/scatterlist.h \
    $(wildcard include/config/need/sg/dma/length.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/io.h \
    $(wildcard include/config/pci.h) \
    $(wildcard include/config/need/mach/io/h.h) \
    $(wildcard include/config/pcmcia/soc/common.h) \
    $(wildcard include/config/isa.h) \
    $(wildcard include/config/pccard.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/pci_iomap.h \
    $(wildcard include/config/no/generic/pci/ioport/map.h) \
    $(wildcard include/config/generic/pci/iomap.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/xen/xen.h \
    $(wildcard include/config/xen.h) \
    $(wildcard include/config/xen/dom0.h) \
    $(wildcard include/config/xen/pvh.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/asm-generic/io.h \
    $(wildcard include/config/generic/iomap.h) \
    $(wildcard include/config/has/ioport/map.h) \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/vmalloc.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/include/linux/uaccess.h \
  /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/sources/linux-imx/arch/arm/include/asm/uaccess.h \
    $(wildcard include/config/have/efficient/unaligned/access.h) \
  /home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/icm20608/../../../include/uapi/smarthome_icm20608.h \

/home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/icm20608/icm20608.o: $(deps_/home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/icm20608/icm20608.o)

$(deps_/home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/icm20608/icm20608.o):
