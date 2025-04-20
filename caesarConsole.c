#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define TAG 0


void encrypt(char *s, int l) {
    for (int i = 0; i < l;i++) {
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
    char input[500];
    char result_text[500];
    int inputL = 0;
    int op;
    int subSize;
    char local[1000];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numofP);

    if (rank == 0) {
        printf("Enter input to process :");
        scanf("%499s", input);
        inputL = strlen(input);
        if (inputL % numofP != 0) {
            int newL = ((inputL / numofP) + 1) * numofP;
            for (int i = inputL; i < newL; i++) {
                input[i] = ' ';
            }
            input[newL] = '\0';
            inputL = newL;
        }
        
        printf("Choose encrypt or decrypt:\n1. Encrypt\n2. Decrypt\nI choose : ");
        scanf("%d", &op);
        
        for (int i = 1; i < numofP; i++) {
            MPI_Send(&op, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&inputL, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(&op, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&inputL, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    subSize = inputL / numofP;


    if (rank == 0) {
        
        int start = subSize; 
        for (int i = 1; i < numofP; i++) {
            MPI_Send(input + start, subSize, MPI_CHAR, i, 3, MPI_COMM_WORLD);
            start += subSize;
        }
        
        strncpy(local, input, subSize);
    }
    else {

        MPI_Recv(local, subSize, MPI_CHAR, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
            MPI_Recv(result_text + start, subSize, MPI_CHAR, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            start += subSize;
        }
        result_text[inputL] = '\0';
        
        
        int i = inputL - 1;
        while (i >= 0 && result_text[i] == ' ') {
            result_text[i] = '\0';
            i--;
        }
        printf("Result: %s\n", result_text);
    }
    else {
        
        MPI_Send(local, subSize, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
