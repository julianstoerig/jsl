# JSL

is a set of non-performance oriented (although I sincerely attempt to avoid pessimisation),
strictly **C99** utilities which I often want in my programs, but which the C standard library
does not provide attractive alternatives for.

Since I want to use these in a variety of projects with varying legal status, I decided to
[UNLICENSE](https://www.unlicense.org) them.

# Contributions

Due to the challenges imposed by copyright law, I do not accept patches
written by somebody else. If you would like to report a bug or ask for a
feature to be implemented, it is still useful for me if you append a patch,
but do not be annoyed if I fully rewrite it myself.

# To Do

- [x] Stretchy Buffer / Dynamic Array
- [ ] Add Explicit
    - [x] Dynamic Arrays
    - [ ] Hash Tables
    - [ ] Pools
    - [x] Linked Lists
        - [x] Stacks
        - [x] Queues
        - [x] Doubly Linked Lists
- [ ] Unicode (UTF-8) Aware Strings
- [ ] Numerics
    - [ ] Scalar
        - [ ] Root-Finding 
            - [x] Newton
            - [x] Bisection
            - [x] Secant
            - [x] Secant-Bisection
            - [x] Fixpoint
        - [ ] Integration
            - [ ] Riemann, left
            - [ ] Riemann, right
            - [ ] Trapezoid
            - [ ] Simpson's
        - [ ] Differentiation
            - [ ] Forward Difference
            - [ ] Central Difference
    - [ ] Linear Algebra
        - [x] Vector
        - [x] Matrix
        - [x] Transposition
        - [x] Matrix Multiplication
        - [ ] QR Decomposition
        - [ ] LR Decomposition
        - [ ] Cholesky Decomposition
        - [ ] Gauss-Seidel
        - [ ] Matrix versions of fixpoint and rootfinding algorithms
        - [ ] Newton
        - [ ] Bisection
        - [ ] Secant
        - [ ] Secant-Bisection
        - [ ] Fixpoint
