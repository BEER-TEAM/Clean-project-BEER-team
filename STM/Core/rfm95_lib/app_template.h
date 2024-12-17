/*
 * app_template.h
 *
 */

#ifndef INC_APP_TEMPLATE_H_
#define INC_APP_TEMPLATE_H_
#include "usart.h"
#include "choose_mode.h"

static OPERATING_MODE oper_mode_global;


void app_main(OPERATING_MODE oper_mode);
void rx_loop(OPERATING_MODE oper_mode);
void tx_loop(OPERATING_MODE oper_mode);
void init_radio();


#endif /* INC_APP_TEMPLATE_H_ */
