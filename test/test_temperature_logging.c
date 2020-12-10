#include "unity.h"
#include "mock_eeprom.h"
#include "temperature_logging.h"
#include <string.h>
#include <stdio.h>

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
/*
*
*
*
************/
void test_init_when_eeprom_is_empty(void)
{
    //clear eeprom
    memset(simulated_eeprom, 0xFF, FLASH_SIZE);
    //Test init of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    TEST_ASSERT_EQUAL(TL_OK, status);
    //check magic num
    uint32_t checkMagic = 0;
    memcpy(&checkMagic, simulated_eeprom, sizeof(checkMagic));
    TEST_ASSERT_EQUAL_HEX32(TEMP_LOGGING_MAGIC_NUMBER, checkMagic);
    //check indexes
    uint16_t readIndex = 54, writeIndex = 47;
    memcpy(&readIndex, &simulated_eeprom[4], sizeof(readIndex));
    memcpy(&writeIndex, &simulated_eeprom[6], sizeof(writeIndex));
    TEST_ASSERT_EQUAL(0, readIndex);
    TEST_ASSERT_EQUAL(0, writeIndex);
}
/*
*
*
*
*/
void test_of_init_when_eeprom_is_previously_initiated(void)
{
    //clear eeprom
    memset(simulated_eeprom, 0xFF, FLASH_SIZE);
    //write magic num into eeprom
    uint32_t checkMagic = TEMP_LOGGING_MAGIC_NUMBER;
    memcpy(simulated_eeprom, &checkMagic, sizeof(checkMagic));
    //write indexes into eeprom
    uint16_t readIndex = 1, writeIndex = 7;
    memcpy((simulated_eeprom + 4), &readIndex, sizeof(readIndex));
    memcpy((simulated_eeprom + 6), &writeIndex, sizeof(writeIndex));
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    TEST_ASSERT_EQUAL(TL_OK, status);
    
    TEST_ASSERT_EQUAL(readIndex, emptyBuffer.readIndex);
    TEST_ASSERT_EQUAL(writeIndex, emptyBuffer.writeIndex);     
}


/*
*
*
*
*/
void test_basic_write_funcionality(void)
{
    //clear eeprom
    memset(simulated_eeprom, 0xFF, FLASH_SIZE);
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    //writing some values into eeprom
    for(uint16_t i = 0; i<5; i++)
    {
        status = tempLogging_write(&emptyBuffer, &i);
        TEST_ASSERT_EQUAL(TL_OK, status);
        
        uint16_t writtenValue = 0;
        memcpy(&writtenValue, (simulated_eeprom + 8 + (i * 2)), sizeof(writtenValue)); //8 is buffer address in eeprom
        //check written value
        TEST_ASSERT_EQUAL(i, writtenValue);
        //when write() is used value of indexes in eeprom and ram should be the same
        uint16_t writeIndex = 0;
        memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex)); 
        TEST_ASSERT_EQUAL(emptyBuffer.writeIndex, writeIndex);    
    }
}

/*
*
*
*
*/
void test_basic_read_funcionality(void)
{
    //clear eeprom
    memset(simulated_eeprom, 0xFF, FLASH_SIZE);
    //write magic num into eeprom
    uint32_t checkMagic = TEMP_LOGGING_MAGIC_NUMBER;
    memcpy(simulated_eeprom, &checkMagic, sizeof(checkMagic));
    //write indexes into eeprom
    uint16_t readIndex = 0, writeIndex = 7;
    memcpy((simulated_eeprom + 4), &readIndex, sizeof(readIndex));
    memcpy((simulated_eeprom + 6), &writeIndex, sizeof(writeIndex));
    //fill the eeprom
    for(uint16_t i = 0; i<7; i++)
    {
        memcpy(simulated_eeprom + 8 + (i * 2), &i, sizeof(i));  
    }    
    
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    TEST_ASSERT_EQUAL(TL_OK, status);
    //read values from eeprom
    for(uint16_t i = 0; i<7; i++)
    {
        uint16_t readValue = 0;
        status = tempLogging_read(&emptyBuffer, &readValue);
        TEST_ASSERT_EQUAL(TL_OK, status);
        //printf("%d,", readValue);
        TEST_ASSERT_EQUAL(i, readValue);
    }
    
    //test will be 0xFFFF returned when eeprom is empty
    uint16_t readValue = 0;
    status = tempLogging_read(&emptyBuffer, &readValue);
    TEST_ASSERT_EQUAL_HEX16(0xFFFF, readValue);
    
    //test if readIndex in eeprom is changed
    readIndex = 99;
    memcpy(&readIndex, (simulated_eeprom + 4), sizeof(readIndex));
    //should be 0 in eeprom
    TEST_ASSERT_EQUAL(0, readIndex);
    //should be 7 in ram
    TEST_ASSERT_EQUAL(7, emptyBuffer.readIndex);
    
}







