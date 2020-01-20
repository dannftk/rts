/* encoding: UTF-8 */

#include <stdio.h>
#include <string.h>

#include "gateway_common.h"
#include "gateway_scheduler.h"
#include "gateway_tasks.h"
#include "gateway_database.h"
#include "gateway_socket.h"
#include "gateway_clients.h"

extern int g_mtrx_A_row[COLUMNS];
extern int g_mtrx_C_row[COLUMNS];

extern int g_vector_b[VECTOR_SIZE];
extern int g_vector_d[VECTOR_SIZE];

extern int g_vector_Ab_res_val;
extern int g_vector_Cd_res_val;

extern int g_vector_res_val;

extern int g_mtrx_A_row_written[COLUMNS];
extern int g_mtrx_C_row_written[COLUMNS];

extern int g_vector_b_written[VECTOR_SIZE];
extern int g_vector_d_written[VECTOR_SIZE];

extern int g_vector_Ab_res_rec_count;
extern int g_vector_Cd_res_rec_count;

extern int g_vector_res_rec_count;

extern gateway_clients_t g_gateway_clients[GATEWAY_CLIENT_TYPES_COUNT];

void gateway_tasks_put_mtrx_A_value(mtrx_fmt_t mtrx)
{
    g_mtrx_A_row[mtrx.col_pos] = mtrx.value;
    g_mtrx_A_row_written[mtrx.col_pos] = 1;

    if (g_vector_b_written[mtrx.col_pos] &&
        2 != g_mtrx_A_row_written[mtrx.col_pos])
    {
        task_data_t task_data = {
            .task_type = MUL_MTRX_A_VALUE_ON_VEC_b_VALUE,
            .data.mtrx_col_val_pos = mtrx.col_pos
        };

        gateway_scheduler_add_task_to_queue(task_data);
    }
}

void gateway_tasks_put_mtrx_C_value(mtrx_fmt_t mtrx)
{
    g_mtrx_C_row[mtrx.col_pos] = mtrx.value;
    g_mtrx_C_row_written[mtrx.col_pos] = 1;

    if (g_vector_d_written[mtrx.col_pos] &&
        2 != g_mtrx_C_row_written[mtrx.col_pos])
    {
        task_data_t task_data = {
            .task_type = MUL_MTRX_C_VALUE_ON_VEC_d_VALUE,
            .data.mtrx_col_val_pos = mtrx.col_pos
        };

        gateway_scheduler_add_task_to_queue(task_data);
    }
}

void gateway_tasks_put_vector_b_value(vector_fmt_t vector)
{
    g_vector_b[vector.pos] = vector.value;
    g_vector_b_written[vector.pos] = 1;

    if (1 == g_mtrx_A_row_written[vector.pos])
    {
        task_data_t task_data = {
            .task_type = MUL_VECTOR_b_VALUE_ON_MTRX_A_VALUE,
            .data.vector_val_pos = vector.pos
        };

        gateway_scheduler_add_task_to_queue(task_data);
    }
}

void gateway_tasks_put_vector_d_value(vector_fmt_t vector)
{
    g_vector_d[vector.pos] = vector.value;
    g_vector_d_written[vector.pos] = 1;

    if (1 == g_mtrx_C_row_written[vector.pos])
    {
        task_data_t task_data = {
            .task_type = MUL_VECTOR_d_VALUE_ON_MTRX_C_VALUE,
            .data.vector_val_pos = vector.pos
        };

        gateway_scheduler_add_task_to_queue(task_data);
    }
}

void gateway_tasks_mul_mtrx_A_val_on_vector_b_val(int mtrx_col_val_pos)
{
    if (1 == g_mtrx_A_row_written[mtrx_col_val_pos])
    {
        g_vector_Ab_res_val += g_mtrx_A_row[mtrx_col_val_pos] * g_vector_b[mtrx_col_val_pos];
        g_mtrx_A_row_written[mtrx_col_val_pos] = 2;
        ++g_vector_Ab_res_rec_count;

        if (COLUMNS == g_vector_Ab_res_rec_count)
        {
            memset(g_mtrx_A_row, 0, sizeof(int) * COLUMNS);
            memset(g_mtrx_A_row_written, 0, sizeof(int) * COLUMNS);
            memset(g_vector_b, 0, sizeof(int) * VECTOR_SIZE);
            memset(g_vector_b_written, 0, sizeof(int) * VECTOR_SIZE);
            g_vector_Ab_res_rec_count = 0;

            task_data_t task_data = {
                .task_type = ADD_RES_Ab_VECTOR_VALUE_TO_RES_VECTOR_VALUE
            };

            gateway_scheduler_add_task_to_queue(task_data);
        }
    }
}

