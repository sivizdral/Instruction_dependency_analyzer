#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct cvor {
	char var;
	struct cvor* sled;
} CVOR;

typedef struct zaglavlje {
	CVOR* prvi;
	int tezina;
	char var;
} ZAGLAVLJE;

typedef struct graf {
	ZAGLAVLJE* niz;
	int V;
} GRAF;

int dodajCvor(GRAF* G, int redni, int tezina, char var) {
	G->niz = (ZAGLAVLJE*)realloc(G->niz, G->V * sizeof(ZAGLAVLJE));
	G->niz[redni - 1].prvi = NULL;
	G->niz[redni - 1].tezina = tezina;
	G->niz[redni - 1].var = var;
	return 0;
}

int dodajGranu(GRAF* G, char pocetni, char krajnji) {
	CVOR* novi = (CVOR*)malloc(sizeof(CVOR));
	novi->sled = NULL;
	novi->var = krajnji;
	int i = 0;
	while (G->niz[i].var != pocetni && i < G->V) i++;
	if (i < G->V) {
		CVOR* trenutni = G->niz[i].prvi;
		CVOR* prethodni = trenutni;
		if (!trenutni) G->niz[i].prvi = novi;
		else if (trenutni->var > krajnji) {
			novi->sled = G->niz[i].prvi;
			G->niz[i].prvi = novi;
		}
		else {
			while (trenutni && trenutni->var <= krajnji) {
				prethodni = trenutni;
				trenutni = trenutni->sled;
			}
			prethodni->sled = novi;
			novi->sled = trenutni;
		}
	}
	

	return 0;
}

GRAF* napraviGraf(FILE* ulaz, FILE* izlaz) {
	fprintf(izlaz, "U svakom redu unosite po jednu operaciju u formatu a = b op c za binarne, odnosno a = b op za unarne operacije:\n");
	GRAF* G = (GRAF *) malloc(sizeof(GRAF));
	G->V = 0;
	G->niz = (ZAGLAVLJE*)malloc(sizeof(ZAGLAVLJE));
	int tezina = 0, redni;
	char c, var, poc1, poc2;
	char red[50];
	fgets(red, 50, ulaz);
	while (red[0]!='\n') {
		G->V++;
		poc1 = '0';
		poc2 = '0';
		var = red[0];
		c = red[4];
		int i = 4;
		if (c >= 'a' && c <= 'z') {
			poc1 = c;
			i++;
		}
		else while (c != ' ') c = red[++i];
		i++;
		c = red[i];
		switch (c) {
			case '+': case '-': {
				tezina = 3;
			} break;
			case '*': {
				tezina = 10;
			} break;
			case '/': {
				tezina = 23;
			} break;
			case 'a': case 'o': case 'x': case 'n': {
				tezina = 1;
			} break;
		}
		dodajCvor(G, G->V, tezina, var);
		if (c != 'n') {
			while (c != ' ') c = red[++i];
			i++;
			c = red[i];
			if (c >= 'a' && c <= 'z') {
				poc2 = c;
			}
		}
		if (poc1 != '0') dodajGranu(G, poc1, var);
		if (poc2 != '0') dodajGranu(G, poc2, var);
		fgets(red, 50, ulaz);
	}
	return G;
}

int obrisiGranu(GRAF* G, char pocetni, char krajnji) {
	int i = 0;
	while (G->niz[i].var != pocetni) i++;
	CVOR* trenutni = G->niz[i].prvi;
	CVOR* prethodni = trenutni;
	while (trenutni->var != krajnji && trenutni) {
		prethodni = trenutni;
		trenutni = trenutni->sled;
	}
	if (!trenutni) printf("Trazena grana ne postoji u grafu, te se ne moze ni obrisati.");
	else {
		prethodni->sled = trenutni->sled;
		free(trenutni);
	}
	return 0;
}

int obrisiCvor(GRAF* G, char var) {
	int i = 0;
	while (i < G->V && G->niz[i].var != var) i++;
	CVOR* trenutni = G->niz[i].prvi, *stari;
	while (trenutni) {
		stari = trenutni;
		trenutni = trenutni->sled;
		free(stari);
	}
	for (int j = i; j < G->V - 1; j++) {
		G->niz[j] = G->niz[j + 1];
	}
	G->V--;
	G->niz = (ZAGLAVLJE*)realloc(G->niz, G->V * sizeof(ZAGLAVLJE));
	for (int i = 0; i < G->V; i++) {
		CVOR* trenutni = G->niz[i].prvi;
		CVOR* prethodni = NULL;
		while (trenutni) {
			if (G->niz[i].prvi->var == var) {
				G->niz[i].prvi = trenutni->sled;
				CVOR* stari = trenutni;
				trenutni = trenutni->sled;
				free(stari);
			}
			else if (trenutni->var == var) {
				prethodni->sled = trenutni->sled;
				CVOR* stari = trenutni;
				trenutni = trenutni->sled;
				free(stari);
			}
			else {
				prethodni = trenutni;
				trenutni = trenutni->sled;
			}
		}
	}
	return 0;
}

