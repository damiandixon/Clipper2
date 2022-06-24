/*******************************************************************************
* Author    :  Angus Johnson                                                   *
* Version   :  Clipper2 - beta                                                 *
* Date      :  21 June 2022                                                    *
* Website   :  http://www.angusj.com                                           *
* Copyright :  Angus Johnson 2010-2022                                         *
* Purpose   :  This module provides a simple interface to the Clipper Library  *
* License   :  http://www.boost.org/LICENSE_1_0.txt                            *
*******************************************************************************/

#ifndef CLIPPER_H
#define CLIPPER_H

#include <cstdlib>
#include <vector>

#include "clipper.core.h"
#include "clipper.engine.h"
#include "clipper.offset.h"
#include "clipper.minkowski.h"

namespace Clipper2Lib 
{

  static const Rect64 MaxInvalidRect64 = Rect64(
    (std::numeric_limits<int64_t>::max)(),
    (std::numeric_limits<int64_t>::max)(),
    (std::numeric_limits<int64_t>::lowest)(),
    (std::numeric_limits<int64_t>::lowest)());

  static const RectD MaxInvalidRectD = RectD(
      (std::numeric_limits<double>::max)(),
      (std::numeric_limits<double>::max)(),
      (std::numeric_limits<double>::lowest)(),
      (std::numeric_limits<double>::lowest)());

  inline Paths64 BooleanOp(ClipType cliptype, FillRule fillrule,
    const Paths64& subjects, const Paths64& clips)
  {
    Paths64 result;
    Clipper64 clipper;
    clipper.AddSubject(subjects);
    clipper.AddClip(clips);
    clipper.Execute(cliptype, fillrule, result);
    return result;
  }

  inline PathsD BooleanOp(ClipType cliptype, FillRule fillrule,
    const PathsD& subjects, const PathsD& clips)
  {
    PathsD result;
    ClipperD clipper;
    clipper.AddSubject(subjects);
    clipper.AddClip(clips);
    clipper.Execute(cliptype, fillrule, result);
    return result;
  }

  inline Paths64 Intersect(const Paths64& subjects, const Paths64& clips, FillRule fillrule)
  {
    return BooleanOp(ClipType::Intersection, fillrule, subjects, clips);
  }
  
  inline PathsD Intersect(const PathsD& subjects, const PathsD& clips, FillRule fillrule)
  {
    return BooleanOp(ClipType::Intersection, fillrule, subjects, clips);
  }

  inline Paths64 Union(const Paths64& subjects, const Paths64& clips, FillRule fillrule)
  {
    return BooleanOp(ClipType::Union, fillrule, subjects, clips);
  }

  inline PathsD Union(const PathsD& subjects, const PathsD& clips, FillRule fillrule)
  {
    return BooleanOp(ClipType::Union, fillrule, subjects, clips);
  }

  inline Paths64 Union(const Paths64& subjects, FillRule fillrule)
  {
    Paths64 result;
    Clipper64 clipper;
    clipper.AddSubject(subjects);
    clipper.Execute(ClipType::Union, fillrule, result);
    return result;
  }

  inline PathsD Union(const PathsD& subjects, FillRule fillrule)
  {
    PathsD result;
    ClipperD clipper;
    clipper.AddSubject(subjects);
    clipper.Execute(ClipType::Union, fillrule, result);
    return result;
  }

  inline Paths64 Difference(const Paths64& subjects, const Paths64& clips, FillRule fillrule)
  {
    return BooleanOp(ClipType::Difference, fillrule, subjects, clips);
  }

  inline PathsD Difference(const PathsD& subjects, const PathsD& clips, FillRule fillrule)
  {
    return BooleanOp(ClipType::Difference, fillrule, subjects, clips);
  }

  inline Paths64 Xor(const Paths64& subjects, const Paths64& clips, FillRule fillrule)
  {
    return BooleanOp(ClipType::Xor, fillrule, subjects, clips);
  }

  inline PathsD Xor(const PathsD& subjects, const PathsD& clips, FillRule fillrule)
  {
    return BooleanOp(ClipType::Xor, fillrule, subjects, clips);
  }

  static bool IsFullOpenEndType(EndType et)
  {
    return (et != EndType::Polygon) && (et != EndType::Joined);
  }

