#ifndef COUPLED_EQUATION_H
#define COUPLED_EQUATION_H

#include "SparseMatrix.h"
#include "SparseVector.h"
#include "ScalarFiniteVolumeField.h"
#include "VectorFiniteVolumeField.h"

class CoupledEquation
{
public:

    CoupledEquation(const ScalarFiniteVolumeField& rho, const ScalarFiniteVolumeField& mu, VectorFiniteVolumeField& u, ScalarFiniteVolumeField& p);

    Scalar solve(Scalar timeStep);

protected:

    void computeD();
    void assembleMomentumEquation(Scalar timeStep);
    void assembleContinuityEquation();
    void rhieChowInterpolation();

    size_t nActiveCells_, nVars_;

    const ScalarFiniteVolumeField &rho_, &mu_;
    VectorFiniteVolumeField& u_, gradP_;
    ScalarFiniteVolumeField& p_, d_;

    SparseMatrix spMat_;
    SparseVector rhs_;
};

#endif