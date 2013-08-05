#include "operators.h"

/* SQOp::SQOp(std::string orb)
{
  _gender=(isupper((char)orb[0]) ? Creator : Annihilator);
  orb[0] = std::tolower(orb[0]);
  _orb = orb;
} */
SQOp::SQOp(Gender gender, Orbital orb)
{
  _gender=gender;
  _orb = orb;
}
SQOp::Gender SQOp::gender() const
{ return _gender; }
SQOp::Gender SQOp::genderPH() const
{ if (_orb.type()==Orbital::Occ && _gender==Creator) return Annihilator;
  if (_orb.type()==Orbital::Occ && _gender==Annihilator) return Creator;
  if (_orb.type()==Orbital::GenT || _orb.type()==Orbital::Act) return Gen;
  return _gender; }
Orbital SQOp::orb() const
{ return _orb; }
bool SQOp::operator==(const SQOp& o) const
{ return _gender==o._gender && _orb==o._orb; }
bool SQOp::operator<(const SQOp& o) const
{
  if ( _gender<o._gender )
    return true;
  if (o._gender<_gender )
    return false;
  return _orb<o._orb;
}
void SQOp::replace(Orbital orb1, Orbital orb2)
{
  if (_orb==orb1) _orb=orb2;
}

std::ostream & operator << (std::ostream & o, SQOp const & op)
{
  o << "\\op{" << op.orb() << "}";
  if ( op.gender()==SQOp::Creator )
    o << "^\\dg";
  
  return o;
}

Oper::Oper()
{  
  _prefac=1; 
  _type=Ops::None;
}

Oper::Oper(Ops::Type type, bool antisym)
{
  assert( InSet(type, Ops::FluctP,Ops::Fock,Ops::XPert) );
  std::string name;
  _type=type;
  if (type == Ops::Fock )
    name="F";
  else if (type == Ops::FluctP )
    name="W";
  else
    name="X";
  create_Oper(name,antisym);
}
Oper::Oper(Ops::Type type, short int exccl, std::string name, int lm)
{
  assert( !InSet(type, Ops::FluctP,Ops::Fock,Ops::XPert) );
  _type=type;
  Orbital orb0(std::string("A"));
  Orbital orb1(std::string("I"));
  create_Oper(exccl,orb1,orb0,name,lm);
}
Oper::Oper(Ops::Type type, short int exccl, 
           void * term, Orbital (*freeorb)(void * term, Orbital::Type type), std::string name, int lm)
{
  assert( !InSet(type, Ops::FluctP,Ops::Fock,Ops::XPert) );
  _type=type;
  Orbital orb0(freeorb(term,Orbital::Virt));
  Orbital orb1(freeorb(term,Orbital::Occ));
  create_Oper(exccl,orb1,orb0,name,lm);
}
Oper::Oper(Ops::Type type, short int exccl, Orbital occ, Orbital virt, std::string name, int lm)
{
  assert( !InSet(type, Ops::FluctP,Ops::Fock,Ops::XPert) );
  _type=type;
  create_Oper(exccl,occ,virt,name,lm);
//   xout << *this << std::endl;
}
Oper::Oper(Ops::Type type, short int exccl, const std::map< Orbital::Type, Orbital >& orbnames, 
           const std::vector< Product< Orbital::Type > >& orbtypes, std::string name, int lm)
{
  assert( !InSet(type, Ops::FluctP,Ops::Fock,Ops::XPert) );
  assert( orbtypes.size() == 2 );
  assert( int(orbtypes[0].size()) == exccl );
  assert( int(orbtypes[1].size()) == exccl+lm );
  _type=type;
  create_Oper(exccl,orbnames,orbtypes,name,lm);
}

