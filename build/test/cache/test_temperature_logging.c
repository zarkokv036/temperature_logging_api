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

   ), (UNITY_UINT)(94), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)(((0x10101010))), (UNITY_INT)(UNITY_INT32)((simulatedEeprom.magic_number)), (

   ((void *)0)

   ), (UNITY_UINT)(96), UNITY_DISPLAY_STYLE_HEX32);



    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((simulatedEeprom.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(98), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(99), UNITY_DISPLAY_STYLE_INT);

}















void test_InitWhenEepromIsPreviouslyInitiated(void)

{



    setInitiateStateOfEEPROM(1, 7);



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(115), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)((simulatedEeprom.readIndex)), (UNITY_INT)((emptyBuffer.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(117), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((simulatedEeprom.writeIndex)), (UNITY_INT)((emptyBuffer.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(118), UNITY_DISPLAY_STYLE_INT);

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

       ), (UNITY_UINT)(138), UNITY_DISPLAY_STYLE_INT);



        UnityAssertEqualNumber((UNITY_INT)((i)), (UNITY_INT)((simulatedEeprom.buffer[i])), (

       ((void *)0)

       ), (UNITY_UINT)(140), UNITY_DISPLAY_STYLE_INT);



        UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.readIndex)), (UNITY_INT)((simulatedEeprom.readIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(142), UNITY_DISPLAY_STYLE_INT);

        UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(143), UNITY_DISPLAY_STYLE_INT);

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

   ), (UNITY_UINT)(169), UNITY_DISPLAY_STYLE_INT);





    for(uint16_t i = 0; i<7; i++)

    {

        uint16_t readValue = 0;

        status = tempLogging_read(&emptyBuffer, &readValue);

        UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

       ((void *)0)

       ), (UNITY_UINT)(176), UNITY_DISPLAY_STYLE_INT);

        UnityAssertEqualNumber((UNITY_INT)((i)), (UNITY_INT)((readValue)), (

       ((void *)0)

       ), (UNITY_UINT)(177), UNITY_DISPLAY_STYLE_INT);

    }





    uint16_t readValue = 0;

    status = tempLogging_read(&emptyBuffer, &readValue);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT16)((0xFFFF)), (UNITY_INT)(UNITY_INT16)((readValue)), (

   ((void *)0)

   ), (UNITY_UINT)(183), UNITY_DISPLAY_STYLE_HEX16);







    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((simulatedEeprom.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(187), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)((7)), (UNITY_INT)((emptyBuffer.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(189), UNITY_DISPLAY_STYLE_INT);

}

void test_readWriteWhenEepromIsAlmostEmpty(void)

{



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);



    uint16_t readValue = 0;

    status = tempLogging_read(&emptyBuffer, &readValue);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(209), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((0xFFFF)), (UNITY_INT)(UNITY_INT32)((readValue)), (

   ((void *)0)

   ), (UNITY_UINT)(211), UNITY_DISPLAY_STYLE_HEX32);





    for(uint16_t i = 0; i < 2; i++)

    {

        status = tempLogging_write(&emptyBuffer, &i);

        UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((TL_OK)), (UNITY_INT)(UNITY_INT32)((status)), (

       ((void *)0)

       ), (UNITY_UINT)(217), UNITY_DISPLAY_STYLE_HEX32);

    }



    uint16_t readIndex = 99, writeIndex = 54;



    for(uint16_t i = 0; i < 2; i++)

    {

        status = tempLogging_read(&emptyBuffer, &readValue);

        UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

       ((void *)0)

       ), (UNITY_UINT)(225), UNITY_DISPLAY_STYLE_INT);



        UnityAssertEqualNumber((UNITY_INT)((i)), (UNITY_INT)((readValue)), (

       ((void *)0)

       ), (UNITY_UINT)(227), UNITY_DISPLAY_STYLE_INT);



        UnityAssertEqualNumber((UNITY_INT)((i + 1)), (UNITY_INT)((emptyBuffer.readIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(229), UNITY_DISPLAY_STYLE_INT);

        UnityAssertEqualNumber((UNITY_INT)((2)), (UNITY_INT)((emptyBuffer.writeIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(230), UNITY_DISPLAY_STYLE_INT);



        UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((simulatedEeprom.readIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(232), UNITY_DISPLAY_STYLE_INT);

        UnityAssertEqualNumber((UNITY_INT)((2)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(233), UNITY_DISPLAY_STYLE_INT);

    }





    status = tempLogging_read(&emptyBuffer, &readValue);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(238), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((0xFFFF)), (UNITY_INT)(UNITY_INT32)((readValue)), (

   ((void *)0)

   ), (UNITY_UINT)(240), UNITY_DISPLAY_STYLE_HEX32);





    uint16_t value = 1l;

    status = tempLogging_write(&emptyBuffer, &value);

    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.readIndex)), (UNITY_INT)((simulatedEeprom.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(245), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((simulatedEeprom.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(246), UNITY_DISPLAY_STYLE_INT);

}

void test_readWriteWhenEepromIsAlmostFull(void)

{



    for(uint16_t i = 0; i < ((8192 - 8) / 2) - 1; i++)

    {



        simulatedEeprom.buffer[i] = i ;



    }





    uint16_t readIndex = 0;

    uint16_t writeIndex = ((8192 - 8) / 2) - 1;

    setInitiateStateOfEEPROM(readIndex, writeIndex);





    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);





    uint16_t value = 88;

    status = tempLogging_write(&emptyBuffer, &value);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(280), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((emptyBuffer.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(281), UNITY_DISPLAY_STYLE_INT);





    status = tempLogging_write(&emptyBuffer, &value);

    UnityAssertEqualNumber((UNITY_INT)((88)), (UNITY_INT)((simulatedEeprom.buffer[0])), (

   ((void *)0)

   ), (UNITY_UINT)(285), UNITY_DISPLAY_STYLE_INT);





    UnityAssertEqualNumber((UNITY_INT)((simulatedEeprom.readIndex)), (UNITY_INT)((simulatedEeprom.writeIndex + 1)), (

   ((void *)0)

   ), (UNITY_UINT)(288), UNITY_DISPLAY_STYLE_INT);









    uint16_t readValue = 0;

    for(uint16_t i = 0; i < ((8192 - 8) / 2); i++)

    {

        tempLogging_read(&emptyBuffer, &readValue);

    }

    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((emptyBuffer.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(298), UNITY_DISPLAY_STYLE_INT);

}
