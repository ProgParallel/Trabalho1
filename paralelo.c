#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "timer.h"

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
	struct city *cities;
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
  stackP->contents = (stackElementT *) malloc(sizeof(stackElementT) * 1);
  /* Allocate a new array to hold the contents. */

  stackP->size = 1;
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
  if (StackIsFull(stackP)) {
  	//printf("Full\n");
  	stackP->size++;
    stackP->contents = (stackElementT *) realloc(stackP->contents, sizeof(stackElementT) * stackP->size);
  }


  /* Put information in array; update top. */
  //printf("push antes\n");
  stackP->contents[++stackP->top] = element;
  //printf("push depois\n");
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

int getCost(struct city cidade, int destinoID, int num_cidades)
{
	int i;
	for (i = 0; i < num_cidades; i++)
	{
		if (cidade.ligacoes[i].origemID == cidade.id && cidade.ligacoes[i].destinoID == destinoID)
		{
			return cidade.ligacoes[i].custo;
		}
	}
	return 0;
}

void addCity(struct tour *t, struct city cidade, int num_cidades)
{
	//printf("addCity\n");
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
	//printf("removeCity\n");
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

void copyTour(struct tour *k, struct tour t, int num_cidades){
	k->cities = (struct city*) malloc((num_cidades+1)*sizeof(struct city));
	k->num_cities = t.num_cities;
	k->cost = t.cost;
	memcpy(k->cities, t.cities, (num_cidades+1)*sizeof(struct city));
}

void initTour(struct tour *t, struct city cidadeInicial, int num_cidades)
{
	t->cities = (struct city*) malloc((num_cidades+1)*sizeof(struct city));
	t->cost = 0;
	t->num_cities = 1;
	t->cities[0] = cidadeInicial;
}

void calculateMinimumCost(struct tour tourInicial, struct tour *bestTour, int num_cidades, int cidadeInicial, struct city *cidades)
{
	int i;
	struct tour t;
	stackT stack;
	StackInit(&stack);

	StackPush(&stack, tourInicial);
	while(!StackIsEmpty(&stack)){
		t = StackPop(&stack);
		if (t.num_cities == num_cidades + 1)
		{
		# 	pragma omp critical
			if (t.cost < bestTour->cost)
			{
				*bestTour = t;
			}
		}
		else{

			for (i = num_cidades - 1; i >= 0; i--)
			{
				struct tour k;
				if (checkTour(t, i, cidadeInicial, num_cidades))
				{
					copyTour(&k, t, num_cidades);
					addCity(&k, cidades[i], num_cidades);
					StackPush(&stack, k);
					removeCity(&k, cidades[i], num_cidades);
				}
			}
		}
	}
	StackDestroy(&stack);
	if (bestTour->cities != t.cities) free(t.cities);

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
	int i, j, thread_count;
	int num_cidades;
	int cidadeInicial;
	struct city *cidades;
	struct tour t;
	struct tour bestTour;
	stackT stack; 
	stackT stackTemp;
	double start, finish;
	struct tour *tourIniciais;


	bestTour.cost = 99999;

	thread_count = strtol(argv[1], NULL, 10);

	scanf("%d", &num_cidades);
	scanf("%d", &cidadeInicial);

	cidades = (struct city*) malloc(num_cidades*(sizeof(struct city)));
	tourIniciais = (struct tour*) malloc((num_cidades-1)*(sizeof(struct tour)));


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

	// for (i = 0; i < num_cidades; i++)
	// {
	// 	printf("Cidade %d\n", cidades[i].id);
	// 	for (j = 0; j < num_cidades; j++)
	// 	{
	// 		printf("Rota: origem %d, destino %d, custo %d\n", cidades[i].ligacoes[j].origemID, cidades[i].ligacoes[j].destinoID, cidades[i].ligacoes[j].custo);
	// 	}
	// }

	initTour(&t, cidades[cidadeInicial], num_cidades);

	GET_TIME(start);
	j = 0;
	for (i = 0; i < num_cidades; i++)
	{
		if (i != cidadeInicial)
		{
			initTour(&tourIniciais[j], cidades[cidadeInicial], num_cidades);
			addCity(&tourIniciais[j], cidades[i], num_cidades);
			j++;
		}
	}
#	pragma omp parallel for num_threads(thread_count)
	for (i = 0; i < num_cidades-1; i++)
	{
		calculateMinimumCost(tourIniciais[i], &bestTour, num_cidades, cidadeInicial, cidades);
	}	

	
	GET_TIME(finish);

	printf("finish\n");
	printTour(bestTour);
	printf("tempo=%f\n", finish-start);

	free(bestTour.cities);
	//if (bestTour.cities != t.cities) free(t.cities);
	free(cidades);

	return 0;
}