Oper::Oper(Ops::Type type, short int exccl, const Product< Orbital >& occs, const Product< Orbital >& virts, 
           std::string name, int lm)
{
  assert( occs.size() + lm == virts.size() );
  assert( occs.size() == uint(exccl) );
  assert( !InSet(type, Ops::FluctP,Ops::Fock,Ops::XPert) );
  _type=type;
  create_Oper(occs,virts,name);
}
Oper::Oper(Ops::Type type, short int exccl, 
           void * term, Orbital (*freeorb)(void * term, Orbital::Type type), 
           const std::vector< Product< Orbital::Type > >& orbtypes, std::string name, int lm)
{
  assert( !InSet(type, Ops::FluctP,Ops::Fock,Ops::XPert) );
  assert( orbtypes.size() == 2 );
  assert( int(orbtypes[0].size()) == exccl );
  assert( int(orbtypes[1].size()) == exccl+lm );
  
  _type=type;
  std::map<Orbital::Type,Orbital> orbnames;
  for ( uint i=0; i<orbtypes.size(); ++i ){
    Product<Orbital::Type>::const_iterator iot;
    _foreach(iot,orbtypes[i]){
      if ( orbnames.count(*iot) == 0 ){
        // new orb type
        orbnames[*iot] = freeorb(term,*iot);
      }
    }
  }
  create_Oper(exccl,orbnames,orbtypes,name,lm);
}

void Oper::create_Oper(const std::string& name, bool antisym)
{
  assert( InSet(_type, Ops::FluctP,Ops::Fock,Ops::XPert) );
  Product<Orbital> porbs;
  Matrices::Spinsym spinsym = Matrices::Singlet;
  // operators with general indices
  Orbital orb(std::string("P"));
  _SQprod*=SQOp(SQOp::Creator,orb);
  porbs*=orb;
  _sumindx.insert(orb);
//   orb=Orbital(std::string("Q"),orb.spin());//same spin as in P
  orb=Orbital(std::string("Q"));//same spin as in P
  porbs*=orb;
  _sumindx.insert(orb);
  _prefac=1;
  if ( InSet(_type, Ops::Fock,Ops::XPert) ) {
    _SQprod*=SQOp(SQOp::Annihilator,orb);
  } else {
   // we use chemical notation (PQ|RS) P^\dg R^\dg S Q
    orb=Orbital(std::string("R"));
    _SQprod*=SQOp(SQOp::Creator,orb);
    porbs*=orb;
    _sumindx.insert(orb);
//     orb=Orbital(std::string("S"),orb.spin());//same spin as in R
    orb=Orbital(std::string("S"));//same spin as in R
    _SQprod*=SQOp(SQOp::Annihilator,orb);
    porbs*=orb;
    _sumindx.insert(orb);
    orb=Orbital(std::string("Q"));
    _SQprod*=SQOp(SQOp::Annihilator,orb);
    _prefac /= 4;
  }
  short npairs = porbs.size()/2;
  _mat=Matrices(_type,porbs,npairs,name,spinsym,antisym);
}
void Oper::create_Oper(short int const & exccl,Orbital const & occ, Orbital const & virt, std::string const & name, int lm)
{
  assert( !InSet(_type, Ops::FluctP,Ops::Fock,Ops::XPert) );
  std::string excl;
  Product<Orbital> occs, virts; 
  short 
    nocc = exccl,
    nvirt = exccl+lm,
    nmax = std::max(nocc,nvirt);
  for (short i=0; i<nmax ; ++i) {  
    if (i>0) excl=num2str(i,std::dec);
    if ( i < nocc )
      occs *= Orbital(occ.name()+excl,occ.spin());
    if ( i < nvirt )
      virts *= Orbital(virt.name()+excl,virt.spin());
  }
  create_Oper(occs,virts,name);
}
void Oper::create_Oper(const short int& exccl, const std::map< Orbital::Type, Orbital >& orbnames, 
                       const std::vector< Product< Orbital::Type > >& orbtypes, const std::string& name, int lm)
{
  assert( !InSet(_type, Ops::FluctP,Ops::Fock,Ops::XPert) );
  std::string excl;
  Product<Orbital> occs, virts; 
  short 
    nocc = exccl,
    nvirt = exccl+lm;
  const Product<Orbital::Type> & occtype(orbtypes[0]);
  const Product<Orbital::Type> & virtype(orbtypes[1]);
  std::map< Orbital::Type, uint > num4type;
  assert(int(occtype.size()) == nocc);
  assert(int(virtype.size()) == nvirt);
  for (short i = 0; i < nocc; ++i) {  
    uint i4t = num4type[occtype[i]];
    ++num4type[occtype[i]];
    if (i4t > 0) {
      excl = num2str(i4t,std::dec);
    } else {
      excl = "";
    }
    const Orbital& occ(orbnames.at(occtype[i]));
    occs *= Orbital(occ.name()+excl,occ.spin());
  }
  for (short i = 0; i < nvirt; ++i) {  
    uint i4t = num4type[virtype[i]];
    ++num4type[virtype[i]];
    if (i4t > 0) {
      excl=num2str(i4t,std::dec);
    } else {
      excl = "";
    }
    const Orbital& virt(orbnames.at(virtype[i]));
    virts *= Orbital(virt.name()+excl,virt.spin());
  }
  create_Oper(occs,virts,name);
}

