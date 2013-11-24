#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genes.h"
#include "hashtable.h"

const char *validRemainders[] = { "", "1", "11", "2", "22", "3" };

GENE genes[MAX_NUMBER_OF_GENES];
RESULT results[6][MAX_NUMBER_OF_GENES];

unsigned short remainderResultIndexes[6];

unsigned short geneCount = 0;
const int geneSize = GENE_SIZE;

RESULT zeroResult;

struct hashtable  *geneHashTable;

int max = 10;

GENE_ITERATION iterations[200];


/* used by the hash table for creating keys */
static unsigned int geneHash(void *_geneString)
{
	int i;
	unsigned int hash = 0;
	int len;
	
	char *geneString = (char *)_geneString;
	
	len = strlen(geneString);
	
	if (strlen(geneString) != 0)
	{
		for (i = 0; i < len; i++)
		{
			hash += i * (geneString[i] - '0');
		}
	}
	
	return hash;
}

/* used by the hash table */
static int genes_equal_fn ( void *key1, void *key2 )
{
	return !strcmp((char *) key1, (char *) key2);
}


void addNextCharToGene(char *gene, int maxLength)
{
	int l;
	char c;
	char *newgene;

	//fprintf(stderr, "Adding new char to: '%s' (maxl = %d)... \n", gene, maxLength);
	l = strlen(gene);
	for (c = '1'; c < '4'; c++)
	{
		//fprintf(stderr, "c = %c, l = %d... ", c, l);
		if ( l < 2 || c != gene[l-1] || ((c == '1' || c == '2') && l < 3) )
		{
			// do nothing
		}
		else if(gene[l-1] == gene[l-2] && gene[l-2] == '3')
		{
			continue;
		}
		else if(gene[l-1] == gene[l-2] && gene[l-2] == gene[l-3] && gene[l-3] == c)
		{
			continue;
		}
		
		newgene = malloc( (l + 3) * sizeof(*newgene) );
		if (newgene == 0)
		{
			//fprintf(stderr, "OUT OF MEMORY!\n");
			exit(-5);
		}

		strncpy(newgene, gene, l);
		newgene[l] = c;
		newgene[l+1] = '\0';
		if (l < maxLength)
		{
			addNextCharToGene(newgene, maxLength);
			free(newgene);
		}
		else
		{
			genes[geneCount].geneString = newgene;
			genes[geneCount].length = strlen(newgene);
			genes[geneCount].number = geneCount;
			hashtable_insert(geneHashTable, genes[geneCount].geneString, &genes[geneCount]);
			//printf("Gene %d is %s.\n", geneCount, newgene);
			geneCount++;
		}
	}

	//free(gene); //- this should be OK but isn't? hmm.. we're wasting memory here
}

/*
# calculate the total number of combinations
# that can happen in n characters
#
# rules as follows: 
#  only digits 1, 2, 3 can be present
#  there can only be a maximum of 3 1s in a row, 3 2s or 2 3s.
*/

void generateGenes()
{
	int i;
	char *startGene;

	startGene = malloc( (geneSize + 1) * sizeof(char));
	
	genes[0].geneString = "\0";
	genes[0].length = 0;
	genes[0].number = 0;
	hashtable_insert(geneHashTable, genes[0].geneString, &genes[0]);
	geneCount = 1;

	//genes = malloc(40000 * sizeof(GENE));
	for (i = 0; i < geneSize; i++)
	{
		// zero startGene:
		startGene[0] = '\0';
		addNextCharToGene(startGene, i);
	}
	
	free(startGene);
	fprintf(stderr, "generated %d genes for length %d.\n", geneCount, geneSize);
}


unsigned short getGeneIndex(char *geneString)
{
	GENE *genep = hashtable_search(geneHashTable, geneString);
	if (genep != NULL)
	{
		//fprintf(stderr, "Found gene: %s, number %d\n", geneString, genep->number);
		return genep->number;
	}
	else 
	{
		fprintf(stderr, "Gene %s not found in hashtable!!\n", geneString);
		exit(-3);
	}
}

unsigned char getRemainderIndex(char *remainder)
{
	unsigned char i;
	for (i = 0; i < 6; i++)
	{
		if (strcmp(remainder, validRemainders[i]) == 0)
		{
			break;
		}
	}
	
	return i;
}

/*
# for the given precursor and gene, work out what the relevant gene pair would be
# in the next element of the sequence
*/

