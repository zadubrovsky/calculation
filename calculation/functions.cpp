#include "stdafx.h"
#include "functions.h"
#include <stdexcept>


/*==============.TXT=FILE=PROCESSING=FUNCTIONS================================*/

std::vector<std::vector<double>> txtParse(std::string const& path)
{
    std::fstream tx;

    tx.open(path, std::fstream::in);

    std::vector<std::vector<double>> table;

    try
    {
       if (!tx.is_open())
       {
           throw std::invalid_argument("OpenError: .txt file");
       }
       else
       {
//          cout << "Success: .txt file opened" << '\n';

          std::string str;

          std::vector<double> temp;

          while (std::getline(tx, str))
          {
               std::stringstream ss(str);

               while (!ss.eof())
               {
                   ss >> str;

                   temp.push_back(std::stod(str));
               }

               table.push_back(temp);

               temp.clear();

               ss.clear();
          }
       }
    }
    catch (std::invalid_argument& ia)
    {
        std::cerr << ia.what() << '\n';
    }

    tx.close();

//    std::cout << "Success: .txt file closed" << '\n';

    return table;
};

std::vector<std::pair<double, double>> GetMeshTXT(const std::vector<std::vector<double>>& table, int colNumber){

    std::vector<std::pair<double, double>> mesh;

    for (auto& it : table)
    {
        mesh.push_back(std::make_pair(it[0], it[colNumber]));
    }

    return mesh;
}

/*==============.INP=FILE=PROCESSING=FUNCTIONS================================*/

std::vector<std::vector<double>> inpParse(std::string& path_inp) {

    std::ifstream inp(path_inp, std::ifstream::in);

    std::vector<std::vector<double>> table;

    std::string str;

    std::vector<double> NUM; // [0] - NUMBER OF PARAGRAPHS (LINES),
                             // [1] - NUMBER OF VALUES WHITHIN EACH ONE.

    std::vector<double> T, LAM, LINE;

    try {if (!inp.is_open()) {throw std::invalid_argument("OpenError: .inp file");}

         else {
//            cout << "Success: .inp file opened" << '\n';

/*===============RETRIEVING=FIRST=LINE========================================*/

    for (size_t i = 0; i < 2; ) {

            std::getline(inp, str, ' ');

            try {std::stod(str); NUM.push_back(std::stod(str)); ++i;}

            catch (std::exception& ex) {continue;}
        }

/*===========================================================================*/

    while (table.size() < NUM[0]) {

        int count = 0;

        while (std::getline(inp, str, ' ') && LINE.size() < NUM[1]) {

            try {
                 double val = std::stod(str);
                 ++count;
                 if (count == 2) {T.push_back(val);}
                 if (count > 2) {LINE.push_back(val);}
            }

            catch (std::exception& ex) {continue;}
        }

        if (LINE[0]) {table.push_back(LINE);}

        LINE.clear();
    }


    while (std::getline(inp, str, ' ')) {

        try {LAM.push_back(std::stod(str));}

        catch (std::exception& ex) {continue;}

    }

    table.push_back(LAM);   // table[table.size() - 2] -- LAM; LAM.size() == 7201

    table.push_back(T);     // table[table.size() - 1] -- T; T.size() == 70

    // Sorting T range, just in case
    std::sort(table[table.size() - 1].begin(),table[table.size() - 1].end());

    // Sorting LAM range, just in case
    std::sort(table[table.size() - 2].begin(),table[table.size() - 2].end());

    }

    } catch (std::invalid_argument& ia) {std::cerr << ia.what() << '\n';}

    inp.close();

//    std::cout << "Success: .inp file closed" << '\n';

    return table;
}

std::vector<std::pair<double, double>> GetMeshINP(const std::vector<std::vector<double>>& table, double& T) {

    std::vector<std::pair<double, double>> mesh;

    //Define a lambda that returns true if the t-value
    //of the given temp range is < the caller's t value
    auto lessThan =
        [](const double temp, double t)
        {return temp < t;};

    try {

        //Find the first table entry whose value is >= caller's T value
        auto iter =
            std::lower_bound(table[table.size() - 1].cbegin(), table[table.size() - 1].cend(), T, lessThan);

        //If the caller's T value is greater than the largest
        //T value in the temp range, we can't interpolate.
        if(iter == table[table.size() - 1].cend()) {
          throw std::invalid_argument("Input T > T_MAX in .inp file");
        }

        //If the caller's T value is less than the smallest
        //T value in the temp range, we can't interpolate.
        if(iter == table[table.size() - 1].cbegin() and T < table[table.size() - 1][0]) {
          throw std::invalid_argument("Input T < T_MIN in .inp file");
        }

        // Interpolation's linear coefficient
        double coef = (T - *(iter - 1)) / (*iter - *(iter - 1));

        // Index of the lower t value in range: table[table.size() - 1][index] < T
        double index = std::distance(table[table.size() - 1].cbegin(), iter) - 1;

        // Fill in new MESH
        for (size_t i = 0; i < table[table.size() - 2].size(); ++i) {

            // if T is not in .INP, but T_MIN <= T <= T_MAX -- interpolate
            if (!isinT(T, table)) {

                double deltaY = table[index + 1][i] - table[index][i];

                if (deltaY >= 0) {mesh.push_back(std::make_pair(
                                  table[table.size() - 2][i],
                                  table[index][i] + coef * deltaY));}

                if (deltaY < 0) {mesh.push_back(std::make_pair(
                                 table[table.size() - 2][i],
                                 table[index][i] + std::pow(coef, -1) * deltaY));}
            } else {

                //if T value coincides with some in .INP -- fill in without interpolation
                //check if coef == 1 -- fill in with the next K(T[index + 1])
                if (coef < 1) {mesh.push_back(std::make_pair(table[table.size() - 2][i],
                                                  table[index][i]));}
                else {mesh.push_back(std::make_pair(table[table.size() - 2][i],
                                     table[index + 1][i]));}
            }
        }
    }
    catch (std::exception& ex) {std::cerr << ex.what() << '\n';}

    // mesh_T[lambda_index].first == LAM[lambda_index]
    // mesh_T[lambda_index].second == K_T[lambda_index]
    return mesh;
}

