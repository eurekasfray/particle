# Source code organization

All the source code lives in the base directory. Source code is divided into some logical concepts to help define an understanding of the design of the program. I picture three important concepts in the design of this program:

- **Units.** I use the natural concept of C's translation units, and treat each file as a unit of a system. The job of a unit is to process data. A unit takes in data in one form and outputs it in another form.
- **ADTs or basic data structures.** I use ADTs to store data in a form that the computer can easily understand and handle. Units operate on data structures. Each data structure must have its own operations.
- **Utilities.** A utility is a piece of code that performs a job to assist the main task of the program. If an item is not a unit (i.e. it does not process data) or it is not an ADT, then we may consider it a utility.

# External variables and functions have a prepended namespace

If a variable or function is to be exposed externally to other functions then they must be prepended with a namespace in order to avoid name conflicts.

The name must be prepended with the namespace of its parent unit, ADT, or utility. For example, if a function `next_token()` in the unit `lexer.c` will be exposed, then we shall decided on a namespace for this unit and prepend all its external variables and functions with this namespace. So, back to our example, if we choose the namespace `lexer` then all our external function `next_token` will become `lexer_next_token`. This applies for all units, ADTS, and utilities.
