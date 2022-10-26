//-----------------------------------------
// NAME: Hritik Punj
// STUDENT NUMBER: 007923452
// COURSE: COMP 2160, SECTION: A01
// INSTRUCTOR: Mehdi Niknam
// ASSIGNMENT: Assignment 4
//
// PURPOSE: This program contains the implementation of the object manager. This program uses linked list to
//          keep track of existing objects. It clears the objects no longer required and makes space for new objects
//          I use one buffer at a time, while making the buffer compact we allocate memory for new buffer and after
//          transferring the objects we free the previous buffer and make the new buffer the active buffer. This way, we
//          are using two buffers, but maintaing only the current one at a time.
//-----------------------------------------

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "ObjectManager.h"


/*Struct for the implementation of Linked list*/
typedef struct NODE
{

    ulong start;       // The starting index in the buffer array
    ulong size;        // The size of the node
    Ref count;         // The refernce count
    Ref id;            // The id or unique identification of the node
    struct NODE *next; // The pointer to the next object
} node;


/* Global variables*/
static uchar *buffer = NULL; // Pointer to the buffer
static ulong freeptr = 0;    // The index of the first free space in the buffer array
static Ref nextRef = 1;      // A variable to store the next available reference ID
static node *head = NULL;    // The pointer to the head of the list
static Ref numNodes = 0;     // A variable to store number of nodes in the list


/*Function Prototypes*/
static void validateList();
static void addNode(node *);
static void deleteNode(Ref);
static node *findNode(Ref);
static void compact();


//------------------------------------------------------
// initPool
//
// PURPOSE: This function initiates the pool by allocating memory for the buffer
// RETURN VALUE:It doesnot return anything but allocates memory for a new pool if there
//              is none in existence, and prints a message to let the user know that there
//              is already a pool in existence.
//------------------------------------------------------
void initPool()
{
    // Since there is no pool in existance, malloc for new pool
    if (buffer == NULL)
    {
        buffer = (uchar *)malloc(MEMORY_SIZE);
        // Post conditions
        assert(buffer != NULL);
        // If unable to allocate, let the user know
        if (buffer == NULL)
        {
            fprintf(stderr, "Unable to initiate a pool\n");
        }

        assert(buffer != NULL);
        validateList();
    }
    else
    {
        fprintf(stderr, "There is already a pool in existence, cannot initiate another pool. Destroy the previous one to get a new pool\n");
    }
}


//------------------------------------------------------
// insertObject
//
// PURPOSE: This function allocates memory for the object on the buffer and adds new node
//          to the list corresponding to the object. if there is not enough memory on the 
//          buffer it calls compact() function to defragment the memory, and then try to 
//          insert again 
// INPUT PARAMETERS: ulong size: The size of the object for which we want to allocate memory
//                               on the buffer 
// RETURN VALUE:This function returns the unique reference id of the new object created
//              and NULL_REf if it was unable to allocate memory for the new object
//------------------------------------------------------
Ref insertObject(ulong size)
{
    // Preconditions
    validateList();

    /*A variable to store the return value of the function and initialising it to NULL_REF,
    we will update this if we successfully added the object*/
    Ref retValue = NULL_REF;

    // Buffer must be non null
    if (buffer != NULL)
    {
        // check if there is enough memory available, if not copy elements into the second buffer
        if ((MEMORY_SIZE - freeptr) < size)
        {
            // make the buffer compact and then add
            compact();
        }

        /*Check if there is enough memory now, if there is still not enough memory, then
        return NULL_REF since we cannot add new object*/
        if ((MEMORY_SIZE - freeptr) >= size)
        {
            node *newNode = (node *)malloc(sizeof(node));

            // Post conditions
            assert(newNode != NULL);
            if (newNode != NULL)
            {
                // Set the variablesin the new Node
                newNode->size = size;
                newNode->id = nextRef;
                nextRef++;            // Update nextRef to the next available free unique value of ID
                newNode->next = NULL; // The node is pointing to NULL
                newNode->start = freeptr;
                newNode->count = 1; // Update count by 1
                // Update freeptr to the next free index
                freeptr = freeptr + size;

                // Update the return value to the id of node
                retValue = newNode->id;

                // A temporary variable to store the current size of list
                Ref prevSize = numNodes;

                // Add node to the list
                addNode(newNode);

                // Postconditions
                assert(numNodes == (prevSize + 1));
                if (numNodes != (prevSize + 1))
                {
                    fprintf(stderr, "Unable to allocate memory for new object\n");
                    retValue = NULL_REF;
                }
            }
            else
            {
                fprintf(stderr, "Unable to allocate memory for new object\n");
            }
        }
        else
        {
            fprintf(stderr, "Unable to successfully complete memory allocation request.\n");
        }
    }

    validateList();

    return retValue;
}



