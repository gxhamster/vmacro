# Vmacro

## What this is program

Vmacro is a simple utility to execute vim style macros on each line of a file.
Although all I was not able to include all commands of vim yet. 


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

