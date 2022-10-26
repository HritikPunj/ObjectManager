#ObjectManager 

This program manages memory by itself. In C language there is no memory management, I have tried
to induce that behaviour by making this object manager. I have implemented this with all error checks
and included all assumptions using assert statements, and exception/error handling by using if statements

##Files : 
* main.c - This function contains the test set for the object manager 
* ObjectManager.h - The header file for the object manager
* ObjectManager.c - The implementation of object manager
* Object.h - A C++ style header, which contains classes, this file is not required. I created it to 
             test my code by creating some objects of these classes. 
* Makefile - This file contains commands to complile the program

##How to compile:
Place main.c, ObjectManager.c, ObjectManager.h, Object.h, and Makefile in the same directory.
Compile using the following command - 
```
make
```
You can remove the object files by the following command
```
make clean
```
###Additional Comments
Optionally, if you want to use C++ to use this better by making objects, you need to edit the Makefile.
Replace the clang with clang++

