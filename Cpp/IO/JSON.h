#ifndef IO_JSON_H
#define IO_JSON_H

#include <Klampt/Modeling/Resources.h>
#include <KrisLibrary/utils/AnyCollection.h>

///Default conversion to collection
template <class T>
void Convert(const T& x,AnyCollection& c) { c = x; }
///Default conversion from collection
template <class T>
bool Convert(const AnyCollection& c,T& x) { return c.as<T>(x); }

///Default conversion from array to collection
template <class T>
void Convert(const std::vector<T>& v,AnyCollection& c)
{
  c.resize(v.size());
  for(size_t i=0;i<v.size();i++)
    Convert(v[i],c[(int)i]);
}

///Default conversion from collection to array 
template <class T>
bool Convert(const AnyCollection& c,std::vector<T>& v)
{
  v.resize(c.size());
  for(size_t i=0;i<v.size();i++)
    if(!Convert(c[(int)i],v[i])) return false;
  return true;
}

void Convert(const Vector3& x,AnyCollection& c);
void Convert(const Vector& x,AnyCollection& c);
void Convert(const IKGoal& g,AnyCollection& c);
void Convert(const Hold& h,AnyCollection& c);
void Convert(const Grasp& g,AnyCollection& c);
void Convert(const Stance& s,AnyCollection& c);
bool Convert(const AnyCollection& c,Vector3& x);
bool Convert(const AnyCollection& c,Vector& x);
bool Convert(const AnyCollection& c,IKGoal& g);
bool Convert(const AnyCollection& c,Hold& h);
bool Convert(const AnyCollection& c,Grasp& g);
bool Convert(const AnyCollection& c,Stance& s);

template <class T>
void SaveJSON(std::ostream& out,const T& x) {
  AnyCollection msg;
  Convert(x,msg);
  out<<msg;
}


template <class T>
bool LoadJSON(std::istream& in,const T& x) {
  AnyCollection msg;
  in >> msg;
  if(!in) return false;
  return Convert(msg,x);
}

#endif
