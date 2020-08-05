
#ifndef _GENIE_RESET_H_
#define _GENIE_RESET_H_

#define GENIE_RESET_BY_REPEAT_COUNTER		5
#define GENIE_RESET_BY_REPEAT_TIMEOUT		(3*1000)
#define GENIE_RESET_BY_REPEAT_FLASH_COUNTER		5
#define GENIE_RESET_BY_REPEAT_FLASH_DURATION    500

#define GENIE_RESET_WAIT_TIMEOUT            (5*1000)

void genie_reset_by_repeat_init(void);

#endif // _GENIE_RESET_H_
