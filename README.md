# Vmacro

## What is this program

Vmacro is a simple utility to execute vim style macros on each line of a file.
Although all I was not able to include all commands of vim yet. I dont think I 
will implement inserting characters.

### Remember!
The behaviour of the commands might not be exactly like in vim.
The goal was to create a tool that does it similar to vim.
Also there might be some bugs.

## Examples

```console
./macro -f test.txt -m "2l;dw" -d ";"
./macro -f test.txt -m "2dw"

```

### How to compile 

The Project has a simple `Makefile` to execute
The `Makefile` has two options debug (which includes debug symbols)
and all (compiler optimizations turned on).

```console
$ make debug
$ ./macro -f test.txt -m "3fp;l;d$" -d";"
$ |I am so happy this p|
                      ^
```

### Performace
The program in itself is fast. But it could be faster.
Especially when deleting characters.

#### Without compiler optimiztions
For `5000000` lines
```console
$ time ./macro -f test1.txt -m "3w;2dw;dFa" -d";" > /dev/null
real    0m30.714s
user    0m30.600s
sys     0m0.052s
```

#### With compiler optimizations
For `5000000` lines
```console
$ time ./macro -f test1.txt -m "3w;2dw;dFa" -d";" > /dev/null

real    0m26.984s
user    0m26.938s
sys     0m0.040s
```

