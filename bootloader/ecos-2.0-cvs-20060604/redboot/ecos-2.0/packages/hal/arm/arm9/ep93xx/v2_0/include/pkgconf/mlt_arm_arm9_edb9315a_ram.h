// eCos memory layout - Wed Nov 22 10:57:58 2000

// This is a generated file - do not edit

#ifndef __ASSEMBLER__
#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#endif
extern unsigned long SDRAMSize;
#define CYGMEM_REGION_ram (0)
#define CYGMEM_REGION_ram_SIZE (SDRAMSize)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
#ifndef __ASSEMBLER__
extern char CYG_LABEL_NAME (_heap1) [];
#endif
#define CYGMEM_SECTION_heap1 (CYG_LABEL_NAME (_heap1))
#define CYGMEM_SECTION_heap1_SIZE (CYGMEM_REGION_ram_SIZE - (size_t) CYG_LABEL_NAME (_heap1))
