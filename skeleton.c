#include <stdio.h> 
#include <string.h>   /* for all the new-fangled string functions */
#include <stdlib.h>     /* malloc, free, rand */

int Fsize=50;
int cases=10;


int i;
int j;

/*typedef struct tableau tableau;*/

struct tableau {
  char *root;
  struct  tableau *left;
  struct tableau *right;
  struct tableau *parent;
}*tab, *node, *node1, *kid, *pa;



/*put all your functions here.  You will need
1.
int parse(char *g) which returns 1 if a proposition, 2 if neg, 3 if binary, ow 0
2. 
void complete(struct tableau *t)
which expands the root of the tableau and recursively completes any child tableaus.
3. 
int closed(struct tableau *t)
which checks if the whole tableau is closed.
Of course you will almost certainly need many other functions.

You may vary this program provided it reads 10 formulas in a file called "input.txt" and outputs in the way indicated below to a file called "output.txt".
*/

int parse(char *g)
/* returns 0 for non-formulas, 1 for propositions, 2 for negations, 3 for binary connective fmlas.*/
{
  int tempType = 0; // The int to be returned

  tempType = getInitTempType(g); // Test the first character to see what the formula should be
  if (tempType > 0)
  {
    tempType *= checkValid(g+1, tempType);
  }
  
  return tempType;
}

int subParse(char *g)
{
  int tempType = 0; // The int to be returned
  int valid = 0;

  tempType = getInitTempType(g); // Test the first character to see what the formula should be
  if (tempType > 0)
  {
    valid = checkValid(g+1, tempType);
  }

  return valid;
}

int checkValid(char *s, int temp)
{
  int valid = 1;

  switch (temp)
  {
    case 1 :
      valid = checkValidProposition(s);
      break;
    case 2 :
      valid = subParse(s);
      break;
    case 3 :
      valid = checkValidBin(s);
      break;
    default :
      valid = 0;
  }

  return valid;
}

int checkValidProposition(char *s) {
  return (*s != '\0') ? 0 : 1;
}

int checkValidBin(char *s)
{
  int validA = 0, validCon = 0, validB = 0;
  int count = 0;
  int bracketCount = 0;

  while (*(s+count) != '\0')
  {
    bracketCount += bracketType((s+count));
    if (bracketCount == 0 && (*(s+count) == 'v' || *(s+count) == '^' || *(s+count) == '>'))
    {
      char *partA = malloc((count)*sizeof(char));
      memcpy(partA, s, count);
      validA = subParse(partA);

      char binCon = *(s+count);
      validCon = 1;

      char *partB = malloc((strlen(s) - 2 - count)*sizeof(char));
      memcpy(partB, s+count+1, strlen(s) - 2 - count);
      validB = subParse(partB);
    }
      
    count++;
  }
  return validA * validCon * validB;
}

int getInitTempType(char *c)
{
  switch (*c)
  {
    case 'p' :
    case 'q' :
    case 'r' :
      return 1;
      break;
    case '-' :
      return 2;
      break;
    case '(' :
      return 3;
      break;
    default:
      return 0;
      break;
  }
}

