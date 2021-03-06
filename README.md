# Vmacro

## What is this program

Vmacro is a simple utility to execute vim style macros on each line of a file.
I am not able to include all commands of vim yet. The tool will be much more 
faster than doing it inside vim. The idea was that you give certain vim macros 
seperated by a delimeter and the program will execute that macro on each line. 
It can also operate on input coming from stdin.

### Remember!
The behaviour of the commands might not be exactly like in vim.
The goal was to create a tool that does it similar to vim.
Also there might be some bugs. This tool was created to learn more
about how vim macros might work and beacause how slow vim macros can 
be when done over many lines.

### Supported movements and actions
##### Movments
- `w` Word forward
- `b` Word backward
- `l` Character forward
- `h` Character backward
- `^` Start of line
- `f` Find character
- `F` Find character backward
- `t` Till character
- `T` Till character backward
- `$` End of line
- `/` Search forwards
- `?` Search backward

##### Actions
- `i` Insert characters at cursor
- `d` Delete action (usually assciated with a movement)
- `y` Yank (copy) (can be given movements)
- `p` Paste yanked text at cursor
- `P` Paste yanked text before cursor

With most of these movments you can give a count of how many times to 
repeat it. An action can also given to it. Currently there is only delete
and insert. 

### Options
- `-d` Delimeter to use when using multiple macros
- `-f` Name of file to read from (If not given read from stdin)
- `-c` Number of times to repeat macro
- `-m` The macro to execute on each line

### Examples
The -p flag will prettify the output, showing the location of
the cursor. Omit it if you are outputting to a file instead.
The -c flag will repeat the given macro a given number of times

```console
./vmacro -f test1 -m "$;4db;h;i Hello;f.;dh" -d ";" -p 
./vmacro -f test.txt -m "2l;dw" -d ";"
./vmacro -f test.txt -m "2dw" -c 3 > test1.txt

```

### How to compile 

The Project has a simple `Makefile` to execute
The `Makefile` has two options debug (which includes debug symbols)
and all (compiler optimizations turned on). If you are going to use the tool
i highly recommend to use `make all` command.

```console
$ make debug
$ ./vmacro -f test.txt -m "3fp;l;d$" -d";" -p 
$ |I am so happy this p|
                      ^
```

### Performance
One of the goals of this tool was to make it be able to handle 
a lot of lines easily. The program in itself is fast. But it 
could be faster. Any kind of deleting would obviously be slower
than not doing it, as you constantly creating new buffers to store lines
and identifying the words again.

#### Without compiler optimiztions
For `5000000` lines
```console
$ time ./vmacro -f test1 -m "$;4db;h;i Hello;f.;dh" -d ";" > /dev/null
real    0m41.938s
user    0m41.747s
sys     0m0.099s
```

#### With compiler optimizations
For `5000000` lines
```console
$ time ./vmacro -f test1 -m "$;4db;h;i Hello;f.;dh" -d ";" > /dev/null
real    0m15.629s
user    0m15.516s
sys     0m0.081s
```

#### Contributing
If anyone wishes to contribute. You are welcome to do so by opening 
a pull request. Anyone is free to use this tool. 
