#include <stdio.h>
#include <stdlib.h>

#define DATA_SIZE 10


int received[DATA_SIZE] = {1,3,2,3,1,4,5,4,6,6};
int toSend[DATA_SIZE] = {0,0,0,0,0,0,0,0,0,0};

void removeDuplicate(int* received){
    int i=0, j=0; 
    
    for ( i = 0; i < 10; i++)
    {
        for ( j = 0; j < 10; j++)
        {
            if (received[i] == received[j] && i != j )
            {
                                    
                // printf("%d", received[i]);
                // printf(" %d\n", received[j]);
                received[j]= -1;
            }

        }
    }

}

void addToSend(int* received, int* toSend){
    int i=0, j=0; 
    
    for ( i = 0; i < DATA_SIZE; i++)
    {
            toSend[i] = -1;
    }

    for ( i = 0; i < DATA_SIZE; i++)
    {
        if(received[i] != -1){
            toSend[j] = received[i];
            j++;
        }
    }
    
}

int main(void){
    int i=0;

    removeDuplicate(received);
    addToSend(received, toSend);

    for (i = 0; i < DATA_SIZE; i++)
    {
        printf("aqui %d\n", toSend[i]);
    }

    
    return 0;
}