void ispisGrafa(GRAF * G, FILE* izlaz) {
		fprintf(izlaz, "Graf je cuvan u memoriji u vidu lista susednosti. U svakom redu za svaki cvor ispisuju se njegovi susedi:\n");
		for (int i = 0; i < G->V; i++) {
			fprintf(izlaz, "[%d:%c] ->", G->niz[i].tezina, G->niz[i].var);
			CVOR* trenutni = G->niz[i].prvi;
			while (trenutni) {
				fprintf(izlaz, " [%c] ->", trenutni->var);
				trenutni = trenutni->sled;
			}
			fprintf(izlaz, " NULL\n");
		}
}

void topSort(GRAF* G, char T[26]) {
	GRAF* K = (GRAF*)malloc(sizeof(GRAF));
	K->V = G->V;
	K->niz = (ZAGLAVLJE*)malloc(K->V * sizeof(ZAGLAVLJE));
	for (int i = 0; i < K->V; i++) {
		K->niz[i].tezina = G->niz[i].tezina;
		K->niz[i].var = G->niz[i].var;
		K->niz[i].prvi = NULL;
		CVOR* trenutni = G->niz[i].prvi, *novi, *drugi=NULL;
		while (trenutni) {
			novi = (CVOR*)malloc(sizeof(CVOR));
			novi->var = trenutni->var;
			novi->sled = NULL;
			if (!K->niz[i].prvi) K->niz[i].prvi = novi;
			else drugi->sled = novi;
			drugi = novi;
			trenutni = trenutni->sled;
		}
	}
	int vektor[26] = { 0 };
	for (int i = 0; i < K->V; i++) {
		CVOR* trenutni = K->niz[i].prvi;
		while (trenutni) {
			vektor[trenutni->var - 'a']++;
			trenutni = trenutni->sled;
		}
	}
	int brojac = 0;
	int pomerac = -1;
	while (K->V > 0) {
		for (int i = 0; i < K->V; i++) {
			int promena = 0;
			if (vektor[K->niz[i].var - 'a'] == 0 && !promena) {
				vektor[K->niz[i].var - 'a']--;
				promena = 1;
				T[brojac] = K->niz[i].var;
				brojac++;
			}
		}
		int i = 0;
		pomerac++;
		if (K->V > 1) {
			while (K->niz[i].var != T[pomerac]) i++;
			CVOR* tr = K->niz[i].prvi;
			while (tr) {
				vektor[tr->var - 'a']--;
				tr = tr->sled;
			}
		}
		obrisiCvor(K, T[pomerac]);
	} 
	pomerac ++;
}

void kriticniPut(GRAF* G, int deo, FILE* izlaz) {
	char T[26];
	topSort(G, T);
	int EST[26] = { 0 };
	int LST[26] = { 0 };
	int L[26] = { 0 };
	int makskriticni = 0;
	for (int u = 1; u <= G->V; u++) {
		char i = T[u];
		for (int j = 0; j < G->V; j++) {
			CVOR* trenutni = G->niz[j].prvi;
			int p = 0;
			while (T[p] != G->niz[j].var) p++;
			while (trenutni) {
				if (trenutni->var == i)
					if (EST[p] + G->niz[j].tezina > EST[u]) EST[u] = EST[p] + G->niz[j].tezina;
					
				trenutni = trenutni->sled;
			}
			if ((EST[p] + G->niz[j].tezina) > makskriticni) makskriticni = EST[p] + G->niz[j].tezina;
		}
	}

	for (int i = 0; i < G->V; i++) LST[i] = EST[G->V - 1];
	for (int i = 0; i < G->V; i++) if (G->niz[i].prvi == NULL) {
		int k = 0;
		while (G->niz[i].var != T[k]) k++;
		LST[k] = makskriticni - G->niz[i].tezina;
	}
	for (int i = 0; i < G->V; i++) {
		int k = 0;
		while (G->niz[k].var != T[i]) k++;
		for (int u = (G->V - 2); u >= 0; u--) {
			char i = T[u];
			int k = 0;
			while (G->niz[k].var != i) k++;
			CVOR* trenutni = G->niz[k].prvi;
			while (trenutni) {
				int p = 0;
				while (T[p] != trenutni->var) p++;
				if (LST[p] - G->niz[k].tezina < LST[u]) LST[u] = LST[p] - G->niz[k].tezina;
				trenutni = trenutni->sled;
			}
		}
	}
	for (int i = 0; i < G->V; i++) printf("%d ", EST[i]);
	putchar('\n');
	for (int i = 0; i < G->V; i++) printf("%d ", LST[i]);
	if (deo == 1) {
		fprintf(izlaz, "Duzina kriticnog puta iznosi %d.\n", makskriticni);
		fprintf(izlaz, "Na kriticnom putu nalaze se cvorovi:");
		for (int i = 0; i < G->V; i++) {
			L[i] = LST[i] - EST[i];
			if (L[i] == 0) fprintf(izlaz, " %c", T[i]);
		}
		fputc('\n', izlaz);
	}
	if (deo == 2) {
		for (int i = 0; i <= makskriticni; i++) {
			fprintf(izlaz, "\nU sekundi %d mogu zapoceti sledeci procesi: ", i);
			for (int j = 0; j < G->V; j++) if (i <= LST[j] && i >= EST[j]) fprintf(izlaz, "%c ", T[j]);
		}
		fputc('\n', izlaz);
	}
}

