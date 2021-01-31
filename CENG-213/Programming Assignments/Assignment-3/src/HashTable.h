#ifndef __HASHTABLE__
#define __HASHTABLE__

#include "HashUtils.h"
#include "ItemNotFoundException.h"
/* Do not add new libraries or files */

#define BUCKET_SIZE 2

// Do not modify the public interface of this class.
// Otherwise, your code will note compile!
template <class K, class T>
class HashTable {
    struct Entry {
        K Key;             // the key of the entry
        T Value;   // the value of the entry
        bool Deleted;        // flag indicating whether this entry is deleted
        bool Active;         // flag indicating whether this item is currently used

        Entry() : Key(), Value(), Deleted(false), Active(false) {}
    };

    struct Bucket {
        Entry entries[BUCKET_SIZE];
    };

    int _capacity; // INDICATES THE TOTAL CAPACITY OF THE TABLE
    int _size; // INDICATES THE NUMBER OF ITEMS IN THE TABLE

    Bucket* _table; // THE HASH TABLE

    // == DEFINE HELPER METHODS & VARIABLES BELOW ==
    Entry * getget(const K& key) const{
        if(!_table){
            return NULL;
        }
        int h_code = Hash(key);
        int quad_num = 0;

        while(quad_num*quad_num < _capacity){
            int p = (h_code + quad_num*quad_num) % _capacity;
            for(int i = 0; i < BUCKET_SIZE; i++){
                if(!_table[p].entries[i].Active && !_table[p].entries[i].Deleted){
                    return NULL;
                }else if(_table[p].entries[i].Active && _table[p].entries[i].Key == key){
                    return &(_table[p].entries[i]);
                }
            }
            quad_num++;
        }return NULL;
    }

public:
    // TODO: IMPLEMENT THESE FUNCTIONS.
    // CONSTRUCTORS, ASSIGNMENT OPERATOR, AND THE DESTRUCTOR
    HashTable();
    // COPY THE WHOLE CONTENT OF RHS INCLUDING THE KEYS THAT WERE SET AS DELETED
    HashTable(const HashTable<K, T>& rhs);
    HashTable<K, T>& operator=(const HashTable<K, T>& rhs);
    ~HashTable();

    // TODO: IMPLEMENT THIS FUNCTION.
    // INSERT THE ENTRY IN THE HASH TABLE WITH THE GIVEN KEY & VALUE
    // IF THE GIVEN KEY ALREADY EXISTS, THE NEW VALUE OVERWRITES
    // THE ALREADY EXISTING ONE. IF THE LOAD FACTOR OF THE TABLE IS GREATER THAN 0.6,
    // RESIZE THE TABLE WITH THE NEXT PRIME NUMBER.
    void Insert(const K& key, const T& value);

    // TODO: IMPLEMENT THIS FUNCTION.
    // DELETE THE ENTRY WITH THE GIVEN KEY FROM THE TABLE
    // IF THE GIVEN KEY DOES NOT EXIST IN THE TABLE, THROW ItemNotFoundException()
    void Delete(const K& key);

    // TODO: IMPLEMENT THIS FUNCTION.
    // IT SHOULD RETURN THE VALUE THAT CORRESPONDS TO THE GIVEN KEY.
    // IF THE KEY DOES NOT EXIST, THROW ItemNotFoundException()
    T& Get(const K& key) const;

    // TODO: IMPLEMENT THIS FUNCTION.
    // AFTER THIS FUNCTION IS EXECUTED THE TABLE CAPACITY MUST BE
    // EQUAL TO newCapacity AND ALL THE EXISTING ITEMS MUST BE REHASHED
    // ACCORDING TO THIS NEW CAPACITY.
    // WHEN CHANGING THE SIZE, YOU MUST REHASH ALL OF THE ENTRIES
    void Resize(int newCapacity);

    // THE IMPLEMENTATION OF THESE FUNCTIONS ARE GIVEN TO YOU
    // DO NOT MODIFY!
    int Capacity() const; // RETURN THE TOTAL CAPACITY OF THE TABLE
    int Size() const; // RETURN THE NUMBER OF ACTIVE ITEMS
    void getKeys(K* keys); // PUT THE ACTIVE KEYS TO THE GIVEN INPUT PARAMETER
};


template <class K, class T>
HashTable<K, T>::HashTable() {
    // TODO: IMPLEMENT THIS FUNCTION.
    _table = NULL;
    _capacity = 0;
    _size = 0;
    
}

template <class K, class T>
HashTable<K, T>::HashTable(const HashTable<K, T>& rhs) {
    // TODO: IMPLEMENT THIS FUNCTION.
    _table = new Bucket[rhs._capacity];
    _capacity = rhs._capacity;
    _size = rhs._size;
    for(int i = 0; i < _capacity; i++){
        for (int k = 0; k < BUCKET_SIZE; k++){
            _table[i].entries[k].Active = rhs._table[i].entries[k].Active;
            _table[i].entries[k].Deleted = rhs._table[i].entries[k].Deleted;
            if (rhs._table[i].entries[k].Active){
                _table[i].entries[k].Key = rhs._table[i].entries[k].Key;
                _table[i].entries[k].Value = rhs._table[i].entries[k].Value;
            }
        }
    }
}

