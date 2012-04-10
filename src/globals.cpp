#include "globals.h"

Output::Output()
{
  lenline=nlines=npages=0; 
  hbufline=1.0;
  inequation=false;
  pout = &std::cout;
}

Output::Output(std::ostream& o)
{ 
  lenline=lenbuf=nlines=npages=0; 
  hbufline=1.0;
  inequation=false;
  pout=&o;
}
void Output::flushbuf()
{
  *pout << buf.str();
  buf.str("");
  hline=hbufline;
  hbufline=1.0;
  lenline += lenbuf;
  lenbuf=0;
}
void Output::newpageeqn()
{
  *pout << " \\newpg" << std::endl;
  eeq(false);
  beq();
  nlines=0;
  lenline=0;
//   hbufline=1.0;
  ++npages;
  flushbuf();
}
void Output::newlineeqn()
{
  *pout <<"\\nl"<<std::endl;
  lenline=0;
  nlines+=hline;
  flushbuf();
}
void Output::beq()
{
  *pout << "\\beq" << std::endl;
  *pout << "&&";
  inequation=true;
}
void Output::eeq(bool doflush)
{ 
  if (doflush) flushbuf();
  *pout << "\\eeq" << std::endl; 
  inequation=false;
}
bool Output::breaklongline()
{
  if (buf.str().size()==0) return false; // nothing in buffer
  if (inequation && lenline>0 && lenline+lenbuf > maxlenline)
    {// new line
      if (int(nlines)>maxnlines)
        newpageeqn();
      else
        newlineeqn();
      return true;
    }
  return false;
}

Output MyOut::defout;
Output * MyOut::pcurout = &MyOut::defout;
double Input::minfac=1e-10;
TsInpars Input::sInpars;
TiInpars Input::iInpars;
TfInpars Input::fInpars;