RESULT calculateSuccessors(unsigned int p, unsigned int g)
{
	char current[geneSize * 2 + 1];
	char next[geneSize * 2 + 1];
	char remainder[4];
	RESULT result;

	int clen;
	
	int i;
	char c;
	char last = 0;
	int count = 0;
	int pos = 0;
	char gene1[geneSize + 1];
	
	strcpy(current, validRemainders[p]);
	strcat(current, genes[g].geneString);
	clen = strlen(current);
	
	//fprintf(stderr, "Processing current: %s (p = %s, g = %s)\n", current, validRemainders[p], genes[g].geneString);

	//some combos of precursors and genes are invalid:
	if (clen > 3 && strncmp(current, "1111", 4) == 0) return zeroResult;
	if (clen > 3 && strncmp(current, "2222", 4) == 0) return zeroResult;
	if (clen > 2 && strncmp(current, "333", 3) == 0) return zeroResult;

	for (i = 0; i < clen; i++)
	{
		c = current[i];
		if (c != last && last != 0)
		{
			next[pos++] = count + '0';
			next[pos++] = last;
			count = 1;
			last = c;
		}
		else 
		{
			count ++;
			if (last == 0)
			{
				last = c;
			}
		}
	}

	if (count == 3 || (count == 2 && last == '3'))
	{
		next[pos++] = count + '0';
		next[pos++] = current[clen - 1];
		remainder[0] = '\0';
	}
	else
	{
		for (i = 0; i < count; i++)
		{
			remainder[i] = c;
		}
		remainder[count] = '\0';
	}
	next[pos] = '\0';
	
	//fprintf(stderr, "Got next = %s, remainder = %s, looking up indexes...", next, remainder);
	
	result.remainderIndex = getRemainderIndex(remainder);
	
	// OK, now we need to translate "next" into two genes:
	if (pos > geneSize)
	{
		strncpy(gene1, next, geneSize);
		gene1[geneSize] = '\0';
		result.geneIndex1 = getGeneIndex(gene1);
		result.geneIndex2 = getGeneIndex(next + (geneSize * sizeof(char)));
		
	}
	else
	{
		result.geneIndex1 = getGeneIndex(next);
		result.geneIndex2 = 0;
	}
				
	return result;
}

/*
# This takes every combination of precursor and gene, and 
# calculates the next 2 genes in the sequence:
	
*/

void generateGeneSuccessors()
{
	unsigned int g;
	unsigned int p;

	zeroResult.geneIndex1 = 0;
	zeroResult.geneIndex2 = 0;
	zeroResult.remainderIndex = 0;

	fprintf(stderr, "Generating successor lists...");
	
	for (g = 0; g < geneCount; g++)
	{
		for (p = 0; p < 6; p++)
		{
			results[p][g] = calculateSuccessors(p, g);
		}
	}
	
	fprintf(stderr, "done.\n");
}



void initGeneSystem(int _max)
{
	int i, j;
	max = _max;
	char rgene[3];
	
	for (i = 0; i < max; i++)
	{
		iterations[i].number = i;
		iterations[i].remainderIndex = 0;
		iterations[i].length = 0;
		for (j = 0; j < 16; j++)
		{
			iterations[i].last16[j] = 0;
		}
		iterations[i].last16pos = 0;
	}
	
	geneHashTable = create_hashtable(6400, geneHash, genes_equal_fn);
	generateGenes();
	generateGeneSuccessors();

	// set up successor genes for the remainders as well:
	// - these are the genes that the remainder resolves to in the 
	// next generation, given that we know this is the end of the string
	remainderResultIndexes[0] = 0;
	for (i = 1; i < 6; i++)
	{
		// they're all 2-character genes:
		rgene[0] = strlen(validRemainders[i]) + '0';
		rgene[1] = validRemainders[i][0];
		rgene[2] = '\0';
		remainderResultIndexes[i] = getGeneIndex(rgene);
	}
	
}

void printGeneIteration(GENE_ITERATION *iteration)
{
	int pos, i;
	int printedLength = 0;
	char forPrinting[17];
	char *geneString;
	
	printf("%d : %llu : ", iteration->number + 2, iteration->length);
	
	forPrinting[0] = '\0';
	forPrinting[16] = '\0';
	if (iteration->length < 17)
	{
		//we want the whole lot:
		for (pos = 0; pos < iteration->last16pos; pos++)
		{
			strcat(forPrinting, genes[iteration->last16[pos]].geneString);
		}
	}
	else 
	{
		// we have to count up to 16 :)
		pos = (iteration->last16pos - 1) % 16;
		printedLength = 15;
		while (printedLength >= 0)
		{
			geneString = genes[iteration->last16[pos]].geneString;
			for (i = 0; i < genes[iteration->last16[pos]].length && printedLength >= 0; i++)
			{
				forPrinting[printedLength--] = geneString[strlen(geneString) - i - 1];
			}

			pos--;
			if (pos < 0) 
			{
				pos = 15;
			}
		}
	}

	
	printf("%s : \n", forPrinting);
}

void addGene(GENE_ITERATION *iteration, unsigned short geneIndex)
{
	if (geneIndex != 0)
	{
		iteration->length += genes[geneIndex].length;
		iteration->last16[iteration->last16pos] = geneIndex;
		iteration->last16pos = iteration->last16pos == 15 ? 0 : iteration->last16pos + 1;	
	}
}

void addRemainder(GENE_ITERATION *iteration)
{
	// When we reach the end, we have to add the gene that would correspond to the remainder.
	addGene(iteration, remainderResultIndexes[iteration->remainderIndex]);
}

/*
	CORE ALGORITHM STARTS HERE
 */
void nextGene(const unsigned short geneIndex, const unsigned int itnum, const unsigned short end)
{
	GENE_ITERATION *iteration = &iterations[itnum];
	
	iteration->tmpResult = &results[iteration->remainderIndex][geneIndex];
	
	addGene(iteration, iteration->tmpResult->geneIndex1);
	addGene(iteration, iteration->tmpResult->geneIndex2);
	
	iteration->remainderIndex = iteration->tmpResult->remainderIndex;
	
	if (end > 0)
	{
		addRemainder(iteration);
		printGeneIteration(iteration);
	}
	
	if (itnum < max - 1)
	{
		nextGene(iteration->tmpResult->geneIndex1, itnum + 1, 0);
		nextGene(iteration->tmpResult->geneIndex2, itnum + 1, 0);
		if (end > 0)
		{
			nextGene(remainderResultIndexes[iteration->remainderIndex], itnum + 1, 1);
		}
	}
}
