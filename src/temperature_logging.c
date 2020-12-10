/**
* @file temperature_logging.c
* @author Zarko Milojicic
*/

#include "temperature_logging.h"
#include "eeprom.h" 
#include <string.h> 

#define TEMP_LOGGING_READ_INDEX_ADDRESS     (TEMP_LOGGING_MAGIC_NUMBER_ADDRESS + 4)
#define TEMP_LOGGING_WRITE_INDEX_ADDRESS    (TEMP_LOGGING_READ_INDEX_ADDRESS + 2)
#define TEMP_LOGGING_BUFFER_ADDRESS         (TEMP_LOGGING_WRITE_INDEX_ADDRESS + 2)

#define TEMP_LOGGING_EEPROM_CAPACITY        (8192)
#define TEMP_LOGGING_CONTROL_BLOCK_SIZE     (8)
#define TEMP_LOGGING_BUFFER_ENTRY_SIZE      (2)
#define TEMP_LOGGING_BUFFER_CAPACITY \
    ((TEMP_LOGGING_EEPROM_CAPACITY - TEMP_LOGGING_CONTROL_BLOCK_SIZE) / TEMP_LOGGING_BUFFER_ENTRY_SIZE)


static TempLogging_Status_t readControlBlock(TempLogging_ControlBlock_t *controlBlock, uint32_t *magicNumber)
{
    if ((controlBlock == NULL) || (magicNumber == NULL) )
    {
        return TL_BAD_PARAM;
    }
    //reading magic number from eeprom
    int eStatus = eeprom_read(TEMP_LOGGING_MAGIC_NUMBER_ADDRESS, (uint8_t *)(magicNumber), sizeof(*magicNumber));
    if (eStatus != TL_OK)
    {
        return TL_EEPROM_RW_ERROR;
    }
    
    // if magicNumber is in eeprom write indexes to RAM
    if (*magicNumber == TEMP_LOGGING_MAGIC_NUMBER)
    {
    
        //reading readIndex from eeprom and write it to RAM
        eStatus = eeprom_read(TEMP_LOGGING_READ_INDEX_ADDRESS, (uint8_t *)(&controlBlock->readIndex), sizeof(controlBlock->readIndex));
        if (eStatus != TL_OK)
        {
            return TL_EEPROM_RW_ERROR;
        }
        //reading writeIndex from eeprom and write it to RAM
        eStatus = eeprom_read(TEMP_LOGGING_WRITE_INDEX_ADDRESS, (uint8_t *)(&controlBlock->writeIndex), sizeof(controlBlock->writeIndex)); /// reading writeIndex from eeprom
        if (eStatus != TL_OK)
        {
            return TL_EEPROM_RW_ERROR;
        }
    }
    return TL_OK; 
}


static TempLogging_Status_t writeControlBlock(TempLogging_ControlBlock_t *controlBlock, uint32_t *magicNumber)
{
    if ((controlBlock == NULL) || (magicNumber == NULL) )
    {
        return TL_BAD_PARAM;
    }
    
    //writing readIndex number into rom
    int eStatus = eeprom_write(TEMP_LOGGING_READ_INDEX_ADDRESS,  (const uint8_t *)(&controlBlock->readIndex), sizeof(controlBlock->readIndex));
    if (eStatus != TL_OK)
    {
        return TL_EEPROM_RW_ERROR;
    }
    //writing writeIndex number into rom
    eStatus = eeprom_write(TEMP_LOGGING_WRITE_INDEX_ADDRESS, (const uint8_t *)(&controlBlock->writeIndex), sizeof(controlBlock->writeIndex)); 
    if (eStatus != TL_OK)
    {
        return TL_EEPROM_RW_ERROR;
    }
    
    //writing magic number
    eStatus = eeprom_write(TEMP_LOGGING_MAGIC_NUMBER_ADDRESS, (const uint8_t *)(magicNumber), sizeof(*magicNumber)); /// writing magic num to eeprom 
    if (eStatus != TL_OK)
    {
        return TL_EEPROM_RW_ERROR;
    }
    
    return TL_OK;
}


