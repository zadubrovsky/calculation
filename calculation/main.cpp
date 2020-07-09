#include "stdafx.h"
#include "functions.h"



int main()
{
    Timer t; //time counter

    cout << "thread ID: " << this_thread::get_id() << '\n';

    string path_csv = "..\\input\\data.csv";

    string path_txt = "..\\input\\flow.txt";

    std::string pathTemp = "..\\input\\T";

    std::vector<double> Temperatures = GetTempField(pathTemp);

/*=================CREATING=AN=OUTPUT=FILE====================================*/

    std::ofstream data;    

    data.open("..\\output\\output.csv", std::ios_base::out);

    data << "T, K;Rate, 1/(s*m^3);\n";

/*============================================================================*/

    for (size_t i = 0; i < Temperatures.size(); ++i){

    double T = Temperatures[i];

    vector<pair<double, double>> mesh1, mesh2, mesh1new, mesh2new;

    vector<vector<double>> tableCSV, tableTXT;

/*=============SPLITTING=THE=EXECUTION=UP=INTO=THREADS========================*/

    thread t1([&tableCSV, &path_csv, &mesh1, &T]() { tableCSV = csvParse(path_csv);
        cout << "thread ID-csv: " << this_thread::get_id() << '\n';

        mesh1 = GetMeshCSV(tableCSV, path_csv, T);
    });

    thread t2([&tableTXT, &path_txt, &mesh2]() { tableTXT = txtParse(path_txt);
        cout << "thread ID-txt: " << this_thread::get_id() << '\n';
        mesh2 = GetMeshTXT(tableTXT);
    });

/*===============JOINING=THE=THREADS=BACK======================================*/

    t1.join();

    t2.join();

/*=============INTERPOLATION==================================================*/

//    cout << "INTERPOLATION" << '\n';

//    cout << "=============================" << '\n';

    mesh1new = interMesh(mesh1, mesh2);

    mesh2new = interMesh(mesh2, mesh1);

//    cout << mesh1new.size() << '\t' << mesh2new.size() << '\n';

/*============================================================================*/

//    cout << '\n' << "SIEVING" << '\n';

//    cout << "=============================" << '\n';

/*=====================ERASING=REDUNDANT=POINTS===============================*/

    mesh1new.erase(std::unique(mesh1new.begin(), mesh1new.end(),
             [](const std::pair<double, double>& a, const std::pair<double, double>& b)
             {return std::abs(a.first - b.first) < 1e-11;}), mesh1new.end());

    mesh2new.erase(std::unique(mesh2new.begin(), mesh2new.end(),
             [](const std::pair<double, double>& a, const std::pair<double, double>& b)
             {return std::abs(a.first - b.first) < 1e-11;}), mesh2new.end());

/*=====================SORTING=JUST=IN=CASE===================================*/

    std::sort(std::begin(mesh1new), std::end(mesh1new),
              [](std::pair<double, double>& a, std::pair<double, double>& b)
              {return a.first < b.first;});

    std::sort(std::begin(mesh2new), std::end(mesh2new),
              [](std::pair<double, double>& a, std::pair<double, double>& b)
              {return a.first < b.first;});

/*============================================================================*/

//    cout << "\n";

//    cout << mesh1new.size() << '\t' << mesh2new.size() << '\n';

//    cout << "\n";

//    for (size_t i = 0; i < 10; ++i) {

//        cout << mesh1new[i].first << '\t' << mesh1new[i].second << '\t'
//             << mesh2new[i].first << '\t' << mesh2new[i].second << '\n';
//    }

//    cout << "\n\n";

//    for (size_t i = 0; i < 10; ++i) {

//        cout << mesh1new[mesh1new.size()  - i].first << '\t' << mesh1new[mesh1new.size() - i].second << '\t'
//             << mesh2new[mesh2new.size()  - i].first << '\t' << mesh2new[mesh2new.size() - i].second << '\n';
//    }

//    cout << '\n';

//    cout << mesh1new.size() << '\t' << mesh2new.size() << '\n';



/*==========================CALCULATION=======================================*/

//    cout << "=============================" << '\n';

//    cout << "CALCULATION" << '\n';

//    cout << "=============================" << '\n';

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

//    cout << '\n';

//    cout << func.size() << '\n';

//    cout << '\n';

/*=====================ERASING=REDUNDANT=POINTS===============================*/

    func.erase(std::unique(func.begin(), func.end(),
             [](const std::pair<double, double>& a, const std::pair<double, double>& b)
             {return std::abs(a.first - b.first) < 1e-11;}), func.end());

/*============================================================================*/

//    cout << '\n';

//    cout << func.size() << '\n';

//    cout << '\n';

//    for ( size_t i = 0; i < 10; ++i )
//    {
//        cout << func[i].first << '\t' << func[i].second << '\n';
//    }

//    cout << "\n\n";


//    for ( size_t i = func.size(); i > func.size() - 10; --i )
//    {
//        cout << func[i].first << '\t' << func[i].second << '\n';
//    }

/*=======================INTEGRATION==========================================*/

    double result = 4 * M_PI * integrateTrapezoidal(func);

    data << T << ';' << result << '\n';     //writing into output file

/*===============OUTPUTTING=RESULTS===========================================*/

//    double result_eq = M_PI * integrateTrapezoidal(func_eq);

    double stbol = 5.67e-8 * std::pow(T, 4);

    double plaw = M_PI * integrateTrapezoidal(Planck, 1e-9, 1e-4, 1e7, T);

    cout << "=============================" << '\n';

    cout << '\t' << T << " K" << '\n';

    cout << "=============================" << '\n';

//    cout << "Equilibrium:\t " << result_eq << '\n';

    cout << "Trapezoidal:\t " << result  << '\n';

    if (T == 34800) {

        cout << "Error, %:\t " << 100 * std::abs(result - 1.939e28) / 1.939e28 << '\n';

        cout << "=============================" << '\n';

    } else {cout << "=============================" << '\n';}

    //cout << "Planck:\t" << Planck(1e-8, 1e5)  << '\t' << Planck(1e-4, 1e5) << '\n';

    cout << "Stefan-Boltzman: " << stbol << '\n';

    cout << "Planck's law:\t " << plaw << '\n';

    cout << "Error, %:\t " << 100 * std::abs(stbol - plaw) / stbol << '\n';

    cout << "=============================" << '\n';

/*========WITHOUT=THE=ABOVE=BLOCK=PROGRAMME=EXECUTION=TAKES=2.2=SECONDS=======*/

    }

    data.close();   //closing output file

    return 0;
};
