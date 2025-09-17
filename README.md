
# Extensible C compiler Library

A C compiler designed for writing custom backends with a high level of flexibility

Around 2021, I was putting the finishing touches on my first custom CPU architecture. I had made a few things with it, but only in assembly. I wanted to try compiling C to my assembly language, but after wrestling with LLVM for a while, I found that it just wasn't designed for a CPU as retro as mine. So I decided to make a C compiler focused on backend flexibility! The backend is given the IR and is free to handle it however it wishes!

Here's [an example](https://github.com/PegaFox/pegafox-mc3/tree/main/mc3_tools/C_compiler) of a backend for one of my CPU designs!

## Creating a backend

Your backend should link with the library, this project uses CMake to build and can be integrated into an existing CMake project using add_subdirectory().
The simplest way to run the compiler from within your project is to create and use an instance of the "Compiler" class. After creating an instance, make sure the object's member variables are how you want (for example, you can set default include paths by appending to the "default include directories member"), and then run the compiler with "compileWithArgs(argc, argv)". This function returns an "IRprogram" object containing static variable data and an array of IR functions that can be rearranged freely!
