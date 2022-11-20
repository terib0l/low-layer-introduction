# low-layer-introduction

Reference: Interface 2021/2

## Progress

- [x] Day1. One step into Hard-master
- [x] Day2. Compiler
- [x] Day3. Experiment using Python
- [x] Day4. OS Kernel [Multi-tasks, Memory Management]
- [ ] Day5. Realtime OS
- [x] Day6. Virtualization
- [ ] Day7. Assembly Language

## Memo

### Flow of Compile

```
## Clang (Flontend) ##
#                    #
#  Sourcecode        #
#     |              #
#    AST             #
#     |              #
######################
      |
  LLVM IR/bitcode
      |
## LLVM (Middle/Backend) ##
#     |                   #
#  Machine IR             #
#     |                   #
###########################
      |
  assembly/machicode
```
