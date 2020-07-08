#include "stdafx.h"
#include "functions.h"
#include <stdexcept>

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
           throw invalid_argument("OpenError");
       }
       else
       {
          cout << "Success: file opened" << '\n';

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

    return tempVal;
}

/**
 * Retrieve the index of column within .csv header (with respect to RegEx used)
 * which inludes temp value as a subpattern.
 *
 * @brief GetColIndForTempCSVHead
 * @param str Header line to be parsed
 * @param temp Temperature value to be found
 * @throw std::invalid_argument Thrown if no such temp value is in str
 * @returns int Column number where temp value is situated, -1 otherwise
 */
int GetColIndForTempCSVHead(std::string& str, double& temp){

    std::regex reg("((\\w+,\\w+\\\\\\w+,\\w+);|T=((\\d+|\\d+\\.\\d*));?)");

    auto firstIt = std::sregex_iterator(begin(str), end(str), reg);

    auto lastIt = std::sregex_iterator();

    try{

    for (std::sregex_iterator k = firstIt; k != lastIt; ++k)
    {
        std::smatch match = *k;

        try {if (std::stod(match[match.size() - 1]) == temp)
            {return std::distance(firstIt, k);}
            else{continue;}
        } catch (...) {continue;}
    }

    throw std::invalid_argument("No such value in headline of .csv file");

    }

    catch (std::exception& e){std::cerr << e.what() << '\n';}

    return -1;
};

/**
 * Retrieve header line out of .csv file.
 *
 * @brief GetHeaderLine
 * @param path Path to the .csv file.
 * @returns std::string  Header line as std::string
 */
std::string GetHeaderLine(std::string const& path)
{
    std::fstream fs;

    std::string header;

    fs.open(path, std::fstream::in);

    std::getline(fs, header, '\n');

    std::cout << "Header captured" << '\n';

    return header;
}

std::vector<std::pair<double, double>> GetMeshCSV(const std::vector<std::vector<double>>& table, const std::string& pathCSV, double& T){

    std::vector<std::pair<double, double>> mesh;

    std::string header = GetHeaderLine(pathCSV);

    int colNum = GetColIndForTempCSVHead(header, T);

    for (auto& it : table)
    {
        mesh.push_back(std::make_pair(it[0], it[colNum]));
    }

    return mesh;
}

std::vector<std::pair<double, double>> GetMeshTXT(const std::vector<std::vector<double>>& table, int colNumber){

    std::vector<std::pair<double, double>> mesh;

    for (auto& it : table)
    {
        mesh.push_back(std::make_pair(it[0], it[colNumber]));
    }

    return mesh;
}

std::vector<std::vector<double>> csvParse(std::string const& path, const char delimiter)
{
    std::fstream fs;

    fs.open(path, std::fstream::in);

    std::vector<std::vector<double>> table;

    std::vector<std::pair<double, double>> mesh;

    try
    {
       if (!fs.is_open())
       {
           throw std::invalid_argument("OpenError");
       }
       else
       {
          cout << "Success: .csv file opened" << '\n';

          std::string str;

          std::vector<double> temp;

          std::stringstream ss;

          std::getline(fs, str, '\n');

          str.clear();

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

    std::cout << "Success: .csv file closed" << '\n';

    return table;
};

std::vector<std::vector<double>> txtParse(std::string const& path)
{
    std::fstream tx;

    tx.open(path, std::fstream::in);

    std::vector<std::vector<double>> table;

    try
    {
       if (!tx.is_open())
       {
           throw std::invalid_argument("OpenError");
       }
       else
       {
          cout << "Success: .txt file opened" << '\n';

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

    std::cout << "Success: .txt file closed" << '\n';

    return table;
};

std::vector<std::pair<double, double>> meshCut(
            const std::vector<std::pair<double, double>>& mesh_to_fit,
            std::vector<std::pair<double, double>>& mesh_to_cut)
{
    auto lessThan =
        [](const std::pair<double, double>& A, double B)
        {return A.first < B;};

    auto lessThan1 =
        [](double A, const std::pair<double, double>& B)
        {return A < B.first;};

    for  (auto& it : mesh_to_cut)
    {
        auto iter = std::lower_bound(begin(mesh_to_fit), end(mesh_to_fit), it.first, lessThan);

        if(iter == mesh_to_fit.cend()){mesh_to_cut.erase(lower_bound(begin(mesh_to_cut),
                                 end(mesh_to_cut), it.first, lessThan), end(mesh_to_cut));}

        if(iter == mesh_to_fit.cbegin() and it.first <= mesh_to_fit.cbegin()->first){
            mesh_to_cut.erase(begin(mesh_to_cut), upper_bound(begin(mesh_to_cut),
            end(mesh_to_cut), it.first, lessThan1));}
    };

    return mesh_to_cut;
}

double Planck(double lam, double T)
{
    return 2 * 6.626e-34 * std::pow(3e8, 2) * std::pow(lam, -5) /
            std::expm1(6.626e-34 * 3e8 * std::pow(lam * T * 1.38e-23, -1));
};

std::vector<std::pair<double, double>> unIntNonEq(std::vector<std::pair<double, double>>& K, std::vector<std::pair<double, double>>& F, double z = 1)
{
     std::vector<std::pair<double, double>> func;

     for (std::size_t i = 0; i < F.size(); ++i)
     {
         func.push_back(std::make_pair(F[i].first,
                (4 * M_PI * z * 1e9 * F[i].second * K[i].second * F[i].first)/
                (6.626e-34 * 3e8)));
     }
     return func;
};

std::vector<std::pair<double, double>> unIntEq(std::vector<std::pair<double, double>>& K, double T, double z = 1, double G = 1)
{
    std::vector<std::pair<double, double>> func;

    for (std::size_t i = 0; i < K.size(); ++i)
    {
        func.push_back(std::make_pair(K[i].first,
            ((z * G * Planck(K[i].first, T) * K[i].second * K[i].first) /
            (6.626e-34 * 3e8))));
    }
    return func;
}

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

double integrateSimpson(std::vector<std::pair<double, double>>& F)
{
    double sum = 0;

    for (std::size_t i = 0; i < F.size(); ++i)
    {
        if (i == 0 || i == F.size()) { sum += F[i].second;}

        else if (i % 2 != 0) { sum += 4 * F[i].second;}

        else { sum += 2 * F[i].second;}
    }

    return sum / 3 ;
}
