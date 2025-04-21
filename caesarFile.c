#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define TAG_OP 1
#define TAG_LEN 2
#define TAG_DATA 3
#define MAX_TEXT_LEN 5000

void encrypt(char *s, int l) {
    for (int i = 0; i < l; i++) {
        if (s[i] >= 'a' && s[i] <= 'z') {
            s[i] = 'a' + (s[i] - 'a' + 3) % 26;
        } else if (s[i] >= 'A' && s[i] <= 'Z') {
            s[i] = 'A' + (s[i] - 'A' + 3) % 26;
        }
    }
}

void decrypt(char *s, int l) {
    for (int i = 0; i < l; i++) {
        if (s[i] >= 'a' && s[i] <= 'z') {
            s[i] = 'a' + (s[i] - 'a' - 3 + 26) % 26;
        } else if (s[i] >= 'A' && s[i] <= 'Z') {
            s[i] = 'A' + (s[i] - 'A' - 3 + 26) % 26;
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, numofP;
    char input[MAX_TEXT_LEN] = {0};
    char result_text[MAX_TEXT_LEN] = {0};
    int inputL = 0;
    int op = -1;
    int subSize;
    char local[MAX_TEXT_LEN] = {0};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numofP);

    if (rank == 0) {
        if (argc != 2) {
            printf("This version of code needs the file input name as arg to the main function !!\n");
       MPI_Abort(MPI_COMM_WORLD, 1);
        }

        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            printf("Cannot open file: %s\n", argv[1]);
          MPI_Abort(MPI_COMM_WORLD, 1);
        }

        inputL = fread(input, 1, MAX_TEXT_LEN - 1, file);
        input[inputL] = '\0';
        fclose(file);

      
        if (inputL % numofP != 0) {
            int newL = ((inputL / numofP) + 1) * numofP;
            for (int i = inputL; i < newL; i++) {
                input[i] = ' ';
            }
            input[newL] = '\0';
            inputL = newL;
        }

        while(op!=1&&op!=2){
        printf("Enter operation (1 for encrypt, 2 for decrypt): ");
        scanf("%d", &op);
        }
        for (int i = 1; i < numofP; i++) {
            MPI_Send(&op, 1, MPI_INT, i, TAG_OP, MPI_COMM_WORLD);
            MPI_Send(&inputL, 1, MPI_INT, i, TAG_LEN, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(&op, 1, MPI_INT, 0, TAG_OP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&inputL, 1, MPI_INT, 0, TAG_LEN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    subSize = inputL / numofP;

    if (rank == 0) {
        int start = subSize;
        for (int i = 1; i < numofP; i++) {
            MPI_Send(input + start, subSize, MPI_CHAR, i, TAG_DATA, MPI_COMM_WORLD);
            start += subSize;
        }
        strncpy(local, input, subSize);
    } else {
        MPI_Recv(local, subSize, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    local[subSize] = '\0';

    if (op == 1) {
        encrypt(local, subSize);
    } else {
        decrypt(local, subSize);
    }

    if (rank == 0) {
        strncpy(result_text, local, subSize);
        int start = subSize;
        for (int i = 1; i < numofP; i++) {
            MPI_Recv(result_text + start, subSize, MPI_CHAR, i, TAG_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            start += subSize;
        }
        result_text[inputL] = '\0';

    
        int i = inputL - 1;
        while (i >= 0 && result_text[i] == ' ') {
            result_text[i--] = '\0';
        }

            printf("output:\n%s\n", result_text);
    } else {
        MPI_Send(local, subSize, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

