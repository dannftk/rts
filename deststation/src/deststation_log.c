#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deststation_common.h"
#include "deststation_log.h"

#define LOGING_DESTSTATION_LOG_FILE 1
#define DESTSTATION_LOG_FILE_PATH "deststation.log"

FILE *deststation_log_f;

void deststation_log_deinit_log(void)
{
    if (LOGING_DESTSTATION_LOG_FILE && NULL != deststation_log_f)
    {
        fclose(deststation_log_f);
        deststation_log_f = NULL;
    }
}

enum deststation_error_code_e deststation_log_init_log(void)
{
    deststation_error_code_t err_code = DESTSTATION_SUCCESS;

    if (LOGING_DESTSTATION_LOG_FILE)
    {
        deststation_log_f = fopen(DESTSTATION_LOG_FILE_PATH, "w");
        if (NULL == deststation_log_f)
        {
            err_code = DESTSTATION_LOGINIT_OPEN_LOG_FILE_ERROR;
            goto error;
        }
    }
    else
    {
        deststation_log_f = stdout;
    }

    error:

    return err_code;
}
