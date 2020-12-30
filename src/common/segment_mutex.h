#pragma once

#include <string> 
#include <cassert>
#include <functional>

namespace yas{
    template< typename Mutex>
    class SegmentSharedMutex{
        protected:
        static  int default_num_slots_;
        int num_slots_;
        Mutex* slots_;
        public:
        SegmentSharedMutex():num_slots_(default_num_slots_){
            slots_=new Mutex[num_slots_];
        }
        SegmentSharedMutex(int num_slots):num_slots_(num_slots){
            slots_=new Mutex[num_slots_];
        }
        SegmentSharedMutex(const SegmentSharedMutex&)=delete;
        SegmentSharedMutex& operator=(const SegmentSharedMutex&)=delete;
        virtual ~SegmentSharedMutex(){
            
            if(slots_){
                delete [] slots_;
                slots_=nullptr;
            }
            num_slots_=0;
        }
        SegmentSharedMutex( SegmentSharedMutex&& mutex):num_slots_(0),slots_(nullptr){
             slots_=mutex.slots_;
             num_slots_=mutex.num_slots_;
             mutex.slots_=nullptr;
             mutex.num_slots_=0;
        }

        SegmentSharedMutex& operator=(SegmentSharedMutex&& mutex){
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
  template<typename Mutex> int SegmentSharedMutex<Mutex>::default_num_slots_=20;




  template< typename Mutex,typename Type>
    class SegmentHashSharedMutex:protected SegmentSharedMutex<Mutex>{
        private:
        using SegmentSharedMutex<Mutex>::slots_;
        using SegmentSharedMutex<Mutex>::num_slots_;
        int num_buckets_;
        using HASH_FUNC=std::function<uint64_t(Type,uint64_t)>;
        HASH_FUNC hash_;
        public:
        using key_type=Type;
        SegmentHashSharedMutex(int num_buckets,int num_slots,HASH_FUNC hash ):SegmentSharedMutex<Mutex>(num_slots),num_buckets_(num_buckets),hash_(hash){
        }
        ~SegmentHashSharedMutex()=default;
        SegmentHashSharedMutex(const SegmentHashSharedMutex&)=delete;
        SegmentHashSharedMutex& operator=(const SegmentHashSharedMutex&)=delete;

         SegmentHashSharedMutex( SegmentHashSharedMutex&& mutex):SegmentSharedMutex<Mutex>::num_slots_(0),SegmentSharedMutex<Mutex>::slots_(nullptr){
             slots_=mutex.slots_;
             num_slots_=mutex.num_slots_;
             mutex.slots_=nullptr;
             mutex.num_slots_=0;
        }

        SegmentHashSharedMutex& operator=(SegmentHashSharedMutex&& mutex){
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
            SegmentSharedMutex<Mutex>::lock(slot);
            return bucket_index;
        }

        int lock_shared(Type key){
            uint64_t bucket_index=hash_(key,num_buckets_);
            int slot=bucket_index%num_slots_;
            SegmentSharedMutex<Mutex>::lock_shared(slot);
            return bucket_index;
        }

         int unlock(Type key){
            uint64_t bucket_index=hash_(key,num_buckets_);
            int slot=bucket_index%num_slots_;
            SegmentSharedMutex<Mutex>::unlock(slot);
            return bucket_index;
        }
        
        int unlock_shared(Type key){
            uint64_t bucket_index=hash_(key,num_buckets_);
            int slot=bucket_index%num_slots_;
            SegmentSharedMutex<Mutex>::unlock_shared(slot);
            return bucket_index;
        }

        int get_num_buckets(){
            return num_buckets_;
        }

        void set_num_buckets(int num_buckets){
            num_buckets_=num_buckets;
        }

        int get_num_slots(){
            return num_slots_;
        }
    };


    template<typename Mutex> 
    class ScopedSegmentHashSharedMutex{
        public:
        using key_type=typename Mutex::key_type;
        ScopedSegmentHashSharedMutex(Mutex &mutex,key_type key,bool writable):mutex_(&mutex),writable_(writable),key_(key),bucket_index_(-1){
            if(writable_)
                bucket_index_=mutex_->lock(key);
            else
            {
                bucket_index_=mutex_->lock_shared(key);
            }
            
        }

        ~ScopedSegmentHashSharedMutex(){
            if(writable_)
                mutex_->unlock(key_);
            else
            {
                mutex_->unlock_shared(key_);
            }
            
        }

        int get_bucket_index(){
            return bucket_index_;
        }

        private:
        Mutex* mutex_;
        bool writable_;
        key_type key_;
        int bucket_index_;
    };
}