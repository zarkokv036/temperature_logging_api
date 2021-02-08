#include "build/temp/_test_temperature_logging.c"
#include "src/temperature_logging.h"
#include "build/test/mocks/mock_eeprom.h"
#include "C:/Ruby27-x64/lib/ruby/gems/2.7.0/gems/ceedling-0.30.0/vendor/unity/src/unity.h"














union SimulatedEeprom {

    uint8_t rawData[8192];

    struct {

        uint32_t magic_number;

        uint16_t readIndex;

        uint16_t writeIndex;

        uint16_t buffer[(8192 - 8) / 2];

    };

};



union SimulatedEeprom simulatedEeprom;

static int mock_eeprom_read(uint16_t address, uint8_t *data,

  uint16_t length, int num_calls)

{

    if (address >= 8192)

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

    if (address >= 8192)

    {

        return 1;

    }

    else

    {

        memcpy(&simulatedEeprom.rawData[address], data, length);

        return 0;

    }

}















static void setInitiateStateOfEEPROM(uint16_t readIndex, uint16_t writeIndex)

{



    uint32_t checkMagic = (0x10101010);

    simulatedEeprom.magic_number = checkMagic;



    simulatedEeprom.readIndex = readIndex;

    simulatedEeprom.writeIndex = writeIndex;

}



void setUp(void)

{



    eeprom_read_Stub(mock_eeprom_read);



    eeprom_write_Stub(mock_eeprom_write);



    memset(&simulatedEeprom.rawData, 0xFF, 8192);

}



void tearDown(void)

{

}















void test_initWhenEepromIsEmpty(void)

{



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(122), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)(((0x10101010))), (UNITY_INT)(UNITY_INT32)((simulatedEeprom.magic_number)), (

   ((void *)0)

   ), (UNITY_UINT)(124), UNITY_DISPLAY_STYLE_HEX32);



    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((simulatedEeprom.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(126), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(127), UNITY_DISPLAY_STYLE_INT);

}















void test_InitWhenEepromIsPreviouslyInitiated(void)

{



    setInitiateStateOfEEPROM(1, 7);



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(143), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)((simulatedEeprom.readIndex)), (UNITY_INT)((emptyBuffer.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(145), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((simulatedEeprom.writeIndex)), (UNITY_INT)((emptyBuffer.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(146), UNITY_DISPLAY_STYLE_INT);

}

void test_write(void)

{



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);



    for(uint16_t i = 0; i < 5; i++)

    {

        status = tempLogging_write(&emptyBuffer, &i);

        UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

       ((void *)0)

       ), (UNITY_UINT)(166), UNITY_DISPLAY_STYLE_INT);



        UnityAssertEqualNumber((UNITY_INT)((i)), (UNITY_INT)((simulatedEeprom.buffer[i])), (

       ((void *)0)

       ), (UNITY_UINT)(168), UNITY_DISPLAY_STYLE_INT);



        UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.readIndex)), (UNITY_INT)((simulatedEeprom.readIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(170), UNITY_DISPLAY_STYLE_INT);

        UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(171), UNITY_DISPLAY_STYLE_INT);

    }

}

void test_read(void)

{



    setInitiateStateOfEEPROM(0, 7);



    for(uint16_t i = 0; i < 7; i++)

    {

        simulatedEeprom.buffer[i] = i;

    }



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(196), UNITY_DISPLAY_STYLE_INT);





    for(uint16_t i = 0; i<7; i++)

    {

        uint16_t readValue = 0;

        status = tempLogging_read(&emptyBuffer, &readValue);

        UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

       ((void *)0)

       ), (UNITY_UINT)(203), UNITY_DISPLAY_STYLE_INT);

        UnityAssertEqualNumber((UNITY_INT)((i)), (UNITY_INT)((readValue)), (

       ((void *)0)

       ), (UNITY_UINT)(204), UNITY_DISPLAY_STYLE_INT);

    }







    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((simulatedEeprom.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(209), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)((7)), (UNITY_INT)((emptyBuffer.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(211), UNITY_DISPLAY_STYLE_INT);

}















void test_readEmptyEeprom(void)

{



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);





    uint16_t readValue = 0;

    status = tempLogging_read(&emptyBuffer, &readValue);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(229), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((0xFFFF)), (UNITY_INT)(UNITY_INT32)((readValue)), (

   ((void *)0)

   ), (UNITY_UINT)(231), UNITY_DISPLAY_STYLE_HEX32);

}