  static Paths64 InflatePaths(const Paths64& paths, double delta, 
    JoinType jt, EndType et, double miter_limit = 2.0)
  {
    ClipperOffset clip_offset(miter_limit);
    clip_offset.AddPaths(paths, jt, et);
    return clip_offset.Execute(delta);
  }

  static PathsD InflatePaths(const PathsD& paths, double delta,
    JoinType jt, EndType et, double miter_limit = 2.0, double precision = 2)
  {
    if (precision < -8 || precision > 8)
      throw new Clipper2Exception("Error: Precision exceeds the allowed range.");
    const double scale = std::pow(10, precision);
    ClipperOffset clip_offset(miter_limit);
    clip_offset.AddPaths(ScalePaths<int64_t,double>(paths, scale), jt, et);
    Paths64 tmp = clip_offset.Execute(delta * scale);
    return ScalePaths<double, int64_t>(tmp, 1 / scale);
  }

  inline Path64 OffsetPath(const Path64& path, int64_t dx, int64_t dy)
  {
    Path64 result;
    result.reserve(path.size());
    for (const Point64 pt : path)
      result.push_back(Point64(pt.x + dx, pt.y + dy));
    return result;
  }

  inline PathD OffsetPath(const PathD& path, double dx, double dy)
  {
    PathD result;
    result.reserve(path.size());
    for (const PointD pt : path)
      result.push_back(PointD(pt.x + dx, pt.y + dy));
    return result;
  }

  inline Paths64 OffsetPaths(const Paths64& paths, int64_t dx, int64_t dy)
  {
    Paths64 result;
    result.reserve(paths.size());
    for (const Path64& path : paths)
      result.push_back(OffsetPath(path, dx, dy));
    return result;
  }

  inline PathsD OffsetPaths(const PathsD& paths, double dx, double dy)
  {
    PathsD result;
    result.reserve(paths.size());
    for (const PathD& path : paths)
      result.push_back(OffsetPath(path, dx, dy));
    return result;
  }

  static Rect64 Bounds(const Path64& path)
  {
    Rect64 rec = MaxInvalidRect64;
    for (const Point64& pt : path)
    {
      if (pt.x < rec.left) rec.left = pt.x;
      if (pt.x > rec.right) rec.right = pt.x;
      if (pt.y < rec.top) rec.top = pt.y;
      if (pt.y > rec.bottom) rec.bottom = pt.y;
    }
    if (rec.IsEmpty()) return Rect64();
    return rec;
  }
  
  static Rect64 Bounds(const Paths64& paths)
  {
    Rect64 rec = MaxInvalidRect64;
    for (const Path64& path : paths)
      for (const Point64& pt : path)
      {
        if (pt.x < rec.left) rec.left = pt.x;
        if (pt.x > rec.right) rec.right = pt.x;
        if (pt.y < rec.top) rec.top = pt.y;
        if (pt.y > rec.bottom) rec.bottom = pt.y;
      }
    if (rec.IsEmpty()) return Rect64();
    return rec;
  }

  static RectD Bounds(const PathD& path)
  {
    RectD rec = MaxInvalidRectD;
    for (const PointD& pt : path)
    {
      if (pt.x < rec.left) rec.left = pt.x;
      if (pt.x > rec.right) rec.right = pt.x;
      if (pt.y < rec.top) rec.top = pt.y;
      if (pt.y > rec.bottom) rec.bottom = pt.y;
    }
    if (rec.IsEmpty()) return RectD();
    return rec;
  }

  static RectD Bounds(const PathsD& paths)
  {
    RectD rec = MaxInvalidRectD;
    for (const PathD& path : paths)
      for (const PointD& pt : path)
      {
        if (pt.x < rec.left) rec.left = pt.x;
        if (pt.x > rec.right) rec.right = pt.x;
        if (pt.y < rec.top) rec.top = pt.y;
        if (pt.y > rec.bottom) rec.bottom = pt.y;
      }
    if (rec.IsEmpty()) return RectD();
    return rec;
  }

  namespace details 
  {

    template <typename T>
    static void AddPolyNodeToPaths(const PolyPath<T>& polytree, Paths<T>& paths)
    {
      if (!polytree.polygon.empty())
        paths.push_back(polytree.polygon);
      for (PolyPath<T>* child : polytree.childs)
        AddPolyNodeToPaths(*child, paths);
    }

