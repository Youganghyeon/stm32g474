/*
 * sd.h
 *
 *  Created on: 2026. 3. 14.
 *      Author: yougang
 */

#ifndef SRC_COMMON_HW_INCLUDE_SD_H_
#define SRC_COMMON_HW_INCLUDE_SD_H_

#include "hw_def.h"

#ifdef _USE_HW_SD

typedef struct
{
  uint32_t card_type;
  uint32_t card_version;
  uint32_t card_class;
  uint32_t rel_card_Add;
  uint32_t block_numbers;
  uint32_t block_size;
  uint32_t log_block_numbers;
  uint32_t log_block_size;
  uint32_t card_size;
}sd_info_t;


bool sdInit(void);
bool sdDeinit(void);
bool sdIsInit(void);
bool sdIsDetected(void);
bool sdGetInfo(sd_info_t *p_info);
bool sdIsBusy(void);
bool sdIsReady(uint32_t timeout);

bool sdReadBlocks(uint32_t block_addr, uint8_t *p_data, uint32_t num_of_blocks, uint32_t timeout_ms);
bool sdWriteBlocks(uint32_t block_addr, uint8_t *p_data, uint32_t num_of_blocks, uint32_t timeout_ms);
bool sdEraseBlocks(uint32_t start_addr, uint32_t end_addr);



#endif
#endif /* SRC_COMMON_HW_INCLUDE_SD_H_ */
