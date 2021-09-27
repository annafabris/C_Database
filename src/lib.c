#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*utili per define i vari tipi di query*/
#define CREATE 1
#define INSERT 2
#define SELECT 3
#define NONE 4
/*utili per define i vari tipi di select*/
#define NOFILTER 0
#define ORDER 1
#define GROUP 2
#define WHERE 3

/*Funzioni per gestire la struttura dati*/
/*Questa funzione e' una variante di strcpy*/
char* StringCopy(char* string)
{
	int n, i = 0;
	char* output;

	//Se la stringa passata non esiste termina
	if (string == NULL)
		return NULL;

	//Calcola il numero di caratteri della stringa
	n = strlen(string);

	//Alloca una nuova stringa della stessa dimensione di quella in input
	output = (char *)malloc((n * sizeof(char)) + 1);
	if (output == NULL)
		return NULL;

	//Copia carattere per carattere la stringa in input in quella in output
	while (string[i] != '\0')
	{
		output[i] = string[i];
		i++;
	}
	output[i] = '\0';

	return output;
}

/*Carica nella nostra struttura dati le varie righe*/
char** LoadRow(FILE* fpt, int numero_colonne)
{
	char** riga;
	char row[100];
	char *sptr;
	char *str;
	char c;
	int i;

	//Allocazione struttura contenente le colonne
	riga = (char **)malloc((numero_colonne * sizeof(char*)));
	if (riga == NULL)
		return NULL;

	//Acquisisce una riga della tabella e la salva nella stringa "row"
	i = 0;
	while ((c = fgetc(fpt)) != '\n')
	{
		row[i] = c;
		i++;

		//Nel caso in cui durante la lettura del file si giungesse al termine di quest'ultimo si interrompe il ciclo per evitare un loop infinito
		if (feof(fpt))
			break;
	}
	//Nel caso in cui si giungesse al termine del file si esce dalla funzione restituendo un puntatore a NULL
	if (feof(fpt))
		return NULL;

	row[i - 1] = '\0';

	//Rimuove la parte "ROW " dalla stringa contenente la riga lasciandovi solo i valori delle colonne separati da virgole
	str = SplitString(row, " ", &sptr);
	free(str);
	str = SplitString(sptr, ";", &sptr);
	sptr = StringCopy(str);
	free(str);

	//Suddivide i valori delle colonne nell'array
	i = 0;
	str = SplitString(sptr, ",", &sptr);
	while (str != NULL)
	{
		riga[i] = StringCopy(str);
		free(str);
		i++;
		str = SplitString(sptr, ",", &sptr);
	}

	return riga;
}

/*Alloca una tabella che contiene tutte le informazioni contenute nella tabella (memorizzata nel file)*/
char*** LoadTable(char* nome_file, int numero_colonne, int *numero_righe)
{

	char ***rows;
	char c;
	int i = 0, n = 0, k = 0;
	FILE *fpt;

	//Allocazione struttura dati per contenere le righe
	rows = (char ***)malloc((n * sizeof(char**)));
	if (rows == NULL)
		return NULL;

	//Apertura del file
	fpt = fopen(nome_file, "r");
	if (fpt == NULL)
		return 0;

	//Legge la prima riga per togliere l'intestazione
	while ((c = fgetc(fpt)) != '\n');

	//Lettura del file riga per riga
	while (!feof(fpt))
	{
		//Se il numero di righe lette raggiunge la dimensione dell'array questo viene ampliato
		if (i == n)
		{
			n++;
			rows = (char ***)realloc(rows, n * sizeof(char**));
			if (rows == NULL)
				return NULL;
		}

		//Salva la riga nella i-esima posizione dell'array
		rows[i] = LoadRow(fpt, numero_colonne);
		i++;
		c = fgetc(fpt);
	}
	*numero_righe = i;
	return rows;
}

/*Libera la memoria utilizzata dalla tabella*/
void FreeTable(char*** rows, int numero_colonne, int numero_righe)
{
	int i, j;

	for (i = 0; i <numero_righe; i++)
	{
		for (j = 0; j < numero_colonne; j++)
		{
			free(rows[i][j]);
		}
		free(rows[i]);
	}
	free(rows);
}

/*Funzione per l'ordinamento*/
/*Inverte i due valori*/
void Swap(char ***a, char ***b)
{
	char **c;
	c = *b;
	*b = *a;
	*a = c;
}

/*funzione richiamata ricorsivamente da HeapSort*/
void Heapify(char*** table, int numero_righe, int i, int numero_colonna)
{
	int largest = i;		// l'elemento più grande = radice
	int l = 2 * i + 1;		// left = 2*i + 1
	int r = 2 * i + 2;		// right = 2*i + 2
	unsigned int j = 0;
	bool isANumber = true;

	//Controllo se il valore è un numero o una stringa
	while (j < strlen(table[i][numero_colonna]))
	{
		if (!isdigit(table[i][numero_colonna][j]))
		{
			isANumber = false;
			break;
		}
		j++;
	}

	// Se il figlio di sinistra è maggiore della radice
	if (isANumber == true)
	{
		if (l < numero_righe && atoi(table[l][numero_colonna]) > atoi(table[largest][numero_colonna]))
			largest = l;
	}
	else
	{
		if (l < numero_righe && (strcmp(table[l][numero_colonna], table[largest][numero_colonna]))>0)
			largest = l;
	}

	// Se il figlio di destra è maggiore dell'elemento più grande
	if (isANumber == true)
	{
		if (r < numero_righe && atoi(table[r][numero_colonna]) > atoi(table[largest][numero_colonna]))
			largest = r;
	}
	else
	{
		if (r < numero_righe && (strcmp(table[r][numero_colonna], table[largest][numero_colonna]))>0)
			largest = r;
	}

	// Se l'elemento più grande non è la radice
	if (largest != i)
	{
		Swap(&table[i], &table[largest]);

		// Chiama ricorsivamente Heapify
		Heapify(table, numero_righe, largest, numero_colonna);
	}


}

/*funzione principale che ordina la tabella*/
void HeapSort(char ***table, int numero_righe, int numero_colonna)
{
	int i;

	// costruisce l'heap
	for (i = numero_righe / 2 - 1; i >= 0; i--)
		Heapify(table, numero_righe, i, numero_colonna);

	// estrae un elemento alla volta dall'heap
	for (i = numero_righe - 1; i > 0; i--)
	{
		//muove la radice alla fine
		Swap(&table[0], &table[i]);

		Heapify(table, i, 0, numero_colonna);
	}
}


/*funzioni utili alle funzioni principali*/
/*Controlla se esiste già un file con tale nome */
bool ControlloEsistenzaFile(char * nome_file)
{
	FILE *fpt;
	fpt = fopen(nome_file, "r");      //aprendo il file in lettura, se restituisce NULL non esiste
	if (fpt == NULL)   //il file non esiste quindi lo creo
		return false;
	fclose(fpt);
	return true;
}

/*Agginge ".txt" a nome_tabella e restituisce il nome del file*/
char* Appendtxt(char* nome_tabella)
{
	char *nomeFile;
	nomeFile = (char *)malloc((5 + strlen(nome_tabella)) * sizeof(char));
	if (nomeFile == NULL)
		return NULL;
	strcpy(nomeFile, nome_tabella);
	strcat(nomeFile, ".txt");			//aggiungo .txt al nome della tabella per creare il file
	return nomeFile;
}

/*Controlla che siano presenti tutte le parentesi*/
char* ControlloParentesi(char* nomi_colonne)
{
	int lunghezza,i;
	char* colonneSenzaParentesi;
	lunghezza = strlen(nomi_colonne);
	if (nomi_colonne[0] != '(')
		return false;
	for (i = 1; i<lunghezza; i++)
	{
		if (nomi_colonne[lunghezza - 1] != ')')
			return NULL;
	}
	colonneSenzaParentesi = SplitString(nomi_colonne, "(", &nomi_colonne);
	free(colonneSenzaParentesi);
	colonneSenzaParentesi = SplitString(nomi_colonne, ")", &nomi_colonne);;
	return colonneSenzaParentesi;
}

