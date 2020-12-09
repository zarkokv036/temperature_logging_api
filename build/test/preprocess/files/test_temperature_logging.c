#include "build/temp/_test_temperature_logging.c"
#include "src/temperature_logging.h"
#include "build/test/mocks/mock_eeprom.h"
#include "C:/Ruby27-x64/lib/ruby/gems/2.7.0/gems/ceedling-0.30.0/vendor/unity/src/unity.h"






uint8_t simulated_eeprom[8192];



static int mock_eeprom_read(uint16_t address, uint8_t *data,

  uint16_t length, int num_calls)

{

    if (address >= 8192)

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

    if (address >= 8192)

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

    eeprom_read_Stub(mock_eeprom_read);

    eeprom_write_Stub(mock_eeprom_read);

}



void tearDown(void)

{

}



void test_temperature_logging_NeedToImplement(void)

{

    memset(simulated_eeprom, 0xff, 8192);

    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(57), UNITY_DISPLAY_STYLE_INT);

    uint32_t checkMagic = 0;

    memcpy(&checkMagic , simulated_eeprom, 4);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)(((0x10101010))), (UNITY_INT)(UNITY_INT32)((checkMagic)), (

   ((void *)0)

   ), (UNITY_UINT)(60), UNITY_DISPLAY_STYLE_HEX32);



}
