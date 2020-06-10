# calculation
  Reaction's rate constant calculation
 
 1. To run calculation, .CSV and .TXT data is demanded. As as test case, ones are attached here.
 
 2. Change path_csv, path_txt in main.cpp file.
 
  As of now, calculation demands T as an inlet parameter to retrieve mesh from .CSV file.

3. Pass T values as an array:
	1. Insert the number of the temperatures of thy interest.
	2. Insert the very values.

4. Programme's output is represented as a .CSV table "_build/integral.csv".

5. To change output file's path/name:
	1. Go to: main.cpp (line 57).
	2. Change "integral.csv" to "(ABSOLUTE PATH)\\filename.csv"
	3. Enjoy.
	
