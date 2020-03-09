#pragma once
#include "common.h"
#include <set>

struct Audio
{
private:
    const id_t m_id;
    std::set<id_t> m_linked_fragments;

    const std::string m_path; // fs path 
    float* m_buffer; // decoded data
    const uint32_t m_buffer_size;

public:
    Audio(const id_t id, const std::string &path, float* buffer, const uint32_t buffer_size);

    std::string getPath() const;
    uint32_t getBufferLength() const;
    float* getBuffer() const;

    bool unlinkFragment(id_t fragment_id);
    bool linkFragment(id_t fragment_id);

    id_t getId() const;
    std::set<id_t> getLinkedFragments() const;
};