/// Causal Dynamical Triangulations in C++ using CGAL
///
/// Copyright (c) 2013 Adam Getchell
///
/// Spherical simplicial complexes

#ifndef SPHERICAL_3_COMPLEX_H_
#define SPHERICAL_3_COMPLEX_H_

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/number_utils.h>

/// C++ headers
#include <cassert>
#include <vector>

typedef CGAL::Exact_predicates_exact_constructions_kernel     K;
/// Used so that an integer for each timeslice may be associated
/// with each vertex
typedef CGAL::Triangulation_vertex_base_with_info_3<int, K>   Vb;
typedef CGAL::Triangulation_data_structure_3<Vb>              Tds;
typedef CGAL::Delaunay_triangulation_3<K, Tds>                Delaunay;

///
/// Make 3D spherical simplicial complexes using random points
/// This method does not admit to a consistent foliation of time
/// yet
template <typename T>
void make_random_S3_simplicial_complex(T* S3, int number_of_simplices,
                                        int number_of_timeslices) {
  CGAL::Random_points_in_sphere_3<typename T::Point> rnd;

  /// Initialize triangulation in 3D
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
  /// Start with a 3D sphere with two simplices
  S3->insert(typename T::Point(0,0,0));
  S3->insert(typename T::Point(1, 0, 0));
  S3->insert(typename T::Point(0, 1, 0));
  S3->insert(typename T::Point(0, 0, 1));
  S3->insert(typename T::Point(0, 0, -1));
  /// In 3D with an infinite point we'll get 4 vertices, 10 edges
  /// 10 facets and 5 cells. The point at $\infty$ isn't counted
  /// but the edges, facets, and cells generated by it are
  /// Using the number_of_finite_ functions solves this

  /// Store the timeslice as an integer in each vertex's info field
  Delaunay::Finite_vertices_iterator vit;
  for (vit = S3->finite_vertices_begin(); vit != S3->finite_vertices_end(); ++vit) {
    int timeslice = (int) CGAL::to_double(vit->point().z());
    vit->info() = timeslice; 
  }

  /// Debugging: print out each vertex and the timeslice number associated
  for (vit = S3->finite_vertices_begin(); vit != S3->finite_vertices_end(); ++vit)
    std::cout << '(' << vit->point() << ") (timeslice = " << vit->info() << ")" << std::endl;

  assert(S3->dimension() == 3);

  std::cout  << "Initial seed has ";
  print_results(S3);

  assert(S3->dimension() == 3);
  assert(S3->is_valid());
}
#endif  // SPHERICAL_3_COMPLEX_H_
