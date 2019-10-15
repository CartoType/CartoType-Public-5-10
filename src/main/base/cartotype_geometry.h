/*
cartotype_geometry.h
Copyright (C) CartoType Ltd 2019.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_GEOMETRY__
#define CARTOTYPE_GEOMETRY__

#include <cartotype_path.h>
#include <cartotype_map_object.h>

namespace CartoType
{

/** A template class to hold geometry objects containing various types of point. The point type must be derived from or trivially convertible to TOutlinePointFP. */
template<class point_t> class CGeneralGeometry
    {
    public:
    CGeneralGeometry() { }
    explicit CGeneralGeometry(TCoordType aCoordType,bool aClosed = false):
        m_coord_type(aCoordType),
        m_closed(aClosed)
        {
        }
    CGeneralGeometry(const MPath& aPath,TCoordType aCoordType,bool aClosed):
        m_coord_type(aCoordType),
        m_closed(aClosed)
        {
        for (size_t i = 0; i < aPath.Contours(); i++)
            {
            TContour contour = aPath.Contour(i);
            BeginContour();
            auto& c = m_contour_array.back();
            for (const auto& p : contour)
                c.push_back(point_t(p));
            }
        }
    CGeneralGeometry(const TRectFP& aRect,TCoordType aCoordType):
        m_coord_type(aCoordType),
        m_closed(true)
        {
        AppendPoint(aRect.iTopLeft);
        AppendPoint(aRect.BottomLeft());
        AppendPoint(aRect.iBottomRight);
        AppendPoint(aRect.TopRight());
        }
    CGeneralGeometry(const point_t& aPoint,TCoordType aCoordType):
        m_coord_type(aCoordType),
        m_closed(false)
        {
        AppendPoint(aPoint);
        }
    explicit CGeneralGeometry(const CMapObject& aMapObject):
        CGeneralGeometry(aMapObject,TCoordType::Map,aMapObject.Type() == TMapObjectType::Polygon || aMapObject.Type() == TMapObjectType::Array)
        {
        }
    operator COutline() const
        {
        COutline outline;
        TOutlinePoint p;
        for (const auto& contour : m_contour_array)
            {
            CContour& c = outline.AppendContour();
            c.SetClosed(m_closed);
            for (const auto& point: contour)
                {
                TOutlinePointFP q { point };
                p.iX = Arithmetic::Round(q.iX);
                p.iY = Arithmetic::Round(q.iY);
                p.iType = q.iType;
                c.AppendPoint(p);
                }
            }
        return outline;
        }

    void Clear() { m_contour_array.resize(1); m_contour_array[0].resize(0); m_coord_type = TCoordType::Map; m_closed = false; }
    TCoordType CoordType() const { return m_coord_type; }
    size_t ContourCount() const { return m_contour_array.size(); }
    size_t PointCount(size_t aContourIndex) const { return m_contour_array[aContourIndex].size(); }
    const point_t& Point(size_t aContourIndex,size_t aPointIndex) const { return m_contour_array[aContourIndex][aPointIndex]; }
    point_t& Point(size_t aContourIndex,size_t aPointIndex) { return m_contour_array[aContourIndex][aPointIndex]; }
    bool IsEmpty() const { return m_contour_array[0].empty(); }
    bool IsClosed() const { return m_closed; }
    void SetClosed(bool aClosed) { m_closed = aClosed; }
    TWritableCoordSet CoordSet(size_t aContourIndex)
        {
        auto& c = m_contour_array[aContourIndex];
        if (!c.size())
            return TWritableCoordSet();
        return TWritableCoordSet(&c[0].iX,&c[0].iY,sizeof(point_t),c.size());
        }
    TCoordSet CoordSet(size_t aContourIndex) const
        {
        auto& c = m_contour_array[aContourIndex];
        if (!c.size())
            return TCoordSet();
        return TCoordSet(&c[0].iX,&c[0].iY,sizeof(point_t),c.size());
        }

    TRectFP Bounds() const
        {
        TRectFP bounds;
        if (m_contour_array[0].size())
            bounds.iTopLeft = bounds.iBottomRight = m_contour_array[0][0];
        for (const auto& c : m_contour_array)
            for (const auto& p : c)
                bounds.Combine(p);
        return bounds;
        }

    void AppendPoint(const point_t& aPoint) { m_contour_array.back().push_back(aPoint); }
    void AppendPoint(double aX,double aY) { m_contour_array.back().push_back(point_t(aX,aY)); }
    void AppendPoint(double aX,double aY,TPointType aPointType) { m_contour_array.back().push_back(point_t(aX,aY,aPointType)); }
    void BeginContour() { if (!m_contour_array.back().empty()) m_contour_array.emplace_back(); }
    void Reverse()
        {
        std::reverse(m_contour_array.begin(),m_contour_array.end());
        for (auto& c : m_contour_array)
            std::reverse(c.begin(),c.end());
        }

    private:
    using contour_t = std::vector<point_t>;
    std::vector<contour_t> m_contour_array = std::vector<contour_t>(1);
    TCoordType m_coord_type = TCoordType::Map;
    bool m_closed = false;
    };

/** A geometry class for creating map objects and specifying view areas. */
class CGeometry: public CGeneralGeometry<TOutlinePointFP>
    {
    using CGeneralGeometry::CGeneralGeometry; // inherit constructors
    };

} // namespace CartoType

#endif // #define CARTOTYPE_GEOMETRY__
