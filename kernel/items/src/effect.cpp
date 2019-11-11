#include "effect.h"

Effect::Effect(std::string name, std::string params)
    : m_name(name)
    , m_params(params)
{
}

std::string Effect::getName() const { return m_name; }

std::string Effect::getParams() const { return m_params; }

status_t Effect::setParams(std::string params) 
{
    m_params = params;
    return 0;
}