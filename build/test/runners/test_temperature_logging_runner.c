/* AUTOGENERATED FILE. DO NOT EDIT. */

/*=======Automagically Detected Files To Include=====*/
#include "unity.h"
#include "cmock.h"
#include "mock_eeprom.h"

int GlobalExpectCount;
int GlobalVerifyOrder;
char* GlobalOrderError;

/*=======External Functions This Runner Calls=====*/
extern void setUp(void);
extern void tearDown(void);
extern void test_initWhenEepromIsEmpty(void);
extern void test_InitWhenEepromIsPreviouslyInitiated(void);
extern void test_write(void);
extern void test_read(void);
extern void test_readWriteWhenEepromIsAlmostEmpty(void);
extern void test_readWriteWhenEepromIsAlmostFull(void);


/*=======Mock Management=====*/
static void CMock_Init(void)
{
  GlobalExpectCount = 0;
  GlobalVerifyOrder = 0;
  GlobalOrderError = NULL;
  mock_eeprom_Init();
}
static void CMock_Verify(void)
{
  mock_eeprom_Verify();
}
static void CMock_Destroy(void)
{
  mock_eeprom_Destroy();
}

/*=======Test Reset Options=====*/
void resetTest(void);
void resetTest(void)
{
  tearDown();
  CMock_Verify();
  CMock_Destroy();
  CMock_Init();
  setUp();
}
void verifyTest(void);
void verifyTest(void)
{
  CMock_Verify();
}

/*=======Test Runner Used To Run Each Test=====*/
static void run_test(UnityTestFunction func, const char* name, int line_num)
{
    Unity.CurrentTestName = name;
    Unity.CurrentTestLineNumber = line_num;
#ifdef UNITY_USE_COMMAND_LINE_ARGS
    if (!UnityTestMatches())
        return;
#endif
    Unity.NumberOfTests++;
    UNITY_CLR_DETAILS();
    UNITY_EXEC_TIME_START();
    CMock_Init();
    if (TEST_PROTECT())
    {
        setUp();
        func();
    }
    if (TEST_PROTECT())
    {
        tearDown();
        CMock_Verify();
    }
    CMock_Destroy();
    UNITY_EXEC_TIME_STOP();
    UnityConcludeTest();
}

/*=======MAIN=====*/
int main(void)
{
  UnityBegin("test_temperature_logging.c");
  run_test(test_initWhenEepromIsEmpty, "test_initWhenEepromIsEmpty", 89);
  run_test(test_InitWhenEepromIsPreviouslyInitiated, "test_InitWhenEepromIsPreviouslyInitiated", 108);
  run_test(test_write, "test_write", 129);
  run_test(test_read, "test_read", 156);
  run_test(test_readWriteWhenEepromIsAlmostEmpty, "test_readWriteWhenEepromIsAlmostEmpty", 201);
  run_test(test_readWriteWhenEepromIsAlmostFull, "test_readWriteWhenEepromIsAlmostFull", 258);

  CMock_Guts_MemFreeFinal();
  return UnityEnd();
}
