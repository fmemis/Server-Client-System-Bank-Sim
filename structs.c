#include "structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SumList SumListCreate() {
	SumList list;
	list = malloc(sizeof(InfoSumList));
	list->front = list->rear = NULL;
	list->counter = 0;
	return list;
}
void SumListAddAmount(SumList list,char *Name,float amount) {
	SumListNodePtr temp = list->front;
	int flag = 0;
	while (temp != NULL && !flag) {
		//if name already exists just add the amount of money to this name.
		if (!strcmp(Name,temp->name)) {
			flag = 1;
			temp->sum += amount;
		}
		temp = temp->next;
	}
	if (!flag) {
		temp = malloc(sizeof(SumListNode));
		strcpy(temp->name,Name);
		temp->sum = amount;
		temp->next = NULL;
		if (list->front == NULL) 
			list->front = temp;
		else 
			list->rear->next = temp;
		list->rear = temp;
		++list->counter;
	}
}

void SumListDestroy(SumList list) {
	SumListNodePtr temp,temp2;
	temp = list->front;
	while (temp != NULL) {
		temp2 = temp;
		temp = temp->next;
		free(temp2);
	}
	free(list);
}

Account AccountCreate(char *Name,float amount) {
	Account account;
	if (strlen(Name) > 40) {
		printf("Thats an abnormal long name.The account has not been created\n");
		account.balance = -10;
		return account;
	}
	strcpy(account.name,Name);
	account.balance = amount;
	account.list = SumListCreate();
	return account;
}

void AccountAddAmount(Account *account,float amount) {
	account->balance += amount;
}

int AccountRemoveAmount(Account *account,float amount) {
	if (account->balance >= amount)
		account->balance -= amount;
	else
		return 1;
	return 0; 
}
void AccountPrintBalance(Account *account,float *balance) {
	//printf("The balance of the account is %f\n",account->balance);
	//just give the balance variable to the caller.He will print.
	*balance = account->balance;
}

void AccountAddToSumList(Account *account,char *Name,float amount) {
	SumListAddAmount(account->list,Name,amount);
}

AccountList AccountListCreate() {
	AccountList list;
	list = malloc(sizeof(InfoSumList));
	list->front = list->rear = NULL;
	list->counter = 0;
	return list;
}

void AccountListInsert(AccountList list,Account acc) {
	AccountListNodePtr temp;
	temp = malloc(sizeof(AccountListNode));
	temp->account = acc;
	temp->next = NULL;
	if (list->front == NULL) 
		list->front = temp;
	else 
		list->rear->next = temp;
	list->rear = temp;
	++list->counter;
}
void AccountListDestroy(AccountList list) {
	AccountListNodePtr temp,temp2;
	temp = list->front;
	while (temp != NULL) {
		temp2 = temp;
		temp = temp->next;
		free(temp2);
	}
	free(list);
}

Account* AccountListGetAccount(AccountList list,char *Name) {
	AccountListNodePtr temp = list->front;
	int flag = 0;
	if (temp == NULL) {
		//printf("There are no accounts in this list\n");
		return NULL;
	}
	if (!strcmp(temp->account.name,Name))
			flag = 1;
	while (temp->next != NULL && !flag) {
		temp = temp->next;
		if (!strcmp(temp->account.name,Name))
			flag = 1;
	}
	if (flag == 0) {
		//printf("There is no such account\n");
		return NULL;
	}
	return (&(temp->account));
}