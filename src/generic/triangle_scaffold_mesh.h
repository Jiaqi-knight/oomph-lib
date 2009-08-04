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
#ifndef OOMPH_TRIANGLE_SCAFFOLD_MESH_HEADER
#define OOMPH_TRIANGLE_SCAFFOLD_MESH_HEADER

#include "mesh.h"
#include "Telements.h"

namespace oomph
{


//=====================================================================
/// The Triangle data structure, modified from the triangle.h header
/// supplied with triangle 1.6. by J. R. Schewchuk. 
//=====================================================================
struct triangulateio {
 ///Pointer to list of points x coordinate followed by y coordinate
 double *pointlist;                                         
 ///Pointer to list of point attributes
 double *pointattributelist;          
 ///Pointer to list of point markers
 int *pointmarkerlist;                                       
 int numberofpoints;                                           
 int numberofpointattributes;                                

 int *trianglelist;                                          
 double *triangleattributelist;                                
 double *trianglearealist;                                     
 int *neighborlist;                                          
 int numberoftriangles;                                      
 int numberofcorners;                                        
 int numberoftriangleattributes;                             
 
 int *segmentlist;                                           
 int *segmentmarkerlist;                                     
 int numberofsegments;                                       
 
 double *holelist;                       
 int numberofholes;                    
 
 double *regionlist;                     
 int numberofregions;                  
 
 int *edgelist;                            
 int *edgemarkerlist;          
 double *normlist;               
 int numberofedges;            
};


//=====================================================================
/// \short Triangle Mesh that is based on input files generated by the
/// triangle mesh generator Triangle.
//=====================================================================
class TriangleScaffoldMesh : public virtual Mesh
{

public:
 
 /// Empty constructor
 TriangleScaffoldMesh() {}

 /// \short Constructor: Pass the filenames of the triangle files
 TriangleScaffoldMesh(const std::string& node_file_name,
                      const std::string& element_file_name,
                      const std::string& poly_file_name);
 

 /// \short Constructir that takes a triangulate data structure
 TriangleScaffoldMesh(triangulateio &triangle_data); 

 /// Broken copy constructor
 TriangleScaffoldMesh(const TriangleScaffoldMesh&) 
  { 
   BrokenCopy::broken_copy("TriangleScaffoldMesh");
  } 

 
 /// Broken assignment operator
 void operator=(const TriangleScaffoldMesh&) 
  {
   BrokenCopy::broken_assign("TriangleScaffoldMesh");
  }


 /// Empty destructor 
 ~TriangleScaffoldMesh() {}
 
 /// \short Return the boundary id of the i-th edge in the e-th element:
 /// This is zero-based as in triangle. Zero means the edge is not
 /// on a boundary. Postive numbers identify the boundary.
 /// Will be reduced by one to identify the oomph-lib boundary. 
 unsigned edge_boundary(const unsigned& e, const unsigned& i) const
  {return Edge_boundary[e][i];}

 /// \short Return the attribute of the element e
 double element_attribute(const unsigned &e) const
  {return Element_attribute[e];}

protected:

 /// \short Vector of vectors containing the boundary ids of the
 /// elements' edges
 Vector<Vector<unsigned> > Edge_boundary;

 /// \short Vector of double attributes for each element
 Vector<double> Element_attribute;

};

}

#endif