void Oper::create_Oper(const Product< Orbital >& occs, const Product< Orbital >& virts, const std::string& name)
{
  assert( !InSet(_type, Ops::FluctP,Ops::Fock,Ops::XPert) );
//  assert( occs.size() == virts.size() );
  Matrices::Spinsym spinsym = Matrices::Singlet;
  Product<Orbital> porbs;
  // excitation and deexcitation operators
  const Product<Orbital> 
    * p_orb0 = &virts,
    * p_orb1 = &occs;
  if (InSet(_type, Ops::Deexc,Ops::Deexc0)) std::swap(p_orb0,p_orb1);
  short 
    ncrea = p_orb0->size(),
    nanni = p_orb1->size(),
    nmax = std::max(ncrea,nanni),
    npairs = std::min(ncrea,nanni);
  // symmetry (hash of orbital-type -> number of such electrons)
  std::map<uint,uint> sym;
  _prefac = 1;
  for (unsigned short i = 0; i < nmax; ++i) {  
    Spin spin(Spin::No);
    bool setspindiff = (i == nmax-1 && spinsym != Matrices::Singlet);
    if ( i < ncrea ) {
      Orbital orb = (*p_orb0)[i];
      spin = orb.spin();
      if (setspindiff){ // triplet part -- use spin-difference
        assert(spin.type() != Spin::No);
        spin.settype(Spin::GenD);
        orb.setspin(spin);
      }
      _SQprod*=SQOp(SQOp::Creator, orb);
      porbs *= orb;
      _sumindx.insert(orb);
      if (InSet(_type, Ops::Exc0,Ops::Deexc0)) 
        _fakesumindx.insert(orb);
      // add this type of electron-orbital
      sym[uint(orb.type())] += 1;
    }
    if ( i < nanni ) {
      Orbital orb = (*p_orb1)[i];
      if (spin.type() != Spin::No){ // use same spin for same electrons
        orb.setspin(spin);
      }
      _SQprod *= SQOp(SQOp::Annihilator, orb);
      porbs *= orb;
      _sumindx.insert(orb);
      if (InSet(_type, Ops::Exc0,Ops::Deexc0)) 
        _fakesumindx.insert(orb);
      // add this type of electron-orbital
      sym[uint(orb.type())*Orbital::MaxType] += 1;
    }
  }
  // prefactor
  std::map<uint,uint>::const_iterator is; 
  _foreach(is,sym){
    for (uint i = 0; i < is->second; ++i)
      _prefac *= i+1;
  }
  _prefac = 1/_prefac;
  _mat=Matrices(_type,porbs,npairs,name,spinsym);
}

Matrices Oper::mat() const
{ return _mat;}
Product< SQOp > Oper::SQprod() const
{ return _SQprod;}
TFactor Oper::prefac() const
{ return _prefac;}
const TOrbSet & Oper::sumindx() const
{ return _sumindx;}
TOrbSet Oper::realsumindx() const
{
  TOrbSet realsum;
  for (TOrbSet::const_iterator it = _sumindx.begin(); it != _sumindx.end(); ++it )  
    if (_fakesumindx.count(*it) == 0)
      realsum.insert(*it);
  return realsum;
}



std::ostream & operator << (std::ostream & o, Oper const & op)
{
  if ( _todouble(_abs(_abs(op.prefac()) - 1)) > MyOut::pcurout->small) o << op.prefac();
  if (op.realsumindx().size()>0) o <<"\\sum_{"<<op.realsumindx()<<"}";
  o <<op.mat() << op.SQprod();
  return o;
}