    inline bool GetInt(std::string::const_iterator& iter, const 
      std::string::const_iterator& end_iter, int64_t& val)
    {
      val = 0;
      bool is_neg = *iter == '-';
      if (is_neg) ++iter;
      std::string::const_iterator start_iter = iter;
      while (iter != end_iter &&
        ((*iter >= '0') && (*iter <= '9')))
      {
        val = val * 10 + (static_cast<int64_t>(*iter++) - '0');
      }
      if (is_neg) val = -val;
      return (iter != start_iter);
    }

    inline bool GetFloat(std::string::const_iterator& iter, const 
      std::string::const_iterator& end_iter, double& val)
    {
      val = 0;
      bool is_neg = *iter == '-';
      if (is_neg) ++iter;
      int dec_pos = -1;
      std::string::const_iterator start_iter = iter;
      while (iter != end_iter && (*iter == '.' ||
        ((*iter >= '0') && (*iter <= '9'))))
      {
        if (*iter == '.')
        {
          if (dec_pos >= 0) return false;
          dec_pos = 0;
          ++iter;
          continue;
        }

        if (dec_pos >= 0) dec_pos++;
        val = val * 10 + ((int64_t)(*iter++) - '0');
      }
      if (iter == start_iter || dec_pos == 0) return false;
      if (dec_pos > 0)
        val *= std::pow(10, -dec_pos);
      return true;
    }

    inline void SkipWhiteSpace(std::string::const_iterator& iter, 
      const std::string::const_iterator& end_iter)
    {
      while (iter != end_iter && *iter <= ' ') ++iter;
    }

    inline void SkipSpacesWithOptionalComma(std::string::const_iterator& iter, 
      const std::string::const_iterator& end_iter)
    {
      bool comma_seen = false;
      while (iter != end_iter)
      {
        if (*iter == ' ') ++iter;
        else if (*iter == ',')
        {
          if (comma_seen) return; //don't skip 2 commas!
          comma_seen = true;
          ++iter;
        }
        else return;                
      }
    }

    inline bool has_one_match(const char c, char* chrs)
    {
      while (*chrs > 0 && c != *chrs) ++chrs;
      if (!*chrs) return false;
      *chrs = ' '; //only match once per char
      return true;
    }


    inline void SkipUserDefinedChars(std::string::const_iterator& iter,
      const std::string::const_iterator& end_iter, const std::string skip_chars)
    {
      const size_t MAX_CHARS = 16;
      char buff[MAX_CHARS] = {0};
      std::copy(skip_chars.cbegin(), skip_chars.cend(), &buff[0]);
      while (iter != end_iter && 
        (*iter <= ' ' || has_one_match(*iter, buff))) ++iter;
      return;
    }

  } //end details namespace 

  template <typename T>
  inline Paths<T> PolyTreeToPaths(const PolyTree<T>& polytree)
  {
    Paths<T> result;
    details::AddPolyNodeToPaths(polytree, result);
    return result;
  }

  static Path64 MakePath(const std::string& s, const std::string& skip_chars = "")
  {
    Path64 result;
    std::string::const_iterator s_iter = s.cbegin();
    bool user_defined_skip = (skip_chars.size() > 0 && skip_chars != " ");
    if (user_defined_skip)
      details::SkipUserDefinedChars(s_iter, s.cend(), skip_chars);
    else
      details::SkipWhiteSpace(s_iter, s.cend());
    while (s_iter != s.cend())
    {
      int64_t y = 0, x = 0;
      if (!details::GetInt(s_iter, s.cend(), x)) break;
      details::SkipSpacesWithOptionalComma(s_iter, s.cend());
      if (!details::GetInt(s_iter, s.cend(), y)) break;
      result.push_back(Point64(x, y));
      if (user_defined_skip)
        details::SkipUserDefinedChars(s_iter, s.cend(), skip_chars);
      else
        details::SkipSpacesWithOptionalComma(s_iter, s.cend());
    }
    return result;
  }
  
