#include "operations.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int HashFunction(char *string) {
	int count = strlen(string);
	int i = 0,num = 0;
	for (i = 0;i < count;++i)
		num += (string[i] - '0');
	return (num % 10);
}

//add an account to a list of the hashtable
int AddAccount(float amount,char *Name,AccountList *matrix) {
	Account *check;
	int pos = HashFunction(Name);
	//names must be unique.Return error if account already exists.
	if ((check = AccountListGetAccount(matrix[pos],Name)) !=  NULL)
		return -1;
	//printf("The position is %d\n",pos);
	Account account = AccountCreate(Name,amount);
	if (account.balance < 0)
		return -1;
	AccountListInsert(matrix[pos],account);
	return 0;
}

int AddTransfer(float amount,char *source,char *destination,AccountList *matrix) {
	int pos = HashFunction(source);
	Account *account;
	if ((account = AccountListGetAccount(matrix[pos],source)) == NULL)
		return 1;
	if (AccountRemoveAmount(account,amount))
		return -1;
	pos = HashFunction(destination);
	if ((account = AccountListGetAccount(matrix[pos],destination)) == NULL)
		return 2;
	AccountAddAmount(account,amount);
	SumListAddAmount(account->list,source,amount);
	return 0;
}

int AddMultiTransfer(float amount,char *source,char **destinations,int count,AccountList *matrix) {
	int pos = HashFunction(source);
	Account *account;
	if ((account = AccountListGetAccount(matrix[pos],source)) == NULL)
		return 1;
	int i = 0,newamount;
	newamount = amount * count;
	if (AccountRemoveAmount(account,newamount))
		return -1;
	for (i = 0;i < count;++i) {
		pos = HashFunction(destinations[i]);
		if ((account=AccountListGetAccount(matrix[pos],destinations[i])) == NULL)
			return (i + 2);
		AccountAddAmount(account,amount);
		SumListAddAmount(account->list,source,amount);
	}
	return 0;
}

int PrintBalance(char *Name,AccountList *matrix,float *balance) {
	int pos = HashFunction(Name);
	Account *account;
	if ((account = AccountListGetAccount(matrix[pos],Name)) == NULL)
		return 1;
	AccountPrintBalance(account,balance);
	return 0;
}

int PrintMultiBalance(char **Names,AccountList *matrix,int count,float *balances) {
	int i,pos;
	Account *account;
	float balance;
	for (i = 0;i < count;++i) {
		pos = HashFunction(Names[i]);
		if ((account = AccountListGetAccount(matrix[pos],Names[i])) == NULL)
			return (i + 1);
		AccountPrintBalance(account,&balance);
		balances[i] = balance;
	}
	return 0;
}































