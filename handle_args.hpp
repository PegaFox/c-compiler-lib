#ifndef PF_HANDLE_ARGS_HPP
#define PF_HANDLE_ARGS_HPP

/*

  accepted filetypes
    file.c
      C source code that must be preprocessed.

    file.i
      C source code that should not be preprocessed.

    file.h
      C header file to be turned into a precompiled header.

*/

int handleArgs(int argc, char* argv[]);

#endif // PF_HANDLE_ARGS_HPP