#include "FractionalStepMultiphase.h"
#include "Cicsam.h"
#include "CrankNicolson.h"
#include "AdamsBashforth.h"

FractionalStepMultiphase::FractionalStepMultiphase(const FiniteVolumeGrid2D &grid, const Input &input)
    :
      FractionalStep(grid, input),
      gamma(addScalarField(input, "gamma")),
      ft(addVectorField("ft")),
      gammaEqn_(gamma, "gammaEqn"),
      surfaceTensionForce_(input, *this)
{
    rho1_ = input.caseInput().get<Scalar>("Properties.rho1");
    rho2_ = input.caseInput().get<Scalar>("Properties.rho2");
    mu1_ = input.caseInput().get<Scalar>("Properties.mu1");
    mu2_ = input.caseInput().get<Scalar>("Properties.mu2");

    setInitialConditions(input);

    //surfaceTensionForce_->compute();
    computeRho();
    computeMu();
}

Scalar FractionalStepMultiphase::solve(Scalar timeStep)
{
    solveUEqn(timeStep);
    solvePEqn(timeStep);
    correctVelocity(timeStep);
    solveGammaEqn(timeStep);

    printf("Max Co = %lf\n", courantNumber(timeStep));

    return 0.;
}

//- Protected methods

Scalar FractionalStepMultiphase::solveUEqn(Scalar timeStep)
{
    u.savePreviousTimeStep(timeStep, 1);

    ft = surfaceTensionForce_.compute();

    uEqn_ = (fv::ddt(rho, u, timeStep) + cn::div(rho*u, u) + ib_.eqns(u) == ab::laplacian(mu, u) - fv::source(grad(p)) + fv::source(ft));

    Scalar error = uEqn_.solve();
    computeAdvectingVelocity(timeStep);

    return error;
}

Scalar FractionalStepMultiphase::solveGammaEqn(Scalar timeStep)
{
    gamma.savePreviousTimeStep(timeStep, 1);

    interpolateFaces(gamma);

    //gammaEqn_ = (cicsam::div(u, gamma, timeStep, cicsam::HC) + ib_.eqns(gamma) == 0.);
    gammaEqn_ = (cicsam::div(u, ib_.ibObjs(), gamma, timeStep, cicsam::HC) == 0.);

    Scalar error = gammaEqn_.solve();

    return error;
}

void FractionalStepMultiphase::computeRho()
{
    const ScalarFiniteVolumeField &w = surfaceTensionForce_.gammaTilde();

    for(const Cell &cell: grid_.activeCells())
        rho[cell.id()] = (1. - w[cell.id()])*rho1_ + w[cell.id()]*rho2_;

    //interpolateFaces(rho);
    harmonicInterpolateFaces(rho);
}

void FractionalStepMultiphase::computeMu()
{
    const ScalarFiniteVolumeField &w = surfaceTensionForce_.gammaTilde();

    for(const Cell &cell: grid_.activeCells())
        mu[cell.id()] = (1. - w[cell.id()])*mu1_ + w[cell.id()]*mu2_;

    interpolateFaces(mu);
    //harmonicInterpolateFaces(mu);
}
