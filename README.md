# Text Search Tool

A fast, interactive command-line text search and replace tool built in C++.

## Overview

- Loads a large text file into memory once for lightning-fast repeated searches

- Powered by Knuth-Morris-Pratt (KMP) algorithm → O(N + M) efficiency

- Match modes: Whole (default), Prefix, Substring

- Case-sensitive (default) or insensitive (c=i)

- Supports quoted multi-word phrases

- Highlights matches with [ ]

- Optional safe replace → writes to a new file

- Shows match/replacement count + execution time in microseconds

## How to Run

1. **Compile**

   ```

   g++ main.cpp -o textsearch

   ```

2. **Run**

   ```

   ./textsearch --file=input.txt

   ```

   Replace `input.txt` with the path to your text file.

3. **Interactive Queries**

   ```

   Query <keyword> [c=i|s] [m=Whole|Prefix|Substring] [r=replace_text] [o=output_path]

   ```

   **Examples**

   ```

   Query error

   Query Error c=i m=Substring

   Query bug r=feature o=fixed.txt

   Query "hello world" c=i

   ```

   Type `exit` to quit.
