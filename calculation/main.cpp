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

int main()
{
    Timer t;

    cout << "thread ID: " << this_thread::get_id() << '\n';

//    ios_base::sync_with_stdio(false);

//    cin.tie(NULL);

    string path_csv = "D:\\Documents\\Kintech Lab\\MAY\\interpolation\\data.csv";

    string path_txt = "D:\\Documents\\Kintech Lab\\MAY\\interpolation\\flow.txt";

    std::ofstream data;

    int size;

    cout << "Enter T point number:\t";

    cin >> size;

    double* Temperatures = new double[size];

    for (int i = 0; i < size; ++i)
    {
        cout << "Enter value:\t";
        cin >> Temperatures[i];
    }

    data.open("integral.csv", std::ios_base::out);

    data << "Temperature;Trapezoidal;Simpson;\n";

    for (int i = 0; i < size; ++i)
    {
    int T = Temperatures[i];

    vector<pair<double, double>> mesh1, mesh2, mesh1new, mesh2new, meshNonEq;

    vector<vector<double>> tableCSV, tableTXT;

    vector<double> lambda1, lambda2, lambda;

    thread t1([&tableCSV, &path_csv, &mesh1, &meshNonEq, &T]() { tableCSV = csvParse(path_csv);
        cout << "thread ID-csv: " << this_thread::get_id() << '\n';
        mesh1 = GetMeshCSV(tableCSV, path_csv, T);
        meshNonEq = GetMeshCSV(tableCSV, path_csv, T);
    });

    thread t2([&tableTXT, &path_txt, &mesh2]() { tableTXT = txtParse(path_txt);
        cout << "thread ID-txt: " << this_thread::get_id() << '\n';
        mesh2 = GetMeshTXT(tableTXT);
    });

//      tableCSV = csvParse(path_csv);

//      tableTXT = txtParse(path_txt);

//      mesh1 = GetMeshCSV(tableCSV, path_csv, T);

//      mesh2 = GetMeshTXT(tableTXT);


    t1.join();

    t2.join();

    cout << "==============================" << '\n';

    cout << '\t' << T << " K" << '\n';

    cout << "==============================" << '\n';

    cout << "FILE PARSING" << '\n';

    cout << "==============================" << '\n';

    outputNodes(mesh1, mesh2);

//=====================CUT=MESH=================================================

    meshCut(mesh1, mesh2);

    mesh2.pop_back();

    meshCut(mesh2, mesh1);

    cout << "MESH CUTTING" << '\n';

    cout << "==============================" << '\n';

    outputNodes(mesh1, mesh2);

//==========================UNIFY=LAMBDA=RANGE==================================

    cout << "UNIFYING LAMBDA RANGE" << '\n';

    for (auto it : mesh1){lambda1.push_back(it.first);}

    for (auto it : mesh2){lambda2.push_back(it.first);}

    set_union(begin(lambda1), end(lambda1), begin(lambda2),
              end(lambda2), std::back_inserter(lambda),
              [](double a, double b){return b - a > 1e-12;});

    cout << "==============================" << '\n';

    cout << lambda1.size() << '\t' << lambda2.size() << '\t' << lambda.size() << '\n';


    cout << "==============================" << '\n';

//==============================================================================

    cout << "INTERPOLATION" << '\n';

    Interpolator interp1(mesh1);

    Interpolator interp2(mesh2);

    for (auto lam : lambda){

        mesh1new.push_back(make_pair(lam, interp1.findValue(lam)));

        mesh2new.push_back(make_pair(lam, interp2.findValue(lam)));

    }

    cout << "==============================" << '\n';

    outputNodes(mesh1new, mesh2new);

    for  (size_t i = 0; i < 100; ++i)
    {
        cout << mesh1new[i].first << "\t|\t" << mesh1new[i].second << "\t|\t"
             << mesh2new[i].second << '\n';
    }

    cout << "==============================" << '\n';

//=======================INTEGRATING===========================================

    std::vector<std::pair<double, double>> func = unIntNonEq(mesh1new, mesh2new, 1);
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

    cout << "==============================" << '\n';

    cout << "Trapezoidal:\t" << integrateTrapezoidal(func) << '\n';

    cout << "==============================" << '\n';

    cout << "Simpson:\t" << integrateSimpson(func) << '\n';

    cout << "==============================" << '\n';

    data << T << ';' << integrateTrapezoidal(func)
         << ';' << integrateSimpson(func) << ";\n";

    }

    data.close();

    delete[] Temperatures;
//==============================================================================

    return 0;
};
