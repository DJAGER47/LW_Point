#ifndef SST26VF_CONFIG_H__
#define SST26VF_CONFIG_H__
//FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 8192K
#define SST26VF_PAGE_SIZE       256
#define SST26VF_NUM_PAGES       32768
#define SST26VF_SECTOR_SIZE     4096
#define SST26VF_MAX_ADDR        0x7fffff // (8MB)

#endif
