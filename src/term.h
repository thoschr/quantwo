#ifndef Term_H
#define Term_H

#include <vector>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include "utilities.h"
#include "product.h"
#include "operators.h"
#include "kronecker.h"
#include "sum.h"
#include "globals.h"

#include <iostream>


/*!
    A term consists of a Product of SQOperators and a Product of Kroneckers
*/
class Term {
  public:
    //! default constructor 
    Term();
    //! construct from Product<SQOp>, Product<Kronecker> will be empty
    Term(Product<SQOp> const & opProd);
    //! construct from Product<SQOp> and Product<Kronecker>
    Term(Product<SQOp> const & opProd, Product<Kronecker> const & kProd);
    //! construct from Product<SQOp>, Product<Kronecker>, Product<Matrices>, summation indices and prefactor
    Term(Product<SQOp> const & opProd, Product<Kronecker> const & kProd, 
         Product<Matrices> const & mat, const TOrbSet & sumindx, const TOrbSet & realsumindx, 
         const TFactor& prefac, const std::vector< Product<long int> >& connections);
    //! validate term
    bool term_is_valid();
    //! append Operator
    Term & operator *= (Oper const & t);
    //! multiply by a factor
    Term & operator *= (const TFactor& fac);
    //! multiply by a permutation
    Term & operator *= (Permut const & perm);
    //! add permutator
    Term & operator += (Permut const & perm);
    //! add permutator with a factor
    Term & operator += (std::pair<Permut,TFactor> const & p);
    //! add connections
    void addconnection (Product<long int> const & connections);
    //! add summation indices
    void addsummation (Orbital const & orb, short excl);
    //! add matrix
    void addmatrix (Matrices const & mat);
    //! replace matrix on position ipos
    void replacematrix (Matrices const & mat, unsigned long int ipos);
    //! return contained Product<SQOp>
    Product<SQOp> opProd() const;
    //! return contained Product<Kronecker>
    Product<Kronecker>  kProd() const;
    //! return prefactor
    TFactor prefac() const;
    //! return matrices
    Product<Matrices> mat() const;
    //! return summation indices
    TOrbSet sumindx() const;
    //! return real summation indices
    TOrbSet realsumindx() const;
    //! generate set of external-lines orbitals
    TOrbSet extindx() const;
    //! return Sum of Permutators
    Sum<Permut,TFactor> perm() const;
    //! return connections
    std::vector< Product<long int> > connections() const;
    //! return true if term is zero
    bool term_is_0(double minfac) const;
    //! artificial ordering
    bool operator < (Term const & t) const;
    //! equal terms
    // terms will be not changed! (but const can't be applied) 
    bool equal(Term & t, Permut & perm);
    //! calculate normal ordering
    Sum<Term, TFactor>  normalOrder() const;
    //! calculate normal ordering, fully contracted terms only
    Sum<Term, TFactor>  normalOrder_fullyContractedOnly() const;
    //! calculate normal ordering in Particle/Hole formalism
    Sum<Term, TFactor>  normalOrderPH() const;
    //! calculate normal ordering in Particle/Hole formalism, fully contracted terms only
    Sum<Term, TFactor>  normalOrderPH_fullyContractedOnly() const;
    typedef std::list<int> TWMats;
    typedef std::list<TWMats> TWOps;
    //! Wick's theorem: call recursive routine wick
    // if genw == true: use the generalized Wick's theorem
    Sum<Term, TFactor>  wickstheorem(bool genw = false) const;
    //! Wick's theorem, recursive: opers contains index of SQop in _opProd (divided into individual operators)
    Sum<Term, TFactor>  wick(TWOps& opers, TWMats& krons) const;
    //! generalized Wick's theorem, recursive: opers contains index of SQop in _opProd (divided into individual operators)
    Sum<Term, TFactor>  genwick(TWOps& opers, TWMats& krons, TWMats& densmat) const;
    //! set connections for each matrix
    void setmatconnections();
    //! reduce equation (delete Kroneckers and summation indices)
    void reduceTerm();
    // delete "None" matrices (caution, the order of matrices can be important, so do it AFTER connection stuff!)
    void deleteNoneMats();
    // brilloin condition (return true for terms with occ-virt fock)
    bool brilloin() const;
    //! Determine connections (in reduced term!)
    void matrixkind();
    //! expand integral ( from antisymmetrized form < AB || CD > to the normal form < AB | CD > - < AB | DC > )
    //! if firstpart=true : < AB | CD >, if firstpart=false : < AB | DC >
    //! if return is true: expanded, if false: don't need to expand
    bool expandintegral(bool firstpart);
    //! check if we have any antisymmetrized matrices in term
    bool antisymmetrized();
    //! expand all antisymmetrical matrices in term 
    Sum<Term,TFactor> expand_antisym();
    //! Spin integration (if notfake false: calculate only _nloops, _nintloops, _nocc)
    void spinintegration(bool notfake);
    //! set prefactor of term to one
    void reset_prefac();
    // set permutations to p
    void setperm(const Sum<Permut,TFactor>& p){_perm = p;};
    // permute the term according to p
    void permute(const Permut& p);
    //! compare actual connections with the needed (in _connections)
    //! return true if the term is ok
    bool properconnect() const;
    //! print diagram, which corresponds to this term 
    void printdiag(Output* pout, TFactor fac) const;
    //! return free orbital name
    Orbital freeorbname(Orbital::Type type);
    //! static wrapper-function to be able to callback the member function freeorbname()
    static Orbital getfreeorbname(void * Obj, Orbital::Type type);
//    //! get last orbital
//    Orbital get_lastorb(Orbital::Type type) const { return _lastorb[type]; };
    //! set last orbital (onlylarger: only if it's larger than current one)
    void set_lastorb(Orbital orb, bool onlylarger = false);

  private:
    Sum<Term, TFactor>  normalOrder(bool fullyContractedOnly) const;
    Sum<Term, TFactor>  normalOrderPH(bool fullyContractedOnly) const;

    Product<SQOp> _opProd;
    Product<Kronecker>  _kProd;
    Product<Matrices> _mat;
    TOrbSet _sumindx,_realsumindx;
    TFactor _prefac;
    // connections of matrices in term
    // (abs(value)-1) gives the index of the corresponding matrix in _mat
    // positive sign: connected group of matrices Product<long int>
    // negative sign: disconnected --------------"-----------------
    std::vector< Product<long int> > _connections;

    Sum<Permut,TFactor> _perm;
    std::map< Orbital::Type, Orbital > _lastorb;
    // for term comparison:
    // number of (all and internal only) loops and occupied orbitals (set in spinintegration)
    lui _nloops, _nintloops, _nocc;
};

//! output operator for Term
std::ostream & operator << (std::ostream & o, Term const & t);

class Termel : public Term {
public:
    Termel(){};
    Termel(const Term& term) : Term(term){};
private:
  TElSet _sumel, _realsumel;
};

namespace Q2
{
  template <class T>
  void replace(Product<T> &p, Orbital orb1, Orbital orb2);
  void replace(SQOp &op, Orbital orb1, Orbital orb2);
  void replace(Orbital &orb, Orbital orb1, Orbital orb2);
  void replace(Matrices &mat,Orbital orb1, Orbital orb2);
  void replace(Kronecker &kron, Orbital orb1, Orbital orb2);
}
#endif

