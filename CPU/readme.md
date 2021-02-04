# Processor

Simple processor realisation with assembler.

## Build

You can build a project from under clion using mingw

I couldn't build it directly using cmake :(

## Run
Run CPU, filename is path to code in pseudo-asm

    processor/processor.exe <filename>

----

Run assembler, input is path to file with code in pseudo-asm,
output - path to file where bytecodes will be written

    assembler/assembler.exe <input> <output>

----

There are example files in examples folder

#Available commands

| Mnemonic     | Op code       | Description  |
| -------------|:-------------:| ------------ |
| push double  | 00            | Pushes double value to stack |
| push [abcd]x | 01            | Pops last value from stack and writes it register |
| pop          | 02            | Pops last value from stack |
| pop  [abcd]x | 03            | Pops last value from stack and writes it register |
| add          | 04            | Pops two last values from stack and pushes their sum |
| sub          | 05            | Pops two last values from stack and pushes their subtraction result
| mul          | 06            | Pops two last values from stack and pushes their multiplication result |
| div          | 07            | Pops two last values from stack and pushes their division result |
| sin          | 08            | Pops last value from stack and pushes it's sinus |
| cos          | 09            | Pops last value from stack and pushes it's cosine |
| sqrt         | 10            | Pops last value from stack and pushes it's square root |
| jmp          | 11            | Unconditionally jumps to the label |
| je           | 12            | Pops two last values from stack and jumps to given label if second popped value is equal to the first one |
| jne          | 13            | Pops two last values from stack and jumps to given label if second popped value is not equal to the first one |
| ja           | 14            | Pops two last values from stack and jumps to given label if second popped value is above then the first one |
| jae          | 15            | Pops two last values from stack and jumps to given label if second popped value is above then or equal to the first one |
| jb           | 16            | Pops two last values from stack and jumps to given label if second popped value is below then the first one |
| ibe          | 17            | Pops two last values from stack and jumps to given label if second popped value is below then or equal to the first one |
| call  lbl n  | 18            | Saves current position in call stack, pop n elements from current data stack to function data stack, save current register values, current data stack and jumps to given label |
| ret n        | 19            | Returns to the last saved position in call stack, just after the last executed call command. Restoring register values and push n elements from function data stack to restored data stack |
| stop         | 20            | Stops the execution |
| out          | 21            | Peeks last value from stack and writes to console |

