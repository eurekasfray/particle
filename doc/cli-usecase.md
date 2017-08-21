# Command-line use cases

## Program Behavior

*U* means Use

* U1: Load object code from file and run object code
* U2: Interpret Particle source code from file
* U3: Output the generated assembly code to a file
* U4: Output the generated object code to a file
* U5: Interpret Particle assembly code from file

## CLI use cases

I combine program behaviors in order to visualize the design of the command-line interface.

*C* means Case

* C1: U1
* C2: U2+U3
* C3: U2+U4
* C4: U2+U3+U4
* C5: U5
* C6: U5+U3
* C7: U5+U4
* C8: U5+U3+U4

## Options

* `-i machine` - Tells the interpreter to treat the input file as machine code.
* `-i assembly` - Tells the interpreter to treat the input file as assembly source code.
* `-i particle` - Tells the interpreter to treat the input file as Particle code. This is the interpreter's default input type.
* `-a FILE` - Tells the interpreter to output the generated assembly code to a file.
* `-m FILE` - Tells the interpreter to output to the generated machine code.
* `-v` - Displays the version number
* `-h` - Displays usage
