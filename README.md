# tinyShell

A minimal Unix shell written in C.

## Features
- Builtins: `cd`, `pwd`, `echo`, `type`, `help`, `exit`
- External command execution via `fork`/`execvp`
- Basic multi-pipe support (`cmd1 | cmd2`)
- Quoted argument parsing (`"..."` / `'...'`)
- Command history saved to `~/readLinesHistory`

## Build
```sh
gcc main.c -o tinyshell
```

## Run
```sh
./tinyshell
```

## Structure
```
.
├── main.c            
├── headers.c        
├── .gitignore
├── utils/
│   └── utils.c       
└── parse/
    ├── parse.c       
    └── piping.c      
```
