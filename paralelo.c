#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "timer.h"

struct tour
{
	int *cities;
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
  	stackP->size++;
    stackP->contents = (stackElementT *) realloc(stackP->contents, sizeof(stackElementT) * stackP->size);
  }


  /* Put information in array; update top. */
  stackP->contents[++stackP->top] = element;
}

stackElementT StackPop(stackT *stackP)
{
  if (StackIsEmpty(stackP)) {
    fprintf(stderr, "Can't pop element from stack: stack is empty.\n");
    exit(1);  /* Exit, returning error code. */
  }

  return stackP->contents[stackP->top--];
}

int cityInTour(struct tour *t, int cidade){
	int i;
	for (i = 0; i < t->num_cities; i++)
	{
		if (t->cities[i] == cidade)
		{
			return 1;
		}
	}
	return 0;
}

int checkTour(struct tour *t, int cidade, int cidadeInicial, int num_cidades)
{
	if (t->num_cities == num_cidades && cidade == cidadeInicial)
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

void printTour(struct tour *t)
{
	int i;
	printf("Rota ");
	for (i = 0; i < t->num_cities; i++)
	{
		printf("%d ", t->cities[i]);
	}
	printf("\n");
	printf("Custo = %d\n", t->cost);
}

void printVector(int *cities, int num_cidades){
	int i;
	for (i = 0; i < num_cidades; i++)
	{
		printf("%d ", cities[i]);
	}
	printf("\n");
}

void printStack(stackT stack){
	while(!StackIsEmpty(&stack)){
		struct tour t = StackPop(&stack);
		printVector(t.cities, t.num_cities);
	}
}

void addCity(struct tour *t, int cidade, int num_cidades, int **rotas)
{
	int cidadeOrigem = t->cities[t->num_cities - 1];
	t->cost += rotas[cidadeOrigem][cidade];
	t->cities[t->num_cities++] = cidade;
}

void removeCity(struct tour *t, int cidade, int num_cidades, int **rotas)
{
	t->num_cities--;
	int cidadeOrigem = t->cities[t->num_cities - 1];
	t->cost -= rotas[cidadeOrigem][cidade];
}

struct tour copyTour(struct tour *t, int num_cidades){
	struct tour k;
	int i;
	k.num_cities = t->num_cities;
	k.cost = t->cost;
	k.cities = (int*) malloc((num_cidades+1)*sizeof(int));
	for (i = 0; i < k.num_cities; ++i)
	{
		k.cities[i] = t->cities[i];
	}
	return k;
}

void initTour(struct tour *t, int cidadeInicial, int num_cidades)
{
	t->cities = (int*) malloc((num_cidades+1)*sizeof(int));
	t->cost = 0;
	t->num_cities = 1;
	t->cities[0] = cidadeInicial;
}

int checkBestTour(struct tour *bestTour, struct tour *t)
{
	if (t->cost < bestTour->cost || bestTour->cities == NULL)
	{
		return 1;
	}
	return 0;
}

void updateTour(struct tour *bestTour, struct tour *t)
{
	if (checkBestTour(bestTour, t))
	{
		*bestTour = *t;
	}
}

struct tour calculateMinimumCost(struct tour *tourInicial, int num_cidades, int cidadeInicial, int **rotas)
{
	int i, stop;
	struct tour t;
	struct tour bestTour;
	bestTour.cost = 99999;
	bestTour.cities = NULL;
	stackT stack;
	StackInit(&stack);

	StackPush(&stack, *tourInicial);
	while(!StackIsEmpty(&stack)){
		t = StackPop(&stack);
		if (t.num_cities == num_cidades)
		{
			addCity(&t, cidadeInicial, num_cidades, rotas);
			updateTour(&bestTour, &t);
		}
		else{

			for (i = num_cidades - 1; i >= 0; i--)
			{
				if (checkTour(&t, i, cidadeInicial, num_cidades))
				{
					addCity(&t, i, num_cidades, rotas);
				#	pragma omp critical
					stop = checkBestTour(&bestTour, &t);
					
					if (stop)
					{
						struct tour k = copyTour(&t, num_cidades);
						StackPush(&stack, k);
					}
					removeCity(&t, i, num_cidades, rotas);
				}
			}
		}
		if (bestTour.cities != t.cities) free(t.cities);
	}
	StackDestroy(&stack);
	return bestTour;

}

int main(int argc, char *argv[]){
	int i, j, thread_count;
	int num_cidades;
	int cidadeInicial;
	int **matriz;
	struct tour t;
	struct tour bestTour;
	double start, finish;
	struct tour *tourIniciais;


	bestTour.cost = 99999;
	bestTour.cities = NULL;

	thread_count = strtol(argv[1], NULL, 10);

	scanf("%d", &num_cidades);
	scanf("%d", &cidadeInicial);

	//cidades = (int*) malloc(num_cidades*(sizeof(int)));
	tourIniciais = (struct tour*) malloc((num_cidades-1)*(sizeof(struct tour)));
	matriz = (int **)malloc(num_cidades*sizeof(int*));

	for (i = 0; i < num_cidades; i++)
	{
		matriz[i] = (int *)malloc(num_cidades*sizeof(int));
		for (j = 0; j < num_cidades; j++)
		{
			scanf("%d", &matriz[i][j]);
		}
	}

	initTour(&t, cidadeInicial, num_cidades);

	GET_TIME(start);
	j = 0;
	for (i = 0; i < num_cidades; i++)
	{
		if (i != cidadeInicial)
		{
			initTour(&tourIniciais[j], cidadeInicial, num_cidades);
			addCity(&tourIniciais[j], i, num_cidades, matriz);
			j++;
		}
	}
#	pragma omp parallel for num_threads(thread_count)
	for (i = 0; i < num_cidades-1; i++)
	{
		//printf("%d\n", omp_get_thread_num());
		struct tour localBestTour = calculateMinimumCost(&tourIniciais[i], num_cidades, cidadeInicial, matriz);
#		pragma omp critical
		updateTour(&bestTour, &localBestTour);
	}	

	
	GET_TIME(finish);

	printTour(&bestTour);
	printf("tempo=%f\n", finish-start);

	free(bestTour.cities);
	free(tourIniciais);
	free(matriz);

	return 0;
}
