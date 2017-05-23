/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	int size;
	if (M == 32 && N == 32){
		size = 8;
		for (int row = 0; row < N; row+=size){
			for (int col = 0; col < M; col+=size){
				for (int i = row; i < row+size; i++){
					int temp, index, stat;
					for (int j = col; j < col+size; j++){
						if (i == j){
							temp = A[i][j];
							index = i;
							stat = 1;
						}
						else{
							B[j][i] = A[i][j];
						}
					}
					if (stat){
						stat = 0;
						B[index][index] = temp;
					}
				}
			}
		}
	}
	else if (M == 32 && N == 64){
		for (int row = 0; row < 64; row+=8){
			for (int col = 0; col < 32; col+=4){
				for (int i = row; i < row+8; i++){
					int temp, index, stat;
					for (int j = col; j < col+4; j++){
						if (i == j){
							temp = A[i][j];
							index = i;
							stat = 1;
						}
						else{
							B[j][i] = A[i][j];
						}
					}
					if (stat){
						stat = 0;
						B[index][index] = temp;
					}
				}
			}
		}
	}
	else if(M == 64 && N == 64){
		for(int row = 0; row < N; row += 4)
		{
			for(int col = 0; col < M; col += 4)
			{
				B[col+3][row] = A[row][col+3];
				B[col+3][row+1] = A[row+1][col+3];
				B[col+3][row+2] = A[row+2][col+3];
				int temp4 = A[row+2][col+2];
				int temp3 = A[row+2][col+1];
				int temp2 = A[row+2][col];
				B[col+2][row] = A[row][col+2];
				B[col+2][row+1] = A[row+1][col+2];
				B[col+2][row+2] = temp4;
				temp4 = A[row+1][col+1];
				int temp1 = A[row+1][col];
				int temp0 = A[row][col];
				B[col+1][row] = A[row][col+1];
				B[col+1][row+1] = temp4;
				B[col+1][row+2] = temp3;
				B[col][row] = temp0;
				B[col][row+1] = temp1;
				B[col][row+2] = temp2;
				B[col][row+3] = A[row+3][col];
				B[col+1][row+3] = A[row+3][col+1];
				B[col+2][row+3] = A[row+3][col+2];
				temp0 = A[row+3][col+3];
				B[col+3][row+3] = temp0;
			}
		}
	}
	else if(M == 61 && N == 67){
		size = 20;
		for(int row = 0; row < N; row += size){
			for(int col = 0; col < M; col += size){
				for(int i = row; i < row + size; i++){
					for(int j = col; j < col + size; j++){
						if(!(j > M - 1 || i > N - 1)){
							B[j][i] = A[i][j];
						}
					}
				}
			}
		}
	}
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

