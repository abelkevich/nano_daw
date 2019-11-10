#pragma once
#include "common.h"
#include <set>


struct Effect
{
private:
    const std::string m_name; // lib name
    std::string m_params; // serialized params

public:
    Effect(std::string name, std::string params);

    std::string getName() const;
    std::string getParams() const;
    status_t setParams(std::string params);
};
