# Boolean-Engine

Boolean-Engine is a high-performance C++ tool for manipulating Boolean functions using Positional Cube Notation (PCN).  
It implements the Recursive Complement Algorithm based on Shannon Expansion and supports NOT, AND, and OR operations.  
The tool is intended for EDA and logic synthesis applications and operates using command scripts.

---

## Core Architecture

### Positional Cube Notation (PCN)

Boolean functions are represented using bit-pairs per variable instead of truth tables or SOP forms.

| Literal        | PCN Bit-Pair | Description                    |
|----------------|--------------|--------------------------------|
| xi             | 10           | Positive literal               |
| not xi         | 01           | Negative literal               |
| Don't care     | 11           | Variable absent in cube        |
| Empty          | 00           | Invalid or null state          |

---

## Key Algorithms

- Recursive Complement Algorithm using Shannon Expansion
- AND operation implemented using De Morgan's Law:
  A AND B = NOT ( NOT A OR NOT B )
- Heuristic variable splitting to reduce recursion depth:
  - Binate variables are prioritized
  - Variables with minimal positive-negative imbalance
  - Variables with fewer don't-care literals

---

## Command Syntax

The engine processes a command script line by line and supports up to 32 function slots.

| Command | Syntax               | Description                               |
|--------|----------------------|-------------------------------------------|
| Read   | r index              | Load index.pcn into slot index            |
| NOT    | ! dest src           | Complement src and store in dest          |
| OR     | + dest s1 s2         | Logical OR of s1 and s2                   |
| AND    | & dest s1 s2         | Logical AND of s1 and s2                  |
| Print  | p index              | Write slot index to out.pcn               |
| Quit   | q                    | Exit the program                          |

---

## File Formats

### PCN Input File (.pcn)

Format:
- Line 1: Number of variables (N)
- Line 2: Number of cubes (C)
- Lines 3+: Number of literals followed by literal indices

Positive value represents xi  
Negative value represents not xi

Example: Function (x1 AND not x2) OR x3
3
2
2 1 -2
1 3



---

## Getting Started

### Prerequisites

- C++ compiler supporting C++11 or later

### Compilation

g++ -O3 BooleanCalculator.cpp -o pcn_calc


### Execution

1. Create input files (0.pcn, 1.pcn, etc.)
2. Create a command script (commands.txt)

r 0
r 1
! 2 0
& 3 1 2
p 3
q


3. Run the tool


---

## Project Scope

This project demonstrates:
- Shannon expansion and Boolean decomposition
- Recursive algorithms for logic manipulation
- Heuristic optimization techniques
- PCN-based Boolean representations for EDA tools


