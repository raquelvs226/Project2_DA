# Design of Algorithms Project 2 - Compiler Register Allocation Algorithms

## Grade: 19,6/20

## Register Allocation Tool

- This project implements a register allocation tool for compilers. Given a file containing the live ranges of a program's variables and a configuration file specifying the number of available registers and the algorithm to use, the tool constructs the webs and the interference graph, then applies a graph coloring algorithm to determine the register allocation.

- The program can be executed in two modes:
  - **Interactive mode:** Provides a menu with several options.
  - **Batch mode:** `./Project2 -b ranges.txt registers.txt allocation.txt`

- For this project, we implemented four algorithms: **Basic Register Allocation**, **Register Allocation with Web Spilling**, **Register Allocation with Web Splitting**, and **Register Allocation with DSATUR**.

### Basic Register Allocation

- The goal of Basic Register Allocation is to color the interference graph using N colors (registers), minimizing the number of colors used.
- The algorithm consists of two phases:
  1. **Simplification:** Nodes with a degree lower than N are removed and pushed onto a stack. If no such node exists, the node with the highest degree is pushed onto the stack, and an attempt is made to color it in the next phase.
  2. **Coloring:** Nodes are popped from the stack one by one and assigned the lowest available color that is not used by any of their neighbors.
- If any node cannot be colored, the entire allocation fails. All webs are spilled to memory, the number of registers in the output is set to 0, and all webs are assigned M. A warning is also issued to indicate that the allocation has failed.
- **Complexity:** O(W × E)

### Register Allocation with Web Spilling

- Spilling is used when Basic Register Allocation fails with N available registers. It allows up to K webs to be spilled to memory, simplifying the graph and increasing the chances of successfully coloring the remaining nodes. K is specified in the configuration file (`algorithm: spilling, K`).
- The algorithm performs up to K attempts. In each attempt, it constructs the graph using only the non-spilled webs and runs Basic Register Allocation. If all remaining webs are successfully colored, the algorithm terminates successfully. Otherwise, it selects the web with the highest degree and forces it to spill before trying again. The algorithm always starts with one spill and only increases the number of spills when necessary.
- **Web selection strategy:** We select the web with the highest degree in the graph, i.e., the one with the most interferences with other webs. By removing the most problematic web, we eliminate the largest number of interferences at once, maximizing graph simplification and increasing the likelihood of success in the next attempt. Only uncolored webs are considered, since already colored webs have an assigned register.
- If Basic Register Allocation still fails after K attempts, all webs are spilled to memory.
- **Complexity:** O(K × (W² × P² + W × E)), where K is the maximum number of spills, W is the number of webs, E is the number of interferences, and P is the maximum number of program points per web.

### Register Allocation with Web Splitting

- Splitting is used when Basic Register Allocation fails with N available registers. Unlike spilling, which sends webs to memory, splitting divides up to K webs into two halves to reduce interferences and improve the chances of successful coloring. K is specified in the `registers.txt` configuration file (`algorithm: splitting, K`).
- The algorithm performs up to K attempts. In each attempt, it runs Basic Register Allocation with all webs, including those already split. If all webs are successfully colored, the algorithm terminates successfully. Otherwise, for each uncolored web, it tests all possible split points, selects the one that minimizes `max(degA, degB)`, and splits the web into two halves.
- **Split point selection strategy:** For each uncolored web, all possible split points are tested. For each split, the degree of each resulting half is calculated: `degA` for half A and `degB` for half B. The split point that minimizes `max(degA, degB)` is selected. This ensures that both halves have as few interferences as possible, increasing the likelihood that both can be colored using N registers.
- If Basic Register Allocation still fails after K attempts, all webs are spilled to memory. Note that splitting does not guarantee success: even after splitting K webs, there may still be insufficient registers to color the graph.
- **Complexity:** O(K × (W² × P² + W × P × deg)), where K is the maximum number of splits, W is the number of webs, P is the maximum number of program points per web, and deg is the maximum graph degree.

### Register Allocation with DSATUR

- For the additional algorithm, we chose **DSATUR (Degree of Saturation)**. Unlike Basic Register Allocation, which uses a stack and two phases based on a static degree ordering, DSATUR dynamically selects the next node to color in a single loop. At each step, it selects the node with the highest saturation degree, i.e., the node whose neighbors have the greatest number of distinct colors already assigned. This strategy reduces the likelihood of future conflicts by prioritizing the most constrained nodes, generally aiming to produce colorings with fewer colors overall.
- We implemented two additional selection heuristics:
  - **Primary criterion – Saturation degree:** Nodes with the highest number of distinct colors among their neighbors are colored first (DSATUR).
  - **First tie-breaker – Degree:** When two nodes have the same saturation degree, the node with the higher total degree is preferred.
  - **Second tie-breaker – Articulation points:** When both saturation degree and total degree are equal, preference is given to articulation points, since removing these nodes disconnects the graph and therefore indicates greater structural importance.
- At each step, the selected node is assigned the lowest available color that is not used by any of its neighbors. If no color is available, only that node fails. Unlike Basic Register Allocation, which fails the entire allocation when a node cannot be colored, DSATUR spills only the node that cannot be assigned a color, preserving the register allocations of the remaining webs.
- **Complexity:** O(W² + W × N + E), where W is the number of webs, N is the number of available registers, and E is the number of interferences.
