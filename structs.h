
//a SumList keeps record of all money trasfers to the associated account
//for example:if Jonh has an account and Mairy and George sent him money in the past
//their names and the amount of money they send will be in the sumlist.
typedef struct SumListNode *SumListNodePtr;

typedef struct SumListNode {
	float sum;
	char name[40];
	SumListNodePtr next;
}SumListNode;

typedef struct InfoSumList {
	SumListNodePtr front;
	SumListNodePtr	rear;
	int counter;
}InfoSumList;

typedef InfoSumList *SumList;

SumList SumListCreate();
void SumListAddAmount(SumList list,char *Name,float amount);
void SumListDestroy(SumList list);

typedef struct {
	char name[40];
	float balance;
	SumList list;
}Account;

Account AccountCreate(char *Name,float amount);
void AccountAddAmount(Account *account,float amount);
int AccountRemoveAmount(Account *account,float amount);
void AccountPrintBalance(Account *account,float *balance);
void AccountAddToSumList(Account *account,char *Name,float amount);

typedef struct AccountListNode *AccountListNodePtr;

typedef struct AccountListNode {
	Account account;
	AccountListNodePtr next;
}AccountListNode;

typedef struct InfoAccountList {
	AccountListNodePtr front;
	AccountListNodePtr rear;
	int counter;
}InfoAccountList;

typedef InfoAccountList *AccountList;

AccountList AccountListCreate();
void AccountListInsert(AccountList list,Account acc);
void AccountListDestroy(AccountList list);
Account* AccountListGetAccount(AccountList list,char *Name);