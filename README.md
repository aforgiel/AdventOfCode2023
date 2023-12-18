# AdventOfCode2023

This is my attempts to solve the puzzles from AdventOfCode2023 in plain C++.

Summary:
* Day 1: much more complex than last year! Instead of using Excel, I directly jumped to Visual Studio. Part 1 was fast and easy but I lost some time on part 2 as my FindPattern() function would skip case like TWONE, returing 2-void instead of 2-1...
* Day 2: Part 1 was a mix of MatchPattern() and FindPattern() calls to read the various subsets. Printing comments helps a lot and I've now decided to systematically start with writing down what's going on ;)
* Day 3: forced to breakdown the problem with Schematic, Part, Gear data structures to complete part 2. Traces helped a lot to identify pending issues...
* Day 4: started directly with defining classes of problems' objects (collection and cards), leveraging std:: containers (map, deque,vector). Long part 2 processing : 23 seconds !
* Day 5: wake-up > read instructions > think while shower & dress > code... thinking in range was tough but feasible
* Day 6: back to arithmetic (b^2-4ac). Not so hard but I took my time to think the problem through instead of jumping into a brut force solution
* Day 7: same routine : wake-up > read instructions > think > code, starting with class definition to breakdown the problem. Writing an efficient comparison operator was tough due to the number of cases
* Day 8: using a C-language trick to go from char[] to int and back and prime decomposition for part 2. Lesson learned: brute force never works !
* Day 9: interesting recursive approach to compute n-derivatives. Clear variable naming is key to avoid mistakes
* Day 10: much more difficult ! Lost a lot of time making sense of binary operators which I seldom use. Also, the ray casting in diagonal saved me!
* Day 11: Using a map as a proxy for coordinates made the solution quite simple, avoiding complex array row/column insertion
* Day 12: Nice recursive search slightly optimized but not enough for part 2! Still computing... Finally wrote a recursive cache to accelerate the algorithm, getting the results in 1s instead of 10h!!!
* Day 13: Much simpler than yesterday ! Brute force was enough in detecting symmetry
* Day 14: Anticpated rightly the cardinal cases (S,N,E,W) to speedup part 2. This one was all about finding rhythm ;)
* Day 15: A bit wordy today... Had to create several classes to breakdown the problem; no real difficulty here. Glad the elves are using the same ASCII codes as us!
* Day 16: Again, anticipate what part 2 should be by generalizing the case right from the start. Enjoyed playing with bits and queue-based recursive methods
* Day 17: Hard-learned lesson on the Dijkstra's algorithm with edge cases and constraints. spent some time on the vizualization for debug and experience purpose