void test_readAlmostEmptyEeprom(void)

{



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);





    uint16_t writeValue = 1;

    status = tempLogging_write(&emptyBuffer, &writeValue);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(250), UNITY_DISPLAY_STYLE_INT);



    uint16_t readValue = 0;

    status = tempLogging_read(&emptyBuffer, &readValue);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(254), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((readValue)), (

   ((void *)0)

   ), (UNITY_UINT)(255), UNITY_DISPLAY_STYLE_INT);





    UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((emptyBuffer.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(258), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((emptyBuffer.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(259), UNITY_DISPLAY_STYLE_INT);





    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((simulatedEeprom.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(262), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(263), UNITY_DISPLAY_STYLE_INT);



}















void test_WriteWhenEepromIsAlmostFull(void)

{



    setInitiateStateOfEEPROM(0, ((8192 - 8) / 2) - 1);



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);





    uint16_t writeValue = 1;

    status = tempLogging_write(&emptyBuffer, &writeValue);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(284), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((simulatedEeprom.buffer[((8192 - 8) / 2) - 1])), (

   ((void *)0)

   ), (UNITY_UINT)(286), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((emptyBuffer.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(288), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((emptyBuffer.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(289), UNITY_DISPLAY_STYLE_INT);

}















void test_WriteWhenEepromIsFull(void)

{



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);





    for(uint16_t i = 0; i < ((8192 - 8) / 2); i++)

    {

        status = tempLogging_write(&emptyBuffer, &i);

    }

    uint16_t writeValue = 1;

    status = tempLogging_write(&emptyBuffer, &writeValue);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(311), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((simulatedEeprom.buffer[0])), (

   ((void *)0)

   ), (UNITY_UINT)(313), UNITY_DISPLAY_STYLE_INT);





    UnityAssertEqualNumber((UNITY_INT)((2)), (UNITY_INT)((emptyBuffer.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(316), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((emptyBuffer.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(317), UNITY_DISPLAY_STYLE_INT);

}















void test_flushAfterRead(void)

{

    setInitiateStateOfEEPROM(0, 7);



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);



    for(uint16_t i = 0; i < 3; i++)

    {

        uint16_t readValue = 0;

        tempLogging_read(&emptyBuffer, &readValue);

    }



    tempLogging_flush(&emptyBuffer);



    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.readIndex)), (UNITY_INT)((simulatedEeprom.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(341), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(342), UNITY_DISPLAY_STYLE_INT);

}

void test_flushFuncionalityInWrite(void)

{



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);



    for(uint16_t i = 0; i < 5; i++)

    {

        status = tempLogging_write(&emptyBuffer, &i);

        UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.readIndex)), (UNITY_INT)((simulatedEeprom.readIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(361), UNITY_DISPLAY_STYLE_INT);

        UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(362), UNITY_DISPLAY_STYLE_INT);

    }

    uint16_t readValue = 0;

    tempLogging_read(&emptyBuffer, &readValue);



    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((simulatedEeprom.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(367), UNITY_DISPLAY_STYLE_INT);





    status = tempLogging_write(&emptyBuffer, &readValue);

    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.readIndex)), (UNITY_INT)((simulatedEeprom.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(371), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(372), UNITY_DISPLAY_STYLE_INT);

}











void test_flushAfterWrite(void)

{



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);



    for(uint16_t i = 0; i < 5; i++)

    {

        status = tempLogging_write(&emptyBuffer, &i);

    }

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((simulatedEeprom.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(389), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((5)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(390), UNITY_DISPLAY_STYLE_INT);





    tempLogging_flush(&emptyBuffer);

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((simulatedEeprom.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(394), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((5)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(395), UNITY_DISPLAY_STYLE_INT);

}



void test_flushAfterInit(void)

{



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);

    tempLogging_flush(&emptyBuffer);

    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.readIndex)), (UNITY_INT)((simulatedEeprom.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(404), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(405), UNITY_DISPLAY_STYLE_INT);



}