int bracketType(char *c)
{
  if (*c == '(')
  {
    return 1;
  }
  else if (*c == ')')
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

char *partone(char *g)
/*
Given a formula (A*B) this should return A
 */
{
  int bracketCount = 0;
  int count = 0;
  char *partA = malloc(strlen(g)*sizeof(char));

  while (*(g+count) != '\0')
  {
    bracketCount += bracketType((g+count));
    if (bracketCount == 1 && (*(g+count) == 'v' || *(g+count) == '^' || *(g+count) == '>'))
    {
      memcpy(partA, g+1, count-1);

    }  
    count++;
  }
  return partA;
}

char *parttwo(char *g)
/*
Given a formula (A*B) this should return B
 */
{
  int bracketCount = 0;
  int count = 0;
  char *partB = malloc(strlen(g)*sizeof(char));

  while (*(g+count) != '\0')
  {
    bracketCount += bracketType((g+count));
    if (bracketCount == 1 && (*(g+count) == 'v' || *(g+count) == '^' || *(g+count) == '>'))
    {
      memcpy(partB, g+count+1, strlen(g) - 2 - count);
    }  
    count++;
  }
  return partB;
}

char bin(char *g)
/*
Given a formula (A*B) this should return the character *
 */
{
  int bracketCount = 0;
  int count = 0;
  char binCon;

  while (*(g+count) != '\0')
  {
    bracketCount += bracketType((g+count));
    if (bracketCount == 1 && (*(g+count) == 'v' || *(g+count) == '^' || *(g+count) == '>'))
    {
      binCon = *(g+count);
    }  
    count++;
  }
  return binCon; 
}

void addChildren(struct tableau *t, struct tableau *current)
{
  char *negation = malloc(sizeof(char));
  *negation = '-';
  char *negation2 = malloc(sizeof(char));
  *negation2 = '-';


  switch(parse(t->root)) // may fail when passed an empty node
  {
    // case 1: 
    //   break;
    case 2:
      // if parsing the rest of the string returns 1 then leave it
      switch(parse(t->root+1))
      {
        case 2: // if parsing the rest of the string returns 2 then this is a double negation -> if --A then add A to leftmost child
          current->left = malloc(sizeof(struct tableau));
          *(current->left) = (struct tableau){t->root+2, NULL, NULL, current};
          break;
        case 3: // if parsing the rest of the string returns 3 the work out what kind of binary connective and go from there 
          switch(bin(t->root+1))
          {
            case '^': // beta - negation of part 1 and 2
              current->left = malloc(sizeof(struct tableau));
              *(current->left) = (struct tableau){strcat(negation, partone(t->root+1)), NULL, NULL, current};
              current->right = malloc(sizeof(struct tableau));
              *(current->right) = (struct tableau){strcat(negation2, parttwo(t->root+1)), NULL, NULL, current};
              break;
            case 'v': // alpha - negation of part 1 and 2
              current->left = malloc(sizeof(struct tableau));
              *(current->left) = (struct tableau){strcat(negation, partone(t->root+1)), NULL, NULL, current};
              current = current->left;
              current->left = malloc(sizeof(struct tableau));
              *(current->left) = (struct tableau){strcat(negation2, parttwo(t->root+1)), NULL, NULL, current};
              break;
            case '>': // alpha - part 1 and negation of part 2
              current->left = malloc(sizeof(struct tableau));
              *(current->left) = (struct tableau){partone(t->root+1), NULL, NULL, current};
              current = current->left;
              current->left = malloc(sizeof(struct tableau));
              *(current->left) = (struct tableau){strcat(negation, parttwo(t->root+1)), NULL, NULL, current};
              break;
          }
          break;
      }
      break;
    case 3: 
      switch(bin(t->root))
      {
        case '^': // alpha - part 1 and 2
          current->left = malloc(sizeof(struct tableau));
          *(current->left) = (struct tableau){partone(t->root), NULL, NULL, current};
          current = current->left;
          current->left = malloc(sizeof(struct tableau));
          *(current->left) = (struct tableau){parttwo(t->root), NULL, NULL, current};
          break;
        case 'v': // beta - part 1 and 2
          current->left = malloc(sizeof(struct tableau));
          *(current->left) = (struct tableau){partone(t->root), NULL, NULL, current};
          current->right = malloc(sizeof(struct tableau));
          *(current->right) = (struct tableau){parttwo(t->root), NULL, NULL, current};
          break;
        case '>': // beta - negation of part 1 and part 2
          current->left = malloc(sizeof(struct tableau));
          *(current->left) = (struct tableau){strcat(negation, partone(t->root)), NULL, NULL, current};
          current->right = malloc(sizeof(struct tableau));
          *(current->right) = (struct tableau){parttwo(t->root), NULL, NULL, current};
          break;
      }
      break;
  }
}

void traverseToLeaf(struct tableau *t, struct tableau *current)
{
  if(current->left == NULL)
  {
    addChildren(t, current);
  }
  else
  {
    if (current->left != NULL)
    {
      traverseToLeaf(t, current->left);
    }
    if (current->right != NULL)
    {
      traverseToLeaf(t, current->right);
    }
  }
  
}

void complete(struct tableau *t)
{
  // alpha formulas
  // (A^B), -(AvB), -(A>B), --A

  // beta formulas
  // (AvB), (A>B), -(A^B)
  if(strlen(t->root) < 1)
  {
    return;
  }
  
  struct tableau *current = malloc(sizeof(struct tableau));
  current = t;
  traverseToLeaf(t, current);

  
  if(t->left != NULL)
  {
    complete(t->left);
  }
  if(t->right != NULL)
  {
    complete(t->right);
  }

}

int isLiteral(char* c)
{
  if(parse(c)==1)
  {
    return 1;
  }
  else if(parse(c)==2)
  {
    if(parse(c+1)==1)
    {
      return 1;
    }
  }
  return 0;
}

int traverseUp(struct tableau *t)
{  
  while(t->parent != NULL)
  {
    char *literal = malloc(sizeof(char)*5);
    strcpy(literal, t->root);
    if(isLiteral(literal))
    {
      char *check = malloc(sizeof(char)*5);
      if(parse(literal)==1) 
      {
        *check = '-';
        *(check+1) = *literal;
        *(check+2) = '\0';
      }
      else
      {
        printf("Literal: %s\n", literal);
        *check = *(literal+1);
        *(check+1) = '\0';
        printf("Check: %s\n", check);
      }
      struct tableau *current = malloc(sizeof(struct tableau));
      current = t->parent;
      while(current->parent != NULL)
      {
        if(strcmp(current->root, check) == 0)
        {
          printf("Hey\n");
          return 1;
        }
        current = current->parent;
      }
    }
    t = t->parent;
  }
  return 0;
}

// which checks if the whole tableau is closed.
int closed(struct tableau *t)
{
  int ret = 1;
  if(t->left == NULL) {
    ret *= traverseUp(t);
  }
  else
  {
    if(t->left != NULL) {
      ret *= closed(t->left);
    }
    if(t->right != NULL) {
      ret *= closed(t->right);
    }
  }
  return ret;
}
 


int main()

{ /*input a string and check if its a propositional formula */


  char *name = malloc(Fsize);
  FILE *fp, *fpout;
  /* reads from input.txt, writes to output.txt*/
  if ((  fp=fopen("input.txt","r"))==NULL){printf("Error opening file");exit(1);}
  if ((  fpout=fopen("output2.txt","w"))==NULL){printf("Error opening file");exit(1);}

  int j;
  for(j=0;j<cases;j++)
  {
    fscanf(fp, "%s",name);/*read formula*/
    switch (parse(name))
	  {
      case(0): 
        fprintf(fpout, "%s is not a formula.  ", name);
        break;
	    case(1): 
        fprintf(fpout, "%s is a proposition.  ", name);
        break;
      case(2): 
        fprintf(fpout, "%s is a negation.  ", name);
        break;
      case(3): 
        fprintf(fpout, "%s is a binary.  ", name);
        break;
      default: 
        fprintf(fpout, "What the f***!  ");
    }
    /*make new tableau with name at root, no children, no parent*/
    struct tableau t={name, NULL, NULL, NULL};

    /*expand the root, recursively complete the children*/
    if (parse(name)!=0)
    {
      complete(&t);
      if (closed(&t)) fprintf(fpout, "%s is not satisfiable.\n", name);
      else fprintf(fpout, "%s is satisfiable.\n", name);
    }
    else 
    { 
      fprintf(fpout, "I told you, %s is not a formula.\n", name);
    }
  } // endfor

  fclose(fp);
  fclose(fpout);
  free(name);

  return(0);
}