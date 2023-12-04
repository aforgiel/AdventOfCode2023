# AdventOfCode2023

This is my attempts to solve the puzzles from AdventOfCode2023 in plain C++.

Summary:
* Day 1: much more complex than last year! Instead of using Excel, I directly jumped to Visual Studio. Part 1 was fast and easy but I lost some time on part 2 as my FindPattern() function would skip case like TWONE, returing 2-void instead of 2-1...
* Day 2: Part 1 was a mix of MatchPattern() and FindPattern() calls to read the various subsets. Printing comments helps a lot and I've now decided to systematically start with writing down what's going on ;)
* Day 3: forced to breakdown the problem with Schematic, Part, Gear data structures to complete part 2. Traces helped a lot to identify pending issues...
* Day 4: started directly with defining classes of problems' objects (collection and cards), leveraging std:: containers (map, deque,vector). Long part 2 processing : 23 seconds !