/*Controlla se le colonne inserite nella query sono le stesse di quelle all'interno del file*/
bool ControllaColonne(char* nome_tabella, char* colonne_inserite)
{
	FILE *fpt;
	int i;
	char *nomeFile, *nomeColonne, *temp;

	nomeFile = Appendtxt(nome_tabella);
	if (nomeFile == NULL)
		return false;

	//Apertura dei file di testo e relativo controllo di riuscita
	fpt = fopen(nomeFile, "r");
	if (fpt == NULL)
		return false;

	//Alloco dinamicamente nomeColonne
	nomeColonne = (char*)malloc(strlen(colonne_inserite) + 1);
	if (nomeColonne == NULL)
		return false;

	for (i = 0; i < 4; i++)   /*Ricerca il nome delle colonne all'interno del file*/
	{
		fscanf(fpt, "%s", nomeColonne);
	}
	
	temp = StringCopy(nomeColonne);
	nomeColonne = SplitString(temp, ";", &temp);
	free(nomeFile);
	fclose(fpt);

	if (!strcmp(nomeColonne, colonne_inserite))   //compara le colonne nel file con quelle inserite
	{
		free(nomeColonne);
		return true;
	}
	else
	{
		free(nomeColonne);
		return false;
	}	
}

/*Conta il numero di virgole all'interno della stringa colonne così da stabilire il numero di colonne*/
int NumeroColonne(char* nome_colonne)
{
	int i, count = 0, lunghezza;
	lunghezza = strlen(nome_colonne);
	for (i = 0; i < lunghezza; i++)
	{
		if (nome_colonne[i] == ',' && i != 0)
		{ 
			if (nome_colonne[i - 1] == ',')
				return 0;
			count++;
		}
	}
	return count + 1;
}

/*Trova la posizione delle colonne selizionate dalla query*/
bool TrovaPosizioneColonne(int ** array_posizioni, char ** colonne_selezionate, char** colonne_tabella, int n_colonne_selezionate, int n_colonne)
{
	int * arrayPosizioni;
	int i, j, k = 0;

	/*alloco dinamicamente l'array di posizioni*/
	arrayPosizioni = (int*)malloc(sizeof(int)*n_colonne_selezionate);
	if (arrayPosizioni == NULL)
		return false;

	/*Cerco le posizioni e me le salvo*/
	for (i = 0; i < n_colonne_selezionate; i++)
	{
		arrayPosizioni[i] = -1; /*Inizializzo il vettore a -1*/
		for (j = 0; j < n_colonne; j++)
		{
			if (!strcmp(colonne_selezionate[i], colonne_tabella[j]))
			{
				arrayPosizioni[i] = j;
				break;
			}
		}
	}
	*array_posizioni = arrayPosizioni;

	for (i = 0; i < n_colonne_selezionate; i++)
	{
		if (arrayPosizioni[i] == -1)
			return false;
	}
	return true;
}

/*Questa funzione rilegge dal file le colonne, le salva in un array e le conta*/
char** TrovaNomeColonne(char* nome_file, int *numero_colonne)
{
	char **colonne, *firstRow, *nomeColonne, *temp, c;
	FILE *fpt;
	int i = 0;

	//Apertura del file
	fpt = fopen(nome_file, "r");
	if (fpt == NULL)
		return NULL;

	firstRow = (char*)malloc(100);
	if (firstRow == NULL)
		return false;

	//Legge la prima riga per analizzare il nome delle colonne
	while ((c = fgetc(fpt)) != '\n')
	{
		firstRow[i] = c;
		i++;
	}
	firstRow[i + 1] = '\0';
	temp = StringCopy(firstRow);
	nomeColonne = SplitString(firstRow, " ", &temp);

	/*toglie la prima parte della stringa e memorizza il nome delle colonne*/
	for (i = 0; i < 3; i++)
	{
		free(nomeColonne);
		nomeColonne = SplitString(temp, " ;", &temp);
	}
	*numero_colonne = NumeroColonne(nomeColonne);

	//Allocazione array di nome colonne
	colonne = (char **)malloc((*numero_colonne * sizeof(char*)));
	if (colonne == NULL)
		return NULL;

	/*memorizzo dentro l'array i nomi delle colonne*/
	temp = SplitString(nomeColonne, ",", &nomeColonne);
	colonne[0] = StringCopy(temp);
	for (i = 1; i < *numero_colonne; i++)
	{
		free(temp);
		temp = SplitString(nomeColonne, ",", &nomeColonne);
		colonne[i] = StringCopy(temp);
	}
	free(firstRow);
	free(nomeColonne);
	free(temp);
	return colonne;
}


