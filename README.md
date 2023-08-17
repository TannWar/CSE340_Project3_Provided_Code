
# Linked List Compiler

The project involved creating a small compiler that reads an input program and translates it into a linked list structure. Each node in the linked list represents an instruction from the program, specifying its type and operands. For jump instructions, the next instruction to execute is also defined. The compiler then interprets the generated list of instructions, traversing the data structure and executing each node, which involves modifying memory locations corresponding to operands and determining the next instruction to execute. The compiler's output should match the expected output of the input program.

## Run

To run this project:

1. Clone the repository:
```bash
git clone https://github.com/TannWar/LinkedListCompiler.git
```

2. Navigate to the project directory:
```bash
cd LinkedListCompiler
```
3. Compile the code:
```bash
g++ -o compiler main.cpp
```
4. Run the compiler:
```bash
./compiler
```


## How to Use

1. Write your input program in a text file, e.g., 'input_program.txt' or use one of the provided test cases

2. Write your input program in a text file, e.g., 'input_program.txt'
```bash
  ./a.out < input_program
  OR
  ./a.out < test.txt
```
3. The compiler will generate the intermediate representation and execute the program, providing the output.

```css
a, b;
{
  a , b, c, i;
{
a = 1;
b = 1;
output a;
output b;

i = 3;
WHILE i < 10 {
        c = a + b;
	output c;
        a = b;
        b = c;
	i = i+1;
}

}
3 2 1 4 2
```
    
## Note

This project aims to demonstrate the implementation of a small compiler for educational purposes. It reads a simplified programming language and generates an intermediate representation in a linked list format. The interpreter provided in this project is designed for illustration purposes and may not cover all edge cases or optimizations.
## Acknowledgements

This project is inspired by a programming assignment from a course. The interpreter code and test cases are adapted from the course materials.

