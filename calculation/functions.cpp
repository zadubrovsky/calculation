#include "stdafx.h"
#include "functions.h"
#include <stdexcept>



int GetColIndForTempCSVHead(std::string& str, int& temp){

    std::regex reg("((\\w+,\\w+\\\\\\w+,\\w+);|T=(\\d{1,6});?)");

    auto firstIt = std::sregex_iterator(begin(str), end(str), reg);

    auto lastIt = std::sregex_iterator();

    try{

    for (std::sregex_iterator k = firstIt; k != lastIt; ++k)
    {
        std::smatch match = *k;

        if (match[match.size() - 1] == std::to_string(temp)){return std::distance(firstIt, k);}

        else{continue;}
    }

    throw std::invalid_argument("No such value in headline of .csv file");

    }

    catch (std::exception& e){std::cerr << e.what() << '\n';}

    return -1;
};

std::string GetHeaderLine(std::string const& path)
{
    std::fstream fs;

    std::string header;

    fs.open(path, std::fstream::in);

    std::getline(fs, header, '\n');

    std::cout << "Header captured" << '\n';

    return header;
}

std::vector<std::pair<double, double>> GetMeshCSV(const std::vector<std::vector<double>>& table, const std::string& pathCSV, int& T){

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

        if(iter == mesh_to_fit.cend()){mesh_to_cut.erase(upper_bound(begin(mesh_to_cut),
                                 end(mesh_to_cut), it.first, lessThan1), end(mesh_to_cut));}

        if(iter == mesh_to_fit.cbegin() and it.first <= mesh_to_fit.cbegin()->first){
            mesh_to_cut.erase(begin(mesh_to_cut), upper_bound(begin(mesh_to_cut),
            end(mesh_to_cut), it.first, lessThan1));}
    };

    return mesh_to_cut;
}

double Planck(double lam, double T)
{
    return 2 * 6.626e-34 * pow(3e8, 2) * pow(lam, -5) *
            expm1(6.626e-34 * 3e8 * pow(lam * T * 1.38e-23, -1));
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

double IntRate(std::vector<std::pair<double, double>>& F)
{
    const double h = (prev(end(F))->first - begin(F)->first) / F.size();

    double k1 = 0, k2 = 0;

    for (std::size_t i = 1; i < F.size(); i += 2)
    {
        k1 += F[i].second;

        k2 += F[i + 1].second;
    }
    return h / 3 * (F[0].second + 4 * k1 + 2 * k2);
}
