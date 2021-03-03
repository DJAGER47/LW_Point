#ifndef SST26VF_DRIVER_H__
#define SST26VF_DRIVER_H__

#include <stdint.h>
#include "sst26vf_config.h"
#include "sst26vf_instructions.h"

/*struct spi1_handler {
	static void initSPI();
	static void readWriteDMA(uint8_t *read, uint8_t *write, uint16_t len);
	static void writeDMA(uint8_t *data, uint16_t len);
	static void readDMA(uint8_t *data, uint16_t len);

	static void read(uint8_t *data);
	static void write(uint8_t data);
};*/


class SST26VF {
    public:
        static bool init();
        static bool erase_chip();
        static bool erase_sector(uint32_t sector_num);
        static uint32_t write_buffer(uint32_t addr, void* buf, uint32_t len);
        static uint32_t read_buffer(uint32_t addr, void* buf, uint32_t len);
        //------ Debug functions ------
        static void get_manufacturer_info(uint8_t* mfr_id, uint8_t* dev_type, uint8_t* dev_id);

    private:
        static void write_enable(bool enable);
        static uint8_t read_status();
        static bool wait_until_ready(uint32_t timeout = 1000);
        static uint32_t write_page(uint32_t addr, void* buf, uint32_t len);
};

#endif
