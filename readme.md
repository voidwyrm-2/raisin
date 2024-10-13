# Raisin

Raisin is a bytecode-compiled superset of [Brain\*\*\*\*](<https://en.wikipedia.org/wiki/Brainfuck>)

## Commands

Character | Description
--------- | -----------
Standard Branflakes
`+` | Increments the current cell by 1
`-` | Decrements the current cell by 1
`>` | Increments the current cell position by 1
`<` | Decrements the current cell position by 1
`[` | Jumps to the corresponding `]` if the current cell **is** 0
`]` | Jumps to the corresponding `[` if the current cell **is not** 0
`.` | Prints the current cell
`,` | Takes in a single byte value as user input and sets the current cell to it
Added by Raisin
`_` | Stops the program
`'` | Jumps ahead by the amount in the current cell
`\`` | Jumps back by the amount in the current cell

## Comments

In normal Branflakes, comments are any character that isn't one of the commands

Raisin has actual comments
```
/ this is a comment /

/ this prints '2' /
++.
```

## Macros

In normal Branflakes, a lot of duplicate code is created because of the lack of any abstraction

In Raisin, this is fixed(for sure, totally) by adding macros
```
#add3{+++}

++
#add3
.

/ this prints '5' /
```

<!--
## Built-in Functions

Raisin has a set of built-in functions to make certain things easier

**Print**
Expands to the Branflakes equivalent of that string<br>
This prints the string without a trailing newline, use `println` for that
```
@print("Hello, Catdog")
```
-->