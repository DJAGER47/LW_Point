#ifndef SST26VF_INSTRUC_H__
#define SST26VF_INSTRUC_H__


// Status codes
#define SST26VF_STAT_OK             0x00
#define SST26VF_STAT_BUSY           0x01
#define SST26VF_STAT_WEL            0x02
#define SST26VF_STAT_WSE            0x04
#define SST26VF_STAT_WSP            0x08
#define SST26VF_STAT_WPLD           0x10
#define SST26VF_STAT_SEC            0x20
#define SST26VF_STAT_RES            0x40
#define SST26VF_STAT_BUSY2          0x80

// Configuration instructions
#define SST26VF_CMD_NOP             0x00 // dummy write cmd
#define SST26VF_CMD_RSTEN           0x66
#define SST26VF_CMD_RST             0x99
#define SST26VF_CMD_EQIO            0x38
#define SST26VF_CMD_RSTQIO          0xff
#define SST26VF_CMD_RDSR            0x05 // read status register
#define SST26VF_CMD_WRSR            0x01
#define SST26VF_CMD_RDCR            0x35

// Read instructions
#define SST26VF_CMD_READ            0x03
#define SST26VF_CMD_FREAD           0x0b
#define SST26VF_CMD_SQOR            0x6b
#define SST26VF_CMD_SQIOR           0xeb
#define SST26VF_CMD_SDOR            0x3b
#define SST26VF_CMD_SDIOR           0xbb
#define SST26VF_CMD_SB              0xc0
#define SST26VF_CMD_RBSQI           0x0c
#define SST26VF_CMD_RBSPI           0xec

// Identification instructions
#define SST26VF_CMD_JEDECID         0x9f // Get manufacturer info
#define SST26VF_CMD_QJID            0xaf
#define SST26VF_CMD_SFDP            0x5a

// Write instructions
#define SST26VF_CMD_WREN            0x06 // write enable
#define SST26VF_CMD_WRDI            0x04 // write disable
#define SST26VF_CMD_SE              0x20 // sector erase
#define SST26VF_CMD_BE              0xd8 // block erase
#define SST26VF_CMD_CE              0xc7 // chip erase
#define SST26VF_CMD_PP              0x02
#define SST26VF_CMD_SQPP            0x32
#define SST26VF_CMD_WRSU            0xb0
#define SST26VF_CMD_WRRE            0x30

// Protection instructions
#define SST26VF_CMD_RBPR            0x72
#define SST26VF_CMD_WBPR            0x42
#define SST26VF_CMD_LBPR            0x8d
#define SST26VF_CMD_NVWLDR          0xe8
#define SST26VF_CMD_ULBPR           0x98 // global block Protection unlock
#define SST26VF_CMD_RSID            0x88
#define SST26VF_CMD_PSID            0xa5
#define SST26VF_CMD_LSID            0x85

#endif
