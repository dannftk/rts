/* encoding: UTF-8 */

#ifndef GATEWAY_LOG_H_
#define GATEWAY_LOG_H_

#include <stdio.h>

extern FILE *gateway_log_f;

#define GATEWAY_LOG_LOG(fmt, ...) fprintf(gateway_log_f, fmt, ##__VA_ARGS__)

enum gateway_error_code_e gateway_log_init_log(void);
void gateway_log_deinit_log(void);

#endif /* GATEWAY_LOG_H_ */