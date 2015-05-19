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

int main(int argc, char *argv[]){
	int i, j;
	int num_cidades;
	int cidadeInicial;
	struct city *cidades;

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

	for (i = 0; i < sizeof(cidades); i++)
	{
		printf("Cidade %d\n", cidades[i].id);
		for (j = 0; j < num_cidades; j++)
		{
			printf("Rota: origem %d, destino %d, custo %d\n", cidades[i].ligacoes[j].origemID, cidades[i].ligacoes[j].destinoID, cidades[i].ligacoes[j].custo);
		}
	}



	return 0;
}
