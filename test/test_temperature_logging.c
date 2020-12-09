#include "unity.h"
#include "mock_eeprom.h"
#include "temperature_logging.h"
#include <string.h>

#define FLASH_SIZE 8192
uint8_t simulated_eeprom[FLASH_SIZE];

static int mock_eeprom_read(uint8_t* dest_buffer, uint8_t* source_address,
  size_t count, int num_calls)
{
    if (source_address >= (uint8_t*)FLASH_SIZE) //8192
    {
        return 1;
    }
    else
    {
        memcpy(dest_buffer, &simulated_eeprom[(int)source_address], count);
        return 0;
    }
}



static int mock_eeprom_write(uint8_t* source_data, uint8_t* target_address,
  size_t count, int num_calls)
{
    if (target_address >= (uint8_t*)FLASH_SIZE)
    {
        TEST_FAIL_MESSAGE("invalid flash memory address");
    }
    else
    {
        memcpy(&simulated_eeprom[(int)target_address], source_data, count);
        return 0;
    }
  
}



void setUp(void)
{
    eeprom_read_StubWithCallback(mock_eeprom_read);
    eeprom_write_StubWithCallback(mock_eeprom_read);
}

void tearDown(void)
{
}

void test_temperature_logging_NeedToImplement(void)
{
    memset(simulated_eeprom, 0xff, FLASH_SIZE);
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    TEST_ASSERT_EQUAL(TL_OK, status);
    
}
