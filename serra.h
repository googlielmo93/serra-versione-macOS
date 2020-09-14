/* Header per Progetto Serra - Gruppo La Bua, Caruso, La Mantia */

#include <string.h>
#include <pthread.h>
#pragma GCC diagnostic ignored "-Wall"          
#pragma GCC diagnostic ignored "-Wpointer-sign"

extern int yylex();
extern void yyrestart (FILE *input_file  );
static int op=0; //per le funzioni
static int win=-1; //per il livello della funzione
int flag;
/* Tabella dei Simboli */
struct symbol {
  char *name;
  char *value;
  double dim;
  struct ast *func;         /* stmt per le funzioni */
  struct ast *dev;         /* stmt per le funzioni */
  struct argsList *syms;     /* Lista dei simboli */
  struct ast *ret;
};


/* Tabella di dimensione fissa */
#define DIMHASH 10000
struct symbol symtab[DIMHASH];
struct symbol *funcTab[1000];

/* Dichiarazione Funzione Search() */
struct symbol *search(char*, char*);


struct numval {
  int nodetype;         /* tipo nodo K -> valore costante*/
  double number;
};



struct stringVal {
  int nodetype;         /* tipo nodo C -> valore stringa costante*/
  struct symbol *s;
};



struct device {
  int nodetype;         /* tipo nodo D -> dispositivo inserito nella rete */
  int status;          /* definisce lo stato, acceso 1, spento 0 */
  struct symbol *s;     
  int c;
  struct ast *l;        /* Lista dei device collegati */
};



/* Struttura di collegamento tra un simbolo della tabella e una lista di simboli */
struct argsList {
  struct symbol *sym;
  struct argsList *next;
};


/* Dichiarazione Funzione argsList() */
struct argsList *newargsList(struct symbol *sym, struct argsList *next);


/* Dichiarazione Funzione argsListfree() */
void argsListfree(struct argsList *sl);


/* Tipi di Nodi
 *  + - 
 *  0-7 Tipo di simbolo di comparazione
 *  L statement list
 *  I IF statement
 *  W WHILE statement
 *  N riferimento ad un simbolo
 *  K valore costante
 *  = assegnamento
 *  C Stringa
 *  S Lista di Simboli
 *  F Funzioni Predefinite
 *  O other function di sistema
 *  U Funzioni Utente
 *  D Dispositivo
 */ 


/* Funzioni Predefinite */
enum builtFunc {              
  B_print = 1, 
  B_connect = 2, 
  B_reconnect = 3,
  B_status = 4,
  B_switchOn = 5, 
  B_switchOff = 6,
  B_diagnostic = 7, 
  B_archive = 8,
  B_interval = 9,
  B_insertDevice = 10,
  B_readFile = 11,
  B_add = 12,
  B_get = 13,
  B_set=14,
  B_remove=15,
};


enum builtFuncSystem {            
    B_clear = 1,
    B_help = 2,
};


/* Struttura Nodo dell'albero di Sintassi Astratto */



struct ast {
  int nodetype;
  struct ast *l;
  struct ast *r;
};


/* Struttura Nodo per l'AST, per le funzioni predefinite */
struct funcBuiltIn {    
  int nodetype;         /* tipo nodo F -> Funzioni Built-In*/
  struct ast *l;
  struct ast *r;
  struct ast *t;
  enum builtFunc functype;
};


/* Struttura Nodo per l'AST, per le funzioni predefinite */
struct funcBuiltInSystem {    
  int nodetype;         /* tipo nodo O -> Funzioni Built-In System o senza argomenti */
  enum builtFuncSystem functype;
};

/* Struttura Nodo per l'AST, per le funzioni utente */
struct userfunc {      
  int nodetype;         /* tipo nodo U -> Funzioni Utente*/
  struct ast *l;        /* Lista degli argomenti della funzione utente */
  struct symbol *s;
};


/* Struttura Nodo per i costrutti IF e WHILE */
struct content {
  int nodetype;         /* tipo nodo I o W */
  struct ast *cond;     /* condizione */
  struct ast *tl;       /* costrutti then o do listStmt */
  struct ast *el;       /* opzionale else listStmt */
};


/* Struttura Nodo per un riferimento ad un simbolo della tabella, per le funzioni utente */
/* Anche usato per accedere alle variabili */
struct symref {
  int nodetype;         /* tipo nodo N -> riferimento ad un simbolo*/
  struct symbol *s;
};


/* Struttura Nodo per il simbolo di assegnamento = */
struct symasgn {
  int nodetype;         /* tipo nodo = -> assegnamento */
  struct symbol *s;
  struct ast *v;        /* valore */
};

struct defineArray {
    int nodetype;
    double dimensione;
    double tipo;
    struct symbol *nome;
    
};


/*Funzione interval che dato un tipo ast in input va a verificare l'esistenza di quel device. Nel caso in cui il device esiste cioè è presente nalla tabella dei simboli va a controllare tramite un parametro passato come parametro, cioè l'istante di tempo, quanti secondi devono passare per mantenere attivo il device. Passato l'intervallo di tempo lo pone a 0. Quindi teoricamente questa funzione pretende in input due parametri rappresenti da una stringa che identifica il periodo di tempo in cui deve stare attivo e il device da attivare. Come parametro prende il device da attivare per quell'intervalo di tempo*/
/*interval "device, 10 minuti"-> formato: device, 10 minuti*/
struct thread_time {
    int r;
    char *v;
    float seconds;
};


/* FUNZIONI PER LA GENERAZIONE DI UN AST */
struct ast *newast(int nodetype, struct ast *l, struct ast *r);
//struct ast *newcmp(int cmptype, struct ast *l, struct ast *r);
struct ast *newfunc(int functype, struct ast *l, struct ast *r,  struct ast *t);
struct ast *newfuncSystem(int functype);
struct ast *newcall(struct symbol *s, struct ast *l);
struct ast *newref(struct symbol *s);
struct ast *newasgn(struct symbol *s, struct ast *v);
struct ast *newnum(double d);
struct ast *newString(struct symbol *s);
struct ast *newDev(struct symbol *ps, struct argsList *l);
struct ast *newContent(int nodetype, struct ast *cond, struct ast *tl, struct ast *tr);
struct ast *newAstArray (struct symbol *nome, double dimensione, double tipo);
struct ast* defFunction (struct symbol *name, struct argsList *syms, struct ast *func, struct ast*ret);

//funzioni per l'esecuzione di funzioni: FUNC, FUNCDEV e System
struct ast * callbuiltin(struct funcBuiltIn *);
struct ast * callbuiltinSystem(struct funcBuiltInSystem *);
static char* calluser(struct userfunc *);

//Lancio:
void run (struct ast *r);

//gnerazione array
void newArray (struct symbol *nome, double dimensione, double tipo);


/* Per la definizione di una nuova funzione*/
void defSymRef(struct symbol *name, struct argsList *syms, struct ast *stmts, struct ast *ret);


/* evaluate an AST */
char * eval(struct ast *);


/* delete and free an AST */
void treefree(struct ast *);


/* Interfaccia con il Lexer */
extern int yylineno; /* Variabile del Lexer */
void yyerror(char *s, ...);


extern int debug;
void dumpast(struct ast *a, int level);

/* Read_file*/
static int index_file=0;

extern FILE *yyin;