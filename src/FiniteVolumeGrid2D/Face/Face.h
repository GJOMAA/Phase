#ifndef FACE_H
#define FACE_H

#include <vector>
#include <memory>

#include "Node.h"
#include "Cell.h"

class FiniteVolumeGrid2D;

class Face
{
public:

    enum Type
    {
        INTERIOR, BOUNDARY
    };

    Face(Label lNodeId, Label rNodeId, const FiniteVolumeGrid2D &grid, Type type = INTERIOR);

    void setType(Type type)
    { type_ = type; }

    Type type() const
    { return type_; }

    const Point2D &centroid() const
    { return centroid_; }

    const Vector2D &norm() const
    { return normal_; }

    const Vector2D &tan() const
    { return tangent_; }

    Vector2D outwardNorm(const Point2D &point) const;

    Label id() const
    { return id_; }

    void setId(Label id)
    { id_ = id; }

    bool isInterior() const
    { return type_ == INTERIOR; }

    bool isBoundary() const
    { return type_ == BOUNDARY; }

    const Node &lNode() const
    { return nodes_[nodeIds_.first]; }

    const Node &rNode() const
    { return nodes_[nodeIds_.second]; }

    const Cell &lCell() const
    { return cells_[cellIds_[0]]; }

    const Cell &rCell() const
    { return cells_[cellIds_[1]]; }

    Scalar volumeWeight() const;

    Scalar distanceWeight() const;

    std::vector<Ref<const Cell>> cells() const;

    void addCell(const Cell &cell);

    std::string info() const;

private:
    Type type_;

    Point2D centroid_;
    Vector2D normal_, tangent_;

    Label id_;

    std::pair<Label, Label> nodeIds_;
    std::vector<Label> cellIds_;

    const std::vector<Node> &nodes_;
    const std::vector<Cell> &cells_;
};

#endif
