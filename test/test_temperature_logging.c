/**
* @file test_temperature_logging.c
* @author Zarko Milojicic
*/


#include "unity.h"
#include "mock_eeprom.h"
#include "temperature_logging.h"
#include <string.h>
#include <stdio.h>

#define FLASH_SIZE 8192

/**
* @brief Simulating of EEPROM memory.
* With this we can easly read and write control block in simulated EEPROM, 
  and values of buffer.
  Also if there is need we can access to raw data of EEPROM.
*/
union SimulatedEeprom {
    uint8_t rawData[FLASH_SIZE];
    struct {
        uint32_t magic_number;
        uint16_t readIndex;
        uint16_t writeIndex;
        uint16_t buffer[(FLASH_SIZE - 8) / 2];
    };
};

union SimulatedEeprom simulatedEeprom;


/**
* @brief Mocking of eeprom_read() function.
* Mocking of reading is done by reading some value from the ram memory from some location.
*
* @param[in] address The address of the data to be read.
* @param[in] data Pointer to a variable where read data should be stored.
* @return 0 on success.
* @return 1 if the eeprom driver reports an error.
*/
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


/**
* @brief Mocking of eeprom_write() function.
* Mocking of writing is done by writing some value to the ram memory at some location.
*
* @param[in] address The address where the data will be stored in RAM.
* @param[in] data A pointer to the data to be placed in ram.
* @return 0 on success.
* @return 1 if the eeprom driver reports an error.
*/
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
* @brief This function sets initiate state of EEPROM.
*
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
    //wherever the eeprom_read() is called it will be mocked with mock_eeprom_read() 
    eeprom_read_StubWithCallback(mock_eeprom_read);
    //wherever the eeprom_write() is called it will be mocked with mock_eeprom_write() 
    eeprom_write_StubWithCallback(mock_eeprom_write);
    //clear eeprom
    memset(&simulatedEeprom.rawData, 0xFF, FLASH_SIZE);
}

void tearDown(void)
{
}

/**
* @test Test of log initialization when eeprom is empty, 
  magic number and indexes are not in eeprom
*
*  
*/
void test_initWhenEepromIsEmpty(void)
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
        TEST_ASSERT_EQUAL(i, simulatedEeprom.buffer[i]);
        //when write() is used value of indexes in eeprom and ram should be the same 
        TEST_ASSERT_EQUAL(emptyBuffer.readIndex, simulatedEeprom.readIndex);        
        TEST_ASSERT_EQUAL(emptyBuffer.writeIndex, simulatedEeprom.writeIndex);
    }
}


/**
* @test Test of read function when some values
  are in eeprom. 
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
        simulatedEeprom.buffer[i] = i;
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
    
    //test if readIndex in eeprom is changed
    //should be 0 in eeprom
    TEST_ASSERT_EQUAL(0, simulatedEeprom.readIndex);
    //should be 7 in ram
    TEST_ASSERT_EQUAL(7, emptyBuffer.readIndex);
}


/**
* @test Read empty buffer. Should return 0xFFFF.
*
*
*/
void test_readEmptyEeprom(void)
{
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    
    //Read empty buffer
    uint16_t readValue = 0;
    status = tempLogging_read(&emptyBuffer, &readValue);
    TEST_ASSERT_EQUAL(TL_OK, status);
    //check if 0xFFFF will be returned when empty
    TEST_ASSERT_EQUAL_HEX32(0xFFFF, readValue); 
}


/**
* @test Read buffer when there is only one value in it.
 Behavior of indexes are checked.
*
*
*/
void test_readAlmostEmptyEeprom(void)
{
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    
    //insert one value then read it
    uint16_t writeValue = 1;
    status = tempLogging_write(&emptyBuffer, &writeValue);
    TEST_ASSERT_EQUAL(TL_OK, status);
    
    uint16_t readValue = 0;
    status = tempLogging_read(&emptyBuffer, &readValue);
    TEST_ASSERT_EQUAL(TL_OK, status);
    TEST_ASSERT_EQUAL(1, readValue);
    
    //check indexes in ram
    TEST_ASSERT_EQUAL(1, emptyBuffer.readIndex);
    TEST_ASSERT_EQUAL(1, emptyBuffer.writeIndex);
    
    //check indexes in eeprom
    TEST_ASSERT_EQUAL(0, simulatedEeprom.readIndex);
    TEST_ASSERT_EQUAL(1, simulatedEeprom.writeIndex);
    
}

