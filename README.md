# calculation
  Reaction's rate constant calculation
 
 1. Input:
 	1. ~/input/data.inp (.csv) file with absorbtion coefficients.
	2. ~/input/flow.txt file with RTFC data (radiation intensity).
	3. ~/input/T file with temperature field.
	
    Sample files are attached in "~/input".
 
 2. WARNING: to set the programme to work, make appropriate changes of T-file. Code will fail to run otherwise.

 3. Programme's output is represented as a .CSV table "~/output/output.csv".

 5. To change output file's path/name:
	1. Go to main.cpp:
		data.open("..\\output\\output.csv", std::ios_base::out);
	2. Change "output.csv".
	3. Enjoy.
	
