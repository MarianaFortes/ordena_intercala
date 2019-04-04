//Código escrito em conjunto por Leonardo Izaú (@Vladek96) e Mariana Fortes (@MarianaFortes)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Endereco Endereco;

struct _Endereco
{
	char logradouro[72];
	char bairro[72];
	char cidade[72];
	char uf[72];
	char sigla[2];
	char cep[8];
	char lixo[2];
};

int compara(const void *e1, const void *e2)
{
	return strncmp(((Endereco*)e1)->cep,((Endereco*)e2)->cep,8);
}

void le_ordena(FILE *f, long tam, char nome_arq[])
{
	FILE *saida;
	Endereco* e = (Endereco*) malloc(tam*sizeof(Endereco));
	if(fread(e,sizeof(Endereco),tam,f) == tam)
	{
	    printf("Lido = OK\n");
	}
	qsort(e,tam,sizeof(Endereco),compara);
	printf("Ordenado = OK\n");
	saida = fopen(nome_arq,"w");
	fwrite(e,sizeof(Endereco),tam,saida);
	fclose(saida);
	printf("Escrito = OK\n\n");
	free(e);
}

void divide(FILE *f)
{
	long posicao, qtd, total = 0;
	int i, j;
	char nomeArq[13];
	fseek(f,0,SEEK_END);
	posicao = ftell(f);
	rewind(f);
	qtd = posicao/sizeof(Endereco);
	long oit[8] = {qtd/8};
	total += oit[0];
	for(i = 1; i < 8; i++)
	{
		oit[i] = ((qtd - total)/(8-i));
		total += oit[i];
	}
	for(j = 0; j < 8; j++)
    	{
		sprintf(nomeArq, "cep_%d.dat", j);
		le_ordena(f, oit[j], nomeArq);
	}
}

void intercala(FILE *a, FILE *b, FILE *saida)
{
	Endereco ea, eb;
	fread(&ea,sizeof(Endereco),1,a);
	fread(&eb,sizeof(Endereco),1,b);
	while(!feof(a) && !feof(b))
	{
		if(compara(&ea,&eb)<0)
        	{
			fwrite(&ea,sizeof(Endereco),1,saida);
			fread(&ea,sizeof(Endereco),1,a);
        	}
        	else
		{
			fwrite(&eb,sizeof(Endereco),1,saida);
			fread(&eb,sizeof(Endereco),1,b);
		}
	}
	while(!feof(a))
	{
		fwrite(&ea,sizeof(Endereco),1,saida);
		fread(&ea,sizeof(Endereco),1,a);
	}
	while(!feof(b))
	{
		fwrite(&eb,sizeof(Endereco),1,saida);
        	fread(&eb,sizeof(Endereco),1,b);
	}
	printf("Intercalado = OK\n");
	fclose(a);
	fclose(b);
    	fclose(saida);
}

void processa_intercala(FILE *f)
{
	FILE *a, *b, *saida;
	int i, cont = 8;
	char nomeArq [13];
	for(i = 0; i < 14; i += 2)
	{
		sprintf(nomeArq, "cep%d.dat", i);
		a = fopen(nomeArq, "r");
		sprintf(nomeArq, "cep%d.dat", i+1);
		b = fopen(nomeArq, "r");
		if(cont < 14)
		{
			sprintf(nomeArq, "cep%d.dat", cont);
			saida = fopen(nomeArq, "w");
			intercala(a,b,saida);
		}
		if(cont == 14)
		{
			saida = fopen("cep_ordenado.dat", "w");
			intercala(a,b,saida);
		}
		cont++;
	}
}

void deletaPedacos()
{
	FILE *f;
	int m;
	char nomeArq [13];
	for(m = 0; m < 14; m++)
	{
		sprintf(nomeArq, "cep_%d.dat", m);
		f = fopen(nomeArq, "r");
		if(!f)
			fprintf(stderr, "\nArquivo nao foi aberto");
		else
		{
			fclose(f);
			int r = remove(nomeArq);
			if(r == 0)
				printf("\n%s removido = OK", nomeArq);
			else
				fprintf(stderr, "Arquivo nao foi removido\n");
		}
	}
}

int main(int argc, char**argv)
{
	FILE *f;
	f = fopen("cep.dat","r");
	divide(f);
	processa_intercala(f);
	fclose(f);
	deletaPedacos();
	printf("\n\nArquivo cep.dat ordenado = OK");
	return 0;
}
