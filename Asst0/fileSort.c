#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

struct Node
{
	void* element;
	//struct Node* prev;
	struct Node* next;
};

void swap(struct Node* a, struct Node* b){
 	void* temp = a->element;
 	a->element = b->element;
 	b->element = temp;
 }

int insertionSort(void* toSort, int (*comparator)(void*,void*)){
		struct Node* ptr1 = (struct Node*) toSort;
		struct Node* ptr2 = (struct Node*) toSort;
		ptr2 = ptr2->next;
		while (ptr2) {
			while (ptr2!=ptr1) {
				if(comparator(ptr1->element,ptr2->element) > 0){
					swap(ptr1, ptr2);
				}
				ptr1 = ptr1->next;
			}
			ptr2 = ptr2->next;
			ptr1 = (struct Node*) toSort;
		}
		return 1;
}

struct Node *partition(struct Node *head, int (*comparator)(void*, void*)) { 
    struct Node *pivot = head; 
    struct Node *prev = pivot, *curr = pivot->next; 
    while (curr!=NULL){ 
     	if ((*comparator)(curr->element, pivot->element) < 0){ 
    		swap(pivot, pivot->next);
     		if(curr != pivot->next)
      		swap(pivot, curr);
      	pivot = pivot->next;
    	} 
    	prev = curr;
      curr = curr -> next;
    } 
  
    return pivot; 
} 
  
  

struct Node *qSort(struct Node *head, int (*comparator)(void*, void*)) { 
   if(head==NULL || head->next == NULL) 
   	return head; 
  
   struct Node* newHead = head; 
   struct Node* pivot = partition(head, comparator); 
  
 	if(newHead != pivot) { 
   	struct Node *temp = newHead; 
     	while (temp->next != pivot) 
     		temp = temp->next; 
  		temp->next = NULL; 
    	newHead = qSort(newHead, comparator);
    	temp = newHead;
    	while (temp != NULL && temp->next != NULL) 
       	temp = temp->next; 
      temp->next = pivot; 
    }
    pivot->next = qSort(pivot->next, comparator); 
    return newHead; 
} 
  

int quickSort(struct Node *toSort, int (*comparator)(void*, void*)) { 
    toSort = qSort(toSort, comparator); 
    return 1;
} 


int compare_int(void* a, void* b) {
    if(*(char*)a == '\0')
    	return -1;
    if(*(char*)b == '\0')
    	return 1;
    return atoi((char*) a) > atoi((char*) b) ? 1 : -1;
}

int compare_string(void* a, void* b) {
    int length = strlen(a) > strlen(b) ? strlen(b) : strlen(a);
    char *s1 = (char*) a;
    char *s2 = (char*) b;
    int i;
    for (i = 0; i < length; i++)
    {
        if (s1[i] == s2[i]) 
            continue;
        else {
            return s1[i] > s2[i] ? 1 : -1; 
        }
    }
   return strlen(a) > strlen(b) ? 1 : -1;
}



int main(int argc, char *argv[]){
	if(argc < 3){
		printf("Fatal Error: Program requires 2 parameters\n");
		return -1;
	}

	char* sortFlag = argv[1];
	char* fileName = argv[2];


	if(!(sortFlag[0] == '-' && sortFlag[2] == '\0' && (sortFlag[1] == 'q' || sortFlag[1] == 'i'))){
   	printf("Fatal Error: \"%s\" is not a valid sort flag\n", sortFlag);
   	return -1;
	}
  
	int fd = open(fileName, O_RDONLY);
  
 	if(fd == -1){
   	printf("Fatal Error: file \"%s\" does not exist\n", fileName);
   	close(fd);
   	return -1;
  	}


	//Read in File
	char c = '\0';
	char* data =(char*) malloc(1);
	if(data == NULL){
  		printf("Error: malloc() returned NULL\n");
  		close(fd);
   	return -1;
   }
	int n = 1;
	int maxLength = 0;
	int l = 0;
	int i = 0;
 	while(read(fd, &c, 1) > 0){
  		char* ptr = data;
  		if(c == ','){
   		ptr = (char*)realloc(data,++i);
   		ptr[i-1] = '\0';
      	n++;
      	if(++l > maxLength)maxLength = l;
      	l=0;
 		}else if(c == '\t' || c == '\n' || c == ' '){
    		continue;
    	}else{
     		ptr = (char*)realloc(data,++i);
     		ptr[i-1] = c;
      	l++;
   	}
   	data = (char*)ptr;
	}
	if(i == 0){
    	n=0;
    	//Warning: No Data in File
   	printf("Warning: File \"%s\" contains no readable data\n", fileName);
 	}else{
   	char* ptr = (char*)realloc(data,++i);  
   	ptr[i-1] = '\0';
   	if(++l > maxLength)maxLength = l;
   	data = (char*)ptr;
  
   	 //Tokenize
   	char* tokens[n];
   	for(i = 0; i < n; i++)
   	   tokens[i] = (char*)malloc(maxLength);

   	int j = 0;
   	int d = 0;
   	int firstNonEmpty = n-1;
   	for(i = 0; i < n; i++){    
      	for(j = 0; j < maxLength; j++){ 
				char c = data[d]; 
				tokens[i][j] = c;
				if(c != '\0'){ 
				  d++;	
				  if(i < firstNonEmpty)
					  firstNonEmpty = i;
				}
   	   }
   		d++;
   	}
  		if(tokens[n-1][0]=='\0')
   		n = n-1;
   		
   	//Select Comparator
   	int (*comparator)(void*, void*) = compare_string;    
		if(atoi(tokens[firstNonEmpty]) != 0 || tokens[firstNonEmpty][0] == '0')
   		comparator = compare_int;
     		
     	//Compile Linked List
		struct Node* list = (struct Node*)malloc(sizeof(struct Node));
   	if(list == NULL){
    		printf("Error: malloc() returned NULL\n");
    		close(fd);
  			free(data);
   		for(i = 0; i < n; i++)
  			  free(tokens[i]);
   		return -1;
   	}
   	struct Node* curr = list;
   	for(i = 0; i < n; i++){
   		struct Node* node = (struct Node*)malloc(sizeof(struct Node));
   		if(node == NULL){
    			printf("Error: malloc() returned NULL\n");
    			close(fd);
    			free(data);
   			for(i = 0; i < n; i++)
   			  free(tokens[i]);
   			free(list);
   			return -1;
   		}
     		if(i == 0){
				(*list).element = tokens[i];
      	}else{
				(*node).element = tokens[i];
				(*curr).next = node;
				curr = node;
			}
      }
      
      //Sort List
      if(sortFlag[1] == 'i')
			insertionSort(list, comparator);
     	else
			quickSort(list, comparator);
			
		//Print List
   	struct Node* node = list;
   	while(node!= NULL){
    		struct Node* temp = node;
			printf("%s\n", (char*)(*node).element);
			node = (*node).next;
			free(temp);
   	 }			
   	 
   	for(i = 0; i < n; i++)
   	   free(tokens[i]);
  }

  free(data);
  close(fd);
  return 1;
 }
