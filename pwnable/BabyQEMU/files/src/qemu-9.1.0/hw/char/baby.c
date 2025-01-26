#include "qemu/osdep.h"
#include "hw/pci/pci_device.h"
#include "hw/qdev-properties.h"
#include "qemu/module.h"
#include "sysemu/kvm.h"
#include "qom/object.h"
#include "qapi/error.h"

#include "hw/char/baby.h"

struct PCIBabyDevState {
	PCIDevice parent_obj;

	MemoryRegion mmio;
	struct PCIBabyDevReg *reg_mmio;

	uint8_t buffer[0x100];
};

OBJECT_DECLARE_SIMPLE_TYPE(PCIBabyDevState, PCI_BABY_DEV)

static uint64_t pci_babydev_mmio_read(void *opaque, hwaddr addr, unsigned size);
static void pci_babydev_mmio_write(void *opaque, hwaddr addr, uint64_t val, unsigned size);

static const MemoryRegionOps pci_babydev_mmio_ops = {
	.read       = pci_babydev_mmio_read,
	.write      = pci_babydev_mmio_write,
	.endianness = DEVICE_LITTLE_ENDIAN,
	.impl = {
		.min_access_size = 1,
		.max_access_size = 4,
	},
};

static void pci_babydev_realize(PCIDevice *pci_dev, Error **errp) {
	PCIBabyDevState *ms = PCI_BABY_DEV(pci_dev);
	uint8_t *pci_conf;

	debug_printf("called\n");
	pci_conf = pci_dev->config;
	pci_conf[PCI_INTERRUPT_PIN] = 0;

	ms->reg_mmio = g_malloc(sizeof(struct PCIBabyDevReg));

	memory_region_init_io(&ms->mmio, OBJECT(ms), &pci_babydev_mmio_ops, ms, TYPE_PCI_BABY_DEV"-mmio", sizeof(struct PCIBabyDevReg));
	pci_register_bar(pci_dev, 0, PCI_BASE_ADDRESS_SPACE_MEMORY | PCI_BASE_ADDRESS_MEM_TYPE_64, &ms->mmio);
}

static void pci_babydev_reset(PCIBabyDevState *ms) {
	debug_printf("called\n");

	bzero(ms->reg_mmio, sizeof(struct PCIBabyDevReg));
	bzero(ms->buffer, sizeof(ms->buffer));
}

static void pci_babydev_uninit(PCIDevice *pci_dev) {
	PCIBabyDevState *ms = PCI_BABY_DEV(pci_dev);

	pci_babydev_reset(ms);
	g_free(ms->reg_mmio);
}

static void qdev_pci_babydev_reset(DeviceState *s) {
	PCIBabyDevState *ms = PCI_BABY_DEV(s);

	pci_babydev_reset(ms);
}

static Property pci_babydev_properties[] = {
	DEFINE_PROP_END_OF_LIST(),
};

static void pci_babydev_class_init(ObjectClass *klass, void *data) {
	DeviceClass *dc = DEVICE_CLASS(klass);
	PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

	k->realize = pci_babydev_realize;
	k->exit = pci_babydev_uninit;
	k->vendor_id = BABY_PCI_VENDOR_ID;
	k->device_id = BABY_PCI_DEVICE_ID;
	k->revision = 0x00;
	k->class_id = PCI_CLASS_OTHERS;
	dc->desc = "SECCON CTF 2024 Challenge : Baby QEMU Escape Device";
	set_bit(DEVICE_CATEGORY_MISC, dc->categories);
	dc->reset = qdev_pci_babydev_reset;
	device_class_set_props(dc, pci_babydev_properties);
}

static const TypeInfo pci_babydev_info = {
	.name          = TYPE_PCI_BABY_DEV,
	.parent        = TYPE_PCI_DEVICE,
	.instance_size = sizeof(PCIBabyDevState),
	.class_init    = pci_babydev_class_init,
	.interfaces = (InterfaceInfo[]) {
		{ INTERFACE_CONVENTIONAL_PCI_DEVICE },
		{ },
	},
};

static void pci_babydev_register_types(void) {
	type_register_static(&pci_babydev_info);
}

type_init(pci_babydev_register_types)

static uint64_t pci_babydev_mmio_read(void *opaque, hwaddr addr, unsigned size) {
	PCIBabyDevState *ms = opaque;
	struct PCIBabyDevReg *reg = ms->reg_mmio;

	debug_printf("addr:%lx, size:%d\n", addr, size);

	switch(addr){
		case MMIO_GET_DATA:
			debug_printf("get_data (%p)\n", &ms->buffer[reg->offset]);
			return *(uint64_t*)&ms->buffer[reg->offset];
	}
	
	return -1;
}

static void pci_babydev_mmio_write(void *opaque, hwaddr addr, uint64_t val, unsigned size) {
	PCIBabyDevState *ms = opaque;
	struct PCIBabyDevReg *reg = ms->reg_mmio;

	debug_printf("addr:%lx, size:%d, val:%lx\n", addr, size, val);

	switch(addr){
		case MMIO_SET_OFFSET:
			reg->offset = val;
			break;
		case MMIO_SET_OFFSET+4:
			reg->offset |= val << 32;
			break;
		case MMIO_SET_DATA:
			debug_printf("set_data (%p)\n", &ms->buffer[reg->offset]);
			*(uint64_t*)&ms->buffer[reg->offset] = (val & ((1UL << size*8) - 1)) | (*(uint64_t*)&ms->buffer[reg->offset] & ~((1UL << size*8) - 1));
			break;
	}
}
