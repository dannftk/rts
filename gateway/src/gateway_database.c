/* encoding: UTF-8 */

#include "gateway_database.h"

int g_mtrx_A_row[COLUMNS];
int g_mtrx_C_row[COLUMNS];

int g_vector_b[VECTOR_SIZE];
int g_vector_d[VECTOR_SIZE];

int g_vector_Ab_res_val;
int g_vector_Cd_res_val;

int g_vector_res_val;

/* Permitions */

int g_mtrx_A_row_written[COLUMNS];
int g_mtrx_C_row_written[COLUMNS];

int g_vector_b_written[VECTOR_SIZE];
int g_vector_d_written[VECTOR_SIZE];

int g_vector_Ab_res_rec_count;
int g_vector_Cd_res_rec_count;

int g_vector_res_rec_count;