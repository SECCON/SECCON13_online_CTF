#ifndef HW_BABY_H
#define HW_BABY_H

#define TYPE_PCI_BABY_DEV "baby"

#define BABY_PCI_VENDOR_ID 0x4296
#define BABY_PCI_DEVICE_ID 0x1338

struct PCIBabyDevReg {
	off_t offset;
	uint32_t data;
};

#define MMIO_SET_OFFSET    offsetof(struct PCIBabyDevReg, offset)
#define MMIO_SET_DATA      offsetof(struct PCIBabyDevReg, data)
#define MMIO_GET_DATA      offsetof(struct PCIBabyDevReg, data)

// #define DEBUG_PCI_BABY_DEV

#ifdef  DEBUG_PCI_BABY_DEV
#define debug_printf(fmt, ...) printf("## (%3d) %-20s: " fmt, __LINE__, __func__, ## __VA_ARGS__)
#else
#define debug_printf(fmt, ...)
#endif


#endif