/**
* @test Test write when buffer is almost full.
  Behavior of indexes in ram are checked.
*
*
*/
void test_WriteWhenEepromIsAlmostFull(void)
{
    //writeIndex point to the last place in the buffer simulating like its almost full
    setInitiateStateOfEEPROM(0, ((FLASH_SIZE - 8) / 2) - 1);
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    
    //write log 
    uint16_t writeValue = 1;
    status = tempLogging_write(&emptyBuffer, &writeValue);
    TEST_ASSERT_EQUAL(TL_OK, status);
    //check the last value in buffer
    TEST_ASSERT_EQUAL(1, simulatedEeprom.buffer[((FLASH_SIZE - 8) / 2) - 1]);
    
    //check indexes in ram
    TEST_ASSERT_EQUAL(1, emptyBuffer.readIndex);
    TEST_ASSERT_EQUAL(0, emptyBuffer.writeIndex);  
}

/**
* @test Test write when buffer is full.
  Behavior of indexes  in ram are checked.
*
*
*/
void test_WriteWhenEepromIsFull(void)
{
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    
    //fill the buffer
    for(uint16_t i = 0; i < ((FLASH_SIZE - 8) / 2); i++)
    {
        status = tempLogging_write(&emptyBuffer, &i);     
    }
    //overflow
    uint16_t writeValue = 1;
    status = tempLogging_write(&emptyBuffer, &writeValue);
    TEST_ASSERT_EQUAL(TL_OK, status);
    
    TEST_ASSERT_EQUAL(1, simulatedEeprom.buffer[0]);
    
    //check indexes in ram
    TEST_ASSERT_EQUAL(2, emptyBuffer.readIndex);
    TEST_ASSERT_EQUAL(1, emptyBuffer.writeIndex);   
}

/**
* @test Test of updating eeprom indexes value when
  flush is called.
*
*
*/
void test_flushAfterRead(void)
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


/**
* @test Test of updating eeprom indexes value when
  write is called.
*
*
*/
void test_flushFuncionalityInWrite(void)
{
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    
    //write some values into eeprom
    for(uint16_t i = 0; i < 5; i++)
    {
        status = tempLogging_write(&emptyBuffer, &i);
        TEST_ASSERT_EQUAL(emptyBuffer.readIndex, simulatedEeprom.readIndex);
        TEST_ASSERT_EQUAL(emptyBuffer.writeIndex, simulatedEeprom.writeIndex);
    }
    
    uint16_t readValue = 0;        
    tempLogging_read(&emptyBuffer, &readValue);
    //index should not be updated in eeprom after read function
    TEST_ASSERT_EQUAL(0, simulatedEeprom.readIndex);
    
    //check if indexes are updated in eeprom after write
    status = tempLogging_write(&emptyBuffer, &readValue);
    TEST_ASSERT_EQUAL(emptyBuffer.readIndex, simulatedEeprom.readIndex);
    TEST_ASSERT_EQUAL(emptyBuffer.writeIndex, simulatedEeprom.writeIndex);   
}
/**
* @test Test of flushing after using write function
*
*
*/
void test_flushAfterWrite(void)
{
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    //write some values into eeprom
    for(uint16_t i = 0; i < 5; i++)
    {
        status = tempLogging_write(&emptyBuffer, &i);
    }
    TEST_ASSERT_EQUAL(0, simulatedEeprom.readIndex);
    TEST_ASSERT_EQUAL(5, simulatedEeprom.writeIndex); 
    
    //check if indexes are same after using flush
    tempLogging_flush(&emptyBuffer);
    TEST_ASSERT_EQUAL(0, simulatedEeprom.readIndex);
    TEST_ASSERT_EQUAL(5, simulatedEeprom.writeIndex);  
}

/**
* @test Test of flushing immediately after init of empty buffer
*
*
*/
void test_flushAfterInit(void)
{
    //creating of log
    TempLogging_ControlBlock_t emptyBuffer;
    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);
    tempLogging_flush(&emptyBuffer);
    TEST_ASSERT_EQUAL(emptyBuffer.readIndex, simulatedEeprom.readIndex);
    TEST_ASSERT_EQUAL(emptyBuffer.writeIndex, simulatedEeprom.writeIndex);   
}