/*Funzioni principali per la gestione di tutte le query*/
/*Divide una sringa in due parti: una prima del primo delimitatore e l'altra dopo il primo delimitatore*/
char * SplitString(char * str, const char * delimiters, char ** sptr)
{
	char delimiter[5];
	char* output;
	int i, k, n;

	if (str == NULL)
		return NULL;

	n = strlen(delimiters);
	/*ritorna la stringa stessa se non sono stati inseriti dei delimitatori*/
	if (n == 0)
	{
		*sptr = str;
		return *sptr;
	}
	/*ritorna errore se  sono stati inseriti più di 5 delimitatori*/
	if (n > 5)
	{
		printf("\nERROR: The number of delimiters excede the prefixed amount\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < 5; i++)
		delimiter[i] = '\0';

	/*inserimento del delimitatori inseriti*/
	for (i = 0; i < n; i++)
	{
		if (delimiters[i] == '\0')
			break;
		delimiter[i] = delimiters[i];
	}

	/*Caso primo carrattere della stringa è un carattere delimitatore*/
	if (str[0] == delimiter[0] || str[0] == delimiter[1] || str[0] == delimiter[2] || str[0] == delimiter[3] || str[0] == delimiter[4])
	{
		*sptr = &(str[1]);
		return NULL;
	}

	/*conteggio della lunghezza della stringa prima del primo delimitatore*/
	i = 0;
	do
	{
		i++;
	} while (str[i] != delimiter[0] && str[i] != delimiter[1] && str[i] != delimiter[2] && str[i] != delimiter[3] && str[i] != delimiter[4] && str[i] != '\0');


	output = (char *)malloc((i * sizeof(char)) + 1);
	if (output == NULL)
		return NULL;

	/*memorizazzione della parte della stringa prima del primo delimitatore in output*/
	for (k = 0; k < i; k++)
		output[k] = str[k];

	output[k] = '\0';

	/*sptr viene fatto puntare alla parte della stringa dopo il primo delimitatore*/
	if (str[i] == '\0')
		*sptr = NULL;
	else
		*sptr = &str[i + 1];

	return output;
}

/*Questa funzione restituisce 1 se si tratta dell'istruzione per la creazione di una nuova tabella, 2 se di tratta di un inserimento
e 3 se si tratta dell'istruzione SELECT*/
int TipoDiQuery(char** query_inserita)
{
	char *token;
	int i, lung;

	lung=strlen(*query_inserita);
	/*controllo che la stringa non finisca con \n */
	for (i = 0; i==lung+1; i++)
	{
		if (*(query_inserita[i]) == '\n')
		{
			*(query_inserita[i]) = '\0';
			break;
		}
	}

	token = SplitString(*query_inserita, " ", query_inserita);
	
	if (token == NULL)
		return NONE;

	if (!strcmp(token, "SELECT"))
	{
		free(token);
		return SELECT;
	}

	if (!strcmp(token, "CREATE"))
	{
		free(token);
		token = SplitString(*query_inserita, " ", query_inserita);
		if (token == NULL)
		{	
			free(token);
			return NONE;
		}
		if (!strcmp(token, "TABLE"))
		{
			free(token);
			return CREATE;
		}
		else
		{
			free(token);
			return NONE;
		}
	}
	if (!strcmp(token, "INSERT"))
	{
		token = SplitString(*query_inserita, " ", query_inserita);
		if (token == NULL)
		{
			free(token);
			return NONE;
		}
		if (!strcmp(token, "INTO"))
		{
			free(token);
			return INSERT;
		}
		else
		{ 
			free(token);
			return NONE;
		}
	}
	free(token);
	return NONE;
}

/*Questa funzione cataloga il tipo di filtro utilizzato per la select*/
int TipoSelect(char** query_inserita)
{
	char * tipo;

	/*Se è una select senza filtri ritorno NOFILTER*/
	if (*query_inserita == NULL)
		return NOFILTER;

	tipo = SplitString(*query_inserita, " ", query_inserita);
	/*Se é una order by ritorna ORDER*/

	if (tipo == NULL)
		return NONE;

	if (!strcmp(tipo, "ORDER"))
	{
		free(tipo);
		tipo = SplitString(*query_inserita, " ", query_inserita);
		if (tipo == NULL)
		{
			free(tipo);
			return NONE;
		}
		if (!strcmp(tipo, "BY"))
		{
			free(tipo);
			return ORDER;
		}
		else
		{
			free(tipo);
			return NONE;
		}
	}

	/*Se é una group by ritorna GROUP*/
	if (!strcmp(tipo, "GROUP"))
	{
		free(tipo);
		tipo = SplitString(*query_inserita, " ", query_inserita);
		if (!strcmp(tipo, "BY"))
		{
			free(tipo);
			return GROUP;
		}
		else
		{
			free(tipo);
			return NONE;
		}
	}

	/*Se é una group by ritorna WHERE*/
	if (!strcmp(tipo, "WHERE"))
	{
		free(tipo);
		return WHERE;
	}
	free(tipo);
	return NONE;
}

/*Riconosce la query e la esegue*/
bool executeQuery(char* query_inserita)
{
	int numValori, numColonne, tipo, tipoDiSelect, lunghezza,i;
	bool ok;
	char *nomeTabella, *tipiDato, *comando, *valori, *colonna, *ordine, *condizione, *temp, *temp2;
	tipo = TipoDiQuery(&query_inserita);

	/*Riconosce il tipo di query*/
	switch (tipo)
	{
	case CREATE: //Creazione della tabella
		nomeTabella = SplitString(query_inserita, " ()", &query_inserita);
		temp = SplitString(query_inserita, " ", &query_inserita);
		if (temp == NULL)
			return false;
		//controllo se sono presenti le parentesi
		tipiDato = ControlloParentesi(temp);
		if (tipiDato==NULL)
			return false;
		free(temp);
		lunghezza = strlen(tipiDato);
		//controllo che non siano presenti input come ,,
		if (tipiDato[0] == ',')
			return false;
		for (i = 1; i < lunghezza; i++)
		{
			if (tipiDato[i] == ',' && tipiDato[i - 1] == ',')
			    return false;
		}
		return CreateTable(nomeTabella, tipiDato);
	case INSERT: //Inserimento dei valori all'interno della tabella
		nomeTabella = SplitString(query_inserita, " ()", &query_inserita);
		temp = SplitString(query_inserita, " ", &query_inserita);
		if (temp == NULL)
			return false;
		comando = SplitString(query_inserita, " ", &query_inserita);
		if (comando == NULL)
			return false;
		temp2 = SplitString(query_inserita, "", &query_inserita);
		if (temp2 == NULL)
			return false;
		//controllo se sono presenti le parentesi
		tipiDato = ControlloParentesi(temp);
		if (tipiDato==NULL)
			return false;
		free(temp);
		if (!strcmp(comando, "VALUES"))    //controllo se all'interno della query compare la parola VALUES
		{
			//controllo se sono presenti le parentesi
			valori = ControlloParentesi(temp2);
			if (valori == false)
				return false;
			free(comando);
			ok = ControllaColonne(nomeTabella, tipiDato); //controlla se i nomi delle colonne inseriti nella query sono i medesimi di quelli all'interno del file
			if (ok == true)
			{
				numColonne = NumeroColonne(tipiDato);
				numValori = NumeroColonne(valori);
				free(tipiDato);
				if (numColonne == numValori) //controllo se i valori inseriti sono tanti quanti quelli da inserire
					return InsertIntoTable(nomeTabella, valori); //chiamo la funzione
				else
				{
					free(nomeTabella);
					free(valori);
					return false;
				}
			}
			else 
			{
				free(nomeTabella);
				free(valori);
				return false;
			}
		}
		else 
		{
			free(nomeTabella);
			return false;
		}
	case SELECT:  //stampa valori
		tipiDato = SplitString(query_inserita, " ", &query_inserita);
		if (tipiDato == NULL)
			return false;
		lunghezza = strlen(tipiDato);
		//controllo le varie colonne di modo che non siano presenti input errati
		if (tipiDato[0] == ',' || tipiDato[lunghezza-1] == ',')
			return false;
		for (i = 1; i < lunghezza; i++)
		{
			if (tipiDato[i] == ',' && tipiDato[i - 1] == ',')
				return false;
		}
		comando = SplitString(query_inserita, " ", &query_inserita);
		//controllo se il campo è NULL
		if (comando == NULL)
			return false;
		if (!strcmp(comando, "FROM"))
		{
			free(comando); 
			nomeTabella = SplitString(query_inserita, " ", &query_inserita);
			tipoDiSelect = TipoSelect(&query_inserita);
			if (nomeTabella == NULL)
			{
				free(tipiDato);
				return false;
			}
			if (!strcmp(tipiDato, "*"))
			{
				free(tipiDato);
				/*selezione del tipo di select*/
				switch (tipoDiSelect)
				{
				case NOFILTER:  //stampa di tutta la tabella
					return NoFilterSearch1(nomeTabella);
				case ORDER: //stampa di tutta la tabella in ordine a seconda della colonna inserita
					colonna = SplitString(query_inserita, " ", &query_inserita);
					ordine = SplitString(query_inserita, " ", &query_inserita);
					if (ordine == NULL)
						return false;
					else
						return OrderBy(colonna, nomeTabella, ordine);
				case WHERE: //stampa delle righe che soddisfano la condizione inserita
					condizione = SplitString(query_inserita, "", &query_inserita);
					if (condizione == NULL)
						return false;
					return Where(nomeTabella, condizione);
				default:
					return false;
				}
			}
			else
			{
				/*selezione del tipo di select*/
				switch (tipoDiSelect)
				{
				case NOFILTER: //stampa le colonne selezionate
					return NoFilterSearch2(nomeTabella, tipiDato);
				case ORDER: //stampa le colonne selezionate in ordine rispetto ad una determinata colonna
					colonna = SplitString(query_inserita, " ", &query_inserita);
					ordine = SplitString(query_inserita, " ", &query_inserita);
					if (ordine == NULL)
						return false;
					else
						return OrderBy2(colonna, nomeTabella, ordine, tipiDato);
				case GROUP: //stampa quante occorrenze di uno stesso dato sono presenti all'interno della tabella
					colonna = SplitString(query_inserita, " ", &query_inserita);
					if (colonna == NULL)
						return false;
					if (!strcmp(colonna, tipiDato))
					{	
						free(tipiDato);
						return GroupBy(colonna, nomeTabella);
					}
					else
					{
						free(tipiDato);
						return false;
					}
				case WHERE: //stampa le colonne selezionate rispettando una precisa condizione
					condizione = SplitString(query_inserita, "", &query_inserita);
					if (condizione == NULL)
						return false;
					return Where2(nomeTabella, tipiDato, condizione);
				default:
					return false;
				}
			}
		}
		else
		{
			free(comando);
			return false;
		}

	default:
		return false;
	}
}


/*Funzioni principali*/
/*Questa funzione crea la tabella e la dispone su un file*/
bool CreateTable(char* nome_tabella, char* nomi_colonne)
{
	FILE* fpt;
	char *nomeFile;
	bool esiste;

	nomeFile = Appendtxt(nome_tabella);	//aggiungo .txt e controllo se l'operazione e' andata a buon fine							
	if (nomeFile == NULL)
		return false;

	/*Prima di creare il file controllo se esiste già un file con tale nome*/
	esiste = ControlloEsistenzaFile(nomeFile);
	if (esiste == false)
	{
		fpt = fopen(nomeFile, "w");		//aprendo il file in modalità lettura ("w") il file viene creato
		if (fpt == NULL)
			return false;
		fprintf(fpt, "TABLE %s COLUMNS %s;\n", nome_tabella, nomi_colonne); //scrittura riga: TABLE nome_tabella COLUMNS nome_colonna1,nome_colonna2,…;
		fclose(fpt);
		free(nomi_colonne); 
		free(nome_tabella); 
		free(nomeFile);
		return true;
	}
	free(nomi_colonne); //MOD
	free(nome_tabella); //MOD
	free(nomeFile);
	return false;
}

/*Questa funzione inserisce una nuova riga all'interno della tabella*/
bool InsertIntoTable(char* nome_tabella, char* valori_colonne)
{
	FILE* fpt;
	char *nomeFile;

	nomeFile = Appendtxt(nome_tabella);	//aggiungo .txt e controllo se l'operazione e' andata a buon fine
	if (nomeFile == NULL)
		return false;

	fpt = fopen(nomeFile, "a");		//apro il file in modalità 'append'
	if (fpt == NULL)
		return false;

	fprintf(fpt, "ROW %s; \n", valori_colonne);	//scrittura riga: ROW valore1,valore2,valore3,…;
	free(valori_colonne); 
	free(nomeFile);
	free(nome_tabella); 
	fclose(fpt);
	return true;
}

/*Questa funzione gestisce la select * senza filtri*/
bool NoFilterSearch1(char* nome_tabella)
{
	char c;
	char *nomeFile;
	FILE *fpt1, *fpt2;

	//Allocazione stringa contenente il nome del file in cui è salvata la tabella
	nomeFile = Appendtxt(nome_tabella);
	if (nomeFile == NULL)
		return false;

	//Apertura dei file di testo e relativo controllo di riuscita
	fpt1 = fopen(nomeFile, "r");
	if (fpt1 == NULL)
		return false;

	fpt2 = fopen("queries_results.txt", "a");
	if (fpt2 == NULL)
		return false;

	//Stampa nel file "queries_results.txt" la riga di comando impartita
	fprintf(fpt2, "SELECT * FROM %s\n", nome_tabella);

	//Stampa nel file "queries_results.txt" tutti i dati della tabella
	while ((fscanf(fpt1, "%c", &c)) != EOF)
		fprintf(fpt2, "%c", c);

	//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
	fprintf(fpt2, "\n");

	//Chiusura dei file
	fclose(fpt1);
	fclose(fpt2);
	free(nome_tabella); 
	free(nomeFile);
	return true;
}

/*Questa funzione gestisce la select con colonne selezionate senza filtri*/
bool NoFilterSearch2(char* nome_tabella, char* nomi_colonne_selezionate)
{
	int i, nColonneSelezionate, nColonne, *posizioni;
	char c;
	char row[100];
	bool corretto;
	char *str, **colonne, *colonneSelezionate[20], *colonneDati[20], *nomeFile, *temp, *sptr;
	FILE *fpt1, *fpt2;

	//Allocazione stringa contenente il nome del file in cui è salvata la tabella
	nomeFile = Appendtxt(nome_tabella);
	if (nomeFile == NULL)
		return false;

	//Apertura dei file di testo e relativo controllo di riuscita
	fpt1 = fopen(nomeFile, "r");
	if (fpt1 == NULL)
		return false;

	fpt2 = fopen("queries_results.txt", "a");
	if (fpt2 == NULL)
		return false;

	//tolgo l'intestazione del file
	i = 0;
	while ((c = fgetc(fpt1)) != '\n');

	/*Creo un'array che contiene le colonne della tabella*/
	colonne = TrovaNomeColonne(nomeFile, &nColonne);
	if (colonne == NULL)
		return false;

	/*Mi creo una variabile temporanea in modo da mantenere il valore di nomi colonne selezionate*/
	temp = (char*)malloc(strlen(nomi_colonne_selezionate) + 1);
	if (temp == NULL)
		return false;
	strcpy(temp, nomi_colonne_selezionate);

	//Suddivide le colonne selezionate in un'array di stringhe e le conta
	i = 0;
	colonneSelezionate[i] = SplitString(nomi_colonne_selezionate, ",", &temp);
	while (colonneSelezionate[i] != NULL)
	{
		colonneSelezionate[++i] = SplitString(temp, ",", &temp);
	}
	nColonneSelezionate = i;

	for (i = 0; i < nColonneSelezionate; i++)
		free(colonneSelezionate[i]); 

	/*Cerco le posizioni*/
	corretto = TrovaPosizioneColonne(&posizioni, colonneSelezionate, colonne, nColonneSelezionate, nColonne);
	if (corretto == false)
		return false;

	//Stampa nel file "queries_results.txt" la riga di comando impartita
	fprintf(fpt2, "SELECT %s FROM %s;\n", nomi_colonne_selezionate, nome_tabella);
	
	/*Stampa l'intestazione della query sul file*/
	fprintf(fpt2, "TABLE %s COLUMNS %s;\n", nome_tabella, nomi_colonne_selezionate);

	//Stampa dei dati riga per riga
	while (!feof(fpt1))
	{

		//Acquisisce una riga della tabella e la salva nella stringa "row"
		i = 0;
		while ((c = fgetc(fpt1)) != '\n')
		{
			row[i] = c;
			i++;

			//Nel caso in cui durante la lettura del file si giungesse al termine di quest'ultimo si interrompe il ciclo per evitare un loop infinito
			if (feof(fpt1))
				break;
		}

		//Nel caso in cui durante la lettura del file si giungesse al termine di quest'ultimo si esce dal ciclo di stampa
		if (feof(fpt1))
			break;
		row[i - 1] = '\0';
		sptr = StringCopy(row);

		//Rimuove la parte "ROW " dalla stringa contenente la riga lasciandovi solo i valori delle colonne separati da virgole
		str = SplitString(row, " ", &sptr);
		free(str); 
		str = SplitString(sptr, ";", &sptr);
		strcpy(row, str);
		free(str); 
		sptr = StringCopy(row);

		//Suddivide i valori delle colonne in un array
		i = 0;
		colonneDati[i] = SplitString(row, ",", &sptr);
		while (colonneDati[i] != NULL)
		{
			colonneDati[++i] = SplitString(sptr, ",", &sptr);
		}
		nColonne = i; 

		//Stampa la parte "ROW " e successivamente solo i valori corrispondenti agli indici delle colonne selezionate seguiti da "," o da ";" a seconda se si è o meno a fine riga
		fprintf(fpt2, "ROW ");
		for (i = 0; i < nColonneSelezionate; i++)
		{
			fprintf(fpt2, "%s", colonneDati[posizioni[i]]);
			if (i + 1 < nColonneSelezionate)
				fprintf(fpt2, ",");
		}
		fprintf(fpt2, ";\n");
		for (i = 0; i< nColonne; i++)
			free(colonneDati[i]); //MOD

	}

	//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
	fprintf(fpt2, "\n");

	//Chiusura dei file e free
	for (i = 0; i < nColonne; i++)
	{
		free(colonne[i]);
	}
	free(colonne);
	fclose(fpt1);
	fclose(fpt2);
	free(nome_tabella); 
	free(nomi_colonne_selezionate); 
	free(nomeFile);
	free(temp);
	free(posizioni);
	return true;
}

/*Select che ordina la tabella mostrando tutte le colonne */
bool OrderBy(char* colonna, char* nome_tabella, char* ordine)
{
	char ***testa, *nomeFile, **nomeColonne;
	int nRighe, nColonne, i, posizione, j;
	FILE *fpt;

	if (strcmp(ordine, "ASC") && strcmp(ordine, "DESC"))
		return false;

	//Allocazione stringa contenente il nome del file in cui è salvata la tabella
	nomeFile = Appendtxt(nome_tabella);
	if (nomeFile == NULL)
		return false;

	nomeColonne = TrovaNomeColonne(nomeFile, &nColonne);
	if (nomeColonne == NULL)
		return false;

	/*Creo la struttura dati dal file che andrò poi ad ordinare*/
	testa = LoadTable(nomeFile, nColonne, &nRighe);
	if (testa == NULL)
		return false;

	/*Cerco la posizione della colonna*/
	posizione = -1;
	for (i = 0; i < nColonne; i++)
	{
		if ((strcmp(nomeColonne[i], colonna)) == 0)
			posizione = i;
	}
	/*Se tale posizione e' inesistente ritorno false*/
	if (posizione == -1)
		return false;

	/*Ordino in base alla colonna prescelta*/
	HeapSort(testa, nRighe, posizione);

	/*Apro il file in scrittura*/
	fpt = fopen("queries_results.txt", "a");
	if (fpt == NULL)
		return false;

	if (!strcmp(ordine, "ASC"))
	{
		//Stampa nel file "queries_results.txt" la riga di comando impartita
		fprintf(fpt, "SELECT * FROM %s ORDER BY %s %s\n", nome_tabella, colonna, ordine);

		//Stampa l'intestazione della tabella
		fprintf(fpt, "TABLE %s COLUMNS ", nome_tabella);
		for (j = 0; j < nColonne; j++)
		{
			fprintf(fpt, "%s", nomeColonne[j]);
			if (j + 1 < nColonne)
				fprintf(fpt, ",");
		}
		fprintf(fpt, ";\n");

		for (j = 0; j < nRighe; j++)
		{
			fprintf(fpt, "ROW ");
			for (i = 0; i < nColonne; i++)
			{
				fprintf(fpt, "%s", testa[j][i]);
				if (i + 1 < nColonne)
					fprintf(fpt, ",");
			}
			fprintf(fpt, ";\n");
		}
	}
	else
	{
		//Stampa nel file "queries_results.txt" la riga di comando impartita
		fprintf(fpt, "SELECT * FROM %s ORDER BY %s %s;\n", nome_tabella, colonna, ordine);

		//Stampa l'intestazione della tabella
		fprintf(fpt, "TABLE %s COLUMNS ", nome_tabella);
		for (j = 0; j < nColonne; j++)
		{
			fprintf(fpt, "%s", nomeColonne[j]);
			if (j == nColonne - 1)
				fprintf(fpt, "%c", ';');
			else
				fprintf(fpt, "%c", ',');
		}
		fprintf(fpt, "\n");

		for (j = nRighe - 1; j >= 0; j--)
		{
			fprintf(fpt, "ROW ");
			for (i = 0; i < nColonne; i++)
			{
				fprintf(fpt, "%s", testa[j][i]);
				if (i + 1 < nColonne)
					fprintf(fpt, ",");
			}
			fprintf(fpt, ";\n");
		}
	}
	//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
	fprintf(fpt, "\n");

	//Chiusura dei file
	FreeTable(testa, nColonne, nRighe);
	for (i = 0; i < nColonne; i++)
		free(nomeColonne[i]);
	free(nomeColonne);
	free(colonna); 
	free(ordine); 
	free(nome_tabella); 
	fclose(fpt);
	free(nomeFile);

	return true;
}

/*Select che ordina la tabella mostrando solo alcune colonne specificate*/
bool OrderBy2(char* colonna, char* nome_tabella, char* ordine, char *colonne_da_stampare)
{
	char ***testa, *nomeFile, **nomeColonne, **nomeColonneSelezionate, *temp, *colonneDaStampare;
	int nRighe, nColonne, i, posizione, j, nColonneSelezionate;
	bool trovato;
	int *arrayPosizioni, k = 0;
	FILE *fpt;

	/*controllo se nell'input compare ASC o DESC*/
	if (strcmp(ordine, "ASC") && strcmp(ordine, "DESC"))
		return false;

	//Allocazione stringa contenente il nome del file in cui è salvata la tabella
	nomeFile = Appendtxt(nome_tabella);
	if (nomeFile == NULL)
		return false;

	/*Cerco il nome delle colonne della tabella*/
	nomeColonne = TrovaNomeColonne(nomeFile, &nColonne);
	if (nomeColonne == NULL)
		return false;

	/*Divido le colonne che devo selezionare*/
	nColonneSelezionate = NumeroColonne(colonne_da_stampare);
	nomeColonneSelezionate = (char**)malloc(sizeof(char*)*nColonneSelezionate);
	if (nomeColonneSelezionate == NULL)
		return false;

	/*allocazione memoria con conseguente controllo*/
	colonneDaStampare = (char*)malloc(sizeof(char*)*(strlen(colonne_da_stampare)) + 1);
	if (colonneDaStampare == NULL)
		return false;

	strcpy(colonneDaStampare, colonne_da_stampare);
	/*memorizzo dentro l'array i nomi delle colonne*/
	temp = SplitString(colonneDaStampare, ",", &colonne_da_stampare);
	nomeColonneSelezionate[0] = StringCopy(temp);
	free(temp); //MOD
	for (i = 1; i < nColonneSelezionate; i++)
	{
		temp = SplitString(colonne_da_stampare, ",", &colonne_da_stampare);
		nomeColonneSelezionate[i] = StringCopy(temp);
		free(temp); //MOD
	}

	/*Cerco le posizioni delle colonne*/
	trovato = TrovaPosizioneColonne(&arrayPosizioni, nomeColonneSelezionate, nomeColonne, nColonneSelezionate, nColonne);
	if (trovato == false)
		return false;

	/*Creo la struttura dati dal file che andrò poi ad ordinare*/
	testa = LoadTable(nomeFile, nColonne, &nRighe);
	if (testa == NULL)
		return false;

	/*Cerco la posizione della colonna*/
	posizione = -1;
	for (i = 0; i < nColonne; i++)
	{
		if ((strcmp(nomeColonne[i], colonna)) == 0)
			posizione = i;
	}
	/*Se tale posizione e' inesistente ritoo false*/
	if (posizione == -1)
		return false;

	/*Ordino in base alla colonna prescelta*/
	HeapSort(testa, nRighe, posizione);

	/*Apro il file in scrittura*/
	fpt = fopen("queries_results.txt", "a");
	if (fpt == NULL)
		return false;

	if (!strcmp(ordine, "ASC"))
	{
		//Stampa nel file "queries_results.txt" la riga di comando impartita
		fprintf(fpt, "SELECT %s FROM %s ORDER BY %s %s\n", colonneDaStampare, nome_tabella, colonna, ordine);

		//Stampa l'intestazione della tabella
		fprintf(fpt, "TABLE %s COLUMNS %s;\n", nome_tabella, colonneDaStampare);

		k = 0;
		for (j = 0; j < nRighe; j++)
		{
			fprintf(fpt, "ROW ");
			for (i = 0; i < nColonneSelezionate; i++)
			{
				fprintf(fpt, "%s", testa[j][arrayPosizioni[i]]);
				if (i + 1 < nColonneSelezionate)
					fprintf(fpt, ",");
			}
			fprintf(fpt, ";\n");
		}
	}
	else
	{
		//Stampa nel file "queries_results.txt" la riga di comando impartita
		fprintf(fpt, "SELECT %s FROM %s ORDER BY %s %s;\n", colonneDaStampare, nome_tabella, colonna, ordine);

		//Stampa l'intestazione della tabella
		fprintf(fpt, "TABLE %s COLUMNS %s;\n", nome_tabella, colonneDaStampare);

		for (j = nRighe - 1; j >= 0; j--)
		{
			fprintf(fpt, "ROW ");
			for (i = 0; i < nColonneSelezionate; i++)
			{
				fprintf(fpt, "%s", testa[j][arrayPosizioni[i]]);
				if (i + 1 < nColonneSelezionate)
					fprintf(fpt, ",");
			}
			fprintf(fpt, ";\n");
		}
	}
	//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
	fprintf(fpt, "\n");

	//Chiusura dei file
	FreeTable(testa, nColonne, nRighe);
	for (i = 0; i < nColonneSelezionate; i++)
		free(nomeColonneSelezionate[i]);
	free(nomeColonneSelezionate);
	for (i = 0; i < nColonne; i++)
		free(nomeColonne[i]);
	free(nomeColonne);
	free(colonneDaStampare);
	free(arrayPosizioni);
	fclose(fpt);
	free(nomeFile);
	free(colonna); 
	free(nome_tabella); 
	free(colonne_da_stampare); 
	free(ordine); 
	return true;
}

/*Select che ragruppa i dati a seconda di una colonna e ne conta quelli uguali*/
bool GroupBy(char* colonna, char* nome_tabella)
{
	char ***testa, *nomeFile, **nomeColonne, **colonnaDaStampare;
	int nRighe, nColonne, i, posizione, j, *count;
	FILE *fpt;

	//Allocazione stringa contenente il nome del file in cui è salvata la tabella
	nomeFile = Appendtxt(nome_tabella);
	if (nomeFile == NULL)
		return false;

	nomeColonne = TrovaNomeColonne(nomeFile, &nColonne);
	if (nomeColonne == NULL)
		return false;

	/*Creo la struttura dati dal file che andrò poi ad ordinare*/
	testa = LoadTable(nomeFile, nColonne, &nRighe);
	if (testa == NULL)
		return false;

	/*Cerco la posizione della colonna*/
	posizione = -1;
	for (i = 0; i < nColonne; i++)
	{
		if ((strcmp(nomeColonne[i], colonna)) == 0)
			posizione = i;
	}
	/*Se tale posizione e' inesistente ritorno false*/
	if (posizione == -1)
		return false;

	/*Ordino in base alla colonna prescelta*/
	HeapSort(testa, nRighe, posizione);

	/*allocazione memoria con conseguente controllo*/
	colonnaDaStampare = (char**)malloc(sizeof(char*) * nRighe);
	if (colonnaDaStampare == NULL)
		return false;

	/*allocazione memoria con conseguente controllo*/
	count = (int*)malloc(sizeof(int) * nRighe);
	if (count == NULL)
		return false;

	/*inizializzazioine dei due arrey*/
	for (i = 0; i < nRighe; i++)
	{
		count[i] = 0;
		colonnaDaStampare[i] = testa[i][posizione];
	}

	/*conteggio delle ripetizioni di uno stesso dato*/
	for (i = 0; i < nRighe; i++)
	{
		j = i;
		do
		{
			j++;
			count[i]++;
		} while (j < nRighe && strcmp(colonnaDaStampare[i], colonnaDaStampare[j]) == 0);
		i = j - 1;
	}

	/*Apro il file in scrittura*/
	fpt = fopen("queries_results.txt", "a");
	if (fpt == NULL)
		return false;

	//Stampa nel file "queries_results.txt" la riga di comando impartita
	fprintf(fpt, "SELECT %s FROM %s GROUP BY %s\n", colonna, nome_tabella, colonna);

	//Stampa l'intestazione della tabella
	fprintf(fpt, "TABLE %s COLUMNS %s,COUNT;\n", colonna, colonna);

	for (i = 0; i < nRighe; i++)
	{
		if (count[i] != 0)
			fprintf(fpt, "ROW %s %d;\n", colonnaDaStampare[i], count[i]);
	}

	//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
	fprintf(fpt, "\n");

	//Chiusura dei file
	fclose(fpt);
	FreeTable(testa, nColonne, nRighe);
	for (i = 0; i < nColonne; i++)
		free(nomeColonne[i]);
	free(nomeColonne);
	free(colonnaDaStampare);
	free(count);
	free(nomeFile);
	free(nome_tabella); 
	free(colonna); 

	return true;
}

/*Select di tutta la tabella con utilizzo del filtro Where*/
bool Where(char* nome_tabella, char* condition)
{
	int i, j, nColonne, nRighe, index = -1;
	unsigned int k;
	bool isANumber = true;
	char c;
	char row[100];
	char *conditionColumn, *conditionOperator;
	char *str, *colonne[20], *nomeFile, *sptr;
	char ***table;
	FILE *fpt1, *fpt2;

	//Allocazione stringa contenente il nome del file in cui è salvata la tabella
	nomeFile = Appendtxt(nome_tabella);
	if (nomeFile == NULL)
		return false;

	//Apertura dei file di testo e relativo controllo di riuscita
	fpt1 = fopen(nomeFile, "r");
	if (fpt1 == NULL)
		return false;

	fpt2 = fopen("queries_results.txt", "a");
	if (fpt2 == NULL)
		return false;

	//Salva la prima riga della tabella (in questo caso l'intestazione) nella stringa row
	i = 0;
	while ((c = fgetc(fpt1)) != '\n')
	{
		row[i] = c;
		i++;
	}
	row[i - 1] = '\0';
	sptr = StringCopy(row);

	//Rimuove la parte "TABLE nome_tabella COLUMNS " dalla stringa contenente l'intestazione lasciandovi solo i nomi delle colonne separati da virgole
	colonne[0] = SplitString(row, " ", &sptr);
	free(colonne[0]); 
	for (i = 0; i < 2; i++)
	{
		colonne[0] = SplitString(sptr, " ", &sptr);
		free(colonne[0]); 
	}
	colonne[0] = SplitString(sptr, " ", &sptr);

	strcpy(row, colonne[0]);
	sptr = StringCopy(row);
	free(colonne[0]); 
	
	//Suddivide le colonne in un array di stringhe e ne conta il numero
	i = 0;
	colonne[i] = SplitString(row, ",", &sptr);
	while (colonne[i] != NULL)
	{
		colonne[++i] = SplitString(sptr, ",", &sptr);
	}
	nColonne = i;

	//Salva in conditionColumn la colonna su cui controllare la condition e in conditionOperator l'operatore
	conditionColumn = StringCopy(SplitString(condition, " ", &condition));
	conditionOperator = StringCopy(SplitString(condition, " ", &condition));
	str = SplitString(condition, " ", &condition);
	if (str == NULL)
		return false;
	condition = StringCopy(str);
	free(str);
	for (i = 0; i < nColonne; i++)
	{
		if (strcmp(conditionColumn, colonne[i]) == 0)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
		return false;

	//Stampa nel file "queries_results.txt" la riga di comando impartita
	fprintf(fpt2, "SELECT * FROM %s WHERE %s %s %s;\n", nome_tabella, conditionColumn, conditionOperator, condition);

	//Stampa l'intestazione della tabella
	fprintf(fpt2, "TABLE %s COLUMNS ", row);
	for (i = 0; i<nColonne; i++)
	{
		fprintf(fpt2, "%s", colonne[i]);
		if (i == nColonne - 1)
			fprintf(fpt2, ";");
		else
			fprintf(fpt2, ",");
	}
	fprintf(fpt2, "\n");

	//Stampa dei dati
	table = LoadTable(nomeFile, nColonne, &nRighe);

	if (strcmp(conditionOperator, "==") == 0)
	{
		for (i = 0; i < nRighe; i++)
		{
			str = StringCopy(table[i][index]);
			if (strcmp(str, condition) == 0)
			{

				fprintf(fpt2, "ROW ");
				for (j = 0; j < nColonne; j++)
				{
					fprintf(fpt2, "%s", table[i][j]);
					if (j + 1 < nColonne)
						fprintf(fpt2, ",");
				}
				fprintf(fpt2, ";\n");
			}
		}
		//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
		fprintf(fpt2, "\n");
	}

	if (strcmp(conditionOperator, ">") == 0)
	{
		for (i = 0; i < nRighe; i++)
		{
			str = StringCopy(table[i][index]);
			//Controllo se il valore è un numero
			k = 0;
			while (k < strlen(str))
			{
				if (!isdigit(str[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}
			k = 0;
			while (k < strlen(condition))
			{
				if (!isdigit(condition[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}

			if (isANumber == true)
			{
				if (atoi(str) > atoi(condition))
				{
					fprintf(fpt2, "ROW ");
					for (j = 0; j < nColonne; j++)
					{
						fprintf(fpt2, "%s", table[i][j]);
						if (j + 1 < nColonne)
							fprintf(fpt2, ",");
					}
					fprintf(fpt2, ";\n");
				}

			}
			else if (strcmp(str, condition) > 0)
			{

				fprintf(fpt2, "ROW ");
				for (j = 0; j < nColonne; j++)
				{
					fprintf(fpt2, "%s", table[i][j]);
					if (j + 1 < nColonne)
						fprintf(fpt2, ",");
				}
				fprintf(fpt2, ";\n");
			}
		}
		//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
		fprintf(fpt2, "\n");
	}

	if (strcmp(conditionOperator, ">=") == 0)
	{
		for (i = 0; i < nRighe; i++)
		{
			str = StringCopy(table[i][index]);
			//Controllo se il valore è un numero
			k = 0;
			while (k < strlen(str))
			{
				if (!isdigit(str[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}
			k = 0;
			while (k < strlen(condition))
			{
				if (!isdigit(condition[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}

			if (isANumber == true)
			{
				if (atoi(str) >= atoi(condition))
				{
					fprintf(fpt2, "ROW ");
					for (j = 0; j < nColonne; j++)
					{
						fprintf(fpt2, "%s", table[i][j]);
						if (j + 1 < nColonne)
							fprintf(fpt2, ",");
					}
					fprintf(fpt2, ";\n");
				}

			}
			else if (strcmp(str, condition) >= 0)
			{

				fprintf(fpt2, "ROW ");
				for (j = 0; j < nColonne; j++)
				{
					fprintf(fpt2, "%s", table[i][j]);
					if (j + 1 < nColonne)
						fprintf(fpt2, ",");
				}
				fprintf(fpt2, ";\n");
			}
		}
		//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
		fprintf(fpt2, "\n");
	}

	if (strcmp(conditionOperator, "<") == 0)
	{
		for (i = 0; i < nRighe; i++)
		{
			str = StringCopy(table[i][index]);
			//Controllo se il valore è un numero
			k = 0;
			while (k < strlen(str))
			{
				if (!isdigit(str[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}
			k = 0;
			while (k < strlen(condition))
			{
				if (!isdigit(condition[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}

			if (isANumber == true)
			{
				if (atoi(str) < atoi(condition))
				{
					fprintf(fpt2, "ROW ");
					for (j = 0; j < nColonne; j++)
					{
						fprintf(fpt2, "%s", table[i][j]);
						if (j + 1 < nColonne)
							fprintf(fpt2, ",");
					}
					fprintf(fpt2, ";\n");
				}

			}
			else if (strcmp(str, condition) < 0)
			{

				fprintf(fpt2, "ROW ");
				for (j = 0; j < nColonne; j++)
				{
					fprintf(fpt2, "%s", table[i][j]);
					if (j + 1 < nColonne)
						fprintf(fpt2, ",");
				}
				fprintf(fpt2, ";\n");
			}
		}
		//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
		fprintf(fpt2, "\n");
	}

	if (strcmp(conditionOperator, "<=") == 0)
	{
		for (i = 0; i < nRighe; i++)
		{
			str = StringCopy(table[i][index]);
			//Controllo se il valore è un numero
			k = 0;
			while (k < strlen(str))
			{
				if (!isdigit(str[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}
			k = 0;
			while (k < strlen(condition))
			{
				if (!isdigit(condition[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}

			if (isANumber == true)
			{
				if (atoi(str) <= atoi(condition))
				{
					fprintf(fpt2, "ROW ");
					for (j = 0; j < nColonne; j++)
					{
						fprintf(fpt2, "%s", table[i][j]);
						if (j + 1 < nColonne)
							fprintf(fpt2, ",");
					}
					fprintf(fpt2, ";\n");
				}

			}
			else if (strcmp(str, condition) <= 0)
			{

				fprintf(fpt2, "ROW ");
				for (j = 0; j < nColonne; j++)
				{
					fprintf(fpt2, "%s", table[i][j]);
					if (j + 1 < nColonne)
						fprintf(fpt2, ",");
				}
				fprintf(fpt2, ";\n");
			}
		}
		//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
		fprintf(fpt2, "\n");
	}

	//Chiusura dei file
	fclose(fpt1);
	fclose(fpt2);
	free(nomeFile);
	free(conditionColumn);
	free(conditionOperator);
	free(str);
	FreeTable(table, nColonne, nRighe);
	for (i = 0; i < nColonne; i++)
		free(colonne[i]); 
	free(nome_tabella);
	free(condition); 
	return true;
}

/*Select di alcune colonne della tabella con utilizzo del filtro Where*/
bool Where2(char* nome_tabella, char* nomi_colonne_selezionate, char* condition)
{

	int i, j, nColonneSelezionate, nColonne, nRighe, index = -1;
	unsigned int k;
	bool isANumber = true, trovato;
	int *posizioni;
	char c;
	char *conditionColumn, *conditionOperator;
	char *str, **colonne, *colonneSelezionate[20], *nomeFile, *temp;
	char ***table;
	FILE *fpt1, *fpt2;

	//Allocazione stringa contenente il nome del file in cui è salvata la tabella
	nomeFile = Appendtxt(nome_tabella);
	if (nomeFile == NULL)
		return false;

	//Apertura dei file di testo e relativo controllo di riuscita
	fpt1 = fopen(nomeFile, "r");
	if (fpt1 == NULL)
		return false;

	fpt2 = fopen("queries_results.txt", "a");
	if (fpt2 == NULL)
		return false;

	//Toglie l'intestazione del file
	while ((c = fgetc(fpt1)) != '\n');

	/*Creo un'array che contiene le colonne della tabella*/
	colonne = TrovaNomeColonne(nomeFile, &nColonne);
	if (colonne == NULL)
		return false;

	/*Mi creo una variabile temporanea in modo da mantenere il valore di nomi colonne selezionate*/
	temp = (char*)malloc(strlen(nomi_colonne_selezionate) + 1);
	if (temp == NULL)
		return false;
	strcpy(temp, nomi_colonne_selezionate);

	//Suddivide le colonne selezionate in un'array di stringhe e le conta
	i = 0;
	colonneSelezionate[i] = SplitString(nomi_colonne_selezionate, ",", &temp);
	while (colonneSelezionate[i] != NULL)
	{
		colonneSelezionate[++i] = SplitString(temp, ",", &temp);
	}
	nColonneSelezionate = i;

	trovato = TrovaPosizioneColonne(&posizioni, colonneSelezionate, colonne, nColonneSelezionate, nColonne);
	if (trovato == false)
		return false;

	//Salva in conditionColumn la colonna su cui controllare la condition e in conditionOperator l'operatore
	temp = StringCopy(condition);
	conditionColumn = StringCopy(SplitString(condition, " ", &temp));
	conditionOperator = StringCopy(SplitString(temp, " ", &temp));
	str = SplitString(temp, "", &temp);
	if (str == NULL)
		return false;
	condition = StringCopy(str);
	for (i = 0; i < nColonne; i++)
	{
		if (strcmp(conditionColumn, colonne[i]) == 0)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
		return false;

	//Stampa nel file "queries_results.txt" la riga di comando impartita
	fprintf(fpt2, "SELECT %s FROM %s WHERE %s %s %s;\n", nomi_colonne_selezionate, nome_tabella, conditionColumn, conditionOperator, condition);

	//Stampa l'intestazione della tabella
	fprintf(fpt2, "TABLE %s COLUMNS %s;\n", nome_tabella, nomi_colonne_selezionate);

	//Stampa dei dati
	table = LoadTable(nomeFile, nColonne, &nRighe);

	if (strcmp(conditionOperator, "==") == 0)
	{
		for (i = 0; i < nRighe; i++)
		{
			str = StringCopy(table[i][index]);
			if (strcmp(str, condition) == 0)
			{

				fprintf(fpt2, "ROW ");
				for (j = 0; j < nColonneSelezionate; j++)
				{
					fprintf(fpt2, "%s", table[i][posizioni[j]]);
					if (j + 1 < nColonneSelezionate)
						fprintf(fpt2, ",");
				}
				fprintf(fpt2, ";\n");
			}
		}
		//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
		fprintf(fpt2, "\n");
	}

	if (strcmp(conditionOperator, ">") == 0)
	{
		for (i = 0; i < nRighe; i++)
		{
			str = StringCopy(table[i][index]);
			//Controllo se il valore è un numero
			k = 0;
			while (k < strlen(str))
			{
				if (!isdigit(str[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}
			k = 0;
			while (k < strlen(condition))
			{
				if (!isdigit(condition[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}

			if (isANumber == true)
			{
				if (atoi(str) > atoi(condition))
				{
					fprintf(fpt2, "ROW ");
					for (j = 0; j < nColonneSelezionate; j++)
					{
						fprintf(fpt2, "%s", table[i][posizioni[j]]);
						if (j + 1 < nColonneSelezionate)
							fprintf(fpt2, ",");
					}
					fprintf(fpt2, ";\n");
				}

			}
			else if (strcmp(str, condition) > 0)
			{

				fprintf(fpt2, "ROW ");
				for (j = 0; j < nColonneSelezionate; j++)
				{
					fprintf(fpt2, "%s", table[i][posizioni[j]]);
					if (j + 1 < nColonneSelezionate)
						fprintf(fpt2, ",");
				}
				fprintf(fpt2, ";\n");
			}
		}
		//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
		fprintf(fpt2, "\n");
	}

	if (strcmp(conditionOperator, ">=") == 0)
	{
		for (i = 0; i < nRighe; i++)
		{
			str = StringCopy(table[i][index]);
			//Controllo se il valore è un numero
			k = 0;
			while (k < strlen(str))
			{
				if (!isdigit(str[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}
			k = 0;
			while (k < strlen(condition))
			{
				if (!isdigit(condition[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}

			if (isANumber == true)
			{
				if (atoi(str) >= atoi(condition))
				{
					fprintf(fpt2, "ROW ");
					for (j = 0; j < nColonneSelezionate; j++)
					{
						fprintf(fpt2, "%s", table[i][posizioni[j]]);
						if (j + 1 < nColonneSelezionate)
							fprintf(fpt2, ",");
					}
					fprintf(fpt2, ";\n");
				}

			}
			else if (strcmp(str, condition) >= 0)
			{

				fprintf(fpt2, "ROW ");
				for (j = 0; j < nColonneSelezionate; j++)
				{
					fprintf(fpt2, "%s", table[i][posizioni[j]]);
					if (j + 1 < nColonneSelezionate)
						fprintf(fpt2, ",");
				}
				fprintf(fpt2, ";\n");
			}
		}
		//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
		fprintf(fpt2, "\n");
	}

	if (strcmp(conditionOperator, "<") == 0)
	{
		for (i = 0; i < nRighe; i++)
		{
			str = StringCopy(table[i][index]);
			//Controllo se il valore è un numero
			k = 0;
			while (k < strlen(str))
			{
				if (!isdigit(str[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}
			k = 0;
			while (k < strlen(condition))
			{
				if (!isdigit(condition[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}

			if (isANumber == true)
			{
				if (atoi(str) < atoi(condition))
				{
					fprintf(fpt2, "ROW ");
					for (j = 0; j < nColonneSelezionate; j++)
					{
						fprintf(fpt2, "%s", table[i][posizioni[j]]);
						if (j + 1 < nColonneSelezionate)
							fprintf(fpt2, ",");
					}
					fprintf(fpt2, ";\n");
				}

			}
			else if (strcmp(str, condition) < 0)
			{

				fprintf(fpt2, "ROW ");
				for (j = 0; j < nColonneSelezionate; j++)
				{
					fprintf(fpt2, "%s", table[i][posizioni[j]]);
					if (j + 1 < nColonneSelezionate)
						fprintf(fpt2, ",");
				}
				fprintf(fpt2, ";\n");
			}
		}
		//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
		fprintf(fpt2, "\n");
	}

	if (strcmp(conditionOperator, "<=") == 0)
	{
		for (i = 0; i < nRighe; i++)
		{
			str = StringCopy(table[i][index]);
			//Controllo se il valore è un numero
			k = 0;
			while (k < strlen(str))
			{
				if (!isdigit(str[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}
			k = 0;
			while (k < strlen(condition))
			{
				if (!isdigit(condition[k]))
				{
					isANumber = false;
					break;
				}
				k++;
			}

			if (isANumber == true)
			{
				if (atoi(str) <= atoi(condition))
				{
					fprintf(fpt2, "ROW ");
					for (j = 0; j < nColonneSelezionate; j++)
					{
						fprintf(fpt2, "%s", table[i][posizioni[j]]);
						if (j + 1 < nColonneSelezionate)
							fprintf(fpt2, ",");
					}
					fprintf(fpt2, ";\n");
				}

			}
			else if (strcmp(str, condition) <= 0)
			{

				fprintf(fpt2, "ROW ");
				for (j = 0; j < nColonneSelezionate; j++)
				{
					fprintf(fpt2, "%s", table[i][posizioni[j]]);
					if (j + 1 < nColonneSelezionate)
						fprintf(fpt2, ",");
				}
				fprintf(fpt2, ";\n");
			}
		}
		//Aggiunge una riga di spazio alla fine dei dati stampati per un maggiore ordine dell'output
		fprintf(fpt2, "\n");
	}

	//Chiusura dei file e free di tutte le malloc
	fclose(fpt1);
	fclose(fpt2);
	free(nomeFile);
	free(conditionColumn);
	free(conditionOperator);
	free(str);
	FreeTable(table, nColonne, nRighe);
	for (i = 0; i < nColonne; i++)
		free(colonne[i]); 
	for (i = 0; i < nColonneSelezionate; i++)
		free(colonneSelezionate[i]); 
	free(nome_tabella); 
	free(condition); 
	free(nomi_colonne_selezionate); 

	return true;
}