TempLogging_Status_t tempLogging_init(TempLogging_ControlBlock_t *controlBlock)
{
    if (controlBlock == NULL)
    {
        return TL_BAD_PARAM;
    }
    //reading magic number from eeprom
    uint32_t checkMagic = 0;
    TempLogging_Status_t eStatus = readControlBlock(controlBlock, &checkMagic);
    if (eStatus != TL_OK)
    {
        return eStatus;
    }
    controlBlock->dirtyFlag = false;
    // Check if eeprom is initialized 
    if (checkMagic != TEMP_LOGGING_MAGIC_NUMBER)
    {
        // perform first time initialization
        controlBlock->readIndex = 0;
        controlBlock->writeIndex = 0;
        uint32_t magicNumber = TEMP_LOGGING_MAGIC_NUMBER ;
        return  writeControlBlock(controlBlock, &magicNumber);
    }
    
    return TL_OK;   
}


TempLogging_Status_t tempLogging_read(TempLogging_ControlBlock_t *controlBlock, uint16_t *readValue)
{
    if ((controlBlock == NULL) || (readValue == NULL))
    {
        return TL_BAD_PARAM;
    }
    if (controlBlock->readIndex == controlBlock->writeIndex)
    {
        *readValue = 0xFFFF; ///value when buffer is empty
        return TL_OK;
    }
    
    uint16_t readAdress = TEMP_LOGGING_BUFFER_ADDRESS + 2 * controlBlock->readIndex;   
    int eStatus = eeprom_read(readAdress, (uint8_t *)(readValue), sizeof(*readValue));  /// reading value from buffer in eeprom
    if (eStatus != TL_OK)
    {
        return TL_EEPROM_RW_ERROR;
    }
    controlBlock->readIndex = (controlBlock->readIndex + 1) % TEMP_LOGGING_BUFFER_CAPACITY; //// this makes it go around in a circle
    controlBlock->dirtyFlag = true;
    return TL_OK;
}


TempLogging_Status_t tempLogging_write(TempLogging_ControlBlock_t *controlBlock, uint16_t *writeValue)
{
    if ((controlBlock == NULL) || (writeValue == NULL))
    {
        return TL_BAD_PARAM;
    }

    uint16_t writeAdress = TEMP_LOGGING_BUFFER_ADDRESS + 2 * controlBlock->writeIndex;
    int eStatus = eeprom_write(writeAdress, (const uint8_t *)(writeValue), sizeof(*writeValue)); /// writing new value into buffer
    if (eStatus != TL_OK)
    {
        return TL_EEPROM_RW_ERROR;
    }
    controlBlock->writeIndex = (controlBlock->writeIndex + 1) % TEMP_LOGGING_BUFFER_CAPACITY; //// this makes it go around in a circle
    if (controlBlock->readIndex == controlBlock->writeIndex)
    {
        controlBlock->readIndex = (controlBlock->readIndex + 1) % TEMP_LOGGING_BUFFER_CAPACITY;  
    } /// incrementing readIndex for 1 if buffer is full
    
    controlBlock->dirtyFlag = true;
    return tempLogging_flush(controlBlock); /// writing indexes into eeprom
}


TempLogging_Status_t tempLogging_flush(TempLogging_ControlBlock_t *controlBlock)
{
    if (controlBlock == NULL)
    {
        return TL_BAD_PARAM;
    }
    
    if (controlBlock->dirtyFlag == true)
    {
        uint32_t magicNumber = TEMP_LOGGING_MAGIC_NUMBER ;
        TempLogging_Status_t eStatus = writeControlBlock(controlBlock, &magicNumber);
        if(eStatus != TL_OK)
        {
            return eStatus;
        }
        else
        {
            controlBlock->dirtyFlag = false; 
        }
    }
    return TL_OK;	
}




