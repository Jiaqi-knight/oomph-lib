//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//           Version 0.90. August 3, 2009.
//LIC// 
//LIC// Copyright (C) 2006-2009 Matthias Heil and Andrew Hazel
//LIC// 
//LIC// This library is free software; you can redistribute it and/or
//LIC// modify it under the terms of the GNU Lesser General Public
//LIC// License as published by the Free Software Foundation; either
//LIC// version 2.1 of the License, or (at your option) any later version.
//LIC// 
//LIC// This library is distributed in the hope that it will be useful,
//LIC// but WITHOUT ANY WARRANTY; without even the implied warranty of
//LIC// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//LIC// Lesser General Public License for more details.
//LIC// 
//LIC// You should have received a copy of the GNU Lesser General Public
//LIC// License along with this library; if not, write to the Free Software
//LIC// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//LIC// 02110-1301  USA.
//LIC// 
//LIC// The authors may be contacted at oomph-lib@maths.man.ac.uk.
//LIC// 
//LIC//====================================================================
//Header file for classes that define hp-refineable element objects

//Include guard to prevent multiple inclusions of the header
#ifndef OOMPH_HP_REFINEABLE_ELEMENTS_HEADER
#define OOMPH_HP_REFINEABLE_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

#include "refineable_elements.h"
#include "refineable_line_element.h"
#include "refineable_quad_element.h"
#include "refineable_brick_element.h"
#include "mesh.h"

