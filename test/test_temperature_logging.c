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
    for(uint16_t i = 0; i < 5; i++)
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
    for(uint16_t i = 0; i < 7; i++)
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


/*
*
*
*
*/
void test_basic_read_write_when_eeprom_is_almost_empty(void)
{
    memset(simulated_eeprom, 0xFF, FLASH_SIZE);
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    
    uint16_t readValue = 0;
    status = tempLogging_read(&emptyBuffer, &readValue);
    TEST_ASSERT_EQUAL(TL_OK, status);
    //check if 0xFFFF will be returned when empty
    TEST_ASSERT_EQUAL_HEX32(0xFFFF, readValue);
    
    //write some values into eeprom
    for(uint16_t i = 0; i < 2; i++)
    {
        status = tempLogging_write(&emptyBuffer, &i);
        TEST_ASSERT_EQUAL_HEX32(TL_OK, status);
    }
    
    //test if indexes in eeprom are changed
    uint16_t readIndex = 99, writeIndex = 54;
    memcpy(&readIndex, (simulated_eeprom + 4), sizeof(readIndex));
    memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex));
    //indexes in ram on rom after write should be same
    TEST_ASSERT_EQUAL(readIndex, emptyBuffer.readIndex);
    TEST_ASSERT_EQUAL(writeIndex, emptyBuffer.writeIndex);
    
    //now read first value then test indexes then read second 
    for(uint16_t i = 0; i < 2; i++)
    {
        status = tempLogging_read(&emptyBuffer, &readValue);
        TEST_ASSERT_EQUAL(TL_OK, status);
        //check read value
        TEST_ASSERT_EQUAL(i, readValue);
        //in ram indexes
        TEST_ASSERT_EQUAL(i + 1, emptyBuffer.readIndex);
        TEST_ASSERT_EQUAL(2, emptyBuffer.writeIndex);
        //after read indexes are not changed in eeprom
        memcpy(&readIndex, (simulated_eeprom + 4), sizeof(readIndex));
        memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex));
        TEST_ASSERT_EQUAL(0, readIndex);
        TEST_ASSERT_EQUAL(2, writeIndex);
    }
    
    // check empty status 
    status = tempLogging_read(&emptyBuffer, &readValue);
    TEST_ASSERT_EQUAL(TL_OK, status);
    //check if 0xFFFF will be returned when empty
    TEST_ASSERT_EQUAL_HEX32(0xFFFF, readValue);
    
    //write some value to check are indexes updated in rom
    uint16_t value = 1l;
    status = tempLogging_write(&emptyBuffer, &value);
    memcpy(&readIndex, (simulated_eeprom + 4), sizeof(readIndex));
    memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex));
    TEST_ASSERT_EQUAL(emptyBuffer.readIndex, readIndex);
    TEST_ASSERT_EQUAL(emptyBuffer.writeIndex, writeIndex);
}


/*
*
*
*
*/
void test_read_write_when_eeprom_is_almost_full(void)
{
    //clear eeprom
    memset(simulated_eeprom, 0xFF, FLASH_SIZE);
    //write magic num into eeprom
    uint32_t checkMagic = TEMP_LOGGING_MAGIC_NUMBER;
    memcpy(simulated_eeprom, &checkMagic, sizeof(checkMagic));
    for(uint16_t i = 0; i < ((FLASH_SIZE - 8) / 2) - 1; i++) //one is missing to be full
    {
        memcpy(simulated_eeprom + 8 + (i * 2), &i, sizeof(i));  
    }
    //write indexes into eeprom
    //writeIndex point to the last place in the buffer
    uint16_t readIndex = 0, writeIndex = ((FLASH_SIZE - 8) / 2) - 1;
    memcpy((simulated_eeprom + 4), &readIndex, sizeof(readIndex));
    memcpy((simulated_eeprom + 6), &writeIndex, sizeof(writeIndex));
    
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    
    //after this write writeIndex should point to 0 
    uint16_t value = 88;
    status = tempLogging_write(&emptyBuffer, &value);
    TEST_ASSERT_EQUAL(TL_OK, status);
    TEST_ASSERT_EQUAL(0, emptyBuffer.writeIndex);
    
    memcpy(&readIndex,  (simulated_eeprom + 4), sizeof(readIndex));
    memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex));
    
    //check if the oldest is overwritten
    status = tempLogging_write(&emptyBuffer, &value);
    memcpy(&value, (simulated_eeprom+8), sizeof(value));
    TEST_ASSERT_EQUAL(88, value);
    //test if readIndex is incremented
    memcpy(&readIndex,  (simulated_eeprom + 4), sizeof(readIndex));
    memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex));
    TEST_ASSERT_EQUAL(readIndex, writeIndex + 1); // writeIndex+1 because  readIndex is higher by one
    
    //read after this should point to the last place in buffer
    // readIndex = 2  now
    uint16_t readValue = 0;
    for(uint16_t i = 0; i < ((FLASH_SIZE - 8) / 2); i++)
    { 
        tempLogging_read(&emptyBuffer, &readValue);
    }
  
    TEST_ASSERT_EQUAL(emptyBuffer.writeIndex, emptyBuffer.readIndex);
       
}

/*
*
*
*
*/
void test_flush(void)
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
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    
    for(uint16_t i = 0; i < 3; i++)
    {
        uint16_t readValue = 0;        
        tempLogging_read(&emptyBuffer, &readValue);
    }
    tempLogging_flush(&emptyBuffer);
    memcpy(&readIndex,  (simulated_eeprom + 4), sizeof(readIndex));
    memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex));
    // test if indexes in ram and rom are the same
    TEST_ASSERT_EQUAL(emptyBuffer.readIndex, readIndex);
    TEST_ASSERT_EQUAL(emptyBuffer.writeIndex, writeIndex);
    
    
}






