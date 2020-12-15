/*************************************************
* @file test_temperature_logging.c
* @author Zarko Milojicic
*/


#include "unity.h"
#include "mock_eeprom.h"
#include "temperature_logging.h"
#include <string.h>
#include <stdio.h>

#define FLASH_SIZE 8192

union SimulatedEeprom {
    uint8_t rawData[FLASH_SIZE];
    struct {
        uint32_t magic_number;
        uint16_t readIndex;
        uint16_t writeIndex;
        uint8_t buffer[FLASH_SIZE - 8];
    };
};

union SimulatedEeprom simulatedEeprom;


static int mock_eeprom_read(uint16_t address, uint8_t *data,
  uint16_t length, int num_calls)
{
    if (address >= FLASH_SIZE) //8192
    {
        return 1;
    }
    else
    {
        memcpy(data, &simulatedEeprom.rawData[address], length);
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
        memcpy(&simulatedEeprom.rawData[address], data, length);
        return 0;
    } 
}

/**
* @brief This function set initiate state of EEPROM.
* @param[in] readIndex Initiate value of readIndex.
* @param[in] writeIndex Initiate value of writeIndex.
*/
static void setInitiateStateOfEEPROM(uint16_t readIndex, uint16_t writeIndex)
{
    //write magic num into eeprom
    uint32_t checkMagic = TEMP_LOGGING_MAGIC_NUMBER;
    simulatedEeprom.magic_number = checkMagic;
    //write indexes into 
    simulatedEeprom.readIndex = readIndex;
    simulatedEeprom.writeIndex = writeIndex;
}

void setUp(void)
{
    eeprom_read_StubWithCallback(mock_eeprom_read);
    eeprom_write_StubWithCallback(mock_eeprom_write);
    //clear eeprom
    memset(&simulatedEeprom.rawData, 0xFF, FLASH_SIZE);
}

void tearDown(void)
{
}

/**
* @test Test of log initialization when eeprom is empty 
  magic number and indexes are not in eeprom
*
*  
*/
void test_init_when_eeprom_is_empty(void)
{
    //Test init of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    TEST_ASSERT_EQUAL(TL_OK, status);
    //check magic num
    TEST_ASSERT_EQUAL_HEX32(TEMP_LOGGING_MAGIC_NUMBER, simulatedEeprom.magic_number);
    //check indexes
    TEST_ASSERT_EQUAL(0, simulatedEeprom.readIndex);
    TEST_ASSERT_EQUAL(0, simulatedEeprom.writeIndex);
}

/**
* @test Test of log initialization when eeprom is initiated,
  magic number and indexes are in eeprom
*
* 
*/
void test_InitWhenEepromIsPreviouslyInitiated(void)
{
    //write indexes and magic num into eeprom
    setInitiateStateOfEEPROM(1, 7);
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    TEST_ASSERT_EQUAL(TL_OK, status);
    //check indexes
    TEST_ASSERT_EQUAL(simulatedEeprom.readIndex, emptyBuffer.readIndex);
    TEST_ASSERT_EQUAL(simulatedEeprom.writeIndex, emptyBuffer.writeIndex);     
}


/**
* @test Test of write function when eeprom is empty and
* Imedietly writing of indexes from ram to eeprom when write
* is used.
*
* 
*/
void test_write(void)
{
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    //writing some values into eeprom
    for(uint16_t i = 0; i < 5; i++)
    {
        status = tempLogging_write(&emptyBuffer, &i);
        TEST_ASSERT_EQUAL(TL_OK, status);
        //check written value
        TEST_ASSERT_EQUAL(i, *(simulatedEeprom.buffer + (i * 2)));
        //when write() is used value of indexes in eeprom and ram should be the same 
        TEST_ASSERT_EQUAL(emptyBuffer.readIndex, simulatedEeprom.readIndex);        
        TEST_ASSERT_EQUAL(emptyBuffer.writeIndex, simulatedEeprom.writeIndex);
    }
}

