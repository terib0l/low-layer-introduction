# microc-compiler

* microc.py: メイン
* lexer.py: 字句解析
* parser.py: 構文解析
* analysis.py: 意味解析
* llvmgen.py: 中間コード(LLVM IR生成)
* classes.py: [データ構造の定義]
* util.py: [ユーティリティ関数群]
* fib.mc: {試験用プログラム}

# Requirements

- Python Lex-Yacc 3.11
- Python 3
- Clang/LLVM 11.0

# How to use

```bash
$ vagrant up
$ vagrant ssh
$ cd workspace

# Be made 'fib.ll' and 'main-fib.c'
$ python3 microc.py ./fib.mc

$ clang-${version} fib.ll main-fib.c -o fib
$ ./fib 10
```

