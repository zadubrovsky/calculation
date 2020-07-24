#include "stdafx.h"
#include "functions.h"



int main()
{
    Timer t; //time counter

    std::string path_csv = "..\\input\\data.csv";

    std::string path_inp = "..\\input\\data.inp";

    std::string path_txt = "..\\input\\flow.txt";

    std::string path_tmp = "..\\input\\T";

    std::vector<std::vector<double>> kabs;

    std::vector<std::pair<double, double>> mesh2;

    std::vector<double> Temperatures = GetTempField(path_tmp);

    // Choose the input file format for K_ABS: "inp" or "csv"
    std::string mode = "inp";

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

    data.open("..\\output\\output.csv", std::ios_base::out);

    data << "T, K;Rate, 1/(s*m^3);\n";

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

    std::vector<std::pair<double, double>> func, func_eq;

    for (size_t i = 0; i < mesh1.size(); ++i) {

        double mlt = mesh1[i].second * mesh2new[i].second *
                     mesh1[i].first * 5.0307e33;
        if (mlt > 1e-10 && !std::isnan(mlt)) {func.push_back(
                        std::make_pair(mesh1[i].first, mlt));}
    }

    func.pop_back();

    for (size_t i = 0; i < mesh1.size(); ++i) {

        double mlt = mesh1[i].second * Planck(mesh1[i].first, T) *
                     mesh1[i].first * 5.0307e24;
        if (mlt && !std::isnan(mlt)) {func_eq.push_back(
                        std::make_pair(mesh1[i].first,
                                        mlt));}
    }

/*=====================ERASING=REDUNDANT=POINTS===============================*/

    func.erase(std::unique(func.begin(), func.end(),
             [](const std::pair<double, double>& a, const std::pair<double, double>& b)
             {return std::abs(a.first - b.first) < 1e-11;}), func.end());

/*=======================INTEGRATION==========================================*/

    double result = 4 * M_PI * integrateTrapezoidal(func);

    data << T << ';' << result << '\n';     //writing into output file

/*===============OUTPUTTING=RESULTS===========================================*/

////    double result_eq = M_PI * integrateTrapezoidal(func_eq);

//    double stbol = 5.67e-8 * std::pow(T, 4);

//    double plaw = M_PI * integrateTrapezoidal(Planck, 1e-9, 1e-4, 1e7, T);

//    cout << "=============================" << '\n';

//    cout << '\t' << T << " K" << '\n';

//    cout << "=============================" << '\n';

////    cout << "Equilibrium:\t " << result_eq << '\n';

    cout << T <<  " K\tRate:\t " << result  << '\n';

//    if (T == 34800) {

//        cout << "Error, %:\t " << 100 * std::abs(result - 1.939e30) / 1.939e30 << '\n';

//        cout << "=============================" << '\n';

//    } else {cout << "=============================" << '\n';}

//    //cout << "Planck:\t" << Planck(1e-8, 1e5)  << '\t' << Planck(1e-4, 1e5) << '\n';

//    cout << "Stefan-Boltzman: " << stbol << '\n';

//    cout << "Planck's law:\t " << plaw << '\n';

//    cout << "Error, %:\t " << 100 * std::abs(stbol - plaw) / stbol << '\n';

//    cout << "=============================" << '\n';

/*============================================================================*/

    }

    data.close();   //closing output file



    return 0;
};
