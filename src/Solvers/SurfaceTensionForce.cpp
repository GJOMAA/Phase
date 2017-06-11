#include "SurfaceTensionForce.h"

SurfaceTensionForce::SurfaceTensionForce(const Input &input,
                                         Solver &solver)
        :
        gamma_(solver.scalarFields().find("gamma")->second),
        u_(solver.vectorFields().find("u")->second),
        rho_(solver.scalarFields().find("rho")->second),
        mu_(solver.scalarFields().find("mu")->second),
        n_(solver.addVectorField("n")),
        kappa_(solver.addScalarField("kappa")),
        gradGamma_(solver.vectorFields().find("gradGamma")->second),
        solver_(solver)
{
    sigma_ = input.caseInput().get<Scalar>("Properties.sigma");
    thetaAdv_ = input.caseInput().get<Scalar>("Properties.advancingContactAngle") * M_PI / 180.;
    thetaRec_ = input.caseInput().get<Scalar>("Properties.recedingContactAngle") * M_PI / 180.;

    std::string contactAngleType = input.caseInput().get<std::string>("Solver.contactAngleType", "static");

    //- Must figure out which patches to enforce the contact angle on
    for (const auto &boundaryInput: input.boundaryInput().get_child("Boundaries.gamma"))
    {
        std::string status = boundaryInput.second.get<std::string>("contactAngle", "off");

        if (status == "on")
        {
            contactAnglePatches_.push_back(gamma_.grid.patch(boundaryInput.first));
            printf("Contact angles will be applied to patch \"%s\".\n", boundaryInput.first.c_str());
        }
        else if (status == "off")
            continue;
        else
            throw Exception("SurfaceTensionForce", "SurfaceTensionForce",
                            "invalid contact angle status \"" + status + "\".");
    }

    for (const ImmersedBoundaryObject &ibObj: solver.ibObjs())
    {
        ibContactAngles_.insert(
                std::make_pair(ibObj.id(),
                               input.boundaryInput().get<Scalar>(
                                       "ImmersedBoundaries." + ibObj.name() + ".gamma.contactAngle",
                                       input.caseInput().get<Scalar>("Properties.advancingContactAngle")
                               ) * M_PI / 180.)
        );
    }
}

Scalar SurfaceTensionForce::theta(const Cell& cell)
{
    return thetaAdv_;
}

Vector2D SurfaceTensionForce::computeContactLineNormal(const Vector2D &gradGamma, const Vector2D &wallNormal,
                                                       const Vector2D &vel, Scalar theta) const
{
    const Vector2D nt = wallNormal.tangentVec();
    return dot(-gradGamma, nt) > 0. ? nt.rotate(M_PI / 2. - theta).unitVec() : nt.rotate(M_PI / 2. + theta).unitVec();
}

Vector2D SurfaceTensionForce::computeContactLineNormal(const Vector2D &gradGamma, const Vector2D &wallNormal,
                                                       const Vector2D &vel) const
{
    const Vector2D nt = wallNormal.tangentVec();
    const Scalar theta = dot(-gradGamma, vel) >= 0. ? thetaAdv_ : thetaRec_;
    return dot(-gradGamma, nt) > 0. ? nt.rotate(M_PI / 2. - theta).unitVec() : nt.rotate(M_PI / 2. + theta).unitVec();
}

void SurfaceTensionForce::computeGhostCellVals(const ImmersedBoundaryObject &ibObj, Scalar theta)
{
    throw Exception("SurfaceTensionForce", "computeGhostCellVals", "deprecated.");
}

bool SurfaceTensionForce::isContactLinePatch(const Patch &patch) const
{
    for (const Patch &clPatch: contactAnglePatches_)
    {
        if (clPatch.id() == patch.id())
            return true;
    }

    return false;
}

Scalar SurfaceTensionForce::ibTheta(const ImmersedBoundaryObject &ibObj) const
{
    return ibContactAngles_.find(ibObj.id())->second;
}
