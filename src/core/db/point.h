#pragma once

#include <initializer_list>
#include <iterator>
#include <iostream>
#include <cstddef>
#include <sys/types.h>

namespace yas {
  template <class T, int D>
  class Point {
  public:
    Point() :docid(-1) {
    }

    Point(std::initializer_list<T> v,int docid){
      assert(v.size()==D);                        
      std::copy(v.begin(), v. end(),std::begin(point.v));
    }

    Point(const Point& p){
      docid=p.docid;
     std::copy(std::begin(p.point.v),std::end(p.point.v),std::begin(point.v));
    }

    Point& operator=(const Point& p){
      if(&p!=this){
        docid=p.docid;
        std::copy(std::begin(p.point.v),std::end(p.point.v),std::begin(point.v));
      }
      return *this;
    }
  
    virtual ~Point() = default;
    
    int get_docid() {
      return docid;
    }

    T& operator[] (int index) {
     assert(index < D&& index >= 0);
      return point.v[index];
    }

    int dim() {
      return D;
    }

    u_char* bytes(){
      return point.bytes;
    }

    size_t bytes_size(){
      return sizeof(T)*D;
    }

    size_t one_dim_size(){
      return sizeof(T);
    }

 friend std::ostream& operator<<(std::ostream& os,const Point<T,D>& p){
      os<<'[';
      if(D==1)
      std::copy(std::begin(p.point.v),std::end(p.point.v),std::ostream_iterator<T>(os,""));
      else{
        std::copy(std::begin(p.point.v),std::end(p.point.v)-1,std::ostream_iterator<T>(os,","));
        os<<p.point.v[D-1];
      }
      os<<']';
      return os;
    } 

    private:
    int  docid;
    union{        
    T v[D];
    u_char  bytes[sizeof(T)*D];
    } point;
  }; 

             
} //namespace yas                                                                                      