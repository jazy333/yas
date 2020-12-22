#pragma once

#include <string> 
#include <cassert>
#include <functional>

namespace yas{
    template< class Mutex>
    class SegmentMutex{
        protected:
        static  int default_num_slots_;
        int num_slots_;
        Mutex* slots_;
        public:
        SegmentMutex():num_slots_(default_num_slots_){
            slots_=new Mutex[num_slots_];
        }
        SegmentMutex(int num_slots):num_slots_(num_slots){
            slots_=new Mutex[num_slots_];
        }
        SegmentMutex(const SegmentMutex&)=delete;
        SegmentMutex& operator=(const SegmentMutex&)=delete;
        virtual ~SegmentMutex(){
            
            if(slots_){
                delete [] slots_;
                std::cout<<"~SegmentMutex"<<std::endl;
            }
            num_slots_=0;
        }
        SegmentMutex( SegmentMutex&& mutex):num_slots_(0),slots_(nullptr){
             slots_=mutex.slots_;
             num_slots_=mutex.num_slots_;
             mutex.slots_=nullptr;
             mutex.num_slots_=0;
        }

        SegmentMutex& operator=(SegmentMutex&& mutex){
            if(this!=&mutex){
                delete [] slots_;
                slots_=nullptr;
                num_slots_=0;
                slots_=mutex.slots_;
                num_slots_=mutex.num_slots_;
                mutex.slots_=nullptr;
                mutex.num_slots_=0;
            }
            return *this;
        }


        void lock(int index){
            assert(index<num_slots_&&index>=0);
            slots_[index].lock();
        }
        void unlock(int index){
            assert(index<num_slots_&&index>=0);
            slots_[index].unlock();
        }
        void lock_shared(int index){
            assert(index<num_slots_&&index>=0);
            slots_[index].lock_shared();
        }
        void unlock_shared(int index){
            assert(index<num_slots_&&index>=0);
            slots_[index].unlock_shared();
        }

        int get_num_slots(){
            return num_slots_;
        }
    };
  template<typename Mutex> int SegmentMutex<Mutex>::default_num_slots_=20;




  template< typename Mutex,typename Type>
    class SegmentHashMutex:protected SegmentMutex<Mutex>{
        private:
        using SegmentMutex<Mutex>::slots_;
        using SegmentMutex<Mutex>::num_slots_;
        int num_buckets_;
        using HASH_FUNC=std::function<uint64_t(Type,uint64_t)>;
        HASH_FUNC hash_;
        public:
        using key_type=Type;
        SegmentHashMutex(int num_buckets,int num_slots,HASH_FUNC hash ):SegmentMutex<Mutex>(num_slots),num_buckets_(num_buckets),hash_(hash){
        }
        ~SegmentHashMutex()=default;
        SegmentHashMutex(const SegmentHashMutex&)=delete;
        SegmentHashMutex& operator=(const SegmentHashMutex&)=delete;

         SegmentHashMutex( SegmentHashMutex&& mutex):SegmentMutex<Mutex>::num_slots_(0),SegmentMutex<Mutex>::slots_(nullptr){
             slots_=mutex.slots_;
            num_slots_=mutex.num_slots_;
             mutex.slots_=nullptr;
             mutex.num_slots_=0;
        }

        SegmentHashMutex& operator=(SegmentHashMutex&& mutex){
            if(this!=&mutex){
                delete [] slots_;
                slots_=nullptr;
                num_slots_=0;
                slots_=mutex.slots_;
                num_slots_=mutex.num_slots_;
                mutex.slots_=nullptr;
                mutex.num_slots_=0;
            }
            return *this;
        }

        int lock(Type key){
            uint64_t bucket_index=hash_(key,num_buckets_);
            int slot=bucket_index%num_slots_;
            SegmentMutex<Mutex>::lock(slot);
            return bucket_index;
        }

        int lock_shared(Type key){
            uint64_t bucket_index=hash_(key,num_buckets_);
            int slot=bucket_index%num_slots_;
            SegmentMutex<Mutex>::lock_shared(slot);
            return bucket_index;
        }

         int unlock(Type key){
            uint64_t bucket_index=hash_(key,num_buckets_);
            int slot=bucket_index%num_slots_;
            SegmentMutex<Mutex>::unlock(slot);
            return bucket_index;
        }
        
        int unlock_shared(Type key){
            uint64_t bucket_index=hash_(key,num_buckets_);
            int slot=bucket_index%num_slots_;
            SegmentMutex<Mutex>::unlock_shared(slot);
            return bucket_index;
        }

        int get_num_buckets(){
            return num_buckets_;
        }

        int get_num_slots(){
            return num_slots_;
        }
    };


    template<typename Mutex> 
    class ScopedSegmentHashLock{
        private:
        Mutex* mutex_;
        bool writable_;
        using key_type=typename Mutex::key_type;
        key_type key_;
       
        public:
        ScopedSegmentHashLock(Mutex &mutex,key_type key,bool writable){
            writable_=writable;
            key_=key;
            mutex_=&mutex;
            if(writable_)
                mutex_->lock(key);
            else
            {
                mutex_->lock_shared(key);
            }
            
        }

        ~ScopedSegmentHashLock(){
            std::cout<<" ~ScopedSegmentHashLock"<<std::endl;
            if(writable_)
                mutex_->unlock(key_);
            else
            {
                mutex_->unlock_shared(key_);
            }
            
        }
    };
}