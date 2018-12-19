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

- Sudoku Array Numbers Map

   0 1 2 3 4 5 6 7 8
 --------------------
0| 1 2 3 4 5 6 7 8 9
1| 4 5 6 7 8 9 1 2 3
2| 6 8 9 1 2 3 4 5 6
3| 2 1 4 3 6 5 8 9 7
4| 3 6 5 8 9 7 2 1 4
5| 8 9 7 2 1 4 3 6 5
6| 5 3 1 6 4 2 9 7 8
7| 6 4 2 9 7 8 5 3 1
8| 9 7 8 5 3 1 6 4 2


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
#include <semaphore.h>
#define SUDOKU_SIZE 9
#define THREADS_NUMBER 11

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_GREEN   "\x1b[32m"

sem_t s1,s2;
struct subGridData{
    int value;
    int oldRowNum;
    int oldColumnNum;
};
struct sudokuData{
    int rows[SUDOKU_SIZE][SUDOKU_SIZE];
    int columns[SUDOKU_SIZE][SUDOKU_SIZE];
    struct subGridData subGrids[SUDOKU_SIZE][SUDOKU_SIZE];
};
struct sudokuData sudokuInstance;

struct dublicatePoints {
    int row;
    int column;
};

struct dublicatePoints dublicateInstance[SUDOKU_SIZE*SUDOKU_SIZE];
int dublicateNum = 0;

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
            sudokuInstance.subGrids[blockNumber][colNum[blockNumber]].value = input[row][column];
            sudokuInstance.subGrids[blockNumber][colNum[blockNumber]].oldRowNum = row;
            sudokuInstance.subGrids[blockNumber][colNum[blockNumber]].oldColumnNum = column;
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
            printf("%d, ",sudokuInstance.subGrids[row][column].value);
        }
        printf ("\n");
    }
}

int matchedInDub (int row, int column){
    for (int i = 0 ; i < dublicateNum;i++){
        if(row == dublicateInstance[i].row && column == dublicateInstance[i].column )
        return 1;
    }
    return 0;
}

void *th_checkRows(){
    for (int row = 0; row < SUDOKU_SIZE ; row++){
        for (int itemInRow = 0 ; itemInRow < SUDOKU_SIZE ; itemInRow++){
            for (int itemInCol = 0 ; itemInCol < SUDOKU_SIZE ; itemInCol++){
                if(sudokuInstance.rows[row][itemInRow] == sudokuInstance.rows[row][itemInCol] && itemInRow != itemInCol){
                    while(1){
                        if(sem_wait(&s1) == -1){
                            perror("sem_wait");exit(EXIT_FAILURE);
                        }
                            dublicateInstance[dublicateNum].row = row;
                            dublicateInstance[dublicateNum].column = itemInRow;
                            dublicateNum++;
                        if(sem_post(&s1) == -1){
                            perror("sem_post");exit(EXIT_FAILURE);
                        }
                        break;
                    }
                }
            }
        }
    }
    pthread_exit(NULL);
}

void *th_checkColumns(){
    for (int row = 0; row < SUDOKU_SIZE ; row++){
        for (int itemInRow = 0 ; itemInRow < SUDOKU_SIZE ; itemInRow++){
            for (int itemInCol = 0 ; itemInCol < SUDOKU_SIZE ; itemInCol++){
                if(sudokuInstance.columns[row][itemInRow] == sudokuInstance.columns[row][itemInCol] && itemInRow != itemInCol){
                    while(1){
                        if(sem_wait(&s1) == -1){
                            perror("sem_wait");exit(EXIT_FAILURE);
                        }
                            dublicateInstance[dublicateNum].column = row;
                            dublicateInstance[dublicateNum].row = itemInRow;
                            dublicateNum++;
                        if(sem_post(&s1) == -1){
                            perror("sem_post");exit(EXIT_FAILURE);
                        }
                        break;
                    }
                }
            }
        }
    }
    pthread_exit(NULL);
}

void *th_checkSubGrids(){
    for (int row = 0; row < SUDOKU_SIZE ; row++){
        for (int itemInRow = 0 ; itemInRow < SUDOKU_SIZE ; itemInRow++){
            for (int itemInCol = 0 ; itemInCol < SUDOKU_SIZE ; itemInCol++){
                if(sudokuInstance.subGrids[row][itemInRow].value == sudokuInstance.subGrids[row][itemInCol].value && itemInRow != itemInCol){
                    while(1){
                        if(sem_wait(&s1) == -1){
                            perror("sem_wait");exit(EXIT_FAILURE);
                        }
                            dublicateInstance[dublicateNum].row = sudokuInstance.subGrids[row][itemInRow].oldRowNum;
                            dublicateInstance[dublicateNum].column = sudokuInstance.subGrids[row][itemInRow].oldColumnNum;
                            dublicateNum++;
                        if(sem_post(&s1) == -1){
                            perror("sem_post");exit(EXIT_FAILURE);
                        }
                        break;
                    }
                }
            }
        }
    }
    pthread_exit(NULL);
}

int isValid(int number, int row, int column) {
    int i=0;
    int modRow = 3*(row/3);
    int modCol = 3*(column/3);
    int row1 = (row+2)%3;
    int row2 = (row+4)%3;
    int col1 = (column+2)%3;
    int col2 = (column+4)%3;

    for (i=0; i<9; i++) 
    {
        if (sudokuInstance.rows[i][column] == number) return 0;
        if (sudokuInstance.rows[row][i] == number) return 0;
    }  
    if(sudokuInstance.rows[row1+modRow][col1+modCol] == number) return 0;
    if(sudokuInstance.rows[row2+modRow][col1+modCol] == number) return 0;
    if(sudokuInstance.rows[row1+modRow][col2+modCol] == number) return 0;
    if(sudokuInstance.rows[row2+modRow][col2+modCol] == number) return 0;
    return 1;
}

