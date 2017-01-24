
/**
 * Neill Lewis
 * 
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Implements a dictionary's functionality.
 */


#include <stdbool.h>
#include <stdlib.h>
#include <string.h>     //strlen
#include <strings.h>    //strcasecmp
#include <stdio.h>      //printf
#include <ctype.h>      
#include "dictionary.h"

#define HASHTABLE_SIZE 16384//modify as needed in correlation with dictionary input size
#define MEANING_OF_LIFE 42 //just for fun

typedef struct Node
{
    char word[LENGTH+1];
    struct Node* next;
}Node;

Node* hashtable[HASHTABLE_SIZE];

unsigned int numWords = 0;


/**
 * HASH FUNCTION
 * from user 'delipity' at
 * https://www.reddit.com/r/cs50/comments/1x6vc8/pset6_trie_vs_hashtable/
**/

int hash_it(const char* needs_hashing)
{
    unsigned int hash = 0;
    for (int i=0, n=strlen(needs_hashing); i<n; i++)
        hash = (hash << 2) ^ needs_hashing[i];
    return hash % HASHTABLE_SIZE;
}

/**
 * Jenkin's One at A Time Hash
 * Adapted with some love from https://en.wikipedia.org/wiki/djb2_hash_function
 */
 
 
unsigned int jenkins_hash(char *key)
{
    unsigned int hash, i;
    for(hash = i = 0; i < strlen(key); ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash % HASHTABLE_SIZE;
}

/**
 * DJB2 Hash
 * http://www.cse.yorku.ca/~oz/hash.html
 **/

unsigned int djb2_hash(char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % HASHTABLE_SIZE;
}


/**
 * Recursively free nodes
 * from response by "Bradley Swain"
 * http://stackoverflow.com/questions/6417158/c-how-to-free-nodes-in-the-linked-list
**/
 
void freeList(struct Node* currentNode)
{
    if(currentNode->next != NULL) { freeList(currentNode->next); }
    free(currentNode);
}


/**
 * string to-lower
 * http://cboard.cprogramming.com/c-programming/88987-strcmp-problem-capital-letters.html
 */

char* strLower( char* str ) {
  char *temp = NULL;
 
  for ( temp = str; *temp; temp++ ) {
    *temp = tolower( *temp );
  }
 
  return str;
  //free(temp);
}





/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{

    char* lower_word;
    lower_word = (char*)malloc(strlen(word) + 1); //declare a string to hold all chars and end
    strcpy(lower_word,word);
    lower_word = strLower(lower_word); //make word lowercase
    int lower_hashvalue = djb2_hash(lower_word);
    free(lower_word); // SEE COMMENT BELOW
    //printf("\nword: %s hashvalue: %d | lower word: %s hashvalue:%d\n",word,hashvalue,lower_word,lower_hashvalue); //for error testing
    if(hashtable[lower_hashvalue] == NULL) { //if word is not in the dictionary
        return false;
    } else //if word MAY be in a linked list 
    {
        Node* searcher = hashtable[lower_hashvalue]; //make a search node
        do{
            if(strcasecmp(searcher->word,word) == 0) {return true;} //compare the dictionary word and the lowercased word //SEE CS50 NOTES BELOW
            searcher = searcher->next; //update searcher to the next pointer
        }while(searcher != NULL); //until the end of the linked list
    }
    return false;
}




/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    
    FILE* dptr = fopen(dictionary, "r"); //open the dictionary, load it into file pointer dptr (dictionary pointer)
    if(dptr == NULL) // if unable to open the dictionary
    {
        printf("Unable to load the dictionary file.\n");
        return false;
    }
    
    char currentWord[LENGTH+1]; // +1 for string terminator '\0'
    int index = 0;
    for (int c = fgetc(dptr); c != EOF; c = fgetc(dptr)) //get characters until end of document
    {
        if(isalpha(c) || c == '\'') //assume all words in dictionary are alpha or with apostrophe 
        {
            currentWord[index] = c;
            index++;
        } 
        else
        {
            currentWord[index] = '\0'; //end the current word because you encountered non alpha  non apostrophe character
            index = 0; //reset index to 0 to prepare for next iteration
            int hashvalue = djb2_hash(currentWord); //get hashvalue of new word
            //printf("current word %s | location %d\n",currentWord,hashvalue); //error checking
            Node* newNode = malloc(sizeof(Node)); //make a new node
            if(newNode == NULL) {
                printf("unable to complete malloc() of new node\n");
                return false;
            }
            strcpy(newNode->word, currentWord);//put the current word in the node
            numWords++;
            if(hashtable[hashvalue] == NULL) //if hashtable value isn't currently pointing to a node
            {
                //point hashhtable[hashvalue] to the new node with the word
                hashtable[hashvalue] = newNode;
            }
            else 
            {
                //if yes, copy hashtable[hashvalue] to newnode->next and change hashtable[hashvalue] to point to newnode.
                newNode->next = hashtable[hashvalue];
                hashtable[hashvalue] = newNode;
            }
        }
    }
    //put all characters into a string
    //pass string to the hash function
    fclose(dptr);
    return true;
}

/**
 *  Node Printer
 *  Prints Hashtable to Terminal
 *  For Evaluative Purposes
 */
void nodesPrint(void)
{
    for(int i = 0; i < HASHTABLE_SIZE; i++)
    {
        if(hashtable[i] == NULL) {
            printf("hashtable[%d] is null\n",i);
        } else {
            printf("hashtable[%d]",i);
            Node* searcher = hashtable[i];
            do{
                printf("-%s-",searcher->word);
                searcher = searcher->next;
                  
            }while(searcher != NULL);
            printf("\n");
        }
    }
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return numWords;
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{
    //nodesPrint();
    for(int i = 0; i < HASHTABLE_SIZE; i++)
        
        if(hashtable[i] != NULL)
        {
            Node* ucursor = hashtable[i];
            freeList(ucursor);
        }
    
    return true;
}