/*==============.CSV=FILE=PROCESSING=FUNCTIONS================================*/

std::vector<std::vector<double>> csvParse(std::string const& path, const char delimiter)
{
    std::fstream fs;

    fs.open(path, std::fstream::in);

    std::vector<std::vector<double>> table;

    std::vector<double> _TEMP;

    try
    {
       if (!fs.is_open())
       {
           throw std::invalid_argument("OpenError: .csv file");
       }
       else
       {
//          cout << "Success: .csv file opened" << '\n';

          std::string str, header;

          std::vector<double> temp;

          std::stringstream ss;

          std::getline(fs, header, '\n');

          _TEMP = GetTempCSV(header);

          while (std::getline(fs, str, '\n'))
          {
               ss << str;

               while (std::getline(ss, str, delimiter))
               {
                    temp.push_back(std::stod(*(&str)));
               }

               table.push_back(temp);

               temp.clear();

               ss.clear();
          }

          for (auto& row : table){*begin(row) *= 1e-9;}
       }
    }
    catch (std::invalid_argument& ia)
    {
        std::cerr << ia.what() << '\n';
    }

    fs.close();

//    std::cout << "Success: .csv file closed" << '\n';

    table.push_back(_TEMP);


    // K_ABS[lambda][T] == table[lambda][T] [1/cm]
    // T == table[table.size() - 1] [K]
    return table;
};

std::vector<std::pair<double, double>> GetMeshCSV(const std::vector<std::vector<double>>& table, double& T){

    std::vector<std::pair<double, double>> mesh;

    //Define a lambda that returns true if the t-value
    //of the given temp range is < the caller's t value
    auto lessThan =
        [](const double temp, double t)
        {return temp < t;};

    try {

        //Find the first table entry whose value is >= caller's T value
        auto iter =
            std::lower_bound(table[table.size() - 1].cbegin(), table[table.size() - 1].cend(), T, lessThan);

        //If the caller's T value is greater than the largest
        //T value in the temp range, we can't interpolate.
        if(iter == table[table.size() - 1].cend()) {
          throw std::invalid_argument("Input T > T_MAX in .csv file");
        }

        //If the caller's T value is less than the smallest
        //T value in the temp range, we can't interpolate.
        if(iter == table[table.size() - 1].cbegin() and T < table[table.size() - 1][0]) {
          throw std::invalid_argument("Input T < T_MIN in .csv file");
        }

        // Interpolation's linear coefficient
        double coef = (T - *(iter - 1)) / (*iter - *(iter - 1));

        // Index of the lower t value in range: table[table.size() - 1][index] < T
        double index = std::distance(table[table.size() - 1].cbegin(), iter);

        // Fill in new MESH
        for (size_t i = 0; i < table.size() - 1; ++i) {

            // if T is not in .csv, but T_MIN <= T <= T_MAX -- interpolate
            if (!isinT(T, table)) {

                double deltaY = table[i][index + 1] - table[i][index];

                if (deltaY >= 0) {mesh.push_back(std::make_pair(
                                  table[i][0],
                                  100 * (table[i][index] + coef * deltaY)));}

                if (deltaY < 0) {mesh.push_back(std::make_pair(
                                 table[i][0],
                                 100 * (table[i][index] + std::pow(coef, -1) * deltaY)));}
            } else {

                //if T value coincides with some in .csv -- fill in without interpolation
                //check if coef == 1 -- fill in with the next K(T[index + 1])
                if (coef < 1) {mesh.push_back(std::make_pair(table[i][0],
                                                  100 * table[i][index]));}
                else {mesh.push_back(std::make_pair(table[i][0],
                                     100 * table[i][index + 1]));}
            }
        }
    }
    catch (std::exception& ex) {std::cerr << ex.what() << '\n';}

    // mesh_T[lambda_index].first == LAM[lambda_index]
    // mesh_T[lambda_index].second == K_T[lambda_index]
    return mesh;
}