int sudokuHelper(int row, int column) {
    int nextNumber = 1;
    if (row == 9) 
    {
        return 1;
    }
    if (sudokuInstance.rows[row][column]) 
    {
        if (column == 8) 
        {
            if (sudokuHelper(row+1, 0)) return 1;
        } 
        else 
        {
            if (sudokuHelper(row, column+1)) return 1;
        }
        return 0;
    }

    for (; nextNumber<10; nextNumber++) 
    {
        if(isValid(nextNumber, row, column)) 
        {
            sudokuInstance.rows[row][column] = nextNumber;
            sudokuInstance.rows[row][column] = nextNumber;
            if (column == 8) 
            {
                if (sudokuHelper(row+1, 0)) return 1;
            } 
            else 
            {
                if (sudokuHelper(row, column+1)) return 1;
            }
            sudokuInstance.rows[row][column] = 0;
        }
    }
    return 0;
}

int solveSudoku() {
    return sudokuHelper(0, 0);
}

void printSudoku()
{
    for (int row=0; row < SUDOKU_SIZE ; row++)
    {
        for (int column=0; column < SUDOKU_SIZE ; column++)
        {
            printf("%d ",sudokuInstance.rows[row][column]);
        }
        printf ("\n");
    }
}
int main (){
    int puzzle[9][9];
    int inputData[SUDOKU_SIZE][SUDOKU_SIZE];
    int option;
    if(sem_init(&s1, 0, 1) == -1){
		perror("semaphore_init");exit(EXIT_FAILURE);
	}
    while(1)
    {
        printf("\n\n");
        printf("1- Enter a Sudoku for the Checker\n");
        printf("2- Enter a Sudoku for the Solver\n");
        printf("3- Verify Sudoku Solver\n");
        printf("4- Exit\n");
        printf("Chooes an option from above:");
        scanf("%d", &option);

        if(option == 1)
        {
            //Store InputDataSudoku
            printf("Enter Sudoku Data , %d numbers in each row\n",SUDOKU_SIZE);
            for (int row=0; row < SUDOKU_SIZE ; row++){
                for (int column=0; column < SUDOKU_SIZE ; column++){
                    scanf("%d",&inputData[row][column]);
                }
            }

            //store structured sudoku
            storeSudokuData(inputData);

            // Print the Sudoku and all the generated data.
            //printSudokuData();

            //Checking Sudoku using threads
            pthread_t tid[THREADS_NUMBER];

            if(pthread_create(&tid[0], NULL, th_checkRows, NULL) != 0){
                perror("pthread_create"), exit(1);
            }
            if(pthread_create(&tid[1], NULL, th_checkColumns, NULL) != 0){
                perror("pthread_create"), exit(1);
            }

            if(pthread_create(&tid[2], NULL, th_checkSubGrids, NULL) != 0){
                perror("pthread_create"), exit(1);
            }

            if(pthread_join(tid[0], NULL) != 0){
                perror("pthread_join"), exit(1);
            }

            if(pthread_join(tid[1], NULL) != 0){
                perror("pthread_join"), exit(1);
            }
            if(pthread_join(tid[2], NULL) != 0){
                perror("pthread_join"), exit(1);
            }
            if(dublicateNum!=0){
                printf("Dublicates Found :\n");
                for (int row=0; row < SUDOKU_SIZE ; row++){
                    for (int column=0; column < SUDOKU_SIZE ; column++){
                        if(matchedInDub(row,column)){
                            printf(ANSI_COLOR_RED"%d"ANSI_COLOR_RESET" ",sudokuInstance.rows[row][column]);
                        }else{
                            printf("%d ",sudokuInstance.rows[row][column]);
                        }
                    }
                    printf ("\n");
                }
            dublicateNum=0;
            }
            else if(dublicateNum==0){ printf("\nValid Sudoku."); }
            
            if(sem_destroy(&s1) == -1){
                perror("semaphore_destroy");exit(EXIT_FAILURE);
            }
        }
        else if(option == 2) {
            int i;
            /*for (i=0; i<SUDOKU_SIZE; i++) {
                scanf("%d %d %d %d %d %d %d %d %d", &puzzle[i][0],
                &puzzle[i][1], &puzzle[i][2], &puzzle[i][3], &puzzle[i][4],
                &puzzle[i][5], &puzzle[i][6], &puzzle[i][7], &puzzle[i][8]);
            }
                for (int row=0; row < SUDOKU_SIZE ; row++){
                    for (int column=0; column < SUDOKU_SIZE ; column++){
                        inputData[row][column] = puzzle[row][column];
                    }
            }*/
            printf("Enter Sudoku Data , %d numbers in each row\n",SUDOKU_SIZE);
            for (int row=0; row < SUDOKU_SIZE ; row++){
                for (int column=0; column < SUDOKU_SIZE ; column++){
                    scanf("%d",&inputData[row][column]);
                }
            }
            storeSudokuData(inputData);
            if (solveSudoku(puzzle)) 
            {
                printf("\nSolved Sudoku:\n");
                printSudoku(); /* Prints only the Sudoku in usual form. */
                //printSudokuData(); /* Will print all Stored data: Rows/Columns/Grids. */
            } 
            else {
                printf("Something went wrong.");
            }
        }
        else if(option == 3) { }
        else if(option == 4) { printf("\n Exiting, Thanks for using!\n"); break; }
    }

    return 1;
}