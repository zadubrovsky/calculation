#include "stdafx.h"
#include "functions.h"



int main()
{
    Timer t; //time counter

    std::string path_csv = "..\\input\\data_sample.csv";

    std::string path_inp = "..\\input\\data_sample.inp";

    std::string path_txt = "..\\input\\flow_sample.txt";

    std::string path_con = "..\\input\\con_sample.csv";

    std::string path_tmp = "..\\input\\T";

    std::vector<std::vector<double>> kabs;

    std::vector<std::pair<double, double>> mesh2;

    std::vector<double> Temperatures = GetTempField(path_tmp);

    std::vector<std::pair<double, double>> ConcentrationS2 = concParse(path_con, Temperatures);

    // Choose the input file format for K_ABS: "inp" or "csv"
    std::string mode = "csv";

    if (mode == "inp") {

        thread t1([&path_inp, &kabs]() {kabs = inpParse(path_inp);});

        thread t2([&path_txt, &mesh2]() {mesh2 = GetMeshTXT(txtParse(path_txt));});

        t1.join();

        t2.join();
    }

    if (mode == "csv") {

        thread t1([&path_csv, &kabs]() {kabs = csvParse(path_csv);});

        thread t2([&path_txt, &mesh2]() {mesh2 = GetMeshTXT(txtParse(path_txt));});

        t1.join();

        t2.join();
    }

/*=================CREATING=AN=OUTPUT=FILE====================================*/

    std::ofstream data;

    data.open("..\\output\\phptorate.dat", std::ios_base::out);

    data << Temperatures.size() << "\n(\n";

/*==============ITERATING=OVER=TEMPERATURE=RANGE==============================*/

    for (size_t i = 0; i < Temperatures.size(); ++i){

    double T = Temperatures[i];

    vector<pair<double, double>> mesh1, mesh1new, mesh2new;

/*=============GETTING=MESH=FROM=KABS=========================================*/

    if (mode == "inp") {mesh1 = GetMeshINP(kabs, T);}

    if (mode == "csv") {mesh1 = GetMeshCSV(kabs, T);}

/*=============INTERPOLATION==================================================*/

    mesh1new = interMesh(mesh1, mesh2);

    mesh2new = interMesh(mesh2, mesh1);

/*=====================ERASING=REDUNDANT=POINTS===============================*/

    mesh1new.erase(std::unique(mesh1new.begin(), mesh1new.end(),
             [](const std::pair<double, double>& a, const std::pair<double, double>& b)
             {return std::abs(a.first - b.first) < 1e-11;}), mesh1new.end());

    mesh2new.erase(std::unique(mesh2new.begin(), mesh2new.end(),
             [](const std::pair<double, double>& a, const std::pair<double, double>& b)
             {return std::abs(a.first - b.first) < 1e-11;}), mesh2new.end());

/*=========================SORTING=CHECK======================================*/

    if (!std::is_sorted(std::begin(mesh1new), std::end(mesh1new),
                   [](std::pair<double, double>& a, std::pair<double, double>& b)
                   {return a.first < b.first;}))
              {std::sort(std::begin(mesh1new), std::end(mesh1new),
              [](std::pair<double, double>& a, std::pair<double, double>& b)
              {return a.first < b.first;});}

    if (!std::is_sorted(std::begin(mesh2new), std::end(mesh2new),
                        [](std::pair<double, double>& a, std::pair<double, double>& b)
                        {return a.first < b.first;}))
              {std::sort(std::begin(mesh2new), std::end(mesh2new),
              [](std::pair<double, double>& a, std::pair<double, double>& b)
              {return a.first < b.first;});}

/*==========================CALCULATION=======================================*/

    std::vector<std::pair<double, double>> func/*, func_eq*/;

    for (size_t i = 0; i < mesh1.size(); ++i) {

        double mlt = mesh1[i].second * mesh2new[i].second *
                     mesh1[i].first * 5.0307e33;
        if (mlt > 1e-10 && !std::isnan(mlt)) {func.push_back(
                        std::make_pair(mesh1[i].first, mlt));}
    }

    func.pop_back();

/*=====================ERASING=REDUNDANT=POINTS===============================*/

    func.erase(std::unique(func.begin(), func.end(),
             [](const std::pair<double, double>& a, const std::pair<double, double>& b)
             {return std::abs(a.first - b.first) < 1e-11;}), func.end());

/*=======================INTEGRATION==========================================*/

    double result = 4 * M_PI * integrateTrapezoidal(func) / ConcentrationS2[i].second;

    data << result << '\n';     //writing into output file

/*===============OUTPUTTING=RESULTS===========================================*/

//    std::cout << T <<  " K\tRate:\t " << result  << '\n';

/*============================================================================*/

    }

    data << ")\n";

    data.close();   //closing output file

/*===========FLOW=FILE=GENERATING================================================*/

//    double te = 5000;

//    std::ofstream flow("..\\output\\flow_my.txt", std::ios_base::out);
    
////    std::vector<std::vector<double>> table_csv = csvParse("..\\input\\absS2_my.csv", ';');
    
////    std::vector<std::pair<double, double>> m_csv = GetMeshCSV(table_csv, te);
    
//    for (size_t i = 20; i < 801; ++i) {

//        flow << " " << i * 1e-9 << " " << Planck(i * 1e-9, te) / 1e-9
//             << " " << NAN << "  " << NAN << '\n';
//    }

//    flow.close();

/*============================================================================*/


    return 0;
};
