typedef struct node {
    void * value;
    struct node * next;
} node;

node* newList(void *value);
node* addNode(node *head, void *value);
node* rmNode(node *head, void *value);
int contains(node *head, void *value);
void printList(node *head);
void freeList(node *head);

/*
 * returns the head with value of a new list
 */
node* newList(void *value) {
    node *output = malloc(sizeof(node));

    if (output == NULL) {
        return NULL;
    }

    output->value = value;
    output->next = NULL;

    return output;
}

/*
 * adds a node to the start of the list starting at head
 */
node* addNode(node *head, void *value) {
    node *newNode = malloc(sizeof(node));
    newNode->value = value;
    newNode->next = head;
    return newNode;
}

/*
 * frees the memory storing list starting at head
 */
void freeList(node *head) {
    if (head->next != NULL) {
        freeList(head->next);
    } 
    free(head->value);
    free(head);
}

/*
 * removes the first node with value from list starting at head
 */
node* rmNode(node *head, void *value) {
    if (head == NULL)
        return NULL;

    if (head->value == value) {
        node *tempNext = head->next;
        free(head);
        return tempNext;
    }

    head->next = rmNode(head->next, value);
    return head;
}

/*
 * returns 1 if value is contained in list, else 0
 */
int contains(node *head, void *value) {
    if (head->value == value)
        return 1;

    if (head->next != NULL)
        return contains(head->next, value);

    return 0;
}

