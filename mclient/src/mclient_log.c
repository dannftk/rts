#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mclient_common.h"
#include "mclient_log.h"

#define LOGING_MCLIENT_LOG_FILE 1
#define MCLIENT_LOG_FILE_PATH "_mclient.log"

FILE *mclient_log_f;

void mclient_log_deinit_log(void)
{
    if (LOGING_MCLIENT_LOG_FILE && NULL != mclient_log_f)
    {
        fclose(mclient_log_f);
        mclient_log_f = NULL;
    }
}

enum mclient_error_code_e mclient_log_init_log(char const *prefix)
{
    mclient_error_code_t err_code = MCLIENT_SUCCESS;

    if (LOGING_MCLIENT_LOG_FILE)
    {
        char file_log_name[100];
        strcpy(file_log_name, prefix);
        mclient_log_f = fopen(strcat(file_log_name, MCLIENT_LOG_FILE_PATH), "w");
        if (NULL == mclient_log_f)
        {
            err_code = MCLIENT_LOGINIT_OPEN_LOG_FILE_ERROR;
            goto error;
        }
    }
    else
    {
        mclient_log_f = stdout;
    }

    error:

    return err_code;
}
