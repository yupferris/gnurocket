#include "SyncTrack.h"
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <cassert>

// Useful boost methods
template <class T>
T next(T x) { return ++x; }

template <class T, class Distance>
T next(T x, Distance n) {
    std::advance(x, n);
    return x;
}

template <class T>
T prior(T x) { return --x; }

template <class T, class Distance>
T prior(T x, Distance n) {
    std::advance(x, -n);
    return x;
}

float key_linear(const SyncKey &k0, const SyncKey &k1, float row) {
    double t = (row - k0.row) / (k1.row - k0.row);
    return (float)(k0.value + (k1.value - k0.value) * t);
}

float key_smooth(const SyncKey &k0, const SyncKey &k1, double row)
{
    double t = (row - k0.row) / (k1.row - k0.row);
    t = t * t * (3 - 2 * t);
    return (float)(k0.value + (k1.value - k0.value) * t);
}

float key_ramp(const SyncKey &k0, const SyncKey &k1, double row)
{
    double t = (row - k0.row) / (k1.row - k0.row);
    t = pow(t, 2.0);
    return (float)(k0.value + (k1.value - k0.value) * t);
}

SyncTrack::SyncTrack(std::string name)
    : name(name)
{
}

SyncKey SyncTrack::GetPrevKey(int row)
{
    assert(keys.size() != 0);

    if (keys.lower_bound(row) == keys.begin()) {
        return keys.lower_bound(row)->second;
    }

    std::map<int,SyncKey>::iterator it = keys.upper_bound(row);
    it--;
    if (it == keys.end())
        return it->second;
    if (it == prior(keys.end()))
        return it->second;

    return it->second;
}

SyncKey SyncTrack::GetExactKey(int row)
{
    assert(keys.size() != 0);
    assert(keys.count(row) == 1);
    return keys[row];
}


float SyncTrack::GetValue(double row) {
    if (keys.size() == 0)
        return 0.0;
    int irow = (int)floor(row);
    //std::map<int,SyncKey>::iterator it2 = keys.begin();
    //std::cout << "row: " << irow << std::endl;
    //for (; it2 != keys.end(); it2++) {
        //std::cout << it2->second.row << " " << it2->second.value << std::endl;
    //}
    //std::cout << std::endl;

    if (keys.lower_bound(irow) == keys.begin()) {
        return keys.lower_bound(irow)->second.value;
    }

    std::map<int,SyncKey>::iterator it = keys.upper_bound(irow);
    it--;
    if (it == keys.end())
        return it->second.value;
    if (it == prior(keys.end()))
        return it->second.value;

    switch (it->second.type) {
    case SyncKey::STEP:
        return it->second.value;
    case SyncKey::LINEAR:
        return key_linear(it->second, next(it)->second, row);
    case SyncKey::SMOOTH:
        return key_smooth(it->second, next(it)->second, row);
    case SyncKey::RAMP:
        return key_ramp(it->second, next(it)->second, row);
    default:
        std::cout << it->second.type << std::endl;
        assert(false);
        break;
    }
    return 0.0f;
}

void SyncTrack::SetKey(SyncKey key)
{
    //std::cout << "val: " << key.value << std::endl;
    keys[key.row] = key;
}

void SyncTrack::DelKey(int row)
{
    SyncTrack::iterator it = keys.find(row);
    if (it != keys.end()) {
        std::cout << it->second.row << " " << it->second.value << std::endl;
        keys.erase(it);
    }
}

void SyncTrack::LoadFromFile(std::string base) {
    FILE *fp = fopen("sync.track", "rb");
    if (!fp)
        assert(0);

    uint32_t num_keys;
    fread(&num_keys, sizeof(uint32_t), 1, fp);
    if (feof(fp))
        assert(0);
    keys.clear();
    for (uint32_t i = 0; i < num_keys; ++i) {
        SyncKey key;
        fread(&key.row, sizeof(key.row), 1, fp);
        fread(&key.value, sizeof(key.value), 1, fp);
        fread(&key.type, 1, 1, fp);
        keys[key.row] = key;
    }
}


bool SyncTrack::IsKeyFrame(int row)
{
    if (keys.find(row) != keys.end())
        return true;
    return false;
}

std::string SyncTrack::GetName()
{
    return name;
}

std::map<int,SyncKey> SyncTrack::GetKeyMap() {
    return keys;
}