int push(CVOR** stek, char c) {
	CVOR* novi = malloc(sizeof(CVOR));
	novi->var = c;
	novi->sled = NULL;
	if (!*stek) *stek = novi;
	else {
		novi->sled = (*stek);
		*stek = novi;
	}
	return 0;
}

int empty(CVOR* stek) {
	if (!stek) return 1;
	return 0;
}

char pop(CVOR** stek) {
	if (!empty(*stek)) {
		CVOR* novi = *stek;
		*stek = (*stek)->sled;
		char c = novi->var;
		free(novi);
		return c;
	}
}

char top(CVOR* stek) {
	if (!empty(stek)) {
		return stek->var;
	}
}

 int tranzitivnost(GRAF* G, char c, FILE* izlaz) {
	int visit[26];
	CVOR* stek = NULL;
	putchar('\n');
	for (int i = 0; i < 26; i++) visit[i] = 0;
	int j = 0;
	while (G->niz[j].var != c) j++;
	push(&stek, G->niz[j].var);
	while (!empty(stek)) {
		char next = pop(&stek);
		if (!visit[next - 'a']) {
			visit[next - 'a']++;
			int k = 0;
			while (G->niz[k].var != next) k++;
			CVOR* ubaci = G->niz[k].prvi;
			while (ubaci) {
				push(&stek, ubaci->var);
				ubaci = ubaci->sled;
			}
		}
		else visit[next - 'a']++;
	}
	CVOR* trenutni = G->niz[j].prvi;
	int br = 0;
	while (trenutni) {
		if (visit[trenutni->var - 'a'] > 1) {
			fprintf(izlaz, "Postoji tranzitivna grana izmedju operacija %c i %c.\n", c, trenutni->var);
			br++;
		}
		trenutni = trenutni->sled;
	}
	if (!br) fprintf(izlaz, "Nema tranzitivnih grana za zadatu operaciju.\n\n");
	return 0;
}

int main() {
	printf("Ukoliko zelite da uvezete podatke iz neke datoteke, napisite njen naziv.\n");
	printf("Ukoliko zelite da unosite podatke na standardni ulaz, napisite stdin.\n");
	char ulazna[100];
	scanf("%s", ulazna);
	printf("Ukoliko zelite da ispisete podatke u neku datoteku, napisite njen naziv.\n");
	printf("Ukoliko zelite da se podaci ispisuju na standardni izlaz, napisite stdout\n");
	char izlazna[100];
	scanf("%s", izlazna);
	FILE* ulaz, *izlaz;
	if (strcmp(ulazna, "stdin") != 0) ulaz = fopen(ulazna, "r");
	else ulaz = stdin;
	if (strcmp(izlazna, "stdout") != 0) izlaz = fopen(izlazna, "w");
	else izlaz = stdout;
	int izbor=0;
	GRAF* G = NULL;
	while (izbor != 6) {
		fprintf(izlaz,"Imate ponudjen meni mogucih izbora. Izaberite stavku koju zelite:\n");
		fprintf(izlaz,"1. Unos operacija i konstrukcija grafa\n");
		fprintf(izlaz,"2. Ispis grafa i svih zavisnosti u njemu\n");
		fprintf(izlaz,"3. Odredjivanje duzine kriticnog puta i cvorova na njemu\n");
		fprintf(izlaz,"4. Prikazivanje svih mogucih rasporeda operacija\n");
		fprintf(izlaz,"5. Odredjivanje postojanja tranzitivnih grana\n");
		fprintf(izlaz,"6. Brisanje grafa i prekid programa\n");
		fscanf(ulaz,"%d", &izbor);
		switch (izbor) {
		case 1: {
			fgetc(ulaz);
			G = napraviGraf(ulaz, izlaz);
		} break;
		case 2: {
			ispisGrafa(G, izlaz);
		} break;
		case 3: {
			int deo = 1;
			kriticniPut(G, deo, izlaz);
		} break;
		case 4: {
			int deo = 2;
			kriticniPut(G, deo, izlaz);
		} break;
		case 5: {
			char pocetni;
			fprintf(izlaz,"Izaberite operaciju za koju zelite da vidite da li tranzitivno utice na druge operacije:\n");
			fgetc(ulaz);
			fscanf(ulaz, "%c", &pocetni);
			tranzitivnost(G, pocetni, izlaz);
		} break;
		}
	}
	fclose(ulaz);
	fclose(izlaz);
	return 0;
}