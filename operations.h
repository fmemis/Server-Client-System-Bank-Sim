#include "structs.h"

int HashFunction(char *string);
int AddAccount(float amount,char *Name,AccountList *matrix);
int AddTransfer(float amount,char *source,char *destination,AccountList *matrix);
int AddMultiTransfer(float amount,char *source,char **destinations,int count,AccountList *matrix);
int PrintBalance(char *Name,AccountList *matrix,float *balance);
int PrintMultiBalance(char **Names,AccountList *matrix,int count,float *balances);