#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vclient_common.h"
#include "vclient_log.h"

#define LOGING_VCLIENT_LOG_FILE 1
#define VCLIENT_LOG_FILE_PATH "vclient.log"

FILE *vclient_log_f;

void vclient_log_deinit_log(void)
{
    if (LOGING_VCLIENT_LOG_FILE && NULL != vclient_log_f)
    {
        fclose(vclient_log_f);
        vclient_log_f = NULL;
    }
}

enum vclient_error_code_e vclient_log_init_log(void)
{
    vclient_error_code_t err_code = VCLIENT_SUCCESS;

    if (LOGING_VCLIENT_LOG_FILE)
    {
        vclient_log_f = fopen(VCLIENT_LOG_FILE_PATH, "w");
        if (NULL == vclient_log_f)
        {
            err_code = VCLIENT_LOGINIT_OPEN_LOG_FILE_ERROR;
            goto error;
        }
    }
    else
    {
        vclient_log_f = stdout;
    }

    error:

    return err_code;
}
