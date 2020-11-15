#include "ProjectConfig.h"

#if defined(ESP_PLATFORM)
#include <esp8266.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#include "SCDE_s.h"

#include "SCDE.h"



/* helper
 * --------------------------------------------------------------------------------------------------
 *  FName: TimeNow
 *  Desc: Returns current system time. (IT IS NOT THE SCDE TIME STAMP)
 *  Para: -/-
 *  Rets: time_t timeNow -> current system time
 * --------------------------------------------------------------------------------------------------
 */
time_t
TimeNow()
{
  // for time stamp storage
  time_t timeNow;

  // get time stamp
  time(&timeNow);

  return timeNow;
}
