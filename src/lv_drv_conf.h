#include "ecu_configs.h"

#if WIN_ECU_DISPLAY
#include "lv_drv_conf_win.h"
#else
#include "lv_drv_conf_pi.h"
#endif // WIN_ECU_DISPLAY