namespace oomph
{
//======================================================================
/// p-refineable version of RefineableQElement<1,INITIAL_NNODE_1D>.
/// Generic class definitions
//======================================================================
template<unsigned INITIAL_NNODE_1D>
class PRefineableQElement<1,INITIAL_NNODE_1D> : public RefineableQElement<1>,
                            public virtual QElement<1,INITIAL_NNODE_1D>,
                            public virtual PRefineableElement
 {
public:

 /// Constructor
 PRefineableQElement() : PRefineableElement(), RefineableQElement<1>()
  {}
 
 /// \short Initial setup of element (set the correct p-order and
 /// integration scheme)
 void initial_setup();
 
 /// \short Pre-build (search father for required nodes which may already
 /// exist)
 void pre_build(Mesh*& mesh_pt, Vector<Node*>& new_node_pt);
 
 /// \short p-refine the element (refine if inc>0, unrefine if inc<0).
 void p_refine(const int &inc, Mesh* const &mesh_pt);
 
 /// Overload the shape functions
 void shape(const Vector<double> &s, Shape &psi) const;
 
 void dshape_local(const Vector<double> &s, Shape &psi, DShape &dpsi) const;
 
 void d2shape_local(const Vector<double> &s, Shape &psi, DShape &dpsids,
                    DShape &d2psids) const;
 
 /// \short Perform additional hanging node procedures for variables
 /// that are not interpolated by all nodes (e.g. lower order interpolations
 /// for the pressure in Taylor Hood). 
 void further_setup_hanging_nodes() {}
 
 /// \short Returns the number of nodes along each edge of the element.
 /// Overloaded to return the (variable) p-order rather than the template
 /// argument.
 unsigned nnode_1d() const {return this->p_order();}
 
 /// Get the initial P_order
 unsigned initial_p_order() const {return INITIAL_NNODE_1D;}
  
 // Overloaded from QElement<1,NNODE_1D> to use nnode_1d() instead of
 // template argument.
 Node* get_node_at_local_coordinate(const Vector<double> &s);
 
 Node* node_created_by_son_of_neighbour(const Vector<double> &s_fraction, 
                                        bool &is_periodic);
 
 // Overload nodal positions -- these elements have GLL-spaced nodes.
 /// Get local coordinates of node j in the element; vector sets its own size
 void local_coordinate_of_node(const unsigned& n, Vector<double>& s);

 /// Get the local fractino of node j in the element
 void local_fraction_of_node(const unsigned &n, Vector<double> &s_fraction);

 /// The local one-d fraction is the same
 double local_one_d_fraction_of_node(const unsigned &n1d, const unsigned &i);

 /// Rebuild the element. This needs to find any nodes in the sons which
 /// are still required.
 void rebuild_from_sons(Mesh* &mesh_pt);

 /// \short Check the integrity of interpolated values across element
 /// boundaries.
 void check_integrity(double& max_error);

protected:

 /// \short Set up hanging node information. Empty for 1D elements.
 void binary_hang_helper(const int &value_id, const int &my_edge,
                         std::ofstream& output_hangfile);
 
};

//=======================================================================
/// p-refineable version of RefineableQElement<2,INITIAL_NNODE_1D>.
//=======================================================================
template<unsigned INITIAL_NNODE_1D>
class PRefineableQElement<2,INITIAL_NNODE_1D> : public RefineableQElement<2>,
                            public virtual QElement<2,INITIAL_NNODE_1D>,
                            public virtual PRefineableElement
 {
public:

 /// Constructor
 PRefineableQElement() : PRefineableElement(), RefineableQElement<2>()
  {}

 /// Create and return a clone of myself (like a "virtual" constructor).
 /// This is required during the p-refinement so that the element can
 /// read data from "itself" while it builds itself with a new p-order.
 /// Must be defined in the derived class to return an object of the
 /// correct type.
 //BENFLAG: This is not a "fully-functioning" clone! It will merely contain
 //         all the required information normally obtained from the father
 //         in the RefineableQElement's build() procedure, and it has no
 //         memory leaks (I think).
 virtual PRefineableQElement<2,INITIAL_NNODE_1D>* make_backup_clone() const=0;
 
 /// \short Initial setup of element (set the correct p-order and integration
 /// scheme)
 void initial_setup();
 
 /// \short Pre-build (search father for required nodes which may already
 /// exist)
 void pre_build(Mesh*& mesh_pt, Vector<Node*>& new_node_pt);

 /// \short p-refine the element (refine if inc>0, unrefine if inc<0).
 void p_refine(const int &inc, Mesh* const &mesh_pt);
 
 // Overload the shape functions
 void shape(const Vector<double> &s, Shape &psi) const;
 
 void dshape_local(const Vector<double> &s, Shape &psi, DShape &dpsi) const;
 
 void d2shape_local(const Vector<double> &s, Shape &psi, DShape &dpsids,
                    DShape &d2psids) const;
 
 /// \short Perform additional hanging node procedures for variables
 /// that are not interpolated by all nodes (e.g. lower order interpolations
 /// for the pressure in Taylor Hood). 
 void further_setup_hanging_nodes() {}
 
 /// \short Returns the number of nodes along each edge of the element.
 /// Overloaded to return the (variable) p-order rather than the template
 /// argument.
 unsigned nnode_1d() const {return this->p_order();}
 
 /// Get the initial P_order
 unsigned initial_p_order() const {return INITIAL_NNODE_1D;}
  
 // Overloaded from QElement<2,NNODE_1D> to use nnode_1d() instead of
 // template argument.
 Node* get_node_at_local_coordinate(const Vector<double> &s);
 
 Node* node_created_by_neighbour(const Vector<double> &s_fraction, 
                                 bool &is_periodic);
 
 Node* node_created_by_son_of_neighbour(const Vector<double> &s_fraction, 
                                        bool &is_periodic);
 
 // Overload nodal positions -- these elements have GLL-spaced nodes.
 /// Get local coordinates of node j in the element; vector sets its own size
 void local_coordinate_of_node(const unsigned& n, Vector<double>& s);

 /// Get the local fractino of node j in the element
 void local_fraction_of_node(const unsigned &n, Vector<double> &s_fraction);

 /// The local one-d fraction is the same
 double local_one_d_fraction_of_node(const unsigned &n1d, const unsigned &i);

 /// Rebuild the element. This needs to find any nodes in the sons which
 /// are still required.
 void rebuild_from_sons(Mesh* &mesh_pt);
 
 /// \short Check the integrity of interpolated values across element
 /// boundaries.
 /// Note: with the mortar method, continuity is enforced weakly across non-
 /// conforming element boundaries, so it makes no sense to check the
 /// continuity of interpolated values across these boundaries.
 void check_integrity(double& max_error);

protected:

 /// \short Set up hanging node information.
 /// Overloaded to implement the mortar method rather than constrained
 /// approximation. This enforces continuity weakly via an integral matching
 /// condition at non-conforming element boundaries.
 void quad_hang_helper(const int &value_id, const int &my_edge,
                       std::ofstream& output_hangfile);
 
 /// \short Return the value of the intrinsic boundary coordinate
 /// interpolated along the edge (S/W/N/E) of the element before
 /// p-refinement. Requires a vector of pointers to the element's
 /// original nodes and the original p-order of the element before
 /// refinement in addition to the arguments to the standard
 /// interpolated_zeta_on_edge(...) function.
 // BENFLAG: This is required during p-refinement because new nodes in
 //          elements with curvilinear boundaries normally interpolate
 //          their boundary coordinate from their element's father, but
 //          with p-refinement they should instead interpolate from the
 //          current element before it was refined.
 void interpolated_zeta_on_edge_before_p_refinement(
       const unsigned &boundary,
       const int &edge,
       const Vector<double> &s,
       const unsigned &old_p_order,
       const Vector<Node*> &old_node_pt,
       Vector<double> &zeta);
 
 // Set up node update info for (newly created) algebraic node: Work out its
 // node update information by interpolation from its father element, 
 // based on pointer to father element and its local coordinate in 
 // the father element. We're creating the node update info
 // for update functions that are shared by all nodes in the
 // father element. 
 void bens_setup_algebraic_node_update_generic(
       Node*& node_pt,
       const Vector<double>& s,
       FiniteElement* father_el_pt,
       const unsigned& old_p_order,
       const Vector<Node*>& old_node_pt) const;
 
};

//=======================================================================
/// p-refineable version of RefineableQElement<3,INITIAL_NNODE_1D>.
//=======================================================================
template<unsigned INITIAL_NNODE_1D>
class PRefineableQElement<3,INITIAL_NNODE_1D> : public RefineableQElement<3>,
                            public virtual QElement<3,INITIAL_NNODE_1D>,
                            public virtual PRefineableElement
 {
public:

 /// Constructor
 PRefineableQElement() : PRefineableElement(), RefineableQElement<3>()
  {
   throw OomphLibError(
    "3-dimensional PRefineableQElements are not fully implemented yet!\n",
    "PRefineableQElement<3,INITIAL_NNODE_1D>::PRefineableQElement()",
    OOMPH_EXCEPTION_LOCATION);
  }
 
 /// \short Initial setup of element (set the correct p-order and integration
 /// scheme)
 void initial_setup();
 
 /// \short Pre-build (search father for required nodes which may already
 /// exist)
 void pre_build(Mesh*& mesh_pt, Vector<Node*>& new_node_pt);
 
 /// \short p-refine the element (refine if inc>0, unrefine if inc<0).
 void p_refine(const int &inc, Mesh* const &mesh_pt);
 
 // Overload the shape functions
 void shape(const Vector<double> &s, Shape &psi) const;
 
 void dshape_local(const Vector<double> &s, Shape &psi, DShape &dpsi) const;
 
 void d2shape_local(const Vector<double> &s, Shape &psi, DShape &dpsids,
                    DShape &d2psids) const;
 
 /// \short Perform additional hanging node procedures for variables
 /// that are not interpolated by all nodes (e.g. lower order interpolations
 /// for the pressure in Taylor Hood). 
 void further_setup_hanging_nodes() {}
 
 /// \short Returns the number of nodes along each edge of the element.
 /// Overloaded to return the (variable) p-order rather than the template
 /// argument.
 unsigned nnode_1d() const {return this->p_order();}
 
 /// Get the initial P_order
 unsigned initial_p_order() const {return INITIAL_NNODE_1D;}
  
 // Overloaded from QElement<3,NNODE_1D> to use nnode_1d() instead of
 // template argument.
 Node* get_node_at_local_coordinate(const Vector<double> &s);
 
 Node* node_created_by_neighbour(const Vector<double> &s_fraction);
 
 Node* node_created_by_son_of_neighbour(const Vector<double> &s_fraction);
 
 // Overload nodal positions -- these elements have GLL-spaced nodes.
 /// Get local coordinates of node j in the element; vector sets its own size
 void local_coordinate_of_node(const unsigned& n, Vector<double>& s);

 /// Get the local fractino of node j in the element
 void local_fraction_of_node(const unsigned &n, Vector<double> &s_fraction);

 /// The local one-d fraction is the same
 double local_one_d_fraction_of_node(const unsigned &n1d, const unsigned &i);

 /// Rebuild the element. This needs to find any nodes in the sons which
 /// are still required.
 void rebuild_from_sons(Mesh* &mesh_pt);
 
 /// \short Check the integrity of interpolated values across element
 /// boundaries.
 /// Note: with the mortar method, continuity is enforced weakly across non-
 /// conforming element boundaries, so it makes no sense to check the
 /// continuity of interpolated values across these boundaries.
 void check_integrity(double& max_error);

protected:
 
 /// \short Set up hanging node information.
 /// Overloaded to implement the mortar method rather than constrained
 /// approximation. This enforces continuity weakly via an integral matching
 /// condition at non-conforming element boundaries.
 void oc_hang_helper(const int &value_id, const int &my_edge,
                     std::ofstream& output_hangfile);
 
// /// \short Return the value of the intrinsic boundary coordinate
// /// interpolated along the face (S/W/N/E) of the element before
// /// p-refinement. Requires a vector of pointers to the element's
// /// original nodes and the original p-order of the element before
// /// refinement in addition to the arguments to the standard
// /// interpolated_zeta_on_edge(...) function.
// // BENFLAG: This is required during p-refinement because new nodes in
// //          elements with curvilinear boundaries normally interpolate
// //          their boundary coordinate from their element's father, but
// //          with p-refinement they should instead interpolate from the
// //          current element before it was refined.
// void interpolated_zeta_on_face_before_p_refinement(
//       const unsigned &boundary,
//       const int &edge,
//       const Vector<double> &s,
//       const unsigned &old_p_order,
//       const Vector<Node*> &old_node_pt,
//       Vector<double> &zeta);
 
};

}

#endif
