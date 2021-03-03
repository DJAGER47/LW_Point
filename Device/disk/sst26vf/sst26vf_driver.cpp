#include "stm32f1xx_hal.h"
#include "sst26vf_driver.h"
#include "main.h"

//==============================================================================
//==============================================================================

#define SPI_DISP    hspi1
extern SPI_HandleTypeDef SPI_DISP;
#define CS_Port       FLASH_CS_GPIO_Port
#define CS_Pin        FLASH_CS_Pin

#define CS_HIGH()     HAL_GPIO_WritePin(CS_Port, CS_Pin, GPIO_PIN_SET)
#define CS_LOW()      HAL_GPIO_WritePin(CS_Port, CS_Pin, GPIO_PIN_RESET)
//==============================================================================

//------------------------------------------------------------------------------------------------
bool SST26VF::init() {
	CS_HIGH();

	write_enable(true);// unlock write protected blocks
	if (!(read_status() & SST26VF_STAT_WEL))
		return false;

	CS_LOW();
	uint8_t send = SST26VF_CMD_ULBPR;
	// The default state after powerup reset is write-protected
	HAL_SPI_Transmit(&SPI_DISP, &send, sizeof(send), HAL_MAX_DELAY);
	CS_HIGH();

	return true;
}
//private:
uint8_t SST26VF::read_status() { //+
	uint8_t status = SST26VF_CMD_RDSR;
	CS_LOW();
	HAL_SPI_Transmit(&SPI_DISP, &status, sizeof(status), HAL_MAX_DELAY);
	HAL_SPI_Receive(&SPI_DISP, &status, sizeof(status), HAL_MAX_DELAY);
	CS_HIGH();
	return status;
}

void SST26VF::write_enable(bool enable) { //+
	CS_LOW();
	uint8_t status = enable ? SST26VF_CMD_WREN : SST26VF_CMD_WRDI;
	HAL_SPI_Transmit(&SPI_DISP, &status, sizeof(status), HAL_MAX_DELAY);
	CS_HIGH();
}

bool SST26VF::wait_until_ready(uint32_t timeout) { //+
	uint8_t status;
	// Read status bit to check if busy
	// while (timeout > 0) {
	//         status = read_status() & (SST26VF_STAT_BUSY | SST26VF_STAT_BUSY2);
	//         if (!status)
	//                 return false;

	//         asm __volatile__("nop");
	//         timeout--;
	// }
	do {
		status = read_status();
		status &= (SST26VF_STAT_BUSY | SST26VF_STAT_BUSY2);
	} while (status);

	return false;
}


//------------------------------------------------------------------------------------------------
//public:
uint32_t SST26VF::write_buffer(uint32_t addr, void *buf, uint32_t len) { //-
	// If the data is only on one page we can take a shortcut
	uint8_t* buff = (uint8_t*) buf;

	if ((addr % SST26VF_PAGE_SIZE) + len <= SST26VF_PAGE_SIZE)
		return write_page(addr, buff, len);


	uint32_t byteW = SST26VF_PAGE_SIZE - (addr % SST26VF_PAGE_SIZE);
	if (!write_page(addr, buff, byteW)) return 0;
	addr += byteW;
	buff += byteW;
	len -= byteW;

	while (len >= SST26VF_PAGE_SIZE) {
		if (!write_page(addr, buff, 256)) return byteW;
		byteW += 256;
		addr += 256;
		buff += 256;
		len -= 256;
	}

	if (len > 0){
		if (!write_page(addr, buff, len)) return byteW;
		byteW += len;
	}

	return byteW;
}

uint32_t SST26VF::write_page(uint32_t addr, void *buf, uint32_t len) { //+
	/*// Make sure the address is valid
	if (addr >= SST26VF_MAX_ADDR) return 0;
	// Make sure that the supplied data is no larger than the page size
	if (len > SST26VF_PAGE_SIZE) return 0;
	// Make sure that the data won't wrap around to the beginning of the sector
	if ((addr % SST26VF_PAGE_SIZE) + len > SST26VF_PAGE_SIZE) return 0;*/

	wait_until_ready();
	write_enable(true);
	if (!(read_status() & SST26VF_STAT_WEL))
		return 0;// Throw a write protection error

	CS_LOW();
	uint8_t *p = (uint8_t*)&addr;
	uint8_t cmd_addr[] = { SST26VF_CMD_PP, p[2], p[1], p[0] };
	HAL_SPI_Transmit(&SPI_DISP, cmd_addr, sizeof(cmd_addr), HAL_MAX_DELAY);
	HAL_SPI_Transmit_DMA(&SPI_DISP, (uint8_t*)buf, len);
	while (HAL_SPI_GetState(&SPI_DISP) != HAL_SPI_STATE_READY);
	CS_HIGH();
	return len;
}

uint32_t SST26VF::read_buffer(uint32_t addr, void *buf, uint32_t len) { //+
	/*// Make sure the address is valid
	if (addr >= SST26VF_MAX_ADDR) return 0;*/

	wait_until_ready();

	if ((addr + len) > SST26VF_MAX_ADDR) {
		len = SST26VF_MAX_ADDR - addr;
	}

	CS_LOW();
	uint8_t *p = (uint8_t*)&addr;
	uint8_t cmd_addr[] = { SST26VF_CMD_READ, p[2], p[1], p[0] };
	HAL_SPI_Transmit(&SPI_DISP, cmd_addr, sizeof(cmd_addr), HAL_MAX_DELAY);
	HAL_SPI_Receive_DMA(&SPI_DISP, (uint8_t*)buf, len);
	while (HAL_SPI_GetState(&SPI_DISP) != HAL_SPI_STATE_READY);
	CS_HIGH();
	return len;
}

bool SST26VF::erase_chip() { //+
	wait_until_ready();
	write_enable(true);
	if (!(read_status() & SST26VF_STAT_WEL))
		return false; // Throw a write protection error

	CS_LOW();
	uint8_t send = SST26VF_CMD_CE;
	HAL_SPI_Transmit(&SPI_DISP, &send, sizeof(send), HAL_MAX_DELAY);
	CS_HIGH();
	return true;
}

bool SST26VF::erase_sector(uint32_t sector_num) {
	wait_until_ready();
	write_enable(true);

	if (!(read_status() & SST26VF_STAT_WEL))
		return false;

	uint32_t addr = sector_num * SST26VF_SECTOR_SIZE;
	uint8_t *p = (uint8_t*)&addr;
	uint8_t cmd_addr[] = { 	SST26VF_CMD_SE, p[2], p[1], p[0] };

	CS_LOW();
	HAL_SPI_Transmit(&SPI_DISP, cmd_addr, sizeof(cmd_addr), HAL_MAX_DELAY);
	CS_HIGH();
	return true;
}

// --------- Debug functions -----------
void SST26VF::get_manufacturer_info(uint8_t *mfr_id, uint8_t *dev_type, uint8_t *dev_id) { //+
	CS_LOW();

	uint8_t read[4], write[4] = {SST26VF_CMD_JEDECID, 0,0,0};
	HAL_SPI_TransmitReceive(&SPI_DISP, write, read, sizeof(write), HAL_MAX_DELAY);
	*mfr_id = read[1];
	*dev_type = read[2];
	*dev_id = read[3];


	CS_HIGH();
}