void gateway_tasks_mul_mtrx_C_val_on_vector_d_val(int mtrx_col_val_pos)
{
    if (1 == g_mtrx_C_row_written[mtrx_col_val_pos])
    {
        g_vector_Cd_res_val += g_mtrx_C_row[mtrx_col_val_pos] * g_vector_d[mtrx_col_val_pos];
        g_mtrx_C_row_written[mtrx_col_val_pos] = 2;
        ++g_vector_Cd_res_rec_count;

        if (COLUMNS == g_vector_Cd_res_rec_count)
        {
            memset(g_mtrx_C_row, 0, sizeof(int) * COLUMNS);
            memset(g_mtrx_C_row_written, 0, sizeof(int) * COLUMNS);
            memset(g_vector_d, 0, sizeof(int) * VECTOR_SIZE);
            memset(g_vector_d_written, 0, sizeof(int) * VECTOR_SIZE);
            g_vector_Cd_res_rec_count = 0;

            task_data_t task_data = {
                .task_type = ADD_RES_Cd_VECTOR_VALUE_TO_RES_VECTOR_VALUE
            };

            gateway_scheduler_add_task_to_queue(task_data);
        }
    }
}

void gateway_tasks_mul_vector_b_val_on_mtrx_A_val(int vector_val_pos)
{
    if (g_vector_b_written[vector_val_pos] &&
        2 != g_mtrx_A_row_written[vector_val_pos])
    {
        g_vector_Ab_res_val += g_mtrx_A_row[vector_val_pos] * g_vector_b[vector_val_pos];
        g_mtrx_A_row_written[vector_val_pos] = 2;
        ++g_vector_Ab_res_rec_count;

        if (COLUMNS == g_vector_Ab_res_rec_count)
        {
            memset(g_mtrx_A_row, 0, sizeof(int) * COLUMNS);
            memset(g_mtrx_A_row_written, 0, sizeof(int) * COLUMNS);
            memset(g_vector_b, 0, sizeof(int) * VECTOR_SIZE);
            memset(g_vector_b_written, 0, sizeof(int) * VECTOR_SIZE);
            g_vector_Ab_res_rec_count = 0;

            task_data_t task_data = {
                .task_type = ADD_RES_Ab_VECTOR_VALUE_TO_RES_VECTOR_VALUE
            };

            gateway_scheduler_add_task_to_queue(task_data);
        }
    }
}

void gateway_tasks_mul_vector_d_val_on_mtrx_C_val(int vector_val_pos)
{
    if (g_vector_d_written[vector_val_pos] &&
        2 != g_mtrx_C_row_written[vector_val_pos])
    {
        g_vector_Cd_res_val += g_mtrx_C_row[vector_val_pos] * g_vector_d[vector_val_pos];
        g_mtrx_C_row_written[vector_val_pos] = 2;
        ++g_vector_Cd_res_rec_count;

        if (COLUMNS == g_vector_Cd_res_rec_count)
        {
            memset(g_mtrx_C_row, 0, sizeof(int) * COLUMNS);
            memset(g_mtrx_C_row_written, 0, sizeof(int) * COLUMNS);
            memset(g_vector_d, 0, sizeof(int) * VECTOR_SIZE);
            memset(g_vector_d_written, 0, sizeof(int) * VECTOR_SIZE);
            g_vector_Cd_res_rec_count = 0;

            task_data_t task_data = {
                .task_type = ADD_RES_Cd_VECTOR_VALUE_TO_RES_VECTOR_VALUE
            };

            gateway_scheduler_add_task_to_queue(task_data);
        }
    }
}

void gateway_tasks_add_Ab_vector_res_val_to_res_vector_val(void)
{
    g_vector_res_val += g_vector_Ab_res_val;
    ++g_vector_res_rec_count;

    if (2 == g_vector_res_rec_count)
    {
        g_vector_res_rec_count = 0;

        g_vector_Ab_res_val = 0;
        g_vector_Cd_res_val = 0;

        task_data_t task_data = {
            .task_type = SEND_VELUE_TO_DESTSTATION
        };

        gateway_scheduler_add_task_to_queue(task_data);
    }
}

void gateway_tasks_add_Cd_vector_res_val_to_res_vector_val(void)
{
    g_vector_res_val += g_vector_Cd_res_val;
    ++g_vector_res_rec_count;

    if (2 == g_vector_res_rec_count)
    {
        g_vector_res_rec_count = 0;

        g_vector_Ab_res_val = 0;
        g_vector_Cd_res_val = 0;

        task_data_t task_data = {
            .task_type = SEND_VELUE_TO_DESTSTATION
        };

        gateway_scheduler_add_task_to_queue(task_data);
    }
}

void gateway_tasks_send_value_to_destination_station(void)
{
    send_vector_value_to_deststation_data_t send_vector_value_to_deststation_data = {
        .header = "vec",
        .vector_val = g_vector_res_val
    };

    g_vector_res_val = 0;

    if (-1 == gateway_socket_send(g_gateway_clients[GATEWAY_CLIENT_TYPE_DESTSTATION].socket_fd,
                                  &send_vector_value_to_deststation_data,
                                  sizeof(send_vector_value_to_deststation_data)))
    {
        GATEWAY_COMMON_ASSERT(0);
    }
}



