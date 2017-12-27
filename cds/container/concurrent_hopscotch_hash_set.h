/*
* concurrent_hopscotch_hash_set.h
*
*  Created on: 23 дек. 2017 г.
*      Author: LEONID, ANDREY, ROMAN 
*/

#ifndef CONCURRENTHOPSCOTCHHASHSET_H_
#define CONCURRENTHOPSCOTCHHASHSET_H_
#include <iostream>
#include<atomic>
#include <mutex>
#include<malloc.h>
#include<pthread.h>

namespace cds {
	namespace container {
		template<class KEY, class DATA>
		class Hopscotch {
		  private:
			static const int HOP_RANGE = 32;
			static const int ADD_RANGE = 256;
		 	static const int MAX_SEGMENTS = 1048576; // Including neighbourhodd for last hash location
			static const int MAX_TRIES = 2;
		  KEY* BUSY;
	
		  struct Bucket {
		
				unsigned int volatile _hop_info;
				KEY* volatile _key;
				DATA* volatile _data;
				unsigned int volatile _lock; 
				unsigned int volatile _timestamp;
				pthread_mutex_t lock_mutex;
				pthread_cond_t lock_cv;	

		    Bucket(){
				  _hop_info = 0;
					_lock = 0;
					_key = NULL;
		      _data=NULL;
					_timestamp = 0;
					pthread_mutex_init(&lock_mutex,NULL);
					pthread_cond_init(&lock_cv, NULL);
		  	}
		
		    void lock(){
		      
		      			pthread_mutex_lock(&lock_mutex);
		      while(1){        
				    if (_lock==0){
							_lock =1;
						  pthread_mutex_unlock(&lock_mutex);
						  break;              
					  }
				    pthread_cond_wait(&lock_cv, &lock_mutex); 
		  
		 		  }
				}

				void unlock(){
					pthread_mutex_lock(&lock_mutex);
					_lock = 0;
				  pthread_cond_signal(&lock_cv);
			    pthread_mutex_unlock(&lock_mutex);
				}

			};
		
			Bucket* segments_arys;

		  public:
			Hopscotch(){
				segments_arys = new Bucket[MAX_SEGMENTS+256];
			  BUSY=(int *)malloc(sizeof(int));
			  *BUSY=-1;
			}
		  ~Hopscotch();
			bool contains(KEY* key){
				unsigned int hash = ((*key)&(MAX_SEGMENTS-1));
				Bucket* start_bucket = segments_arys+hash;
				unsigned int try_counter = 0;
				unsigned int timestamp;
				do{
					//cout << "Fast Path : try counter" << try_counter << endl;
					timestamp = start_bucket->_timestamp;
					unsigned int hop_info = start_bucket->_hop_info;
			    
					Bucket* check_bucket = start_bucket;
			    		unsigned int temp;

					for( int i = 0 ; i < HOP_RANGE ; i++){
					      temp=hop_info;
					      temp=temp>>i;
			 
						if(temp&1){
							if(*key == *(check_bucket->_key)){
								//cout << "Found!!" << endl;
								return true;
							}	
						}
			 
						++check_bucket;
					}

					++try_counter;
				}while(timestamp != start_bucket->_timestamp && try_counter < MAX_TRIES);
				//cout << "Slow path " << endl;
				if(timestamp != start_bucket->_timestamp){
					Bucket* check_bucket = start_bucket;
					for(int i=0; i<HOP_RANGE ; i++){
						if(*key == *(check_bucket->_key))
							return true;
						++check_bucket;
					}
				}
				//cout << "Not found!!" << endl;
				return false;
			}
			bool add(KEY *key, DATA *data){
				  int val = 1; 
				  unsigned int hash=((*key)&(MAX_SEGMENTS-1));
				  Bucket* start_bucket=segments_arys+hash;
				  start_bucket->lock();
				  //sleep(1);
				  if(contains(key)){
				    start_bucket->unlock();
				    return false;
				  }

				  Bucket* free_bucket=start_bucket;
				  int free_distance=0;
				  for(;free_distance<ADD_RANGE;++free_distance){
				    if(NULL==free_bucket->_key && NULL == __sync_val_compare_and_swap(&(free_bucket->_key),NULL,BUSY))
				      break;
				    ++free_bucket;
				  }

				  if(free_distance < ADD_RANGE){
				    do{
				      if(free_distance < HOP_RANGE){
					start_bucket->_hop_info |= (1<<free_distance);
					free_bucket->_data=data;
					free_bucket->_key=key;
					start_bucket->unlock();
					return true;
				      }  
				      find_closer_bucket(&free_bucket,&free_distance,val);
				    }while(0 != val);
				  }
				  start_bucket->unlock();
				  //cout<<"Called Resize"<<endl;
				  return false;
				};
			DATA* remove(KEY *key){
				unsigned int hash = ((*key)&(MAX_SEGMENTS-1));
				Bucket* start_bucket = segments_arys+hash;
				start_bucket->lock();
	
				unsigned int hop_info = start_bucket->_hop_info;
				unsigned int mask = 1;
				for(int i=0;i<HOP_RANGE;++i,mask<<=1){
					if(mask & hop_info){
						Bucket* check_bucket = start_bucket+i;
						if(*key==*(check_bucket->_key)){
							int* rc = check_bucket->_data;
							check_bucket->_key=NULL;
							check_bucket->_data=NULL;
							start_bucket->_hop_info &=~(1<<i);
							start_bucket->unlock();
							return rc;
						}
					}
				}
				start_bucket->unlock();
				return NULL;
			}
			  void find_closer_bucket(Bucket** free_bucket,int* free_distance,int &val) {
				Bucket* move_bucket = *free_bucket -(HOP_RANGE-1);
				for(int free_dist = (HOP_RANGE -1);free_dist>0;--free_dist) {
					unsigned int start_hop_info = move_bucket->_hop_info;
					int move_free_distance = -1;
					unsigned int mask =1;
					for (int i=0;i<free_dist;++i,mask <<=1) {
						if(mask & start_hop_info){
							move_free_distance = i;
							break;
						}		
					}
					if (-1 != move_free_distance) {
						move_bucket->lock();
						if(start_hop_info == move_bucket->_hop_info) {
							Bucket* new_free_bucket = move_bucket + move_free_distance;
							move_bucket->_hop_info |= (1 << free_dist);
							(*free_bucket)->_data = new_free_bucket->_data;
							(*free_bucket)->_key = new_free_bucket->_key;
							++(move_bucket->_timestamp);
							new_free_bucket->_key =BUSY;
							new_free_bucket->_data =BUSY;
							move_bucket->_hop_info &= ~(1<<move_free_distance);
							*free_bucket = new_free_bucket;
							*free_distance -=free_dist;
							move_bucket->unlock();
							return;
						}
						move_bucket->unlock();
					}
					++move_bucket;
				}
				(*free_bucket)->_key = NULL;
				val = 0;
				*free_distance = 0;
			}

		};
	}
}

#endif /* CONCURRENTHOPSCOTCHHASHSET_H_ */