//------------------------------------------------------
// retrieveObject
//
// PURPOSE: This function checks if a specific object is in the list by the reference id
//          If it is in the list it returns the pointer to the start of that object, and 
//          if the object is not in the list then it returns NULL  
// INPUT PARAMETERS: Ref ref: The refrence ID of the object whose location is required 
// RETURN VALUE:This function returns a pointer. If the object with the passed reference ID
//              is in the list then it returns the pointer to it's beginning else returns NULL
//------------------------------------------------------
void *retrieveObject(Ref ref)
{
    validateList();

    // A variable to store the pointer to the desired node or NULL if the node is nor in the list
    void *pointer = NULL;

    // Buffer must be non null
    if (buffer != NULL)
    {
        // Variable to store the pointer to the node which contains the passed reference ID
        node *refNode = findNode(ref);

        if (refNode != NULL)
        {
            Ref index = refNode->start; // Index of the start of the object in the buffer
            pointer = (void *)(&buffer[index]);
        }
    }

    validateList();

    return pointer;
}



//------------------------------------------------------
// addReference
//
// PURPOSE: This function adds 1 to the count of an object with the passed referance ID
//          If the object is in the list, it increases its count.
// INPUT PARAMETERS: Ref ref: The referance ID of the object whose count needs to be incremented 
// RETURN VALUE: This function does not return anything but increases the count of an object
//               with the passed reference ID
//------------------------------------------------------
void addReference(Ref ref)
{
    validateList();

    // Buffer must be non null
    if (buffer != NULL)
    {
        // Variable to store the pointer to the node which contains the passed reference ID
        node *refNode = findNode(ref);

        // If there exists a node with the ref ID, then increment the count
        if (refNode != NULL)
        {
            refNode->count = refNode->count + 1;
        }
    }

    validateList();
}



//------------------------------------------------------
// dropReference
//
// PURPOSE: This function subtracts 1 from the count of an object with the passed referance ID
//          If the object is in the list, it decreases its count. If the count becomes zero, it
//          removes the object from list
// INPUT PARAMETERS: Ref ref: The referance ID of the object whose count needs to be decreased 
// RETURN VALUE: This function does not return anything but decreases the count of an object
//               with the passed reference ID, and if the count becomes zero then remove the node
//               from the list 
//------------------------------------------------------
void dropReference(Ref ref)
{
    validateList();

    // Buffer must be non null
    if (buffer != NULL)
    {
        // A variable to store the pointer to the node with ID as ref
        node *reqNode = findNode(ref);
        // If there exists a node with this ID then decrease the count of that node
        if (reqNode != NULL)
        {
            // Decrease the count counter by 1
            reqNode->count = (reqNode->count - 1);

            // If the count is zero, then remove the node
            if (reqNode->count == 0)
            {
                // A variable to store the previous size of list
                Ref prevSize = numNodes;

                deleteNode(reqNode->id);

                // Postconditions
                assert(numNodes == (prevSize - 1));
                if(numNodes != (prevSize - 1))
                {
                    fprintf(stderr, "Unable to completely drop the reference\n");
                }
            }
        }
    }

    validateList();
}


