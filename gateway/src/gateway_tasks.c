/* encoding: UTF-8 */

#include "gateway_scheduler.h"
#include "gateway_tasks.h"
#include "gateway_database.h"

extern int g_mtrx_A[ROWS][COLUMNS];
extern int g_mtrx_C[ROWS][COLUMNS];

extern int g_vector_b[VECTOR_SIZE];
extern int g_vector_d[VECTOR_SIZE];

extern int g_vector_Ab_res[VECTOR_SIZE];
extern int g_vector_Cd_res[VECTOR_SIZE];

extern int g_vector_res[VECTOR_SIZE];

extern int g_mtrx_A_written[ROWS][COLUMNS];
extern int g_mtrx_C_written[ROWS][COLUMNS];

extern int g_vector_b_written[VECTOR_SIZE];
extern int g_vector_d_written[VECTOR_SIZE];

extern int g_vector_Ab_res_rec_count[VECTOR_SIZE];
extern int g_vector_Cd_res_rec_count[VECTOR_SIZE];

extern int g_vector_res_rec_count[VECTOR_SIZE];

void gateway_tasks_put_mtrx_A_value(mtrx_fmt_t mtrx)
{
    g_mtrx_A[mtrx.pos.row][mtrx.pos.col] = mtrx.value;
    g_mtrx_A_written[mtrx.pos.row][mtrx.pos.col] = 1;

    if (g_vector_b_written[mtrx.pos.col])
    {
        task_data_t task_data = {
            .task_type = MUL_MTRX_A_VALUE_ON_VEC_b_VALUE,
            .data.mtrx_val_pos = mtrx.pos
        };

        gateway_scheduler_add_task_to_queue(task_data);
    }
}

void gateway_tasks_put_mtrx_C_value(mtrx_fmt_t mtrx)
{
    g_mtrx_C[mtrx.pos.row][mtrx.pos.col] = mtrx.value;
    g_mtrx_C_written[mtrx.pos.row][mtrx.pos.col] = 1;

    if (g_vector_d_written[mtrx.pos.col])
    {
        task_data_t task_data = {
            .task_type = MUL_MTRX_C_VALUE_ON_VEC_d_VALUE,
            .data.mtrx_val_pos = mtrx.pos
        };

        gateway_scheduler_add_task_to_queue(task_data);
    }
}

void gateway_tasks_put_vector_b_value(vector_fmt_t vector)
{
    task_data_t task_data = {
        .task_type = MUL_VECTOR_b_VALUE_ON_MTRX_A_COL,
        .data.vector_val_pos = vector.pos
    };

    g_vector_b[vector.pos] = vector.value;
    g_vector_b_written[vector.pos] = 1;

    gateway_scheduler_add_task_to_queue(task_data);
}

void gateway_tasks_put_vector_d_value(vector_fmt_t vector)
{
    task_data_t task_data = {
        .task_type = MUL_VECTOR_d_VALUE_ON_MTRX_C_COL,
        .data.vector_val_pos = vector.pos
    };

    g_vector_d[vector.pos] = vector.value;
    g_vector_d_written[vector.pos] = 1;

    gateway_scheduler_add_task_to_queue(task_data);
}

void gateway_tasks_mul_mtrx_A_val_on_vector_b_val(mtrx_val_pos_t mtrx_val_pos)
{
    if (1 == g_mtrx_A_written[mtrx_val_pos.row][mtrx_val_pos.col])
    {
        g_vector_Ab_res[mtrx_val_pos.row] += g_mtrx_A[mtrx_val_pos.row][mtrx_val_pos.col] * g_vector_b[mtrx_val_pos.col];
        g_mtrx_A_written[mtrx_val_pos.row][mtrx_val_pos.col] = 2;
        ++g_vector_Ab_res_rec_count[mtrx_val_pos.row];

        if (COLUMNS == g_vector_Ab_res_rec_count[mtrx_val_pos.row])
        {
            task_data_t task_data = {
                .task_type = ADD_RES_Ab_VECTOR_VALUE_TO_RES_VECTOR_VALUE,
                .data.value_position = mtrx_val_pos.row
            };

            gateway_scheduler_add_task_to_queue(task_data);
        }
    }
}

