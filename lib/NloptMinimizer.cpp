/*
 * NloptMinimizer.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2016 Antonin Portelli
 *
 * LatAnalyze 3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze 3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze 3.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <LatAnalyze/NloptMinimizer.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                      NloptMinimizer implementation                         *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
NloptMinimizer::NloptMinimizer(const Algorithm algorithm)
{
    setAlgorithm(algorithm);
}

NloptMinimizer::NloptMinimizer(const Index dim, const Algorithm algorithm)
: Minimizer(dim)
{
    setAlgorithm(algorithm);
}

// access //////////////////////////////////////////////////////////////////////
NloptMinimizer::Algorithm NloptMinimizer::getAlgorithm(void) const
{
    return algorithm_;
}

void NloptMinimizer::setAlgorithm(const Algorithm algorithm)
{
    algorithm_ = algorithm;
}

// minimization ////////////////////////////////////////////////////////////////
const DVec & NloptMinimizer::operator()(const DoubleFunction &f)
{
    DVec &x = getState();
    
    // resize minimizer state to match function number of arguments
    if (f.getNArg() != x.size())
    {
        resize(f.getNArg());
    }
    
    // create and set minimizer
    nlopt::opt     min(getAlgorithm(), x.size());
    NloptFuncData  data;
    vector<double> lb(x.size()), hb(x.size());
    
    min.set_maxeval(getMaxIteration());
    min.set_xtol_rel(getPrecision());
    data.f = &f;
    min.set_min_objective(&funcWrapper, &data);
    for (Index i = 0; i < x.size(); ++i)
    {
        lb[i] = hasLowLimit(i)  ? getLowLimit(i)  : -HUGE_VAL;
        hb[i] = hasHighLimit(i) ? getHighLimit(i) :  HUGE_VAL;
    }
    min.set_lower_bounds(lb);
    min.set_upper_bounds(hb);
    
    // minimise
    double         res;
    vector<double> vx(x.size());
    nlopt::result  status;
    unsigned int   n = 0;
    
    for (Index i = 0; i < x.size(); ++i)
    {
        vx[i] = x(i);
    }
    do
    {
        if (getVerbosity() >= Verbosity::Normal)
        {
            cout << "========== NLopt minimization, pass #" << n + 1;
            cout << " ==========" << endl;
            cout << "Algorithm: " << min.get_algorithm_name() << endl;
            cout << "Max eval.= " << min.get_maxeval();
            cout << " -- Precision= " << min.get_xtol_rel() << endl;
            cout << "starting f(x)= " << f(x) << endl;
        }
        try
        {
            status = min.optimize(vx, res);
        }
        catch (invalid_argument &e)
        {
            LATAN_ERROR(Runtime, "NLopt has reported receving invalid "
                        "arguments (if you are using a global minimizer, did "
                        "you specify limits for all variables?)");
        }
        if (getVerbosity() >= Verbosity::Normal)
        {
            cout << "Found minimum " << res << " at:" << endl;
            for (Index i = 0; i < x.size(); ++i)
            {
                printf("%8s= %e\n", f.varName().getName(i).c_str(), vx[i]);
            }
            cout << "after " << data.evalCount << " evaluations" << endl;
            cout << "Minimization ended with code " << status;
            cout << " (" << returnMessage(status) << ")";
            cout << endl;
        }
        data.evalCount = 0;
        for (Index i = 0; i < x.size(); ++i)
        {
            x(i) = vx[i];
        }
        n++;
    } while ((status != nlopt::XTOL_REACHED) and (status != nlopt::SUCCESS)
             and (n < getMaxPass()));
    if (getVerbosity() >= Verbosity::Normal)
    {
        cout << "=================================================" << endl;
    }
    if ((status != nlopt::XTOL_REACHED) and (status != nlopt::SUCCESS))
    {
        LATAN_WARNING("invalid minimum: " + returnMessage(status));
    }
    
    return x;
}

// NLopt return code parser ////////////////////////////////////////////////////
string NloptMinimizer::returnMessage(const nlopt::result status)
{
    switch (status)
    {
        case nlopt::SUCCESS:
            return "success";
        case nlopt::STOPVAL_REACHED:
            return "stopping value reached";
        case nlopt::FTOL_REACHED:
            return "tolerance on function reached";
        case nlopt::XTOL_REACHED:
            return "tolerance on variable reached";
        case nlopt::MAXEVAL_REACHED:
            return "maximum function evaluation reached";
        case nlopt::MAXTIME_REACHED:
            return "maximum time reached";
        default:
            return "";
    }
}

// NLopt function wrapper //////////////////////////////////////////////////////
double NloptMinimizer::funcWrapper(unsigned int n __dumb, const double *arg,
                                   double *grad , void *vdata)
{
    NloptFuncData &data = *static_cast<NloptFuncData *>(vdata);
    
    assert(grad == nullptr);
    data.evalCount++;
    
    return (*data.f)(arg);
}
