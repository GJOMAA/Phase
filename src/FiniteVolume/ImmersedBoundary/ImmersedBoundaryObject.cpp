#include <memory>

#include "ImmersedBoundaryObject.h"
#include "TranslatingMotion.h"
#include "OscillatingMotion.h"

ImmersedBoundaryObject::ImmersedBoundaryObject(const std::string &name,
                                               Label id,
                                               FiniteVolumeGrid2D &grid)
    :
      name_(name),
      grid_(grid),
      id_(id)
{
    cells_ = CellZone("Cells", grid.cellZoneRegistry());

    zoneRegistry_ = std::shared_ptr<CellZone::ZoneRegistry>(new CellZone::ZoneRegistry());

    ibCells_ = CellZone("IbCells", zoneRegistry_);
    solidCells_ = CellZone("SolidCells", zoneRegistry_);
    freshCells_ = CellZone("FreshCells", zoneRegistry_);
    deadCells_ = CellZone("DeadCells", zoneRegistry_);
}

void ImmersedBoundaryObject::initCircle(const Point2D &center, Scalar radius)
{
    shapePtr_ = std::shared_ptr<Circle>(new Circle(center, radius));
}


void ImmersedBoundaryObject::initBox(const Point2D &center, Scalar width, Scalar height)
{
    shapePtr_ = std::shared_ptr<Box>(new Box(
                                         Point2D(center.x - width / 2., center.y - height / 2.),
                                         Point2D(center.x + width / 2., center.y + height / 2.)
                                         ));
}

void ImmersedBoundaryObject::setMotionType(const std::map<std::string, std::string> &properties)
{
    std::string type = properties.find("type")->second;

    if(type == "none")
        motion_ = nullptr;
    else if(type == "translating")
    {
        Vector2D vel = properties.find("velocity")->second;
        Vector2D acc = properties.find("acceleration")->second;

        motion_ = std::shared_ptr<TranslatingMotion>(new TranslatingMotion(shapePtr_->centroid(), vel, acc));
    }
    else if(type == "oscillating")
    {
        Vector2D freq = properties.find("frequency")->second;
        Vector2D amp = properties.find("amplitude")->second;
        Vector2D phase = properties.find("phase")->second;

        motion_ = std::shared_ptr<OscillatingMotion>(new OscillatingMotion(shapePtr_->centroid(), freq, amp, phase, 0.));
    }
    else
        throw Exception("ImmersedBoundaryObject", "update", "invalid motion type \"" + type + "\".");
}

void ImmersedBoundaryObject::setZone(CellZone &zone)
{
    fluid_ = &zone;
    cells_ = CellZone("Cells", zone.registry());
}

void ImmersedBoundaryObject::clear()
{
    fluid_->add(cells_);
    ibCells_.clear();
    solidCells_.clear();
    freshCells_.clear();
    deadCells_.clear();
    grid_.setCellsActive(*fluid_);
}

LineSegment2D ImmersedBoundaryObject::intersectionLine(const Point2D &ptA, const Point2D &ptB) const
{
    LineSegment2D ln(ptA, ptB);
    return isInIb(ptB) ? LineSegment2D(ptA, shapePtr_->intersections(ln)[0]) : ln;
}

std::pair<Point2D, Vector2D> ImmersedBoundaryObject::intersectionStencil(const Point2D &ptA, const Point2D &ptB) const
{
    auto intersections = shape().intersections(LineSegment2D(ptA, ptB));

    Point2D xc;
    if (intersections.empty()) //- fail safe, in case a point is on an ib
    {
        Point2D nPtA = shape().nearestIntersect(ptA);
        Point2D nPtB = shape().nearestIntersect(ptB);

        if ((nPtA - ptA).magSqr() < (nPtB - ptB).magSqr())
            xc = ptA;
        else
            xc = ptB;
    }
    else
        xc = intersections[0];

    LineSegment2D edge = shapePtr_->nearestEdge(xc);

    return std::make_pair(
                xc, -(edge.ptB() - edge.ptA()).normalVec()
                );
}

void ImmersedBoundaryObject::addBoundaryType(const std::string &name, BoundaryType boundaryType)
{
    boundaryTypes_[name] = boundaryType;
}

template<>
Scalar ImmersedBoundaryObject::getBoundaryRefValue<Scalar>(const std::string &name) const
{
    return boundaryRefScalars_.find(name)->second;
}

template<>
Vector2D ImmersedBoundaryObject::getBoundaryRefValue<Vector2D>(const std::string &name) const
{
    return boundaryRefVectors_.find(name)->second;
}

void ImmersedBoundaryObject::addBoundaryRefValue(const std::string &name, Scalar boundaryRefValue)
{
    boundaryRefScalars_[name] = boundaryRefValue;
}

void ImmersedBoundaryObject::addBoundaryRefValue(const std::string &name, const Vector2D &boundaryRefValue)
{
    boundaryRefVectors_[name] = boundaryRefValue;
}

void ImmersedBoundaryObject::addBoundaryRefValue(const std::string &name, const std::string &value)
{
    try
    {
        boundaryRefScalars_[name] = std::stod(value);
        return;
    }
    catch (...)
    {
        boundaryRefVectors_[name] = Vector2D(value);
    }
}

Vector2D ImmersedBoundaryObject::acceleration() const
{
    return motion_ ? motion_->acceleration(): Vector2D(0., 0.);
}

Vector2D ImmersedBoundaryObject::acceleration(const Point2D &point) const
{
    return motion_ ? motion_->acceleration(point): Vector2D(0., 0.);
}

Vector2D ImmersedBoundaryObject::velocity() const
{
    return motion_ ? motion_->velocity(): Vector2D(0., 0.);
}

Vector2D ImmersedBoundaryObject::velocity(const Point2D &point) const
{
    return motion_ ? motion_->velocity(point): Vector2D(0., 0.);
}

void ImmersedBoundaryObject::clearFreshCells()
{
    fluid_->add(freshCells_); //- Should also clear cells from cells_
    freshCells_.clear();
}
