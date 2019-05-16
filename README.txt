This project successfully implemented virtual memory through
paging and swapping to "disc". I did not use the struct approach
to fitting the page tables in a compact memory space. I used 2 in
the present byte to signify if the page was in memory, and set the
pfn to an invalid number to signify if that adress was invalid to
solve the page table size problem. I did not write to a text file
to simulate a hard drive, Professor Shue said we could just
simulate it as a bigger array.

My testing was done through the test.txt file. The program was run
through the command line as so:

  make all
  ./Mem < test.text

The output is copy pasted below:

Write commands below in form:
	process_id,instruction_type,virtual_adress,value
Valid instruction types are:
	map
	store
	load
Instruction? Mapped virtual adress 0 (Page 0) into physical frame 1
Instruction? Mapped virtual adress 0 (Page 0) into physical frame 3
Instruction? Stored value 123 into virtual adress 0 (Physical adress 16)
Instruction? Stored value 45 into virtual adress 0 (Physical adress 48)
Instruction? Swapped frame 0 to disc frame 0
Swapped frame 1 to disc frame 1
Mapped virtual adress 0 (Page 0) into physical frame 1
Instruction? Page is not writable!
Instruction? Mapped virtual adress 0 (Page 0) into physical frame 1
Instruction? Stored value 55 into virtual adress 0 (Physical adress 16)
Instruction? Swapped frame 2 to disc frame 2
Swapped frame 3 to disc frame 3
Mapped virtual adress 48 (Page 3) into physical frame 3
Instruction? Stored value 255 into virtual adress 51 (Physical adress 51)
Instruction? Swapped frame 0 to disc frame 4
Put page table for pid 0 into physical frame 0
Swapped frame 1 to disc frame 5
Put disc frame 1 into physical frame 1
The value 123 is virtual adress 0 (Physical adress 16)
Instruction? Swapped frame 2 to disc frame 1
Put page table for pid 1 into physical frame 2
Swapped frame 3 to disc frame 6
Put disc frame 3 into physical frame 3
The value 45 is virtual adress 0 (Physical adress 48)
Instruction? Swapped frame 0 to disc frame 3
Put page table for pid 2 into physical frame 0
Swapped frame 1 to disc frame 7
Put disc frame 5 into physical frame 1
The value 55 is virtual adress 0 (Physical adress 16)
Instruction? Swapped frame 2 to disc frame 5
Put page table for pid 3 into physical frame 2
Swapped frame 3 to disc frame 8
Put disc frame 6 into physical frame 3
The value 255 is virtual adress 51 (Physical adress 51)
Instruction? Swapped frame 0 to disc frame 6
Put page table for pid 0 into physical frame 0
Swapped frame 1 to disc frame 9
Put disc frame 7 into physical frame 1
Mapped virtual adress 0 (Page 0) into physical frame 1
Instruction? The value 123 is virtual adress 0 (Physical adress 16)
Instruction? Page is not writable!
Instruction? Swapped frame 2 to disc frame 7
Put page table for pid 1 into physical frame 2
Swapped frame 3 to disc frame 10
Mapped virtual adress 32 (Page 2) into physical frame 3
Instruction? Stored value 142 into virtual adress 45 (Physical adress 61)
Instruction? The value 123 is virtual adress 0 (Physical adress 16)
Instruction? Swapped frame 0 to disc frame 11
Put disc frame 8 into physical frame 0
The value 45 is virtual adress 0 (Physical adress 0)
Instruction? Swapped frame 1 to disc frame 8
Put page table for pid 2 into physical frame 1
Swapped frame 2 to disc frame 12
Put disc frame 9 into physical frame 2
The value 55 is virtual adress 0 (Physical adress 32)
Instruction? Swapped frame 3 to disc frame 9
Put page table for pid 3 into physical frame 3
Swapped frame 0 to disc frame 13
Put disc frame 10 into physical frame 0
The value 255 is virtual adress 51 (Physical adress 3)
Instruction? Swapped frame 1 to disc frame 10
Put page table for pid 1 into physical frame 1
Swapped frame 2 to disc frame 14
Put disc frame 9 into physical frame 2
The value 142 is virtual adress 45 (Physical adress 45)
Instruction? Invalid number of commands!
