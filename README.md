# cdocs2
## Author: Ariel Traver

_To turn this into a shell command, go to your zshrc (or bashrc) and make an alias to wherever you store the cdocs executable._

### Function: error
* takes an input from some function and prints the appropriate error message, then quits.
* parameters:
    * errornum, the error code

### Function: init_readme
* opens a file where the readme will be written.
* overwrites any existing readme files with the same name.

### Function: print_headers
* Prints all the lines contained within the specified delimeters
* Parameters:
    * buffer, the buffer containing text file contents
    * opn_delim, provided opening delimeter or none
    * cls_delim, provided closing delimeter or none