void gateway_tasks_mul_mtrx_C_val_on_vector_d_val(mtrx_val_pos_t mtrx_val_pos)
{
    if (1 == g_mtrx_C_written[mtrx_val_pos.row][mtrx_val_pos.col])
    {
        g_vector_Cd_res[mtrx_val_pos.row] += g_mtrx_C[mtrx_val_pos.row][mtrx_val_pos.col] * g_vector_d[mtrx_val_pos.col];
        g_mtrx_C_written[mtrx_val_pos.row][mtrx_val_pos.col] = 2;
        ++g_vector_Cd_res_rec_count[mtrx_val_pos.row];

        if (COLUMNS == g_vector_Cd_res_rec_count[mtrx_val_pos.row])
        {
            task_data_t task_data = {
                .task_type = ADD_RES_Cd_VECTOR_VALUE_TO_RES_VECTOR_VALUE,
                .data.value_position = mtrx_val_pos.row
            };

            gateway_scheduler_add_task_to_queue(task_data);
        }
    }
}

void gateway_tasks_mul_vec_b_value_on_column_mtrx_A(int vector_val_pos)
{
    int row;

    for (row = 0; row < ROWS; row++)
    {
        if (1 == g_mtrx_A_written[row][vector_val_pos])
        {
            g_vector_Ab_res[row] += g_mtrx_A[row][vector_val_pos] * g_vector_b[vector_val_pos];
            g_mtrx_A_written[row][vector_val_pos] = 2;
            ++g_vector_Ab_res_rec_count[row];

            if (COLUMNS == g_vector_Ab_res_rec_count[row])
            {
                task_data_t task_data = {
                    .task_type = ADD_RES_Ab_VECTOR_VALUE_TO_RES_VECTOR_VALUE,
                    .data.value_position = row
                };

                gateway_scheduler_add_task_to_queue(task_data);
            }
        }
    }
}

void gateway_tasks_mul_vec_d_value_on_column_mtrx_C(int vector_val_pos)
{
    int row;

    for (row = 0; row < ROWS; row++)
    {
        if (1 == g_mtrx_C_written[row][vector_val_pos])
        {
            g_vector_Cd_res[row] += g_mtrx_C[row][vector_val_pos] * g_vector_d[vector_val_pos];
            g_mtrx_C_written[row][vector_val_pos] = 2;
            ++g_vector_Cd_res_rec_count[row];

            if (COLUMNS == g_vector_Cd_res_rec_count[row])
            {
                task_data_t task_data = {
                    .task_type = ADD_RES_Cd_VECTOR_VALUE_TO_RES_VECTOR_VALUE,
                    .data.value_position = row
                };

                gateway_scheduler_add_task_to_queue(task_data);
            }
        }
    }
}

void gateway_tasks_add_Ab_vector_res_val_to_res_vector_val(int value_position)
{
    g_vector_res[value_position] += g_vector_Ab_res[value_position];
    ++g_vector_res_rec_count[value_position];

    if (2 == g_vector_res_rec_count[value_position])
    {
        task_data_t task_data = {
            .task_type = SEND_VELUE_TO_DESTSTATION,
            .data.value_position = value_position
        };

        gateway_scheduler_add_task_to_queue(task_data);
    }
}

void gateway_tasks_add_Cd_vector_res_val_to_res_vector_val(int value_position)
{
    g_vector_res[value_position] += g_vector_Cd_res[value_position];
    ++g_vector_res_rec_count[value_position];

    if (2 == g_vector_res_rec_count[value_position])
    {
        task_data_t task_data = {
            .task_type = SEND_VELUE_TO_DESTSTATION,
            .data.value_position = value_position
        };

        gateway_scheduler_add_task_to_queue(task_data);
    }
}

void gateway_tasks_send_value_to_destination_station(int value_position)
{

}

void gateway_tasks_send_end_signal_to_destination_station(void)
{

}

void gateway_tasks_handle_end_mtrx_A_signal(void)
{

}

void gateway_tasks_handle_end_mtrx_C_signal(void)
{

}

void gateway_tasks_handle_end_vector_b_signal(void)
{

}

void gateway_tasks_handle_end_vector_d_signal(void)
{

}
