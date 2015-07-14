#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gateway_common.h"
#include "gateway_log.h"

#define LOGING_GATEWAY_LOG_FILE 1
#define GATEWAY_LOG_FILE_PATH "gateway.log"

FILE *gateway_log_f;

void gateway_log_deinit_log(void)
{
    if (LOGING_GATEWAY_LOG_FILE && NULL != gateway_log_f)
    {
        fclose(gateway_log_f);
        gateway_log_f = NULL;
    }
}

enum gateway_error_code_e gateway_log_init_log(void)
{
    gateway_error_code_t err_code = GATEWAY_SUCCESS;

    if (LOGING_GATEWAY_LOG_FILE)
    {
        gateway_log_f = fopen(GATEWAY_LOG_FILE_PATH, "w");
        if (NULL == gateway_log_f)
        {
            err_code = GATEWAY_LOGINIT_OPEN_LOG_FILE_ERROR;
            goto error;
        }
    }
    else
    {
        gateway_log_f = stdout;
    }

    error:

    return err_code;
}
