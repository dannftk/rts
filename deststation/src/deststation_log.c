#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deststation_common.h"
#include "deststation_log.h"

#define LOGING_DESTSTATION_LOG_FILE 1
#define PROJECT_ROOT_PATH_ENV_VARIABLE "RTS_PROJ_ROOT"
#define DESTSTATION_LOG_FILE_PATH "log/deststation.log"

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
        char *project_root_path;
        char log_file_path[100];

        project_root_path = getenv("RTS_PROJ_ROOT");
        if (NULL == project_root_path)
        {
            err_code = DESTSTATION_LOGINIT_RTS_PROJ_ROOT_ENV_VAR_NOT_SET_ERROR;
            goto error;
        }

        strcpy(log_file_path, project_root_path);
        strcat(log_file_path, "/");
        strcat(log_file_path, DESTSTATION_LOG_FILE_PATH);

        deststation_log_f = fopen(log_file_path, "w");
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