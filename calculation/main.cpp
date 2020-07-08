#include "stdafx.h"
#include "functions.h"
//#include <bits/stdc++.h>


void outputNodes(std::vector<std::pair<double, double>>& mesh1,
                 std::vector<std::pair<double, double>>& mesh2){

    cout << "1: " << mesh1.size() << "\t2: " << mesh2.size() << '\n';

    cout << "==============================" << '\n';

    cout << "1[F]: "<< (begin(mesh1))->first << "\t" << (begin(mesh1))->second << '\n';

    cout << "1[L]: "<< prev(end(mesh1))->first << "\t" << prev(end(mesh1))->second << '\n';

    cout << "==============================" << '\n';

    cout << "2[F]: "<< (begin(mesh2))->first << "\t" << (begin(mesh2))->second << '\n';

    cout << "2[L]: "<< prev(end(mesh2))->first << "\t" << prev(end(mesh2))->second << '\n';

    cout << "==============================" << '\n';

}

bool isinX(double x, std::vector<std::pair<double, double>>& mesh) {

    std::vector<double> valuesX;

    for (auto& it: mesh) {valuesX.push_back(it.first);};

    if (std::find(std::begin(valuesX), std::end(valuesX), x) != std::end(valuesX)){return true;}

    return false;
}

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


int main()
{
    Timer t;

    cout << "thread ID: " << this_thread::get_id() << '\n';

//    ios_base::sync_with_stdio(false);

//    cin.tie(NULL);

    string path_csv = "D:\\Documents\\Kintech Lab\\MAY\\interpolation\\data.csv";

    string path_txt = "D:\\Documents\\Kintech Lab\\MAY\\interpolation\\flow.txt";

    std::string pathTemp = "D:\\Documents\\Kintech Lab\\JUNE\\T";

    std::ofstream data;

    std::vector<double> Temperatures = GetTempField(pathTemp);

    data.open("C:\\Users\\user\\Desktop\\integral.csv", std::ios_base::out);

    data << "Temperature;Trapezoidal;Simpson;\n";

    for (size_t i = 0; i < Temperatures.size(); ++i)
    {
    double T = Temperatures[i];

    vector<pair<double, double>> mesh1, mesh2, mesh1new, mesh2new, meshNonEq;

    vector<vector<double>> tableCSV, tableTXT;

    vector<double> lambda1, lambda2, lambda;

    thread t1([&tableCSV, &path_csv, &mesh1, /*&meshNonEq,*/ &T]() { tableCSV = csvParse(path_csv);
        cout << "thread ID-csv: " << this_thread::get_id() << '\n';

        mesh1 = GetMeshCSV(tableCSV, path_csv, T);

        //meshNonEq = GetMeshCSV(tableCSV, path_csv, T);
    });

    thread t2([&tableTXT, &path_txt, &mesh2]() { tableTXT = txtParse(path_txt);
        cout << "thread ID-txt: " << this_thread::get_id() << '\n';
        mesh2 = GetMeshTXT(tableTXT);
    });

    t1.join();

    t2.join();

    cout << "==============================" << '\n';

    cout << '\t' << T << " K" << '\n';

    cout << "==============================" << '\n';

    cout << "FILE PARSING" << '\n';

    cout << "==============================" << '\n';

    outputNodes(mesh1, mesh2);

//=====================CUT=MESH=================================================

//    meshCut(mesh1, mesh2);

//    mesh2.pop_back();

//    meshCut(mesh2, mesh1);

//    cout << "MESH CUTTING" << '\n';

//    cout << "==============================" << '\n';

//    outputNodes(mesh1, mesh2);

//==========================UNIFY=LAMBDA=RANGE==================================

//    cout << "UNIFYING LAMBDA RANGE" << '\n';

//    for (auto it : mesh1){lambda1.push_back(it.first);}

//    for (auto it : mesh2){lambda2.push_back(it.first);}

//    set_union(begin(lambda1), end(lambda1), begin(lambda2),
//              end(lambda2), std::back_inserter(lambda),
//              [](double a, double b){return b - a > 1e-12;});

//    cout << "==============================" << '\n';

//    cout << lambda1.size() << '\t' << lambda2.size() << '\t' << lambda.size() << '\n';


//    cout << "==============================" << '\n';

//==============================================================================

    cout << "INTERPOLATION" << '\n';

    cout << "==============================" << '\n';

    mesh1new = interMesh(mesh1, mesh2);

    mesh2new = interMesh(mesh2, mesh1);

    cout << mesh1new.size() << '\t' << mesh2new.size() << '\n';

    std::vector<std::pair<double, double>> func;

    for (size_t i = 0; i < mesh1new.size(); ++i) {

        double mlt = mesh1new[i].second * mesh2new[i].second *
                mesh1new[i].first;

        if (mlt && !std::isnan(mlt) && mlt > 1e-12) {func.push_back(
                        std::make_pair(mesh1new[i].first,
                                        mlt));}
    }

//    Interpolator interp1(mesh1);

//    Interpolator interp2(mesh2);

//    for (auto lam : lambda){

//        mesh1new.push_back(make_pair(lam, interp1.findValue(lam)));

//        mesh2new.push_back(make_pair(lam, interp2.findValue(lam)));

//    }

//    outputNodes(mesh1new, mesh2new);

//    for  (size_t i = 0; i < 100; ++i)
//    {
//        cout << mesh1new[i].first << "\t|\t" << mesh1new[i].second << "\t|\t"
//             << mesh2new[i].second << '\n';
//    }

//    cout << "==============================" << '\n';

//=======================INTEGRATING===========================================

//    std::vector<std::pair<double, double>> func = unIntNonEq(mesh1new, mesh2new, 1);
//    std::vector<std::pair<double, double>> func = unIntEq(meshNonEq, T, 1, 1);


    for ( size_t i = 0; i < 10; ++i )
    {
        cout << func[i].first << '\t' << func[i].second << '\n';
    }

    cout << "==============================" << '\n';


    for ( size_t i = func.size(); i > func.size() - 10; --i )
    {
        cout << func[i].first << '\t' << func[i].second << '\n';
    }

    double result = 4 * M_PI * 5.0307e33 * integrateTrapezoidal(func);

    cout << "==============================" << '\n';

    cout << "Trapezoidal:\t" << result  << '\n';

    cout << "==============================" << '\n';

    data << T << ';' << result << '\n';

    }

    data.close();

//==============================================================================

    return 0;
};
