/************************
-Exaamble Suduko

1 2 3 4 5 6 7 8 9
4 5 6 7 8 9 1 2 3
6 8 9 1 2 3 4 5 6
2 1 4 3 6 5 8 9 7
3 6 5 8 9 7 2 1 4
8 9 7 2 1 4 3 6 5
5 3 1 6 4 2 9 7 8
6 4 2 9 7 8 5 3 1
9 7 8 5 3 1 6 4 2

- SubGrids Array Row Number Map

+---+---+---+
| 0 | 1 | 2 |
+---+---+---+
| 3 | 4 | 5 |
+---+---+---+
| 6 | 7 | 8 |
+---+---+---+

*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define SUDOKU_SIZE 9
#define THREADS_NUMBER 11

struct sudokuData{
    int rows[SUDOKU_SIZE][SUDOKU_SIZE];
    int columns[SUDOKU_SIZE][SUDOKU_SIZE];
    int subGrids[SUDOKU_SIZE][SUDOKU_SIZE];
};

struct sudokuData sudokuInstance;

void storeSudokuData(int input[SUDOKU_SIZE][SUDOKU_SIZE]){
    //Store rows
    for (int row=0; row < SUDOKU_SIZE ; row++){
        for (int column=0; column < SUDOKU_SIZE ; column++){
            sudokuInstance.rows[row][column] = input[row][column];
        }
    }
    //Store Columns
    for (int row=0; row < SUDOKU_SIZE ; row++){
        for (int column=0; column < SUDOKU_SIZE ; column++){
            sudokuInstance.columns[column][row] = input[row][column];
        }
    }
    //Store Sub-Grids
    int colNum[SUDOKU_SIZE]={},blockNumber=0;
    for (int row=0; row < SUDOKU_SIZE ; row++){
        for (int column=0; column < SUDOKU_SIZE ; column++){
            blockNumber = (row/3)*3 + (column/3);
            sudokuInstance.subGrids[blockNumber][colNum[blockNumber]] = input[row][column];
            colNum[blockNumber]++;
        }
    }
}

void printSudokuData(){
    //Print Rows
    printf("Rows Stored :\n");
    for (int row=0; row < SUDOKU_SIZE ; row++){
        for (int column=0; column < SUDOKU_SIZE ; column++){
            printf("%d, ",sudokuInstance.rows[row][column]);
        }
        printf ("\n");
    }
    //Store Columns
    printf("Columns Stored :\n");
    for (int row=0; row < SUDOKU_SIZE ; row++){
        for (int column=0; column < SUDOKU_SIZE ; column++){
            printf("%d, ",sudokuInstance.columns[row][column]);
        }
        printf ("\n");
    }
    //Store Sub-Grids
    printf("Sub-Grids Stored :\n");
    for (int row=0; row < SUDOKU_SIZE ; row++){
        for (int column=0; column < SUDOKU_SIZE ; column++){
            printf("%d, ",sudokuInstance.subGrids[row][column]);
        }
        printf ("\n");
    }
}

void *th_checkRows(){
    int first = 0;
    for (int row = 0; row < 9 ; row++){
        for (int itemInRow = 0 ; itemInRow < 9 ; itemInRow++){
            for (int itemInCol = 0 ; itemInCol < 9 ; itemInCol++){
                if(sudokuInstance.rows[row][itemInRow] == sudokuInstance.rows[row][itemInCol] && itemInRow != itemInCol){
                    first==0?printf("Intersection Found in Rows : [%d : %d] ",row+1,itemInCol+1):printf("and [%d : %d]\n",row+1,itemInCol+1);
                    if (first==0) first = -1 ;else first = 0;
                    break;
                }
            }
        }
    }
}

int main (){
    int inputData[SUDOKU_SIZE][SUDOKU_SIZE];
    //Store InputDataSudoku
    printf("Enter Sudoku Data , %d numbers in each row\n",SUDOKU_SIZE);
    for (int row=0; row < SUDOKU_SIZE ; row++){
        for (int column=0; column < SUDOKU_SIZE ; column++){
            scanf("%d",&inputData[row][column]);
        }
    }

    //store structured sudoku
    storeSudokuData(inputData);

    //Show Data Entered
    printSudokuData();

    //Checking Sudoku using threads
    pthread_t tid[THREADS_NUMBER];

    if(pthread_create(&tid[0], NULL, th_checkRows, NULL) != 0){
        perror("pthread_create"), exit(1);
    }
    if(pthread_join(tid[0], NULL) != 0){
        perror("pthread_join"), exit(1);
    }
    
    return 1;
}