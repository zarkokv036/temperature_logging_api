#include "unity.h"
#include "mock_eeprom.h"
#include "temperature_logging.h"
#include <string.h>

#define FLASH_SIZE 8192
uint8_t simulated_eeprom[FLASH_SIZE];

static int mock_eeprom_read(uint16_t address, uint8_t *data,
  uint16_t length, int num_calls)
{
    if (address >= FLASH_SIZE) //8192
    {
        return 1;
    }
    else
    {
        memcpy(data, &simulated_eeprom[address], length);
        return 0;
    }
}



static int mock_eeprom_write(uint16_t address, const uint8_t* data,
  uint16_t length, int num_calls)
{
    if (address >= FLASH_SIZE)
    {
        return 1;
    }
    else
    {
        memcpy(&simulated_eeprom[address], data, length);
        return 0;
    } 
}



void setUp(void)
{
    eeprom_read_StubWithCallback(mock_eeprom_read);
    eeprom_write_StubWithCallback(mock_eeprom_write);
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
    
    uint32_t checkMagic = 0;
    memcpy(&checkMagic, simulated_eeprom, sizeof(checkMagic));
    TEST_ASSERT_EQUAL_HEX32(TEMP_LOGGING_MAGIC_NUMBER, checkMagic);
    
    uint16_t readIndex = 54, writeIndex = 47;
    memcpy(&readIndex, &simulated_eeprom[4], sizeof(readIndex));
    memcpy(&writeIndex, &simulated_eeprom[6], sizeof(writeIndex));
    TEST_ASSERT_EQUAL(0, readIndex);
    TEST_ASSERT_EQUAL(0, writeIndex);
    
    
    
    
}
