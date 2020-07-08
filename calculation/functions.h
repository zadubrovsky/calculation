#ifndef FUNC_H
#define FUNC_H

#include <vector>
#include <string>


std::vector<std::vector<double>> txtParse(std::string const&);
std::vector<std::vector<double>> csvParse(std::string const&, const char deliiter = ';');
std::vector<std::pair<double, double>> GetMeshCSV(const std::vector<std::vector<double>>&, const std::string&, double&);
std::vector<std::pair<double, double>> GetMeshTXT(const std::vector<std::vector<double>>&, int colNumber = 1);
std::vector<double> GetTempField(std::string&);
std::string GetHeaderLine(std::string const&);
int GetColIndForTempCSVHead(std::string&, double&);
std::vector<std::pair<double, double>> interMesh(std::vector<std::pair<double, double>>&,
                                                 std::vector<std::pair<double, double>>&);
bool isinX(double x, std::vector<std::pair<double, double>>&);
std::vector<std::pair<double, double>> meshCut(const std::vector<std::pair<double, double>>&,std::vector<std::pair<double, double>>&);

double Planck(double lam, double T);
std::vector<std::pair<double, double>> unIntNonEq(std::vector<std::pair<double, double>>&, std::vector<std::pair<double, double>>&, double);
std::vector<std::pair<double, double>> unIntEq(std::vector<std::pair<double, double>>&, double, double, double);
double underInt(double, double, double, double, double);
double Simpson(double f(double, double, double, double, double), std::vector<std::vector<double>>);
double IntRate(std::vector<std::pair<double, double>>&);
double IntRate1(std::vector<std::pair<double, double>>&);
double integrateTrapezoidal(std::vector<std::pair<double, double>>&);

double integrateTrapezoidal(double(&f)(double, double), double, double, double, double);
double integrateSimpson(std::vector<std::pair<double, double>>&);


#endif // FUNC_H
