#include <stdio.h>
#include <string.h>

#include "mclient_common.h"
#include "mclient_log.h"

#define LOGING_MCLIENT_LOG_FILE 1
#define PROJECT_ROOT_PATH_ENV_VARIABLE "RTS_PROJ_ROOT"
#define MCLIENT_LOG_FILE_PATH "log/mclient.log"

FILE *mclient_log_f;

void mclient_log_deinit_log(void)
{
    if (LOGING_MCLIENT_LOG_FILE && NULL != mclient_log_f)
    {
        fclose(mclient_log_f);
        mclient_log_f = NULL;
    }
}

enum mclient_error_code_e mclient_log_init_log(void)
{
    mclient_error_code_t err_code = MCLIENT_SUCCESS;

    if (LOGING_MCLIENT_LOG_FILE)
    {
        char *project_root_path;
        char log_file_path[100];

        project_root_path = getenv("RTS_PROJ_ROOT");
        if (NULL == project_root_path)
        {
            err_code = MCLIENT_LOGINIT_RTS_PROJ_ROOT_ENV_VAR_NOT_SET_ERROR;
            goto error;
        }

        strcpy(log_file_path, project_root_path);
        strcat(log_file_path, "/");
        strcat(log_file_path, MCLIENT_LOG_FILE_PATH);

        mclient_log_f = fopen(log_file_path, "w");
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