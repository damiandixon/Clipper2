using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;

namespace Clipper2Lib.UnitTests
{
  using Path64 = List<Point64>;
  using Paths64 = List<List<Point64>>;

  [TestClass]
  public class TestPolytree
  {

    void PolyPathContainsPoint(PolyPath64 pp, Point64 pt, ref int counter)
    {
      if (pp.Polygon.Count > 0 &&
        Clipper.PointInPolygon(pt, pp.Polygon) != PointInPolygonResult.IsOutside)
          if (pp.IsHole) --counter; else ++counter;
      for (int i = 0; i < pp.ChildCount; i++)
      {
        PolyPath64 child = (PolyPath64) pp.GetChild(i);
        PolyPathContainsPoint(child, pt, ref counter);
      } 
    }

    private bool PolytreeContainsPoint(PolyTree64 pp, Point64 pt)
    {
      int counter = 0;
      for (int i = 0; i < pp.ChildCount; i++)
      {
        PolyPath64 child = (PolyPath64) pp.GetChild(i);
        PolyPathContainsPoint(child, pt, ref counter);
      }
      Assert.IsTrue(counter >= 0, "Polytree has too many holes");
      return counter != 0;
    }

  private bool PolyPathFullyContainsChildren(PolyPath64 pp)
    {
      for (int i = 0; i < pp.ChildCount; i++)
      {
        PolyPath64 child = (PolyPath64) pp.GetChild(i);
        foreach (Point64 pt in child.Polygon)
          if (Clipper.PointInPolygon(pt, pp.Polygon) == PointInPolygonResult.IsOutside)
            return false;
        if (child.ChildCount > 0 && !PolyPathFullyContainsChildren(child))
          return false;
      }
      return true;
    }

    private bool CheckPolytreeFullyContainsChildren(PolyTree64 polytree)
    {
      for (int i = 0; i < polytree.ChildCount; i++)
      {
        PolyPath64 child = (PolyPath64) polytree.GetChild(i);
        if (child.ChildCount > 0 && !PolyPathFullyContainsChildren(child))
          return false;
      }    
      return true;
    }

  [TestMethod]
    public void TestPolytree2()
    {
      ClipType cliptype;
      FillRule fillrule;
      long area;
      int count;
      string caption;
      Paths64 subject = new Paths64(), subjectOpen = new Paths64(), clip = new Paths64();

      Assert.IsTrue(ClipperFileIO.LoadTestNum("..\\..\\..\\..\\..\\Tests\\PolytreeHoleOwner2.txt",
        1, subject, subjectOpen, clip, out cliptype, out fillrule, out area, out count, out caption),
        "Unable to read PolytreeHoleOwner2.txt");


      PolyTree64 solutionTree = new PolyTree64();
      Paths64 solution_open = new Paths64();
      Clipper64 clipper = new Clipper64();

      List<Point64> pointsOfInterestOutside = new List<Point64>();
      pointsOfInterestOutside.Add(new Point64(21887, 10420));
      pointsOfInterestOutside.Add(new Point64(21726, 10825));
      pointsOfInterestOutside.Add(new Point64(21662, 10845));
      pointsOfInterestOutside.Add(new Point64(21617, 10890));

      foreach(Point64 pt in pointsOfInterestOutside)
      {
        foreach (Path64 path in subject)
        {
          Assert.IsTrue(Clipper.PointInPolygon(pt, path) == PointInPolygonResult.IsOutside, 
            "outside point of interest found inside subject");
        }
      }

      List<Point64> pointsOfInterestInside = new List<Point64>();
      pointsOfInterestInside.Add(new Point64(21887, 10430));
      pointsOfInterestInside.Add(new Point64(21843, 10520));
      pointsOfInterestInside.Add(new Point64(21810, 10686));
      pointsOfInterestInside.Add(new Point64(21900, 10461));

      foreach (Point64 pt in pointsOfInterestInside)
      {
        int poi_inside_counter = 0;
        foreach (Path64 path in subject)
        {
          if (Clipper.PointInPolygon(pt, path) == PointInPolygonResult.IsInside)
            poi_inside_counter++;
        }
        Assert.IsTrue(poi_inside_counter == 1,
          string.Format("poi_inside_counter - expected 1 but got {0}", poi_inside_counter));
      }

      clipper.AddSubject(subject);
      clipper.AddOpenSubject(subjectOpen);
      clipper.AddClip(clip);
      clipper.Execute(cliptype, fillrule, solutionTree, solution_open);

      Paths64 solutionPaths = Clipper.PolyTreeToPaths(solutionTree);
      double a1 = Clipper.Area(solutionPaths), a2 = solutionTree.Area();

      Assert.IsTrue(a1 > 330000, 
        string.Format("solution has wrong area - value expected: 331,052; value returned; {0} ", a1));

      Assert.IsTrue(Math.Abs(a1 - a2) < 0.0001,
        string.Format("solution tree has wrong area - value expected: {0}; value returned; {1} ", a1, a2));

      Assert.IsTrue(CheckPolytreeFullyContainsChildren(solutionTree),
        "The polytree doesn't properly contain its children");

      foreach (Point64 pt in pointsOfInterestOutside)
        Assert.IsFalse(PolytreeContainsPoint(solutionTree, pt),
          "The polytree indicates it contains a point that it should not contain");

      foreach (Point64 pt in pointsOfInterestInside)
        Assert.IsTrue(PolytreeContainsPoint(solutionTree, pt),
          "The polytree indicates it does not contain a point that it should contain");

    }
  }
}
