# Advent of Code 2022 solutions written in C++.
## Author: Tim Behrendsen

Link: https://adventofcode.com/2022/

Advent of Code is a series of puzzles over 25 days, each with a part 1 and part 2. The difficulty roughly rises
each day, with the later puzzles often requiring some tricky algorithms to solve.

Various days are in separate directories, each with their own main. Typically each module has a part1 and a part2,
though a few times I didn't keep the part1 once I solved it.

## Summary:

Day 1: Simple parsing and totaling.

Day 2: Simulate simple strategy game and determine total score.

Day 3: Analyze elf "ruck sacks" and determine bad priorities.

Day 4: Simulate elf "cleaning sections" and determine overlapping
    assignment pairs.

Day 5: Simulate cargo crane loading procedure and determine the top of the stacks

Day 6: Decode communication stream and find start-of-message markers.

Day 7: Parse a log of shell commands and simulate a file system. Find smallest
    directory that would free up sufficient space to run an update. Required
    recursive search to optimize.

Day 8: From forest map, calculate "visibility distance" and "scenic score".

Day 9: Simulate movement of a rope, where the motion propagates down a rope
    with a number of knots.

Day 10: Simulate "CRT Monitor" and generate image based on input instructions.

Day 11: Calculate monkey "worry level" based on optimizing chasing strategy.

Day 12: Navigate map and determine fewest steps to get to location with best signal.
    Uses Diykstra's Algorithm for optimizing graph path.

Day 13: Decode nested packet format and determine "decoder key". Requires parsing and
    tricky nested data structure.

Day 14: Simulate falling sand and calculate amount of sand under various scenarios.

Day 15: Find beacon coordinate based on overlapping sensor maps. Part 2 required calculating
    overlaps using data structures.

Day 16: Simulate elephants opening valves in a cavern map, and find the optimal strategy.
    Part 2 required tricky dynamic programming to cull the tree size to reasonable numbers.

Day 17: Simulate falling rocks and calculate height of tower. Rocks may be irregularly
    shaped, so required some collision detection. Part 2 simulated 1,000,000,000,000
    rocks dropping, so required figuring out a pattern and extrapolating.

Day 18: Calculate total surface areas of a set of cubes with adjacent faces.

Day 19: Calculate largest number of "geodes" that can be produced by "machine
    blueprints". Required optimizing a graph with a large number of nodes.

Day 20: Decrypt "grove positioning system" using the given algorithm.

Day 21: Interpret "monkey math" operations and generate answer. Part 2 required doing
    a gradient ascent algorithm to find the optimal solution.

Day 22: Simulate a map on the surface of a cube and trace path used by monkeys.
    Required simulating cube faces and following a path across it.

Day 23: Optimization problem of simulating a travel process finding a round where
    "no elf moves".

Day 24: Navigate through a map, avoiding blizzards, and calculate fewest number of
    minutes. Part 2 was much larger scale. Uses a Breadth-First-Search algorithm
    to optimize the path.

Day 25: Convert odd base-5 numbers to and from regular integers, where the base actually
    contains negative numbers. Math problem.
