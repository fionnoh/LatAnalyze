/*
 * XYStatData.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2015 Antonin Portelli
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

#include <LatAnalyze/XYStatData.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/Math.hpp>

using namespace std;
using namespace Latan;
using namespace Math;

/******************************************************************************
 *                          FitResult implementation                          *
 ******************************************************************************/
// access //////////////////////////////////////////////////////////////////////
double FitResult::getChi2(void) const
{
    return chi2_;
}

double FitResult::getChi2PerDof(void) const
{
    return chi2_/getNDof();
}

double FitResult::getNDof(void) const
{
    return static_cast<double>(nDof_);
}

double FitResult::getPValue(void) const
{
    return chi2PValue(getChi2(), getNDof());;
}

const DoubleFunction & FitResult::getModel(const Index j) const
{
    return model_[static_cast<unsigned int>(j)];
}

/******************************************************************************
 *                         XYStatData implementation                          *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
XYStatData::XYStatData(void)
: chi2_(*this)
{}

XYStatData::XYStatData(const Index nData, const Index xDim, const Index yDim)
: XYStatData()
{
    resize(nData, xDim, yDim);
}

// access //////////////////////////////////////////////////////////////////////
void XYStatData::resize(const Index nData, const Index xDim, const Index yDim)
{
    FitInterface::resize(nData, xDim, yDim);
    x_.resize(nData, xDim);
    y_.resize(nData, yDim);
    var_[xx].resize(xDim, xDim);
    var_[yy].resize(yDim, yDim);
    var_[yx].resize(yDim, xDim);
    FOR_MAT(var_[xx], i1, i2)
    {
        var_[xx](i1, i2).resize(nData, nData);
    }
    FOR_MAT(var_[yy], j1, j2)
    {
        var_[yy](j1, j2).resize(nData, nData);
    }
    FOR_MAT(var_[yx], j, i)
    {
        var_[yx](j, i).resize(nData, nData);
    }
}

void XYStatData::reinitChi2(const bool doReinit)
{
    reinitChi2_ = doReinit;
}

Block<MatBase<double>> XYStatData::x(const PlaceHolder ph1 __dumb,
                                     const PlaceHolder ph2 __dumb)
{
    return x_.block(0, 0, getNData(), getXDim());
}

ConstBlock<MatBase<double>> XYStatData::x(const PlaceHolder ph1 __dumb,
                                          const PlaceHolder ph2 __dumb)
                                          const
{
    return x_.block(0, 0, getNData(), getXDim());
}

Block<MatBase<double>> XYStatData::x(const Index i,
                                     const PlaceHolder ph2 __dumb)
{
    return x_.block(0, i, getNData(), 1);
}

ConstBlock<MatBase<double>> XYStatData::x(const Index i,
                                          const PlaceHolder ph2 __dumb)
                                          const
{
    return x_.block(0, i, getNData(), 1);
}

Block<MatBase<double>> XYStatData::x(const PlaceHolder ph1 __dumb,
                                     const Index k)
{
    return x_.block(k, 0, 1, getXDim());
}

ConstBlock<MatBase<double>> XYStatData::x(const PlaceHolder ph1 __dumb,
                                          const Index k) const
{
    return x_.block(k, 0, 1, getXDim());
}

double & XYStatData::x(const Index i, const Index k)
{
    return x_(k, i);
}

const double & XYStatData::x(const Index i, const Index k) const
{
    return x_(k, i);
}

Block<MatBase<double>> XYStatData::y(const PlaceHolder ph1 __dumb,
                                     const PlaceHolder ph2 __dumb)
{
    return y_.block(0, 0, getNData(), getYDim());
}

ConstBlock<MatBase<double>> XYStatData::y(const PlaceHolder ph1 __dumb,
                                          const PlaceHolder ph2 __dumb)
                                          const
{
    return y_.block(0, 0, getNData(), getYDim());
}

Block<MatBase<double>> XYStatData::y(const Index j,
                                     const PlaceHolder ph2 __dumb)
{
    return y_.block(0, j, getNData(), 1);
}

ConstBlock<MatBase<double>> XYStatData::y(const Index j,
                                          const PlaceHolder ph2 __dumb)
                                          const
{
    return y_.block(0, j, getNData(), 1);
}

Block<MatBase<double>> XYStatData::y(const PlaceHolder ph1 __dumb, const Index k)
{
    return y_.block(k, 0, 1, getYDim());
}

ConstBlock<MatBase<double>> XYStatData::y(const PlaceHolder ph1 __dumb,
                                          const Index k) const
{
    return y_.block(k, 0, 1, getYDim());
}

double & XYStatData::y(const Index j, const Index k)
{
    return y_(k, j);
}

const double & XYStatData::y(const Index j, const Index k) const
{
    return y_(k, j);
}

#define FULL_BLOCK(m) (m).block(0, 0, (m).rows(), (m).cols())

Block<MatBase<double>> XYStatData::xxVar(const Index i1, const Index i2)
{
    return FULL_BLOCK(var_[xx](i1, i2));
}

ConstBlock<MatBase<double>> XYStatData::xxVar(const Index i1,
                                              const Index i2) const
{
    return FULL_BLOCK(var_[xx](i1, i2));
}

Block<MatBase<double>> XYStatData::yyVar(const Index j1, const Index j2)
{
    return FULL_BLOCK(var_[yy](j1, j2));
}

ConstBlock<MatBase<double>> XYStatData::yyVar(const Index j1,
                                              const Index j2) const
{
    return FULL_BLOCK(var_[yy](j1, j2));
}

Block<MatBase<double>> XYStatData::yxVar(const Index j, const Index i)
{
    return FULL_BLOCK(var_[yx](j, i));
}

ConstBlock<MatBase<double>> XYStatData::yxVar(const Index j,
                                              const Index i) const
{
    return FULL_BLOCK(var_[yx](j, i));
}

// fit /////////////////////////////////////////////////////////////////////////
FitResult XYStatData::fit(Minimizer &minimizer, const DVec &init,
                          const vector<const DoubleModel *> &modelVector)
{
    // initialization
    chi2_.setModel(modelVector);
    if (reinitChi2_)
    {
        chi2_.requestInit();
    }
    // initial parameters
    const Index nPoint = getNFitPoint();
    DVec        fullInit = init;
    Index       is = 0, kf = 0;

    fullInit.conservativeResize(chi2_.getNArg());
    for (Index i = 0; i < getXDim(); ++i)
    {
        if (!isXExact(i))
        {
            kf = 0;
            for (Index k = 0; k < getNData(); ++k)
            {
                if (isFitPoint(k))
                {
                    fullInit(chi2_.getNPar() + nPoint*is + kf) = x(i, k);
                    kf++;
                }
            }
            is++;
        }
    }
    minimizer.setInit(fullInit);

    // fit
    DoubleFunction chi2 = chi2_.makeFunction(false);
    FitResult      result;

    result        = minimizer(chi2);
    result.chi2_  = chi2(result);
    result.nDof_  = chi2_.getNDof();
    result.model_.resize(modelVector.size());
    for (unsigned int j = 0; j < modelVector.size(); ++j)
    {
        result.model_[j] = modelVector[j]->fixPar(result);
    }
    
    return result;
}

// residuals ///////////////////////////////////////////////////////////////////
XYStatData XYStatData::getResiduals(const FitResult &fit) const
{
    XYStatData     res(*this);

    for (Index j = 0; j < res.getYDim(); ++j)
    {
        const DoubleFunction &f = fit.getModel(j);

        for (Index k = 0; k < res.getNData(); ++k)
        {
            res.y(j, k) -= f(res.x(_, k).transpose());
        }
    }

    return res;
}

XYStatData XYStatData::getPartialResiduals(const FitResult &fit, const DVec &x,
                                           const Index i) const
{
    XYStatData res(*this);
    DVec       buf(x), xk;

    for (Index j = 0; j < res.getYDim(); ++j)
    {
        const DoubleFunction &f = fit.getModel(j);

        for (Index k = 0; k < res.getNData(); ++k)
        {
            buf(i)       = res.x(i, k);
            res.y(j, k) -= f(res.x(_, k).transpose()) - f(buf);
        }
    }

    return res;
}