/**
 * Retrieve the temperatures' values from .csv header (with respect to RegEx used)
 *
 * @brief GetColIndForTempCSVHead
 * @param str Header line to be parsed
 * @returns std::vector<double> Temperature values
 */
std::vector<double> GetTempCSV(std::string& str){

    std::regex reg("((\\w+,\\w+\\\\\\w+,\\w+);|T=((\\d+|\\d+\\.\\d*));?)");

    std::vector<double> Temp;

    auto firstIt = std::sregex_iterator(begin(str), end(str), reg);

    auto lastIt = std::sregex_iterator();

    try{

    for (std::sregex_iterator k = firstIt; k != lastIt; ++k) {

        std::smatch match = *k;

        try {Temp.push_back(std::stod(match[match.size() - 1]));}

        catch (...) {continue;}

//        try {if (std::stod(match[match.size() - 1]) == temp)
//            {return std::distance(firstIt, k);}
//            else{continue;}
//        } catch (...) {continue;}
        }
    }

    catch (std::exception& e){std::cerr << e.what() << '\n';}

    return Temp;
};

/*==============T=FIELD=FILE=PROCESSING=FUNCTIONS=============================*/

/**
 * Retrieve temperature values from OpenFOAM "T" file.
 *
 * @brief GetTempField
 * @param pathTemp Absolute PATH to "T" file.
 * @return std::vector<double> Temperature values
 */
std::vector<double> GetTempField(std::string& pathTemp)
{
std::ifstream Temperatures(pathTemp, std::ios_base::in);

std::vector<double> tempVal;

try
    {
       if (!Temperatures.is_open())
       {
           throw invalid_argument("OpenError: temp file");
       }
       else
       {
//          cout << "Success: temp file opened" << '\n';

          std::string str;

          while (str != "internalField   nonuniform List<scalar> ")
          {
              getline(Temperatures, str, '\n');
          }

          getline(Temperatures, str, '\n');

          int size = std::stoi(str);

          getline(Temperatures, str, '\n');

          for (int i = 0; i < size; ++i)
          {
              getline(Temperatures, str, '\n');

              tempVal.push_back(std::stod(str));
          }
       }
    }
    catch (invalid_argument& ia)
    {
        cerr << ia.what() << endl;
    }

    Temperatures.close();

//    std::cout << "Success: temp file closed" << '\n';

    return tempVal;
}

/*==============GRID=PROCESSING=FUNCTIONS=====================================*/

std::vector<std::pair<double, double>> interMesh(std::vector<std::pair<double, double>>& m1,
                                                 std::vector<std::pair<double, double>>& m2) {
    Interpolator grid1(m1);

    std::vector<std::pair<double, double>> m1n;

    for (auto& it: m1) {m1n.push_back(it);}

    for (size_t i = 0; i < m2.size(); ++i) {

        double xn = m2[i].first;

        if (!isinX(xn, m1)) {m1n.push_back(std::make_pair(xn, grid1.findValue(xn)));}

    }

    std::sort(std::begin(m1n), std::end(m1n),
              [](std::pair<double, double> a, std::pair<double, double> b) {
        return (b.first - a.first) > 1e-11;
    });

    return m1n;
}

/*==============MATH=FUNCTIONS=AND=BOOLEAN=FILTERS============================*/

bool isinX(double x, std::vector<std::pair<double, double>>& mesh) {

    std::vector<double> valuesX;

    for (auto& it: mesh) {valuesX.push_back(it.first);};

    if (std::find(std::begin(valuesX), std::end(valuesX), x) != std::end(valuesX)){return true;}

    return false;
}

bool isinT(double T, const std::vector<std::vector<double>>& table) {

    int count = 0;

    for (auto& it: table[table.size() - 1]) {if (it == T) {++count;}}

    if (count > 0) {return true;}

    return false;
}

double Planck(double lam, double T)
{
    return 2 * 6.626e-34 * std::pow(3e8, 2) * std::pow(lam, -5) /
            std::expm1(6.626e-34 * 3e8 * std::pow(lam * T * 1.38e-23, -1));
};

double integrateTrapezoidal(std::vector<std::pair<double, double>>& F)
{
    double sum = 0;

    for (std::size_t i = 1; i < F.size(); ++i)
    {
        sum += ((F[i - 1].second + F[i].second) * (F[i].first - F[i - 1].first));

    }
    return sum / 2;
}

double integrateTrapezoidal(double(&f)(double, double), double min, double max, double N, double T){

    double h = (max - min) / N;

    double sum = f(min, T) + f(max, T);

    for (int i = 1; i < N; ++i){sum += 2 * f(min + i * h, T);}

    return sum * (h / 2);
}

/*============================================================================*/