//------------------------------------------------------
// destroyPool
//
// PURPOSE: This function destroys the pool. It deletes all the nodes and frees all the memory
//          corresponding to the nodes. After freeing the linked list, it frees the memory 
//          used by buffer
// RETURN VALUE: This function does not return anything but frees all the used heap memory
//------------------------------------------------------
void destroyPool()
{
    validateList();
    // Clear all nodes and then free the buffer memory

    // Buffer must be non null
    if (buffer != NULL)
    {
        node *current = head;
        node *temp;

        while (current != NULL)
        {
            temp = current->next;
            free(current);
            numNodes--;
            current = temp;
        }

        // After freeing set head to NULL
        head = NULL;
        // Set the nextRef to 1
        nextRef = 1;
        // Set freeptr to 0
        freeptr = 0;

        // post conditions
        assert(numNodes == 0);
        assert(head == NULL_REF);
        assert(nextRef == 1);

        // free the buffer
        free(buffer);
        buffer = NULL;

        // Postconditions
        assert(buffer == NULL);
    }
}



//------------------------------------------------------
// findNode
//
// PURPOSE: This function looks for a node in the list with the reference ID passed in as input.
//          If the node is in the list, it returns the pointer to that node, and returns NULL
//          if there is no node with the ID in the list.
// INPUT PARAMETERS: Ref ref: The referance ID of the an object whose corresponding node is requested 
// RETURN VALUE: This function returns the pointer to the node which has the passed reference ID
//------------------------------------------------------
static node *findNode(Ref ref)
{
    // preconditions
    validateList();

    // A variable to store the pointer to the desired node or NULL if it is not in the list
    node *current = NULL;

    // Buffer must be non null
    if (buffer != NULL)
    {
        // Iterate through the list and return the pointer to node which has Id same as ref
        current = head;

        while ((current != NULL) && (current->id != ref))
        {
            current = current->next;
        }
    }
    // Postconditions
    validateList();

    /*If ref is not in the list then current will be NULL, and if it is
    in the list current will point to that node*/
    return current;
}



//------------------------------------------------------
// dumpPool
//
// PURPOSE: This function traverses through the list of existing objects and prints some 
//          information about them
//------------------------------------------------------
void dumpPool()
{
    validateList();

    // Buffer must be non null
    if (buffer != NULL)
    {
        // Traverse through the list and print inforamtion
        node *current = head;

        while (current != NULL)
        {
            fprintf(stdout, "Reference ID is : %lu, Starting address is : %lu, Size is : %lu, count :%lu\n", current->id, current->start, current->size, current->count);
            current = current->next;
        }
    }

    validateList();
}



//------------------------------------------------------
// addNode
//
// PURPOSE: This function adds a node to the existing linked list
// INPUT PARAMETERS: node* newNode: The pointer to the node which needs to be added to the list 
// RETURN VALUE: This function does not return anything but updates the list by adding
//               the node
//------------------------------------------------------
static void addNode(node *newNode)
{
    validateList();

    // Buffer must be non null
    if (buffer != NULL)
    {
        // Iterate throught the list to the end, and add the node at the end
        // if this is the first node in the list, then head will be pointing to NULL_REF
        if (head == NULL)
        {
            head = newNode; // head must point to newNode if it is the first node in the list
        }
        else
        {
            node *current = head;
            node *temp;

            while (current != NULL)
            {
                temp = current;
                current = current->next;
            }

            // The last node must point to new Node
            temp->next = newNode;
        }

        // Increment the counter for number of nodes in the list
        numNodes++;
    }

    validateList();
}



