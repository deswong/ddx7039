// eCos memory layout - Tue Nov 21 14:25:35 2000

// This is a generated file - do not edit

#include <cyg/infra/cyg_type.h>
#include <stddef.h>

extern unsigned long SDRAMSize;
#define CYGMEM_REGION_ram (0)
#define CYGMEM_REGION_ram_SIZE (SDRAMSize)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
#define CYGMEM_REGION_rom (0x60000000)
#define CYGMEM_REGION_rom_SIZE (0x2000000)
#define CYGMEM_REGION_rom_ATTR (CYGMEM_REGION_ATTR_R)
extern char CYG_LABEL_NAME (_heap1) [];
#define CYGMEM_SECTION_heap1 (CYG_LABEL_NAME (_heap1))
#define CYGMEM_SECTION_heap1_SIZE (CYGMEM_REGION_ram_SIZE - (size_t) CYG_LABEL_NAME (_heap1))
