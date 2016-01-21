#ifndef Lexic_H
#define Lexic_H
#include <string>
#include <assert.h>
#include "utilities.h"
#include "product.h"
#include "types.h"
#include "globals.h"

/*!
    Lexic elements
*/
class Lelem {
  public:
  // enumerate lexic
  enum Lex {Bra, Ket, LPar, RPar, Oper, Param, Num, Frac, Plus, Minus, Times, Div, Sum, Perm };
  // enumerate types expressions in parantheses (Normal, Connected, Disconnected, ...)
  enum Conn {Normal, Connect, Disconnect }; 
  // constructor from name and Lex
  Lelem (std::string name, Lex lex, Conn conn=Normal);
  std::string name() const;
  Lex lex() const;
  Conn conn() const;
  // return if the bra was already expanded
  bool expandedbra() const;
  // return same element with a status "bra is expanded"
  Lelem braexpanded() const;
  // check equality
  bool operator == (Lelem const & lel) const;
  private:
  std::string _name;
  Lex _lex;
  Conn _conn;
  bool _expandedbra;
};

std::ostream & operator << (std::ostream & o, Lelem const & lel);

class LelString;
typedef std::map< std::string, LelString > NewOpMap;

/*
 * string of lexic elements
 */
// typedef Product<Lelem> LelString;
class LelString : public Product<Lelem> {
public:
  LelString():Product< Lelem >(){};
  LelString(const_iterator beg, const_iterator end):Product< Lelem >(beg,end){};
  
  LelString substring(lui beg, lui end) const {
    lui end1=(end < this->size() ? end + 1 : this->size());
    return LelString(this->begin()+beg,this->begin()+end1);
  }; 
  void add(const Lelem& a) { this->push_back(a);}
  void add(const LelString& a) { this->operator*=(a);}
  
  // get position of the closing bracket (which corresponds to the bracket on ipos)
  lui closbrack(lui ipos) const;
  // get position of the opening bracket (which corresponds to the bracket on ipos)
  lui openbrack(lui ipos) const;
  // add connections: beg and end are positions of opening and closing parantheses
  Product<long int> addconnections(lui beg, lui end) const;
  // expand all newops
  LelString expandnewops(const NewOpMap& newops) const;
  // find the end position of current element in aterm, if bk==true: bra and ket are treated as brackets
  lui elem(lui beg, bool bk=false) const;
  // find the end position of current term
  lui term(lui beg) const { return elem(beg,true); };
  // expand parantheses pair
  LelString  expandpar(lui beg, ConnectionsMap& connections) const;
  // expand a term
  LelString expandterm(ConnectionsMap& connections) const;
  // test if eqn is completely expanded
  bool expanded() const;
  // expand full expression
  void expand(ConnectionsMap& connections);
  
};

#endif
