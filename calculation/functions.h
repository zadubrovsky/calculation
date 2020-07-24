#ifndef FUNC_H
#define FUNC_H

#include <vector>
#include <string>

/*==============.TXT=FILE=PROCESSINF=FUNCTIONS================================*/

std::vector<std::vector<double>> txtParse(std::string const&);

std::vector<std::pair<double, double>> GetMeshTXT(const std::vector<std::vector<double>>&, int colNumber = 1);

/*==============.INP=FILE=PROCESSING=FUNCTIONS================================*/

std::vector<std::vector<double>> inpParse(std::string&);

std::vector<std::pair<double, double>> GetMeshINP(const std::vector<std::vector<double>>&, double&);

/*==============.CSV=FILE=PROCESSINF=FUNCTIONS================================*/

std::vector<std::vector<double>> csvParse(std::string const&, const char deliiter = ';');

std::vector<std::pair<double, double>> GetMeshCSV(const std::vector<std::vector<double>>&, double&);

std::vector<double> GetTempCSV(std::string&);

/*==============T=FIELD=FILE=PROCESSINF=FUNCTIONS=============================*/

std::vector<double> GetTempField(std::string&);

/*==============GRID=PROCESSING=FUNCTIONS=====================================*/

std::vector<std::pair<double, double>> interMesh(std::vector<std::pair<double, double>>&,
                                                 std::vector<std::pair<double, double>>&);

/*==============MATH=FUNCTIONS=AND=BOOLEAN=FILTERS============================*/

bool isinX(double x, std::vector<std::pair<double, double>>&);

bool isinT(double T, const std::vector<std::vector<double>>&);

double Planck(double lam, double T);

double integrateTrapezoidal(std::vector<std::pair<double, double>>&);

double integrateTrapezoidal(double(&f)(double, double), double, double, double, double);

/*============================================================================*/


#endif // FUNC_H
