# calculation
 Reaction's rate constant calculation
 
 1. To run calculation, .CSV and .TXT data is demanded. As as tes case, ones are attached here.
 
 2. Change path_csv, path_txt in main.cpp file.
 
 As of now, calculation demands T as an inlet parameter to retrieve mesh from .CSV file.

3. To change the teperature of interest, thou needst either to:
   A. Change it manually in line 47 (main.cpp)
   B. Replace 4 line in main.cpp with this (as an example):
			
       int T = 34800;
							
							to
							
							int T;
							cin >> T;
							
      This'd allow thee to choose value on-the-run.
   If no such a temperature is present in .CSV header line, exception is raised.