/**
* @test Test of read function when some values
  are in eeprom. 
  Tested if read return 0xFFFF when log is empty.
  Tested if indexes are not changed in eeprom when
  read is used.
*
*
*/
void test_read(void)
{
    //write indexes and magic num into eeprom
    setInitiateStateOfEEPROM(0, 7);
    //fill the eeprom
    for(uint16_t i = 0; i < 7; i++)
    {
         memcpy(simulatedEeprom.buffer + i * 2, &i, sizeof(i));
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
        TEST_ASSERT_EQUAL(i, readValue);
    }
    
    //test will be 0xFFFF returned when eeprom is empty
    uint16_t readValue = 0;
    status = tempLogging_read(&emptyBuffer, &readValue);
    TEST_ASSERT_EQUAL_HEX16(0xFFFF, readValue);
    
    //test if readIndex in eeprom is changed
    //should be 0 in eeprom
    TEST_ASSERT_EQUAL(0, simulatedEeprom.readIndex);
    //should be 7 in ram
    TEST_ASSERT_EQUAL(7, emptyBuffer.readIndex);
}


/**
* @test Few values are inserted first with write function into eeprom
 then these values are read with read function.
 Return status of read function after that(when empty) and
 Behavior of indexes are checked.
*
*
*/
void test_readWriteWhenEepromIsAlmostEmpty(void)
{
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
    
    uint16_t readIndex = 99, writeIndex = 54;
    //read first value then test indexes then read second 
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

        TEST_ASSERT_EQUAL(0, simulatedEeprom.readIndex);
        TEST_ASSERT_EQUAL(2, simulatedEeprom.writeIndex);
    }
    
    // check empty status 
    status = tempLogging_read(&emptyBuffer, &readValue);
    TEST_ASSERT_EQUAL(TL_OK, status);
    //check if 0xFFFF will be returned when empty
    TEST_ASSERT_EQUAL_HEX32(0xFFFF, readValue);
    
    //write some value to check are indexes updated in rom
    uint16_t value = 1l;
    status = tempLogging_write(&emptyBuffer, &value);
    TEST_ASSERT_EQUAL(emptyBuffer.readIndex, simulatedEeprom.readIndex);
    TEST_ASSERT_EQUAL(emptyBuffer.writeIndex, simulatedEeprom.writeIndex);
}


/**
* @test EEPROM is loaded until one is missing to be full.
  After log is Initiated one value is iserted and indexes are checked.
  When buffer is full and one more value is inserted overwriting of oldest
  value is checked.
*
*
*/
void test_readWriteWhenEepromIsAlmostFull(void)
{
    //fill the eeprom
    for(uint16_t i = 0; i < ((FLASH_SIZE - 8) / 2) - 1; i++) //one is missing to be full
    {
        memcpy(simulatedEeprom.buffer + (i * 2), &i, sizeof(i));  
    }
    //write indexes into eeprom
    //writeIndex point to the last place in the buffer
    uint16_t readIndex = 0;
    uint16_t writeIndex = ((FLASH_SIZE - 8) / 2) - 1;
    setInitiateStateOfEEPROM(readIndex, writeIndex);
    
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    
    //after this write buffer is full and writeIndex should point to 0 
    uint16_t value = 88;
    status = tempLogging_write(&emptyBuffer, &value);
    TEST_ASSERT_EQUAL(TL_OK, status);
    TEST_ASSERT_EQUAL(0, emptyBuffer.writeIndex);
    
    //check if the oldest is overwritten
    status = tempLogging_write(&emptyBuffer, &value);
    TEST_ASSERT_EQUAL(88, *simulatedEeprom.buffer);
    
    //test if readIndex is incremented
    TEST_ASSERT_EQUAL(simulatedEeprom.readIndex, simulatedEeprom.writeIndex + 1); // writeIndex+1 because  readIndex is higher by one
    
    //readIndex after this should point to the last place in buffer
    //readIndex = 2  now
    uint16_t readValue = 0;
    for(uint16_t i = 0; i < ((FLASH_SIZE - 8) / 2); i++)
    { 
        tempLogging_read(&emptyBuffer, &readValue);
    }
    TEST_ASSERT_EQUAL(emptyBuffer.writeIndex, emptyBuffer.readIndex);    
}

/**
* @test Test of updating eeprom indexes value when
  flush is called.
*
*
*/
void test_flush(void)
{
    setInitiateStateOfEEPROM(0, 7);
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    //3 values are read just to change ram value of indexes
    for(uint16_t i = 0; i < 3; i++)
    {
        uint16_t readValue = 0;        
        tempLogging_read(&emptyBuffer, &readValue);
    }
    tempLogging_flush(&emptyBuffer);
    // test if indexes in ram and rom are the same
    TEST_ASSERT_EQUAL(emptyBuffer.readIndex, simulatedEeprom.readIndex);
    TEST_ASSERT_EQUAL(emptyBuffer.writeIndex, simulatedEeprom.writeIndex);  
}






