/* encoding: UTF-8 */

#ifndef DESTSTATION_LOG_H_
#define DESTSTATION_LOG_H_

#include <stdio.h>

extern FILE *deststation_log_f;

#define DESTSTATION_LOG_LOG(fmt, ...) fprintf(deststation_log_f, fmt, ##__VA_ARGS__)

enum deststation_error_code_e deststation_log_init_log(void);
void deststation_log_deinit_log(void);

#endif /* DESTSTATION_LOG_H_ */