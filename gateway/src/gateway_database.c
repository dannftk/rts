/* encoding: UTF-8 */

#include "gateway_database.h"

int g_mtrx_A[ROWS][COLUMNS];
int g_mtrx_C[ROWS][COLUMNS];

int g_vector_b[VECTOR_SIZE];
int g_vector_d[VECTOR_SIZE];

int g_vector_Ab_res[VECTOR_SIZE];
int g_vector_Cd_res[VECTOR_SIZE];

int g_vector_res[VECTOR_SIZE];

/* Permitions */

int g_mtrx_A_written[ROWS][COLUMNS];
int g_mtrx_C_written[ROWS][COLUMNS];

int g_vector_b_written[VECTOR_SIZE];
int g_vector_d_written[VECTOR_SIZE];

int g_vector_Ab_res_rec_count[VECTOR_SIZE];
int g_vector_Cd_res_rec_count[VECTOR_SIZE];

int g_vector_res_rec_count[VECTOR_SIZE];