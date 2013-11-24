#ifndef __GENES_H
#define __GENES_H 1


typedef struct {
	unsigned short geneIndex1;
	unsigned short geneIndex2;
	unsigned char remainderIndex;
} RESULT;

typedef struct {
	char *geneString;
	unsigned char length;
	unsigned short number;
} GENE;


typedef struct {
	unsigned int number;
	
	unsigned long long length;
	unsigned short last16[16];
	unsigned short last16pos;
	
	unsigned short lastGeneIndex;
	unsigned short remainderIndex;
	
	RESULT *tmpResult;
	
} GENE_ITERATION;

#define GENE_SIZE 10
#define MAX_NUMBER_OF_GENES 65000

extern GENE genes[MAX_NUMBER_OF_GENES];
extern RESULT results[6][MAX_NUMBER_OF_GENES];

extern unsigned short geneCount;

void initGeneSystem(int _max);
void generateGenes();
void generateGeneSuccessors();
void nextGene(const unsigned short geneIndex, const unsigned int itnum, const unsigned short end);

#endif
