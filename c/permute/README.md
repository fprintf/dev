# permute

## SYNOPSIS

```
permute [-h?] [-s start] [-e end] <string> [[string] ...]
```

## DESCRIPTION

permute the given string on the command line, printing each permutation
to stdout. **start** specifies the starting length to permute from and end
specifies where to **end**.

## OPTIONS

```
-h, -?    print this help
-s start  permute the given string starting from this length - 1.
          default: 0
-e end    permute the given string (from start) to this length.
          default: string length
```

## CONTRIB

Feel free to fork this and add additional features options, etc, whatever.

## INSTALL

To install permute you simply need to _clone_ it into a directory
`$ git clone http://github.com/fprintf/dev/c/permute permute`

Then enter the directory and build and install the program
```
$ cd permute
$ make
$ sudo make install
```
