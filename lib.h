#pragma once

#include <stdio.h>
#include <stdlib.h>
#define bool int
#define true 1
#define false 0

/*Funzioni che servono per gestire la struttura dati*/
/*Questa funzione e' una variante di strcpy*/
char* StringCopy(char* string);
/*Carica nella nostra struttura dati le varie righe*/
char** LoadRow(FILE*fpt, int numero_colonne);
/*Alloca una tabella che contiene tutte le informazioni contenute nella tabella (memorizzata nel file)*/
char*** LoadTable(char* nome_file, int numero_colonne, int *numero_righe); 
/*Dealloca la memoria occupata dalla struttura*/
void FreeTable(char*** rows, int numero_colonne, int numero_righe); 

/*Funzione per l'ordinamento*/
/*Inverte i due valori*/
void Swap(char ***a, char ***b);
/*funzione richiamata ricorsivamente da heapSort*/
void Heapify(char*** table, int numero_righe, int i, int numero_colonna);
/*funzione principale che ordina la tabella*/
void HeapSort(char ***table, int numero_righe, int numero_colonna);

/*Funzioni utili alle funzioni principali*/
/*Controlla se esiste già un file con tale nome */
bool ControlloEsistenzaFile(char * nome_file);
/*Agginge ".txt" a nome_tabella e restituisce il nome del file*/
char* Appendtxt(char* nome_tabella);
/*Controlla che siano presenti tutte le parentesi*/
char* ControlloParentesi(char* nomi_colonne);
/*Controlla se le colonne inserite nella query sono le stesse di quelle all'interno del file*/
bool ControllaColonne(char* nome_tabella, char* colonne_inserite);
/*Conta il numero di virgole all'interno della stringa colonne così da stabilire il numero di colonne*/
int NumeroColonne(char* nome_colonne);
/*Trova la posizione delle colonne selizionate dalla query*/
bool TrovaPosizioneColonne(int ** array_posizioni, char ** colonne_selezionate, char** colonne_tabella, int n_colonne_selezionate, int n_colonne);
/*Questa funzione rilegge dal file le colonne, le salva in un array e le conta*/
char** TrovaNomeColonne(char* nome_file, int *numero_colonne);

/*Funzioni principali per la gestione di tutte le query*/
/*Questa funzione simula in comportamento delle strtok*/
char * SplitString(char * str, const char * delimiters, char ** sptr);
/*Questa funzione restituisce 1 se si tratta dell'istruzione per la creazione di una nuova tabella, 2 se di tratta di un inserimento
e 3 se si tratta dell'istruzione SELECT*/
int TipoDiQuery(char** query_inserita);
/*Questa funzione cataloga il tipo di filtro utilizzato per la select*/
int TipoSelect(char** query_inserita);
/*Riconosce la query e la esegue*/
bool executeQuery(char* query_inserita);

/*Funzioni principali*/
/*Questa funzione crea la tabella e la dispone su un file*/
bool CreateTable(char* nome_tabella, char* nomi_colonne);
/*Questa funzione inserisce una nuova riga all'interno della tabella*/
bool InsertIntoTable(char* nome_tabella, char* valori_colonne);
/*Questa funzione gestisce la select * senza filtri*/
bool NoFilterSearch1(char* nome_tabella);
/*Questa funzione gestisce la select con colonne selezionate senza filtri*/
bool NoFilterSearch2(char* nome_tabella, char* nomi_colonne_selezionate);
/*Select che ordina la tabella mostrando tutte le colonne*/
bool OrderBy(char* colonna, char* nome_tabella, char* ordine);
/*Select che ordina la tabella mostrando solo alcune colonne specificate*/
bool OrderBy2(char* colonna, char* nome_tabella, char* ordine, char *colonne_da_stampare);
/*Select che ragruppa i dati a seconda di una colonna e ne conta quelli uguali*/
bool GroupBy(char* colonna, char* nome_tabella);
/*Select di tutta la tabella con utilizzo del filtro Where*/
bool Where(char* nome_tabella, char* condition);
/*Select di alcune colonne della tabella con utilizzo del filtro Where*/
bool Where2(char* nome_tabella, char* nomi_colonne_selezionate, char* condition);