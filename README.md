# Shell_Clone

Implementation of Unix-like shell 

The shell supports features like executing commands, changing directories (`cd`), exiting the shell (`exit`), input/output redirection (`>`, `<`), and pipes (`|`).

## Features Implemented

1. **Basic Structure:** The shell has a main loop that reads user input and executes commands.

2. **Parse User Input:** User input is parsed into commands and arguments.

3. **Fork and Execute:** The shell can fork a new process and execute commands.

4. **Built-in Commands:** Support for built-in commands like `cd` (change directory) and `exit` (exit the shell).

5. **Input/Output Redirection:** Ability to redirect standard input and output using `<` and `>` symbols.

6. **Pipes:** Support for connecting the output of one command to the input of another using the `|` symbol.

7.**Background process execution** When a background process terminates, the shell displays a message indicating the termination status.

8.**Signal handling (Ctrl+C, Ctrl+Z)** 

## Examples:
**Change directory:**   cd /path/to/directory

**Execute a command:**  ls -l

**Exit the shell:**     exit

**Redirect output to a file:**   ls > output.txt

**Redirect input from a file:**   cat < input.txt

**Use pipes:**   ls | grep keyword
