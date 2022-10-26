//-----------------------------------------
// NAME: Hritik Punj
// STUDENT NUMBER: 007923452
// COURSE: COMP 2160, SECTION: A01
// INSTRUCTOR: Mehdi Niknam
// ASSIGNMENT: Assignment 4
//
// PURPOSE: This program contains the test set for checking the functionality of object manager
//-----------------------------------------
 
#include "ObjectManager.h"
#include <stdio.h>

//Global variables
int testsExecuted = 0;
int testsFailed = 0;
#define OBJECTS 2000


//Function prototypes
Ref testAdd(Ref, ulong);
void *testRet(Ref expected, Ref id);
void tester(int d[], int i);

int main(int argc, char *argv[])
{
  void *ptr;
  Ref currentRef = 1;
  Ref id[20];


  //Initialize memory pool for testing
  initPool();
  printf("Beginning testing..................\n\n");
  

  //Testing insertObject function
  printf("Testing the insertObject function\n");
  printf("Adding 20 new objects :\n");
  for(int i = 0; i < 20; i++)
  {
    id[i] = testAdd(currentRef, 100);
    currentRef++;
  }
  printf("Trying to add too big objects, these must not be added and garbage collector must be triggered:\n");
  testAdd(NULL_REF, MEMORY_SIZE);
  testAdd(NULL_REF, MEMORY_SIZE-100);


  //Testing retreiveObject and dropreference functions
  printf("\nTesting the retreiveObject and dropReference functions: \n");
  printf("Dropping reference of first five objects: \n");
  for(int i = 0; i < 5; i++)
  {
    dropReference(id[i]);
  }
  printf("Trying to access the deleted objects: \n");
  for(int i = 0; i < 5; i ++)
  {
    testRet(0, id[i]);
  }
  printf("Access the existing objects: \n");
  for(int i = 5; i < 20; i ++)
  {
    testRet(1, id[i]);
  }
  


  //Testing addreference function
  printf("\nTesting addReference function: \n");
  printf("Adding reference of the objects from id 6 to 10:\n");
  for(int i = 5; i < 10; i ++)
  {
    addReference(id[i]);
  }
  printf("Dropping reference of the objects from id 6 to 10\n");
  for(int i = 5; i < 10; i ++)
  {
    dropReference(id[i]);
  }
  printf("Access the objects from id 6 to 10, they must exist: \n");
  for(int i = 5; i < 10; i ++)
  {
    testRet(1, id[i]);
  }
  


  //Testing if the garbage collector runs
  printf("\nTesting the garbage collector: \n");
  printf("Dropping all objects:\n");
  for(int i = 0; i < 20; i ++)
  {
    dropReference(id[i]);
  }
  //Adding two objects with large size
  printf("Adding two objects with size a quarter of the maximum buffer size\n");
  id[0] = testAdd(currentRef, MEMORY_SIZE/4);
  currentRef++;
  id[1] = testAdd(currentRef, MEMORY_SIZE/4);
  currentRef++;
  printf("Trying to add one object with size three quarters of the maximum buffer size:\n");
  printf("Must trigger garbage collector, and object must not be inserted:\n");
  testAdd(NULL_REF, (3*MEMORY_SIZE)/4);
  printf("\nDropping reference of first two objects, and then trying to add again:\n");
  dropReference(id[0]);
  dropReference(id[1]);
  printf("Now, Trying to add one object with size three quarters of the maximum buffer size:\n");
  printf("It must be added now but must trigger garbage collector:\n");
  testAdd(currentRef, (3*MEMORY_SIZE)/4);


  //Testing dumppool
  printf("\nTesting dumpPool :\n");
  printf("Only one object in memory, so dumppool will print one object:\n");
  dumpPool();


  //Destroying pool
  printf("\nDestroying existing pool:\n");
  destroyPool();

  //Testing edge cases
  printf("\nTesting egde cases\n");
  printf("Program must not terminate:\n");
  ptr = testRet(0, currentRef);
  testAdd(NULL_REF, 1000);
  dropReference(currentRef);
  addReference(currentRef);

  printf("\n\nTesting for few other cases:\n\n");
  initPool();
  int d[OBJECTS];
  //Testing
  for ( int i=0 ; i<(OBJECTS) ; i++ )
  {
    tester(d, i);
  }
  dumpPool();
  destroyPool();



  /*printf("\n\nTesting for few other cases:\n\n");
  initPool();
  int p[4];
  //Testing
  for ( int i=0 ; i<(4) ; i++ )
  {
    p[i] = insertObject(136000);
    dropReference(d[i]);
  }
  dumpPool();
  destroyPool();*/

  //Print summary
  printf("\nTEST SUMMARY :\n");
  printf("Tests executed : %d\n", testsExecuted);
  printf("Tests failed : %d\n", testsFailed);
  printf("\nEnd of processing.......\n");
  
  return 0;
}



//------------------------------------------------------
// testAdd
//
// PURPOSE: This function tests the insertObject function
// INPUT PARAMETERS: Ref expected: The expected return value from the insertObject function
//                   ulong size: The size of the object which needs to be inserted
// RETURN VALUE: This function return the reference id of the object inserted if successful,
//               and NULL_REF otherwise
//------------------------------------------------------
Ref testAdd(Ref expected, ulong size)
{
  Ref ans = insertObject(size);
  testsExecuted++;

  if(ans == expected)
  {
    if(ans)
    {
      printf("Passed, the object is inserted and the ID is %lu\n", ans);
    }
    else
    {
      printf("Passed, the object not inserted.\n");
    }
  }
  else
  {
    if(expected)
    {
      printf("Failed, the object is supposed to be inserted but it is not inserted\n");
    }
    else
    {
      printf("Failed, the object is not supposed to be inserted but it is inserted\n");
    }
    testsFailed++;
  }
  return ans;
}


//------------------------------------------------------
// testRet
//
// PURPOSE: This function tests the retreiveObject function
// INPUT PARAMETERS: Ref expected: 0 or 1, 1 if retreiveObject returns a valid non-null pointer,
//                                and 0 if retreiveObject function returns NULL
//                   Ref Id: The id of the object whose start address is requested
// RETURN VALUE: This function return pointer to the start address of the object if it
//               exists in the memory, and NULL otherwise
//------------------------------------------------------
void *testRet(Ref expected, Ref id)
{
  void* ans = retrieveObject(id);
  testsExecuted++;

  if(expected == 0)
  {
    if(ans == NULL)
    {
      printf("Passed, the function returned NULL as there does not exist an object with ID = %lu\n", id);
    }
    else
    {
      testsFailed++;
      printf("Failed, the function has not returned NULL but it was expected to as there does not exist an object with ID = %lu\n", id);
    }
  }
  else 
  {
    if(ans == NULL)
    {
      testsFailed++;
      printf("Failed, the function returned NULL but it was expected to return a valid address as there does exists an object with ID = %lu\n", id);
    }
    else
    {
      printf("Passed, the function has not returned NULL and it was expected to as there exists an object with ID = %lu\n", id);
    }
  }

  return ans;
}

void tester(int d[], int i)
{
  static Ref ID;
  d[i] = insertObject(1036);
  dropReference(d[i]);
  ID = insertObject(1036);
}