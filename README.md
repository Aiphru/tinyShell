# tinyShell

A minimal Unix shell written in C.

## Features
- Builtins: `cd`, `pwd`, `echo`, `type`, `help`, `exit`
- External command execution via `fork`/`execvp`
- Basic single-pipe support (`cmd1 | cmd2`)
- Quoted argument parsing (`"..."` / `'...'`)
- Command history saved to `~/readLinesHistory`

## Build
```sh
gcc -o tinyshell
```

## Run
```sh
./tinyshell
```

## Structure
```
.
├── main.c            # entry point, REPL, exec/pipe logic, builtins
├── headers.c         # hostname/env helpers
├── .gitignore
├── utils/
│   └── utils.c       # misc utilities
└── parse/
    ├── parse.c       # input tokenizer
    └── piping.c      # pipe detection & splitting
```
