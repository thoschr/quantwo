#ifndef Matrices_H
#define Matrices_H

#include <string>
#include <iostream>
#include "product.h"
#include "orbital.h"
#include "globals.h"

namespace Ops {
  // enumerate operator types 
  enum Type {None, Fock, FluctP, XPert, Exc, Deexc, Exc0, Deexc0, Interm, Number};
  // generate Product<Orbital> from occupied and virtual orbitals and excitation class 
  Product<Orbital> genprodorb(short exccl,Orbital const & occ, Orbital const & virt);
};

/*! 
    Implements class matrices (e.g. amplitudes, integrals etc)
*/
class Matrices {
  public:
  // enumerate vertices types
  enum Spinsym {Singlet, Triplet};
  Matrices();
  // construct from type and orbitals and name
  Matrices(Ops::Type t, Product<Orbital> p, std::string name="T", Spinsym matspinsym=Singlet);
  // return Type
  Ops::Type type() const;
  // return Product of orbitals
  Product<Orbital> orbitals() const;
  // return name
  std::string name() const;
  // return true if antisymmetrized form
  bool antisymform() const;
  // replace orbital orb1 with orb2
  void replace(Orbital orb1, Orbital orb2);
  // expand antisymmetrized matrix ( from antisymmetrized form < AB || CD > to the normal form < AB | CD > - < AB | DC > )
  // if firstpart=true : < AB | CD >, if firstpart=false : < AB | DC >
  // if return is true: expanded, if false: don't need to expand
  bool expandantisym(bool firstpart);
  // artificial ordering
  bool operator < (Matrices const & t) const;
  // equality of two Matrices (including symmetry properties)
  bool operator == (Matrices const & t) const;
  // set "kind" of matrix (_exccl, _intlines, _intvirt)
  void setkind(short exccl, short intlines, short intvirt);
  // get the second orbital for the same electron (if not found return blank orbital)
  Orbital orbel(Orbital const & orb);
  // get the second orbital for the same electron (from position) (if not found return blank orbital)
  Orbital orbel(long int const & ipos);
  // get the spin symmetry of the electron, which corresponds to the orbital on position ipos
  Spinsym spinsym(long int ipos);
  // set no spin for all orbitals
  void set_no_spin();
  // reset vertices
  void reset_vertices();
  // compare vertices
  bool vertices(long int ipos, Matrices & mat, long int ipos1, unsigned int indx);
  // set connections
  void set_connect(Product<long int> connected2);
  // add connection
  void add_connect(long int con);
  // return connections
  Product<long int> connected2() const;
  
  private:
  Ops::Type _type;
  Product<Orbital> _orbs;
  std::string _name;
  Spinsym _matspinsym;
  bool _antisymform; // W is constructed in antisymmetrized form, but can be expanded later.
  // needed for comparison of terms:
  long int _indx;
  // connected to (index of matrix in term (start from 1!!!)):
  Product<long int> _connected2;
  // following variables will be set by Term::matrixkind (according to Kallay and Surjan, JCP, 115 (2001), 2945)
  short _exccl, _intlines, _intvirt;
    
};

std::ostream & operator << (std::ostream & o, Matrices const & mat);

/*! 
    Implements class permutators (\Perm{ia,jb}(ia|jb)=(jb|ia))
*/
class Permut {
  public:
    Permut();
    // construct from product of indices
    Permut(Product<Orbital> p1,Product<Orbital> p2);
    // construct from orbitals
    Permut(Orbital o1,Orbital o2);
    // append permutator
    Permut & operator *= (Permut p);
    // return orbitals "from"
    Product<Orbital> orbsfrom() const;
    // return orbitals "to"
    Product<Orbital> orbsto() const;
    // artificial ordering
    bool operator < (Permut const & p) const;
    // equality of permutators
    bool operator == (Permut const & p) const;
    bool is1() const { return _orbsfrom.size() == 0; };
  private: 
    Product<Orbital> _orbsfrom, _orbsto;
};

std::ostream & operator << (std::ostream & o, Permut const & p);

#endif

