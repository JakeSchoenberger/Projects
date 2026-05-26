
#include <stdio.h> //printf, scanf //
#include <stdlib.h> //fprint, fgets //
#include <string.h>
#include <math.h>


int newMessage(char message[]);

void display(char* array[]);

void decryptMessage(char message[]);

void showDecrypted(char message[], int index);

int start();

int start(){

  char* array[9];

  for(int i=0; i < 10; ++i){

    array[i]="This is the original message.";
    
  }
  
  return 0;

}

int newMessage(char message[]){

  printf("Enter your new message.");
  
  int buffer = 256;
  int position = 0;

  char* cmd = malloc(sizeof(char) * buffer);

  int cha;
  int cont = 1;

  while (cont == 1){

    cha = fgetc(stdin);

    if (cha == EOF || cha == '\n'){

      cmd[position] = '\0';
      cont = 0;

    }

    else{

      cmd[position] = cha;

    }

    position++;

    if (position >= buffer){

      buffer += 256;
      cmd = realloc(cmd, buffer);

    }

  }

  int index = 0;
  
  while(array[index] != "This is the original message." && index < 10){

    index++;

  }

  if (index < 10){

    return index;

    array[index] = cmd;

  }

  else{

    return 0;
    
    array[0] = cmd;

  }
  
}



void display(char* array[]){

  for(int i=0; i < 10; ++i){

    printf("%c" , *array[i]);

  }

}

void decryptMessage(char message[]){

  int numberOfE = 0;
  int maxEIndex = 0;
  int maxE = 0;
  int asciiValuee = 101;
  int asciiValueE = 69;
  int numberOfT = 0;
  int maxTIndex = 0;
  int maxT = 0;
  int asciiValuet = 116;
  int asciiValueT = 84;
  int numberOfA = 0;
  int maxAIndex = 0;
  int maxA = 0;
  int asciiValuea = 97;
  int asciiValueA = 65;
  int numberOfO = 0;
  int maxOIndex = 0;
  int maxO = 0;
  int asciiValueo = 111;
  int asciiValueO = 79;
  int numberOfI = 0;
  int maxIIndex = 0;
  int maxI = 0;
  int asciiValuei = 105;
  int asciiValueI = 73;

  int *workingMessageE = (int*)malloc(strlen(message) * 4);
  
  int *workingMessageT = (int*)malloc(strlen(message) * 4);
  
  int *workingMessageA = (int*)malloc(strlen(message) * 4);
  
  int *workingMessageO = (int*)malloc(strlen(message) * 4);
  
  int *workingMessageI = (int*)malloc(strlen(message) * 4);
  
  
  for (int i = 1; i < 26; i++){

    for (int j = 0; j < strlen(message); j++){

      workingMessageE[j] = (int)message[j] + 1;

      if (workingMessageE[j] == 91){

	workingMessageE[j] = 65;

      }

      else if (workingMessageE[j] == 123){

	workingMessageE[j] = 97;

      }

      else if (workingMessageE[j] == asciiValuee || workingMessageE[j] == asciiValueE){

	numberOfE++;

      }

    }

    if (numberOfE > maxE){

      maxE = numberOfE;
      maxEIndex = i;
      
    }

    numberOfE = 0;
    
  }

  for (int i = 1; i < 26; i++){

    for (int j = 0; j < strlen(message); j++){

      workingMessageT[j] = workingMessageT[j] + 1;

      if (workingMessageT[j] == 91){

        workingMessageT[j] = 65;

      }

      else if (workingMessageT[j] == 123){

        workingMessageT[j] = 97;

      }
      
      else if (workingMessageT[j] == asciiValuet || workingMessageT[j] == asciiValueT){

        numberOfT++;

      }

    }

    if (numberOfT > maxT){

      maxT = numberOfT;
      maxTIndex = i;

    }

    numberOfT = 0;
    
  }

  for (int i = 1; i < 26; i++){

    for (int j = 0; j < strlen(message); j++){

      workingMessageA[j] = workingMessageA[j] + 1;

      if (workingMessageA[j] == 91){

        workingMessageA[j] = 65;
	numberOfA++;

      }

      else if ((int)workingMessageA[j] == 123){

        workingMessageA[j] = 97;
	numberOfA++;
	
      }
      
    }

    if (numberOfA > maxA){

      maxA = numberOfA;
      maxAIndex = i;

    }

    numberOfA = 0;
    
  }

  for (int i = 1; i < 26; i++){

    for (int j = 0; j < strlen(message); j++){

      workingMessageO[j] = workingMessageO[j] + 1;

      if (workingMessageO[j] == 91){

        workingMessageO[j] = 65;

      }

      else if (workingMessageO[j] == 123){

        workingMessageO[j] = 97;

      }

      else if (workingMessageO[j] == asciiValueo || workingMessageO[j] == asciiValueO){

        numberOfO++;

      }

    }

    if (numberOfO > maxO){

      maxO = numberOfO;
      maxOIndex = i;

    }

    numberOfO = 0;
    
  }

  for (int i = 1; i < 26; i++){

    for (int j = 0; j < strlen(message); j++){

      workingMessageI[j] = workingMessageI[j] + 1;

      if (workingMessageI[j] == 91){

        workingMessageI[j] = 65;

      }

      else if (workingMessageI[j] == 123){

        workingMessageI[j] = 97;

      }
      
      else if (workingMessageI[j] == asciiValuei || workingMessageI[j] == asciiValueI){

        numberOfI++;

      }

    }

    if (numberOfI > maxI){

      maxI = numberOfI;
      maxIIndex = i;

    }

    numberOfI = 0;
    
  }

  showDecrypted(message, maxEIndex);
  showDecrypted(message, maxTIndex);
  showDecrypted(message, maxAIndex);
  showDecrypted(message, maxOIndex);
  showDecrypted(message, maxIIndex);

}

void showDecrypted(char message[], int index){

  int asciiValuesDecrypted[strlen(message)];
  char decryptedMessage[strlen(message)];
  
  for (int j = 0; j < strlen(message); j++){

    if (asciiValuesDecrypted[j] > 64 && asciiValuesDecrypted[j] < 91){
      
      asciiValuesDecrypted[j] = (int)message[j] + index;

      if (asciiValuesDecrypted[j] > 90){

	asciiValuesDecrypted[j] -= 26;

      }

    }

    else if (asciiValuesDecrypted[j] > 96 && asciiValuesDecrypted[j] < 123){

      asciiValuesDecrypted[j] = (int)message[j] + index;

      if (asciiValuesDecrypted[j] > 122){

	asciiValuesDecrypted[j] -= 26;

      }

    }
    
  }

  for (int i = 0; i < strlen(message); i++){

    decryptedMessage[i] = asciiValuesDecrypted[i] + '0';

  }

  printf("%s", decryptedMessage);
  
}