//------------------------------------------------------
// deleteNode
//
// PURPOSE: This function deletes a node from the existing linked list
// INPUT PARAMETERS: Ref ref: The reference ID of the object whose corresponding node
//                            have to be removed from the list
// RETURN VALUE: This function does not return anything but updates the list by deleting
//               the node
//------------------------------------------------------
static void deleteNode(Ref ref)
{
    validateList();

    // Buffer must be non null
    if (buffer != NULL)
    {
        // Iterate through the list
        node *current = head;
        node *temp = NULL;

        while ((current != NULL) && (current->id != ref))
        {
            temp = current;
            current = current->next;
        }

        // If the node is in the list then remove it
        if (current != NULL)
        {
            //If it is the first element then make the head point to second node
            if(temp == NULL)
            {
                temp = head->next;
                free(head);
                head = temp;
            }
            else
            {
                // Make the left node point to the next node and then free the node
                temp->next = current->next;
                free(current);
            }

            // Decrement the number of nodes in list counter
            numNodes--;
        }
    }

    validateList();
}



//------------------------------------------------------
// compact
//
// PURPOSE: This function contains the implementation of defragmentation of buffer.
//          I maintain one buffer at a time. While doing defragmentation I create a 
//          new temporary buffer and copy everything over from the existing buffer, and then
//          free the memory used by existing buffer and make the temporary buffer the current/
//          existing buffer
// RETURN VALUE: This function does not return anything but defragments the buffer
//------------------------------------------------------
static void compact()
{
    validateList();

    // Buffer must be non null
    if (buffer != NULL)
    {
        // Allocate memory to new buffer
        uchar *tempBuffer = (uchar *)malloc(MEMORY_SIZE);

        assert(tempBuffer != NULL);
        if (tempBuffer != NULL)
        {
            // Copy all objects over to new buffer
            // Traverse through the list
            node *current = head;

            /*Store the freeptr value in a variable and  Update freeptr
            to the beginning of new buffer. The difference between the
            old freeptr and the new freeptr is the amount of memory freed*/
            ulong prevMem = freeptr;
            freeptr = 0;

            ulong deletedSize = 0; // A variable to store number of bytes cleared

            //A variable to store the next node in while loop
            node* updatedPointer = NULL;

            while (current != NULL)
            {
                // check if the count is zero, if it is then clear that node
                if (current->count == 0)
                {
                    // A variable to store the previous size of list
                    Ref prevSize = numNodes;

                    /*Update the counter before deleting the node, otherwise we will loose
                    access to next via the current because current will be freed*/
                    updatedPointer = current->next;

                    deleteNode(current->id);

                    // Postconditions
                    assert(numNodes == (prevSize - 1));
                    if (numNodes != (prevSize - 1))
                    {
                        fprintf(stderr, "Unsuccessful in deleting object\n");
                    }
                }
                else
                {
                    // Copy over
                    memcpy(&(tempBuffer[freeptr]), &(buffer[current->start]), current->size);

                    // Change the start
                    current->start = freeptr;

                    // Update freeptr
                    freeptr = freeptr + current->size;

                    // Update the counter
                    updatedPointer = current->next;
                }

                current = updatedPointer;
            }

            // free the previous buffer
            free(buffer);

            // make tempBuffer the active buffer
            buffer = tempBuffer;

            // Update deleted size
            deletedSize = (prevMem - freeptr);

            // Print summary
            fprintf(stdout, "\nGarbage collector statistics:\n");
            fprintf(stdout, "objects: %lu   bytes in use: %lu   freed: %lu\n\n", numNodes, freeptr, deletedSize);
        }
    }

    validateList();
}



//------------------------------------------------------
// validateList
//
// PURPOSE: This function is an invariant
//------------------------------------------------------
static void validateList()
{

    // If the list is empty, that means buffer is unused
    if (numNodes == 0)
    {
        assert(head == NULL);
    }
    else
    {
        // Traverse through the list
        node *current = head;
        node *temp;
        Ref number = 0;      // A variable to store the number of nodes in the list
        ulong totalSize = 0; // A variable to store the total occupied size from the buffer

        while (current != NULL)
        {
            number++;
            totalSize += current->size;
            temp = current;
            current = current->next;
        }

        // The reference id available must be greater than the reference id of the last node
        assert(temp->id < nextRef);
        // The number of nodes must equal the global variable numNodes
        assert(numNodes == number);
        // The last item must fit entilely inside the buffer array
        assert((totalSize) <= (MEMORY_SIZE));
    }
}
