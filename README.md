# bi format

Simple Structured Human-Readable Binary Format. It is used by such projects as [rere.py](https://github.com/tsoding/rere.py) and [Porth](https://gitlab.com/tsoding/porth).

The goals of the format:
- So simple you don't need a library to Parse it or Serializer into it (we do provide a reference implementation of a parser in C though [bi.h](./bi.h)),
- If the data stored in the [Blobs](#blob-field) is Text the whole file looks like Text making any changes committed in VCS have a nice Human-Readable Diff.

## Description

A bi file consist of sequence of fields. There are only 2 kinds of fields for now: [Integer](#integer-field) and [Blob](#blob-field). (We may add more kinds of fields in the future, but it's very unlikely, because these two feel pretty sufficient for now). Each field has a name associated with it. Names don't have to be unique.

### Integer field

- An integer field starts with a sequence of bytes that if viewed as ASCII looks like `:i` followed by exactly ONE space,
- After that comes a sequence of arbitrary bytes (excluding newline `\n`) which denotes the name of the field. It is usually recommended to use ASCII Human-Readable names, but you do you. You can even store arbitrary data in the name of a field. The name ends with exactly ONE space,
- After that comes a sequence of ASCII digits denoting the integer stored in the field. There is no upper bound for integers. Handle overflows however you want. (We don't support negative integers right now, but we may in the future if needed),
- Field ends with ASCII newline `\n`.

Here is how a bunch of integer fields would look like in a bi file:

```
:i foo 69
:i bar 420
:i baz 1337
```

### Blob field

- A blob field starts with a sequence of bytes that if viewed as ASCII looks like `:b` followed by exactly ONE space,
- After that comes a sequence of arbitrary bytes (excluding newline `\n`) which denotes the name of the field. The name ends with exactly ONE space,
- After that comes a sequence of ASCII digits denoting the size of the Blob in bytes. There is no upper bound for integers. Handle overflows however you want,
- After that comes exactly ONE newline `\n`,
- After that come the bytes of the Blob,
- After that comes exactly ONE newline `\n`.

Here is how a bi file with a bunch of blob fields and an integer field would look like:

```
:i count 3
:b hello 12
Hello, World
:b foo 7
Foo bar
:b test 163
Test test test

You can have new lines in here.
You can actually store binary data in here.
You can nest another bi file in here, thus
making the format Tree-like.
```

## Be creative

It is really encouraged to get creative with this format. Who said you can't store other formats in the Blobs?

```
:b Person/json 69
{
    "Name": "John Doe",
    "Age": 69,
    "Occupation": "Webdev"
}
```

## External stuff

Some things that have been made using the format/working with the format:

- [rere.py](https://github.com/tsoding/rere.py) - universal testing tool for replaying commands
- [TreeSitter parser](https://github.com/ej-shafran/tree-sitter-bi) - highlighting and the such for the BI format
