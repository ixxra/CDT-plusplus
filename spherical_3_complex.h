/// Causal Dynamical Triangulations in C++ using CGAL
///
/// Copyright (c) 2013 Adam Getchell
///
/// Spherical simplicial complexes

#ifndef SPHERICAL_3_COMPLEX_H_
#define SPHERICAL_3_COMPLEX_H_

/// CDT Headers
#include "sphere_d.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/point_generators_3.h>
// #include <CGAL/number_utils.h>
// #include <CGAL/random_selection.h>

/// C++ headers
#include <cassert>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel    K;
/// Used so that an integer for each timeslice may be associated
/// with each vertex
typedef CGAL::Triangulation_vertex_base_with_info_3<int, K>  Vb;
typedef CGAL::Triangulation_data_structure_3<Vb>             Tds;
typedef CGAL::Delaunay_triangulation_3<K, Tds>               Delaunay;
typedef CGAL::Triangulation_3<K, Tds>                        Triangulation;

///
/// Make 3D spherical simplicial complexes using random points
/// This method does not admit to a consistent foliation of time
/// yet, and because it is random and we don't want odd shapes
/// Delaunay triangulations are used with a starting seed.
template <typename T>
void make_random_S3_simplicial_complex(T* S3, int number_of_simplices,
                                        int number_of_timeslices) {
  CGAL::Random_points_in_sphere_3<typename T::Point> rnd;

  /// Initialize triangulation in 3D
  /// Seed simplex of 1 cell, 4 vertices, 6 edges, and 4 faces
  S3->insert(typename T::Point(0, 0, 0));
  S3->insert(typename T::Point(1, 0, 0));
  S3->insert(typename T::Point(0, 1, 0));
  S3->insert(typename T::Point(0, 0, 1));

  assert(S3->dimension() == 3);

  std::cout  << "Initial seed has ";
  print_results(S3);


  do {
     Delaunay::Point p = *rnd++;

    /// Locate the point
    typename T::Locate_type lt;
    int li, lj;
    typename T::Cell_handle c = S3->locate(p, lt, li, lj);
    if (lt == Delaunay::VERTEX)
      continue;  // Point already exists

    /// Get the cells that conflict with p in a vector V,
    /// and a facet on the boundary of this hole in f
    std::vector<typename T::Cell_handle> V;
    Delaunay::Facet f;

    S3->find_conflicts(p, c,
              /// Get one boundary facet
              CGAL::Oneset_iterator<Delaunay::Facet>(f),
              /// Conflict cells in V
              std::back_inserter(V));

    if ((V.size() & 1) == 0)  // Even number of conflict cells?
      S3->insert_in_hole(p, V.begin(), V.end(), f.first, f.second);

    // // Store the timeslice as an integer in each vertex's info field
    // int timeslice = (int) CGAL::to_double(p.point().z());
    // p.info() = timeslice;

#ifndef NDEBUG
    std::cout << '(' << p << ')' << std::endl;
#endif
  } while (S3->number_of_finite_cells() < number_of_simplices);

  assert(S3->dimension() == 3);
  assert(S3->is_valid());
}
///
/// This function builds up simplicial complexes from a seed
/// which has defined timeslicing
/// We can then use those ergodic moves which increase volume
/// to get to our final number without changing the foliation of time
template <typename T>
void make_S3_simplicial_complex(T* S3, int number_of_simplices, int number_of_timeslices) {
  int simplices_per_timeslice = number_of_simplices / number_of_timeslices;
  /// Start with three simplices suitable for a 2-6 or 2-3 move
  /// These points make one cell (tetrahedron) with 4 vertices, 6 edges, and 4 faces
  /// In 3D with an infinite point we'll get 4 vertices, 10 edges
  /// 10 facets and 5 cells. The point at $\infty$ isn't counted
  /// but the edges, facets, and cells generated by it are.
  /// Using the number_of_finite_ functions solves this
  S3->insert(typename T::Point(0, 0, 1));
  S3->insert(typename T::Point(2, 0, 1));
  S3->insert(typename T::Point(0, 2, 1));
  S3->insert(typename T::Point(0, 0, 2));
  /// This point gives us two cells, 5 vertices, 9 edges, and 7 faces
  /// These two cells are the basis for the 2-6 3D ergodic move
  S3->insert(typename T::Point(0, 0, 0));

  assert(S3->dimension() == 3);

  std::cout  << "Initial seed has ";
  print_results(S3);

  /// This point makes a naive 2-6 move. We should have 6 cells
  S3->insert(typename T::Point(1, 1, 1));
  // S3->insert(typename T::Point(1, 1, .9));

  /// Store the timeslice as an integer in each vertex's info field
  Triangulation::Finite_vertices_iterator vit;
  for (vit = S3->finite_vertices_begin(); vit != S3->finite_vertices_end(); ++vit) {
    int timeslice = (int) CGAL::to_double(vit->point().z());
    vit->info() = timeslice;
  }

    assert(S3->dimension() == 3);
  assert(S3->is_valid());
  std::cout << "Triangulation is valid: " << ((S3->is_valid()) ? "True" : "False") << std::endl;

  /// Debugging: print out each vertex and the timeslice number associated
  #ifndef NDEBUG
  for (vit = S3->finite_vertices_begin(); vit != S3->finite_vertices_end(); ++vit)
    std::cout << '(' << vit->point() << ") (timeslice = " << vit->info() << ")" << std::endl;
  #endif
}

template <typename T>
void make_S3_simplicial_complex_v2(T* S3, int number_of_simplices, int number_of_timeslices) {
  int simplices_per_timeslice = number_of_simplices / number_of_timeslices;
  std::vector<Point> v;
  make_d_sphere(&v, simplices_per_timeslice, 3, 1.0);

  for (size_t i = 0; i < simplices_per_timeslice; i++)
  {
    std::cout << "    " << v[i] << std::endl;
  }

  // Type mismatch so insertion doesn't work yet
  // S3->insert(v.begin(), v.end());

  // for(size_t i = 0; i < simplices_per_timeslice; i++)
  // {
  //   S3->insert(v[i]);
  // }
}
#endif  // SPHERICAL_3_COMPLEX_H_
