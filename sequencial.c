#include <stdio.h>
#include <stdlib.h>

struct rota{
		int origemID;
		int destinoID;
		int custo;
};

struct city{
	int id;
	struct rota *ligacoes;
};

struct tour
{
	struct city cities[20];
	int cost;
	int num_cities;
};

typedef struct tour stackElementT;

typedef struct {
  stackElementT *contents;
  int size;
  int top;
} stackT;

void StackInit(stackT *stackP)
{
  /* Allocate a new array to hold the contents. */

  stackP->size = 0;
  stackP->top = -1;  /* I.e., empty */
}

void StackDestroy(stackT *stackP)
{
  /* Get rid of array. */
  free(stackP->contents);

  stackP->contents = NULL;
  stackP->size = 0;
  stackP->top = -1;  /* I.e., empty */
}

int StackIsEmpty(stackT *stackP)
{
  return stackP->top < 0;
}

int StackIsFull(stackT *stackP)
{
  return stackP->top >= stackP->size - 1;
}

void StackPush(stackT *stackP, stackElementT element)
{
  stackP->size++;
  if (StackIsEmpty(stackP))
  {
  	printf("empty\n");
    stackP->contents = (stackElementT *) malloc(sizeof(stackElementT) * stackP->size);
  }
  if (StackIsFull(stackP)) {
  	printf("Full\n");
    stackP->contents = (stackElementT *) realloc(stackP->contents, sizeof(stackElementT) * stackP->size);
  }

  /* Put information in array; update top. */
  printf("push antes\n");
  stackP->contents[++stackP->top] = element;
  printf("push depois\n");
}

stackElementT StackPop(stackT *stackP)
{
  if (StackIsEmpty(stackP)) {
    fprintf(stderr, "Can't pop element from stack: stack is empty.\n");
    exit(1);  /* Exit, returning error code. */
  }

  return stackP->contents[stackP->top--];
}

int cityInTour(struct tour t, int cidade){
	int i;
	for (i = 0; i < t.num_cities; i++)
	{
		if (t.cities[i].id == cidade)
		{
			return 1;
		}
	}
	return 0;
}

int checkTour(struct tour t, int cidade, int cidadeInicial, int num_cidades)
{
	if (t.num_cities == num_cidades && cidade == cidadeInicial)
	{
		return 1;
	}
	else if (!cityInTour(t, cidade))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void printTour(struct tour t)
{
	int i;
	printf("Rota ");
	for (i = 0; i < t.num_cities; i++)
	{
		printf("%d ", t.cities[i].id);
	}
	printf("\n");
	printf("Custo = %d\n", t.cost);
}

void addCity(struct tour *t, struct city cidade, int num_cidades)
{
	printf("addCity\n");
	int i;
	struct city cidadeOrigem = t->cities[t->num_cities - 1];
	for (i = 0; i < num_cidades; i++)
	{
		if (cidadeOrigem.ligacoes[i].origemID == cidadeOrigem.id && cidadeOrigem.ligacoes[i].destinoID == cidade.id)
		{
			t->cost += cidadeOrigem.ligacoes[i].custo;
		}
	}
	t->cities[t->num_cities++] = cidade;
}

void removeCity(struct tour *t, struct city cidade, int num_cidades)
{
	printf("removeCity\n");
	//printTour(*t);
	//printf("%d ", t->num_cities);
	t->num_cities--;
	int i;
	struct city cidadeOrigem = t->cities[t->num_cities - 1];
	for (i = 0; i < num_cidades; i++)
	{
		if (cidadeOrigem.ligacoes[i].origemID == cidadeOrigem.id && cidadeOrigem.ligacoes[i].destinoID == cidade.id)
		{
			t->cost -= cidadeOrigem.ligacoes[i].custo;
		}
	}
}

void printVector(struct city *cities, int num_cidades){
	int i;
	for (i = 0; i < num_cidades; i++)
	{
		printf("%d ", cities[i].id);
	}
	printf("\n");
}

void printStack(stackT stack){
	while(!StackIsEmpty(&stack)){
		struct tour t = StackPop(&stack);
		printVector(t.cities, t.num_cities);
	}
}

int main(int argc, char *argv[]){
	int i, j;
	int num_cidades;
	int cidadeInicial;
	struct city *cidades;
	struct tour t;
	struct tour bestTour;
	stackT stack; 
	stackT stackTemp;

	StackInit(&stack);
	bestTour.cost = 99999;

	scanf("%d", &num_cidades);
	scanf("%d", &cidadeInicial);

	cidades = (struct city*) malloc(num_cidades*(sizeof(struct city)));


	for (i = 0; i < num_cidades; i++)
	{
		cidades[i].id = i;
		cidades[i].ligacoes = (struct rota*) malloc((num_cidades)*sizeof(struct rota));
		for (j = 0; j < num_cidades; j++)
		{
			//printf("%d %d\n", i, j);
			scanf("%d", &(cidades[i].ligacoes[j].custo));
			cidades[i].ligacoes[j].origemID = i;
			cidades[i].ligacoes[j].destinoID = j;
		}
	}

	for (i = 0; i < num_cidades; i++)
	{
		printf("Cidade %d\n", cidades[i].id);
		for (j = 0; j < num_cidades; j++)
		{
			printf("Rota: origem %d, destino %d, custo %d\n", cidades[i].ligacoes[j].origemID, cidades[i].ligacoes[j].destinoID, cidades[i].ligacoes[j].custo);
		}
	}

	//t.cities = (struct city*) malloc((num_cidades+1)*sizeof(struct city));
	t.cost = 0;
	t.num_cities = 1;
	t.cities[0] = cidades[0];

	StackPush(&stack, t);
	while(!StackIsEmpty(&stack)){
		t = StackPop(&stack);
		if (t.num_cities == num_cidades + 1)
		{
			if (t.cost < bestTour.cost)
			{
				bestTour = t;
			}
		}
		else{

			for (i = num_cidades - 1; i >= 0; i--)
			{
				if (checkTour(t, i, cidadeInicial, num_cidades))
				{
					addCity(&t, cidades[i], num_cidades);
					printf("push stack antes\n");
					StackPush(&stack, t);
					printf("push stack depois\n");
					removeCity(&t, cidades[i], num_cidades);
				}
			}
		}
	}

	printTour(bestTour);

	//free(bestTour.cities);
	//if (bestTour.cities != t.cities) free(t.cities);
	free(cidades);
	StackDestroy(&stack);

	return 0;
}
