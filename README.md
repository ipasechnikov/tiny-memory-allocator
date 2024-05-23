# Tiny Memory Allocator

The project was created just for fun following a cool article [here](arjunsreedharan.org/post/148675821737/write-a-simple-memory-allocator) in my free time to brush up my C skills.

⚠️ You should **NEVER** use this as a real memory allocator in your project.

## Implementation details

`tmalloc` is a shared library (`.so`). Its full filename is `libtmalloc.so`.

The library implements 4 main memory management functions:

- [`malloc()`](https://en.cppreference.com/w/c/memory/malloc)
- [`calloc()`](https://en.cppreference.com/w/c/memory/calloc)
- [`realloc()`](https://en.cppreference.com/w/c/memory/realloc)
- [`free()`](https://en.cppreference.com/w/c/memory/free)

The implementation is very inefficient due to tracking and managing memory with a linked list. But again, the library is for education purpose and has no real-world application whatsoever.

## Compilation

You can use any Linux distro with GCC toolchain available. Even WSL2 works completely fine.

Compilation process is pretty straightforward and uses `Makefile`.

You can compile everything with a good old well-known command:

```shell
make all
```

Or you can simply run the command bellow to build shared library and run main executable to verify that everything works fine.

```shell
make run
```

## Usage

### Usage 1. Current terminal session

`tmalloc` can be easily used as a memory allocator for every program you run from your current terminal session.

1. Compile shared library
```shell
make all
```

2. Run the following command from project's root to force OS to load your functions instead of system's default ones
```shell
export LD_PRELOAD=$PWD/out/libtmalloc.so
```

3. Run any program you want in the current terminal session, even the simplest one like `ls` will do
4. Revert your changes by resetting `LD_PRELOAD`
```shell
unset LD_PRELOAD
```

### Usage 2. Link to any of your C programs

1. Compile shared library
```shell
make all
```

2. Copy shared library to your project
3. Compile your program linking custom allocator and hinting loader where to look for it during runtime:
```shell
gcc my_program.c -o my_program.out -L. -ltmalloc -Wl,-rpath='$ORIGIN'
```
4. To verify that custom allocators loads before default one, run your executable in a specific way:
```shell
LD_DEBUG=libs ./my_program.out
```

## Acknowledgments

Thanks one more time to [this awesome article](arjunsreedharan.org/post/148675821737/write-a-simple-memory-allocator) and its repo that can be found over [here](https://github.com/arjun024/memalloc).

The article is listed in [Project Based Learning](https://github.com/practical-tutorials/project-based-learning) GitHub repo.