  static PathD MakePathD(const std::string& s)
  {
    PathD result;
    std::string::const_iterator s_iter = s.cbegin();
    details::SkipWhiteSpace(s_iter, s.cend());
    while (s_iter != s.cend())
    {
      double y = 0, x = 0;
      if (!details::GetFloat(s_iter, s.cend(), x)) break;
      details::SkipSpacesWithOptionalComma(s_iter, s.cend());
      if (!details::GetFloat(s_iter, s.cend(), y)) break;
      result.push_back(PointD(x, y));
      details::SkipSpacesWithOptionalComma(s_iter, s.cend());
    }
    return result;
  }

  static Path64 TrimCollinear(const Path64& p, bool is_open_path = false)
  {
    size_t len = p.size();
    if (len < 3)
    {
      if (!is_open_path || len < 2 || p[0] == p[1]) return Path64();
      else return p;
    }

    Path64 dst;
    dst.reserve(len);
    Path64::const_iterator srcIt = p.cbegin(), prevIt, stop = p.cend() - 1;

    if (!is_open_path)
    {
      while (srcIt != stop && !CrossProduct(*stop, *srcIt, *(srcIt + 1)))
        ++srcIt;
      while (srcIt != stop && !CrossProduct(*(stop - 1), *stop, *srcIt))
        --stop;
      if (srcIt == stop) return Path64();
    }

    prevIt = srcIt++;
    dst.push_back(*prevIt);
    for (; srcIt != stop; ++srcIt)
    {
      if (CrossProduct(*prevIt, *srcIt, *(srcIt + 1)))
      {
        prevIt = srcIt;
        dst.push_back(*prevIt);
      }
      else if (!is_open_path && dst.size() > 1 &&
        !CrossProduct(*(prevIt - 1), *prevIt, *srcIt))
      {
        dst.pop_back();
        --prevIt;
      }
    }

    if (is_open_path)
      dst.push_back(*srcIt);
    else if (CrossProduct(*prevIt, *stop, dst[0]))
      dst.push_back(*stop);
    else if (dst.size() < 3)
      return Path64();
    return dst;
  }

  static PathD TrimCollinear(const PathD& path, int precision, bool is_open_path = false)
  {
    if (precision > 8 || precision < -8) 
      throw new Clipper2Exception("Error: Precision exceeds the allowed range.");
    const double scale = std::pow(10, precision);
    Path64 p = ScalePath<int64_t, double>(path, scale);
    p = TrimCollinear(p, is_open_path);
    return ScalePath<double, int64_t>(p, 1/scale);
  }

  static PointInPolyResult PointInPolygon(const Point64& pt, const Path64& polygon)
  {
    if (polygon.size() < 3) 
      return PointInPolyResult::IsOutside;

    int val = 0;
    Path64::const_iterator cit = polygon.cbegin(); 
    Path64::const_iterator cend = polygon.cend(), pit = cend -1;
    bool is_above = pit->y < pt.y, first_pass = true;

    while (cit != cend)
    {
      if (is_above)
      {
        while (cit != cend && cit->y < pt.y) ++cit;
        if (cit == cend) break;
      }
      else
      {
        while (cit != cend && cit->y > pt.y) ++cit;
        if (cit == cend) break;
      }

      if (first_pass)
      {
        if (cit != polygon.cbegin()) pit = cit - 1;
        first_pass = false;
      }
      else 
        pit = cit - 1;

      if (cit->y == pt.y)
      {
        if (cit->x == pt.x || (cit->y == pit->y &&
          ((pt.x < pit->x) != (pt.x < pit->x))))
          return PointInPolyResult::IsOn;
        cit++;
        continue;
      }
      
      if (pt.x < cit->x && pt.x < pit->x)
      {
        //we're only interested in edges crossing on the left
      }
      else if (pt.x > pit->x && pt.x > cit->x)
        val = 1 - val; //toggle val
      else
      {
        double d = CrossProduct(*pit, *cit, pt);
        if (d == 0)
          return PointInPolyResult::IsOn;
        else if ((d < 0) == is_above) 
          val  = 1 - val;
      }      
      is_above = !is_above;
      cit++;
    }
    if (val == 0)
      return PointInPolyResult::IsOutside;
    else
      return PointInPolyResult::IsInside;
  }

}  //end Clipper2Lib namespace

#endif  // CLIPPER_H