template <class K, class T>
HashTable<K, T>& HashTable<K, T>::operator=(const HashTable<K, T>& rhs) {
    // TODO: IMPLEMENT THIS FUNCTION.
    if(_table != NULL){
        delete [] _table;
    }
    _table = new Bucket[rhs._capacity];
    _capacity = rhs._capacity;
    _size = rhs._size;
    for(int i = 0; i < _capacity; i++){
        for(int k = 0; k < BUCKET_SIZE; k++){
            _table[i].entries[k].Active = rhs._table[i].entries[k].Active;
            _table[i].entries[k].Deleted = rhs._table[i].entries[k].Deleted;
            if(rhs._table[i].entries[k].Active){
                _table[i].entries[k].Key = rhs._table[i].entries[k].Key;
                _table[i].entries[k].Value = rhs._table[i].entries[k].Value;
            }
        }
    }return *this;
}

template <class K, class T>
HashTable<K, T>::~HashTable() {
    // TODO: IMPLEMENT THIS FUNCTION.
    if(_table != NULL){
        delete [] _table;
    }
}

template <class K, class T>
void HashTable<K, T>::Insert(const K& key, const T& value) {
    // TODO: IMPLEMENT THIS FUNCTION.
    if(_capacity == 0){
        _capacity = NextCapacity(_capacity);
        _table = new Bucket[_capacity];
    }
    Entry * ent = getget(key);
    if(ent){
        ent->Key = key;
        ent->Value = value;
        ent->Active = true;
        ent->Deleted = false;
    }else{
        if((double)_size / (_capacity*BUCKET_SIZE) > 0.6){
            Resize(NextCapacity(_capacity));
        }
        int h_code = Hash(key);
        int quad_num = 0;
        bool flag = false;

        while(!flag){
            int p = (h_code + quad_num*quad_num)%_capacity;
            for(int i = 0; i < BUCKET_SIZE; i++){
                if(!_table[p].entries[i].Active){
                    _table[p].entries[i].Active = true;
                    _table[p].entries[i].Deleted = false;
                    _table[p].entries[i].Key = key;
                    _table[p].entries[i].Value = value;
                    flag = true;
                    break;
                }
            }
            quad_num++;
        }
        if(flag){
            _size++;
        }
    }
}

template <class K, class T>
void HashTable<K, T>::Delete(const K& key) {
    // TODO: IMPLEMENT THIS FUNCTION.
    Entry * dlt = getget(key);
    if(dlt){
        dlt->Active = false;
        dlt->Deleted = true;
        _size--;
    }else{
        throw ItemNotFoundException();
    }
}

template <class K, class T>
T& HashTable<K, T>::Get(const K& key) const {
    // TODO: IMPLEMENT THIS FUNCTION. 
    Entry *wanted = getget(key);
    if(wanted){
        return wanted->Value;
    }
    throw ItemNotFoundException();
}


template <class K, class T>
void HashTable<K, T>::Resize(int newCapacity) {
    // TODO: IMPLEMENT THIS FUNCTION.
    Bucket* tmp_table = new Bucket[newCapacity];
    if(_table){
        for(int i = 0; i < _capacity; i++){
            for(int k = 0; k < BUCKET_SIZE; k++){
                if(_table[i].entries[k].Active){
                    int h_code = Hash(_table[i].entries[k].Key);
                    int quad_num = 0;
                    bool flag = false;

                    while(!flag){
                        int p = (h_code+quad_num*quad_num)%newCapacity;
                        for(int m = 0; m < BUCKET_SIZE; m++){
                            if(!tmp_table[p].entries[m].Active){
                                tmp_table[p].entries[m].Active = true;
                                tmp_table[p].entries[m].Deleted = false;
                                tmp_table[p].entries[m].Key = _table[i].entries[k].Key;
                                tmp_table[p].entries[m].Value = _table[i].entries[k].Value;
                                flag = true;
                                break;
                            }
                        }
                        quad_num++;
                    }
                }
            }
        }
    }
    _capacity = newCapacity;
    delete [] _table;
    _table = tmp_table;
}


template <class K, class T>
int HashTable<K, T>::Capacity() const {
    return _capacity;
}

template <class K, class T>
int HashTable<K, T>::Size() const {
    return _size;
}


template <class K, class T>
void HashTable<K, T>::getKeys(K* keys) {
    int index = 0;

    for (int i = 0; i < _capacity; i++) {
        Bucket& bucket = _table[i];
        for (int j = 0; j < BUCKET_SIZE; j++) {
            if (bucket.entries[j].Active && !bucket.entries[j].Deleted) {
                keys[index++] = bucket.entries[j].Key;
            }
        }
    }
}

#endif